#include "../include/discord.hpp"

Discord::Discord(Config& config) : _config(config) {
}

String Discord::getWebhookUrl() const {
    return "https://discordapp.com/api/webhooks/" + 
           String(_config.getChannelId()) + "/" + 
           String(_config.getToken());
}

bool Discord::isConfigValid() const {
    const char* channelId = _config.getChannelId();
    const char* token = _config.getToken();
    return channelId[0] != '\0' && token[0] != '\0';
}

bool Discord::sendMessage(const String& message) {
    if (!isConfigValid()) {
        Serial.println("Discord config missing - message not sent");
        return false;
    }

    WiFiClientSecure *client = new WiFiClientSecure;

    if (client)
    {
        client->setInsecure();
        HTTPClient https;

        if (https.begin(*client, getWebhookUrl())) {
            https.addHeader("Content-Type", "application/json");
        
            String payload = "{\"content\":\"" + message + "\"}";
            int httpCode = https.POST(payload);  
            https.end();

            delete client;

            bool success = httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_NO_CONTENT;

            if (success) {
                Serial.println("Message sent to Discord successfully");
            } else {
                Serial.println("Failed to send message to Discord");
            }

            return success;
        }
    }

    Serial.println("Failed to send message to Discord");

    return false;
}

bool Discord::sendTextFile(const String& filename, const String& content) {
    if (!isConfigValid()) {
        Serial.println("Discord config missing - file not sent");
        return false;
    }

    WiFiClientSecure *client = new WiFiClientSecure;

    if (client)
    {
        client->setInsecure();
        HTTPClient https;

        String boundary = "------------------------" + String(millis());
        String mimeType = "text/plain";
        String body = "--" + boundary + "\r\n";
        body += "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n";
        body += "Content-Type: " + mimeType + "\r\n\r\n";
        body += content + "\r\n";
        body += "--" + boundary + "--\r\n";

        if (https.begin(*client, getWebhookUrl())) {
            https.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
        
            int httpCode = https.POST(body);    
            https.end();

            delete client;

            bool success = httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_NO_CONTENT;

            if (success) {
                Serial.println("File sent to Discord successfully");
            } else {
                Serial.println("Failed to send file to Discord");
            }

            return success;
        }
    }

    Serial.println("Failed to send file to Discord");

    return false;
}