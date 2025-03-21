#pragma once

#ifndef DISCORD_HPP
#define DISCORD_HPP

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "config.hpp"

class Discord {
private:
    String channelId;
    String token;
    String getWebhookUrl() const;
    Config& config;

public:
    Discord(Config& config);
    bool sendMessage(const String& message);
    bool sendTextFile(const String& filename, const String& content);
};

#endif // DISCORD_HPP