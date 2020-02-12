#pragma once

#include <ArduinoJson.h>
#include <PubSubClient.h>

#include "config.hpp"
#include "logger.hpp"
#include "state.hpp"
#include "wifi.hpp"

namespace mqtt {
    char MQTT_TOPIC_STATE[15];
    char MQTT_TOPIC_COMMAND[20];

    PubSubClient client(wifi::espClient);
    elapsedMillis reconnectTimeElapsed;
    const unsigned int RECONNECT_DELAY = 5000;

    void _publish() {
        StaticJsonDocument<128> doc;

        doc["state"] = state::currentPower ? "ON" : "OFF";
        doc["brightness"] = state::currentBrightness;
        doc["color_temp"] = state::currentColorTemperature;

        char buffer[128];
        serializeJson(doc, buffer);

        client.publish(MQTT_TOPIC_STATE, buffer, true);
    }

    void _callback(char *topic, byte *payload, unsigned int length) {
        if (length >= MQTT_MAX_PACKET_SIZE) {
            logger::error(F("mqtt: payload is too long"));
            return;
        }

        logger::debugf("mqtt: message arrived [%s]\n", topic);

        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
            logger::error(F("mqtt: failed to deserialize payload"));
            return;
        }

        if (doc["command"].is<char*>()) {
            if (doc["command"] == "reset") {
                config::truncate();
                ESP.restart();
            }
            if (doc["command"] == "restart") {
                ESP.restart();
            }
        }

        if (doc["brightness"].is<int>()) {
            state::setBrightness(doc["brightness"]);
        }

        if (doc["color_temp"].is<int>()) {
            state::setColorTemperature(doc["color_temp"]);
        }

        if (doc["state"].is<char*>()) {
            if (doc["state"] == "ON") {
                state::setPower(true);
            } else if (doc["state"] == "OFF") {
                state::setPower(false);
            }
        }

        _publish();
    }

    void _reconnect() {
        logger::debug(F("mqtt: attempting to connect"));

        if (client.connect(config::HOSTNAME, config::conf.mqtt_login, config::conf.mqtt_pass)) {
            logger::debug(F("mqtt: connected"));
            client.subscribe(MQTT_TOPIC_COMMAND);
            logger::debugf("mqtt: subscribed to %s\n", MQTT_TOPIC_COMMAND);
        } else {
            logger::debugf("mqtt: connect failed, rc=%d try again in %u seconds\n", client.state(), RECONNECT_DELAY / 1000);
        }
    }

    void setup() {
        sprintf(MQTT_TOPIC_STATE, config::MQTT_TOPIC_STATE_FORMAT, ESP.getChipId());
        sprintf(MQTT_TOPIC_COMMAND, config::MQTT_TOPIC_COMMAND_FORMAT, ESP.getChipId());
        client.setServer(config::conf.mqtt_host, config::conf.mqtt_port);
        client.setCallback(_callback);
    }

    void handle() {
        if (client.connected()) {
            client.loop();
            return;
        }

        if (reconnectTimeElapsed >= RECONNECT_DELAY) {
            reconnectTimeElapsed = 0; // reset timer
            _reconnect();
        }
    }
}