#pragma once

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "config.hpp"

class WiFiManager {
public:
    static constexpr const char* DEFAULT_HOSTNAME = "PN5180 ESP";
    static constexpr const char* DEFAULT_AP_PASSWORD = "12345678";
    static constexpr uint8_t MAX_CONNECTION_RETRIES = 30;
    static constexpr uint16_t RETRY_DELAY_MS = 500;

    explicit WiFiManager(Config& config);
    void begin();

private:
    void connectToWiFi(const char* ssid, const char* password);
    void setupAccessPoint();
    void printNetworkInfo() const;

    Config& _config;
};