#pragma once

#ifndef DISCORD_HPP
#define DISCORD_HPP

#if defined(ARDUINO_ARCH_ESP32)
    #include <HTTPClient.h>
#elif defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266HTTPClient.h>
#endif

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "config.hpp"

class Discord {
public:
    Discord(Config& config);
    bool sendTextFile(const String& filename, const String& content);

private:
    Config& _config;
    String getWebhookUrl() const;
    bool isConfigValid() const;
};

#endif // DISCORD_HPP