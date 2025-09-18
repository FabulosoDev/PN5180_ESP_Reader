#include "../include/wifi_manager.hpp"

WiFiManager::WiFiManager(Config& config) : _config(config) {}

void WiFiManager::begin() {
    Serial.println(F("----------------------------------"));
    Serial.println(F("Initializing WiFi..."));

    if (!_config.load()) {
        Serial.println(F("Failed to load config, starting AP"));
        setupAccessPoint();
        return;
    }

    if (_config.hasWifiParameters()) {
        Serial.println(F("Found stored WiFi parameters"));
        connectToWiFi(_config.getSsid(), _config.getPassword());
    } else {
        Serial.println(F("No WiFi parameters found, starting AP"));
        setupAccessPoint();
    }
}

void WiFiManager::connectToWiFi(const char* ssid, const char* password) {
    Serial.print(F("Connecting to WiFi network: "));
    Serial.println(ssid);

#if defined(ARDUINO_ARCH_ESP32)
    WiFi.setHostname(DEFAULT_HOSTNAME);
#else
    WiFi.hostname(DEFAULT_HOSTNAME);
#endif
    WiFi.begin(ssid, password);

    uint8_t retries = 0;
    Serial.print(F("Waiting for connection"));
    while (WiFi.status() != WL_CONNECTED && retries < MAX_CONNECTION_RETRIES) {
        delay(RETRY_DELAY_MS);
        retries++;
        Serial.print(F("."));
    }
    Serial.println();

    if (retries < MAX_CONNECTION_RETRIES) {
        Serial.println(F("Successfully connected to WiFi"));
        printNetworkInfo();
    } else {
        Serial.println(F("Failed to connect to WiFi, switching to AP mode"));
        setupAccessPoint();
    }
}

void WiFiManager::setupAccessPoint() {
    Serial.println(F("----------------------------------"));
    Serial.println(F("Setting up Access Point"));
    Serial.print(F("SSID: "));
    Serial.println(DEFAULT_HOSTNAME);
    Serial.print(F("Password: "));
    Serial.println(DEFAULT_AP_PASSWORD);

    WiFi.softAP(DEFAULT_HOSTNAME, DEFAULT_AP_PASSWORD);
    printNetworkInfo();
}

void WiFiManager::printNetworkInfo() const {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print(F("Local IP address: "));
        Serial.println(WiFi.localIP());
    } else {
        Serial.print(F("AP IP address: "));
        Serial.println(WiFi.softAPIP());
    }
}