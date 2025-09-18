#if defined(ARDUINO_ARCH_ESP32)
  #include "SPIFFS.h"
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <FS.h>
#endif

#include "../include/card_storage.hpp"

const char* CardStorage::CARDS_FILE = "/cards.json";

CardStorage::CardStorage(const PN15693& pn15693)
    : _pn15693(pn15693) {
}

SaveCardResult CardStorage::saveCard() const {
    const uint8_t* uid = _pn15693.getUid();
    const uint8_t* data = _pn15693.getData();

    if (!uid || !data) {
        Serial.println(F("Invalid card data"));
        return SaveCardResult::Error;
    }

    char uidString[(PN15693::UID_SIZE * 2) + 1];
    char* ptr = uidString;
    for (uint8_t i = 0; i < PN15693::UID_SIZE; i++) {
        ptr += sprintf(ptr, "%02x", uid[i]);
    }
    *ptr = '\0';

    char dataString[(PN15693::DATA_SIZE * 2) + 1];
    ptr = dataString;
    for (uint8_t i = 0; i < PN15693::DATA_SIZE; i++) {
        ptr += sprintf(ptr, "%02x", data[i]);
    }
    *ptr = '\0';

    DynamicJsonDocument doc(4096);
    JsonArray cards;

    if (SPIFFS.exists(CARDS_FILE)) {
        File file = SPIFFS.open(CARDS_FILE, "r");
        if (!file) {
            Serial.println(F("Failed to open cards file for reading"));
            return SaveCardResult::Error;
        }

        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            Serial.println(F("Failed to parse cards file"));
            return SaveCardResult::Error;
        }

        cards = doc.as<JsonArray>();

        for (const auto& card : cards) {
            if (card.containsKey("uid") && strcmp(card["uid"].as<const char*>(), uidString) == 0) {
                Serial.println(F("Card already in database, skipping"));
                return SaveCardResult::Duplicate;
            }
        }
    } else {
        cards = doc.to<JsonArray>();
    }

    JsonObject card = cards.add<JsonObject>();
    for (size_t i = cards.size() - 1; i > 0; i--) {
        cards[i] = cards[i - 1];
    }
    cards[0] = card;
    cards[0]["uid"] = uidString;
    cards[0]["data"] = dataString;

    File file = SPIFFS.open(CARDS_FILE, "w");
    if (!file) {
        Serial.println(F("Failed to open cards file for writing"));
        return SaveCardResult::Error;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println(F("Failed to write cards file"));
        file.close();
        return SaveCardResult::Error;
    }

    file.close();
    Serial.println(F("Card saved to database"));
    return SaveCardResult::Success;
}