#pragma once

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include "config.hpp"
#include "logger.hpp"

namespace wifi {
WiFiClient espClient;
WiFiManagerParameter custom_mqtt_host("mqtt_host", "MQTT Server", "", 40);
WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", "1883", 8);
WiFiManagerParameter custom_mqtt_login("mqtt_login", "MQTT Login", "", 40);
WiFiManagerParameter custom_mqtt_pass("mqtt_pass", "MQTT Password", "", 40, "type='password'");

elapsedMillis reconnectTimeElapsed;
const unsigned int RECONNECT_DELAY = 5000;

void _saveConfigCallback () {
    logger::debug(F("wifi: connection established, set network mode to provisioned"));
    config::conf.provisioned = true;
    strcpy(config::conf.mqtt_host, custom_mqtt_host.getValue());
    config::conf.mqtt_port = atol(custom_mqtt_port.getValue());
    strcpy(config::conf.mqtt_login, custom_mqtt_login.getValue());
    strcpy(config::conf.mqtt_pass, custom_mqtt_pass.getValue());
    config::persist();
}

void _connect() {
    WiFi.begin();

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        logger::error(F("wifi: connection failed"));
    }

    logger::debugf("wifi: connected, ip address: %s\n", WiFi.localIP().toString().c_str());
}

void setup() {
    if (!config::conf.provisioned) {
        logger::debug(F("wifi: creating AP for wifi provisioning, connect to http://192.168.4.1"));
        WiFiManager wifiManager;

        wifiManager.setDebugOutput(false);
        wifiManager.setSaveConfigCallback(_saveConfigCallback);

        wifiManager.addParameter(&custom_mqtt_host);
        wifiManager.addParameter(&custom_mqtt_port);
        wifiManager.addParameter(&custom_mqtt_login);
        wifiManager.addParameter(&custom_mqtt_pass);

        wifiManager.startConfigPortal(config::HOSTNAME);
        return;
    }

    WiFi.mode(WIFI_STA);
    _connect();
}

void handle() {
    if (WiFi.status() == WL_CONNECTED) return;

    if (reconnectTimeElapsed >= RECONNECT_DELAY) {
        reconnectTimeElapsed = 0; // reset timer
        logger::error(F("wifi: connection lost, reconnecting"));
        _connect();
    }
}

} // namespace wifi
