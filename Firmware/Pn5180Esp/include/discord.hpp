#pragma once

#ifndef DISCORD_HPP
#define DISCORD_HPP

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

class Discord {
private:
    String channelId;
    String token;
    String getWebhookUrl() const;

public:
    Discord(const String& channelId, const String& token);
    bool sendMessage(const String& message);
    bool sendTextFile(const String& filename, const String& content);
};

#endif // DISCORD_HPP