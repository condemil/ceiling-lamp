#pragma once

#include <ArduinoJson.h>
#include <FS.h>

#include "helpers.hpp"
#include "logger.hpp"

namespace config {
    static const uint8_t WARM_WHITE_PIN = 16;
    static const uint8_t COLD_WHITE_PIN = 14;
    static const char *MQTT_TOPIC_STATE_FORMAT = "esp8266/%06x";
    static const char *MQTT_TOPIC_COMMAND_FORMAT = "esp8266/%06x/set";
    static const char *OTA_TOKEN = "REPLACE_ME"; // TODO: move to json and set with WiFiManager
    static char HOSTNAME[15];

    struct Config {
        char mqtt_host[40];
        uint16_t mqtt_port;
        char mqtt_login[40];
        char mqtt_pass[40];
        bool provisioned;
    };

    Config conf;
    const char *filename = "/config.json";

    char mqtt_host[40];
    uint16_t mqtt_port;

    void setup() {
        sprintf(HOSTNAME, "esp8266-%06x", ESP.getChipId());
        logger::debugf("Hostname: %s\n", config::HOSTNAME);

        if (!SPIFFS.begin()) {
            logger::error(F("config: failed to mount FS"));
            return;
        }

        File file = SPIFFS.open(filename, "r");

        if (!file) {
            logger::error(F("config: failed to open file to read"));
            return;
        }

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, file);

        if (error) {
            logger::error(F("config: failed to deserialize"));
            file.close();
            return;
        }

        conf.provisioned = doc["provisioned"];
        strlcpy(conf.mqtt_host, doc["mqtt_host"], sizeof(conf.mqtt_host));
        conf.mqtt_port = doc["mqtt_port"];
        strlcpy(conf.mqtt_login, doc["mqtt_login"], sizeof(conf.mqtt_login));
        strlcpy(conf.mqtt_pass, doc["mqtt_pass"], sizeof(conf.mqtt_pass));

        file.close();
    }

    void persist() {
        File file = SPIFFS.open(filename, "w");

        if (!file) {
            logger::error(F("config: failed to open file to write"));
            return;
        }

        StaticJsonDocument<256> doc;

        // Set the values in the document
        doc["provisioned"] = conf.provisioned;
        doc["mqtt_host"] = conf.mqtt_host;
        doc["mqtt_port"] = conf.mqtt_port;
        doc["mqtt_login"] = conf.mqtt_login;
        doc["mqtt_pass"] = conf.mqtt_pass;

        // Serialize JSON to file
        if (serializeJson(doc, file) == 0) {
            logger::error(F("config: failed to write file"));
        }

        // Close the file
        file.close();
        logger::debug(F("config: persisted"));
    }

    void truncate() {
        conf.provisioned = false;
        conf.mqtt_host[0] = (char)0;
        conf.mqtt_port = 1883;
        conf.mqtt_login[0] = (char)0;
        conf.mqtt_pass[0] = (char)0;
        persist();
    }
}
