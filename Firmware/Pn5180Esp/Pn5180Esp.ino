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
WebServer webServer(config, pn15693);
CardStorage cardStorage(pn15693);
Discord discord("", "");

void setup() {
    Serial.begin(115200);
    Serial.println(F("Booting..."));

    SPIFFS.begin();

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

            FlipperNfc flipper(pn15693);
            String content = flipper.create();
            String filename = flipper.getFilename() + ".nfc";
            
            discord.sendTextFile(filename, content);

            delay(2000);
        }
    }

    delay(100);
}