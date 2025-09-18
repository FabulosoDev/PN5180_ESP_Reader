#include "../include/web_server.hpp"

WebServer::WebServer(Config& config, PN15693& pn15693)
    : _server(80)
    , _events("/events")
    , _config(config)
    , _pn15693(pn15693) {
}

void WebServer::begin() {
    setupRoutes();
    setupEventSource();

#ifdef ENABLE_CORS
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Expose-Headers", "Content-Disposition");
#endif

    _server.begin();
}

void WebServer::setupRoutes() {
    setupStaticFiles();
    setupApiEndpoints();

    _server.onNotFound([](AsyncWebServerRequest* request) {
        request->send(404);
    });
}

void WebServer::setupStaticFiles() {
    _server.serveStatic("/", SPIFFS, "/")
        .setDefaultFile("history.html")
        .setCacheControl("max-age=31536000");
}

void WebServer::setupApiEndpoints() {
    _server.on(READ_PATH, HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleReadCard(request);
    });

    _server.addHandler(new AsyncCallbackJsonWebHandler(
        DELETE_PATH,
        [this](AsyncWebServerRequest* request, JsonVariant& json) {
            handleDeleteCard(request, json);
        }
    ));

    _server.on(DELETE_ALL_PATH, HTTP_DELETE, [this](AsyncWebServerRequest* request) {
        handleDeleteAllCards(request);
    });

    _server.on(SETTINGS_PATH, HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleSettingsGet(request);
    });

    _server.addHandler(new AsyncCallbackJsonWebHandler(
        SETTINGS_PATH,
        [this](AsyncWebServerRequest* request, JsonVariant& json) {
            handleSettingsPost(request, json);
        }
    ));

    _server.on(RESTART_PATH, HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleRestart(request);
    });
}

void WebServer::handleReadCard(AsyncWebServerRequest* request) {
    Serial.println(F("----------------------------------"));
    Serial.println(F("Handling card read request..."));

    #ifdef USE_FAKE_CARD
        const uint8_t* uid = FakeCard::getUid();
        const uint8_t* data = FakeCard::getData();

        Serial.println(F("Using FAKE Card data!"));
    #else
        const uint8_t* uid = _pn15693.getUid();
        const uint8_t* data = _pn15693.getData();
    #endif

    if (!data || !uid) {
        Serial.println(F("Error: Invalid card data"));
        if (!data) {
            Serial.println(F("- Data is null"));
            _events.send("Data is null", "read_error", millis());
        }
        if (!uid) {
            Serial.println(F("- UID is null"));
            _events.send("UID is null", "read_error", millis());
        }

        request->send(404);
        return;
    }

    StaticJsonDocument<500> doc;

    char uidBuffer[(PN15693::UID_SIZE * 2) + 1];
    char* ptr = uidBuffer;
    for (uint8_t i = 0; i < PN15693::UID_SIZE; i++) {
        ptr += sprintf(ptr, "%02x", uid[i]);
    }
    *ptr = '\0';
    doc["uid"] = uidBuffer;

    char dataBuffer[(PN15693::DATA_SIZE * 2) + 1];
    ptr = dataBuffer;
    for (uint8_t i = 0; i < PN15693::DATA_SIZE; i++) {
        ptr += sprintf(ptr, "%02x", data[i]);
    }
    *ptr = '\0';
    doc["data"] = dataBuffer;

    String response;
    serializeJson(doc, response);

    Serial.println(F("Card data:"));
    Serial.println(response);

    request->send(200, "application/json", response);
}

