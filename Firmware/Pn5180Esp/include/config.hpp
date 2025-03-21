#pragma once

#include <ArduinoJson.h>
#include <FS.h>

class Config {
public:
    static constexpr size_t MAX_SSID_LENGTH = 32;
    static constexpr size_t MAX_PASSWORD_LENGTH = 64;
    static constexpr size_t JSON_CAPACITY = 200;
    
    explicit Config(const char* filename);
    
    bool hasCredentials() const { return _ssid[0] != '\0' && _password[0] != '\0'; }
    const char* getSsid() const { return _ssid; }
    const char* getPassword() const { return _password; }
    
    bool load();
    bool save();
    void setCredentials(const char* ssid, const char* password);

private:
    bool openFile(File& file, const char* mode) const;
    void logError(const char* message) const;

    char _ssid[MAX_SSID_LENGTH + 1];
    char _password[MAX_PASSWORD_LENGTH + 1];
    const char* _filename;
};