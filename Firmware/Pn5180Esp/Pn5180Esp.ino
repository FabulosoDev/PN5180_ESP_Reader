#include "include/config.hpp"
#include "include/globals.hpp"
#include "include/pinout.hpp"
#include "include/pn15693.hpp"
#include "include/neopixel.hpp"
#include "include/web_server.hpp"
#include "include/wifi_manager.hpp"
#include "include/card_storage.hpp"
#include "include/discord.hpp"
#include "include/flipper_nfc.hpp"

PN15693 pn15693(PIN_PN5180_NSS, PIN_PN5180_BUSY, PIN_PN5180_RST);
NeoPixel neopixel(PIN_WS2812B);

Config config("/config.json");
WiFiManager wifiManager(config);
Discord discord(config);
WebServer webServer(config, pn15693, discord);
CardStorage cardStorage(pn15693);

uint8_t lastSentUid[PN15693::UID_SIZE] = {0};

void setup() {
    Serial.begin(115200);
    Serial.println(F("Booting..."));

#if defined(ARDUINO_ARCH_ESP32)
    SPIFFS.begin(true);
#else
    SPIFFS.begin();
#endif

    wifiManager.begin();

    webServer.begin();

    neopixel.begin();
    neopixel.feedback(LED_BRIGHTNESS, LED_BRIGHTNESS, LED_BRIGHTNESS, 100);

    if (!pn15693.init()) {
        neopixel.feedback(LED_BRIGHTNESS, 0, 0, 100);
    } else {
        neopixel.feedback(0, LED_BRIGHTNESS, 0, 100);
    }
}

void loop() {
    if (pn15693.isInitialized() && pn15693.unlockCard()) {
        webServer.getEvents().send("Card unlocked successfully.", "unlock_success", millis());
        if (!pn15693.readCard()) {
            neopixel.feedback(LED_BRIGHTNESS, 0, 0, 100);
            webServer.getEvents().send("Failed to read card!", "read_error", millis());
        } else {
            neopixel.feedback(0, LED_BRIGHTNESS, 0, 100);
            webServer.getEvents().send("Card read successfully.", "read_success", millis());

            SaveCardResult result = cardStorage.saveCard();
            switch (result) {
                case SaveCardResult::Success:
                    webServer.getEvents().send("Card saved successfully.", "save_success", millis());
                    break;

                case SaveCardResult::Duplicate:
                    webServer.getEvents().send("Card already in database.", "save_duplicate", millis());
                    break;

                case SaveCardResult::Error:
                    webServer.getEvents().send("Failed to save card!", "save_error", millis());
                    break;
            }

            if (memcmp(pn15693.getUid(), lastSentUid, PN15693::UID_SIZE) != 0) {
                Serial.println(F("New card detected - sending to Discord..."));

                FlipperNfc flipper(pn15693);
                if (discord.sendTextFile(flipper.getFilename() + ".nfc", flipper.create())) {
                    memcpy(lastSentUid, pn15693.getUid(), PN15693::UID_SIZE);
                }
            } else {
                Serial.println(F("Card previously sent to Discord, skipping"));
            }

            delay(2000);
        }
    }

    webServer.processDiscordUpload();

    delay(100);
}
