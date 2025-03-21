#include "../include/config.hpp"

Config::Config(const char* filename) : _filename(filename) {
    _ssid[0] = '\0';
    _password[0] = '\0';
    _channelId[0] = '\0';
    _token[0] = '\0';
    SPIFFS.begin();
}

bool Config::load() {
    StaticJsonDocument<JSON_CAPACITY> doc;
    File file;
    
    if (!openFile(file, "r")) {
        Serial.println(F("Config: File not found"));
        return false;
    }
    
    String jsonString = file.readString();
    file.close();
    
    DeserializationError error = deserializeJson(doc, jsonString);
    if (error) {
        logError(error.c_str());
        return false;
    }
    
    JsonObject json = doc.as<JsonObject>();
    if (!json.containsKey("ssid") || !json.containsKey("password")) {
        logError("Missing credentials in config");
        return false;
    }
    
    strlcpy(_ssid, json["ssid"] | "", MAX_SSID_LENGTH + 1);
    strlcpy(_password, json["password"] | "", MAX_PASSWORD_LENGTH + 1);
    strlcpy(_channelId, json["channel_id"] | "", MAX_CHANNEL_ID_LENGTH + 1);
    strlcpy(_token, json["token"] | "", MAX_TOKEN_LENGTH + 1);
    
    return true;
}

bool Config::save() {
    StaticJsonDocument<JSON_CAPACITY> doc;
    File file;
    
    doc["ssid"] = _ssid;
    doc["password"] = _password;
    doc["channel_id"] = _channelId;
    doc["token"] = _token;
    
    if (!openFile(file, "w")) {
        return false;
    }
    
    bool success = serializeJson(doc, file) > 0;
    file.close();
    
    return success;
}

void Config::setCredentials(const char* ssid, const char* password) {
    strlcpy(_ssid, ssid, MAX_SSID_LENGTH + 1);
    strlcpy(_password, password, MAX_PASSWORD_LENGTH + 1);
}

void Config::setDiscordConfig(const char* channelId, const char* token) {
    strlcpy(_channelId, channelId, MAX_CHANNEL_ID_LENGTH + 1);
    strlcpy(_token, token, MAX_TOKEN_LENGTH + 1);
}

bool Config::openFile(File& file, const char* mode) const {
    file = SPIFFS.open(_filename, mode);
    if (!file) {
        Serial.print(F("Failed to open config file: "));
        Serial.println(_filename);
        return false;
    }
    return true;
}

void Config::logError(const char* message) const {
    Serial.print(F("Config error: "));
    Serial.println(message);
}