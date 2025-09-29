#pragma once

#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include "config.hpp"
#include "fake_card.hpp"
#include "flipper_nfc.hpp"
#include "globals.hpp"
#include "pn15693.hpp"
#include "discord.hpp"

class WebServer {
public:
    WebServer(Config& config, PN15693& pn15693, Discord& discord);
    void begin();
    AsyncEventSource& getEvents() { return _events; }
    void processDiscordUpload();

private:
    void setupRoutes();
    void setupStaticFiles();
    void setupApiEndpoints();
    void setupEventSource();
    void handleSettingsGet(AsyncWebServerRequest* request);
    void handleSettingsPost(AsyncWebServerRequest* request, JsonVariant& json);
    void handleReadCard(AsyncWebServerRequest* request);
    void handleNfcDiscord(AsyncWebServerRequest* request, JsonVariant& json);
    void handleDeleteCard(AsyncWebServerRequest* request, JsonVariant& json);
    void handleDeleteAllCards(AsyncWebServerRequest* request);
    void handleRestart(AsyncWebServerRequest* request);

    struct DiscordTask {
        String uid;
        String data;
        bool pending;
    };
    DiscordTask _discordTask;

    void scheduleDiscordUpload(const String& uid, const String& data);

    AsyncWebServer _server;
    AsyncEventSource _events;
    Config& _config;
    PN15693& _pn15693;
    Discord& _discord;

    static constexpr const char* SETTINGS_PATH = "/settings";
    static constexpr const char* READ_PATH = "/read";
    static constexpr const char* NFC_DISCORD_PATH = "/nfcdiscord";
    static constexpr const char* DELETE_PATH = "/delete";
    static constexpr const char* DELETE_ALL_PATH = "/deleteall";
    static constexpr const char* RESTART_PATH = "/restart";
};