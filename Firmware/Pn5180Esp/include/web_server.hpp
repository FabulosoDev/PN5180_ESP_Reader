#pragma once

#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include "config.hpp"
#include "fake_card.hpp"
#include "globals.hpp"
#include "pn15693.hpp"

class WebServer {
public:
    WebServer(Config& config, PN15693& pn15693);
    void begin();
    AsyncEventSource& getEvents() { return _events; }

private:
    void setupRoutes();
    void setupStaticFiles();
    void setupApiEndpoints();
    void setupEventSource();
    void handleSettingsGet(AsyncWebServerRequest* request);
    void handleSettingsPost(AsyncWebServerRequest* request, JsonVariant& json);
    void handleReadCard(AsyncWebServerRequest* request);
    void handleDeleteCard(AsyncWebServerRequest* request, JsonVariant& json);

    AsyncWebServer _server;
    AsyncEventSource _events;
    Config& _config;
    PN15693& _pn15693;
    
    static constexpr const char* SETTINGS_PATH = "/settings";
    static constexpr const char* READ_PATH = "/read";
    static constexpr const char* DELETE_PATH = "/delete";
};