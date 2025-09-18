#pragma once

#if defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif

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