#pragma once

#ifndef DISCORD_HPP
#define DISCORD_HPP

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "config.hpp"

class Discord {
public:
    Discord(Config& config);
    bool sendMessage(const String& message);
    bool sendTextFile(const String& filename, const String& content);

private:
    Config& _config;
    String getWebhookUrl() const;
    bool isConfigValid() const;    
};

#endif // DISCORD_HPP