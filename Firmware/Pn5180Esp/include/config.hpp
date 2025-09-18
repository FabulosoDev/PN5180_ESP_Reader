#pragma once

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>

class Config {
public:
    Config(const char* filename = "/config.json");
    bool load();
    bool save();
    void setCredentials(const char* ssid, const char* password);
    void setDiscordConfig(const char* channelId, const char* token);

    bool hasWifiParameters() const { return _ssid[0] != '\0' && _password[0] != '\0'; }
    bool hasDiscordParameters() const { return _channelId[0] != '\0' && _token[0] != '\0'; }
    const char* getSsid() const { return _ssid; }
    const char* getPassword() const { return _password; }
    const char* getChannelId() const { return _channelId; }
    const char* getToken() const { return _token; }

private:
    static const size_t MAX_SSID_LENGTH = 32;
    static const size_t MAX_PASSWORD_LENGTH = 64;
    static const size_t MAX_CHANNEL_ID_LENGTH = 24;
    static const size_t MAX_TOKEN_LENGTH = 72;
    static const size_t JSON_CAPACITY = 512;

    const char* _filename;
    char _ssid[MAX_SSID_LENGTH + 1];
    char _password[MAX_PASSWORD_LENGTH + 1];
    char _channelId[MAX_CHANNEL_ID_LENGTH + 1];
    char _token[MAX_TOKEN_LENGTH + 1];

    bool openFile(File& file, const char* mode) const;
    void logError(const char* message) const;
};

#endif // CONFIG_HPP