void WebServer::handleDeleteCard(AsyncWebServerRequest* request, JsonVariant& json) {
    Serial.println(F("----------------------------------"));
    Serial.println(F("Handling card delete request..."));

    if (request->method() != HTTP_POST) {
        Serial.println(F("Error: Invalid HTTP method"));
        request->send(405);
        return;
    }

    String uid = json["uid"].as<String>();
    if (uid.isEmpty()) {
        Serial.println(F("Error: Missing UID"));
        request->send(400, "application/json", "{\"error\":\"Missing uid\"}");
        return;
    }
    Serial.print(F("Deleting card with UID: "));
    Serial.println(uid);

    DynamicJsonDocument doc(4096);
    JsonArray cards;
    bool found = false;

    if (SPIFFS.exists("/cards.json")) {
        File file = SPIFFS.open("/cards.json", "r");
        if (!file) {
            Serial.println(F("Error: Failed to open file for reading"));
            request->send(500, "application/json", "{\"error\":\"Failed to open file\"}");
            return;
        }

        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            Serial.print(F("Error parsing JSON: "));
            Serial.println(error.c_str());
            request->send(500, "application/json", "{\"error\":\"Failed to parse JSON\"}");
            return;
        }

        cards = doc.as<JsonArray>();
        Serial.print(F("Found "));
        Serial.print(cards.size());
        Serial.println(F(" cards in database"));

        for (JsonArray::iterator it = cards.begin(); it != cards.end(); ++it) {
            if ((*it)["uid"] == uid) {
                cards.remove(it);
                found = true;
                Serial.println(F("Card found and removed from array"));
                break;
            }
        }

        if (!found) {
            Serial.println(F("Error: Card not found in database"));
            request->send(404, "application/json", "{\"error\":\"Card not found\"}");
            return;
        }

        file = SPIFFS.open("/cards.json", "w");
        if (!file) {
            Serial.println(F("Error: Failed to open file for writing"));
            request->send(500, "application/json", "{\"error\":\"Failed to write file\"}");
            return;
        }

        if (serializeJson(doc, file) == 0) {
            Serial.println(F("Error: Failed to write JSON data"));
            file.close();
            request->send(500, "application/json", "{\"error\":\"Failed to write JSON\"}");
            return;
        }
        file.close();

        Serial.print(F("Successfully removed card with UID: "));
        Serial.println(uid);
        request->send(200, "application/json", "{\"success\":true}");
        return;
    }

    Serial.println(F("Error: cards.json does not exist"));
    request->send(500, "application/json", "{\"error\":\"Failed to open file\"}");
}

void WebServer::handleDeleteAllCards(AsyncWebServerRequest* request) {
    Serial.println(F("----------------------------------"));
    Serial.println(F("Handling delete all request..."));

    if (request->method() != HTTP_DELETE) {
        Serial.println(F("Error: Invalid HTTP method"));
        request->send(405);
        return;
    }

    File file = SPIFFS.open("/cards.json", "w");
    if (!file) {
        Serial.println(F("Error: Failed to open file for writing"));
        request->send(500, "application/json", "{\"error\":\"Failed to write file\"}");
        return;
    }

    size_t written = file.print("[]");
    file.close();

    if (written < 2) {
        Serial.println(F("Error: Failed to write JSON data"));
        request->send(500, "application/json", "{\"error\":\"Failed to write JSON\"}");
        return;
    }

    Serial.println(F("Successfully removed all cards"));
    request->send(200, "application/json", "{\"success\":true}");
}

void WebServer::handleSettingsGet(AsyncWebServerRequest* request) {
    StaticJsonDocument<400> doc;

    if (_config.hasWifiParameters()) {
        doc["ssid"] = _config.getSsid();
        doc["password"] = _config.getPassword();
    } else {
        doc["ssid"] = "";
        doc["password"] = "";
    }

    if (_config.hasDiscordParameters()) {
        doc["channel_id"] = _config.getChannelId();
        doc["token"] = _config.getToken();
    } else {
        doc["channel_id"] = "";
        doc["token"] = "";
    }

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void WebServer::handleSettingsPost(AsyncWebServerRequest* request, JsonVariant& json) {
    JsonObject settings = json.as<JsonObject>();
    const char* ssid = settings["ssid"];
    const char* password = settings["password"];
    const char* channelId = settings["channel_id"];
    const char* token = settings["token"];

    if (!ssid || !password || strlen(ssid) == 0 || strlen(password) == 0) {
        _events.send("SSID and Password must not be empty!", "settings_warning", millis());
        request->send(400, "text/plain", "SSID and Password must not be empty!");
        return;
    }

    _config.setCredentials(ssid, password);
    _config.setDiscordConfig(channelId, token);

    if (_config.save()) {
        _events.send("Settings saved successfully.", "settings_success", millis());
        request->send(200);
    } else {
        _events.send("Failed to save settings!", "settings_error", millis());
        request->send(500, "text/plain", "Failed to save settings!");
    }
}

void WebServer::setupEventSource() {
    _events.onConnect([](AsyncEventSourceClient* client) {
        if (client->lastId()) {
            Serial.printf("Client reconnected! Last message ID: %u\n", client->lastId());
        }
        client->send("hello!", nullptr, millis(), 10000);
    });

    _server.addHandler(&_events);
}

void WebServer::handleRestart(AsyncWebServerRequest* request) {
    Serial.println(F("Handling restart request..."));

    request->onDisconnect([]() {
        ESP.restart();
    });

    _events.send("Restarting...", "restart", millis());
    request->send(200);
}