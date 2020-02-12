#pragma once

#include <FS.h>
#include <ArduinoOTA.h>

#include "config.hpp"
#include "logger.hpp"

namespace ota {
    void setup() {
        ArduinoOTA.setPort(8266); // port defaults to 8266
        ArduinoOTA.setHostname(config::HOSTNAME); // hostname defaults to esp8266-[ChipID]
        ArduinoOTA.setPassword(config::OTA_TOKEN);

        ArduinoOTA.onStart([]() {
            if (ArduinoOTA.getCommand() == U_FLASH) {
                logger::debug(F("Start updating sketch"));
            } else {
                logger::debug(F("Updating filesystem is unsupported"));
            }
        });

        ArduinoOTA.onEnd([]() {
            logger::debug(F("\nEnd"));
        });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            logger::debugf("Progress: %u%%\r", (progress / (total / 100)));
        });

        ArduinoOTA.onError([](ota_error_t error) {
            logger::debugf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) logger::debug(F("Auth Failed"));
            else if (error == OTA_BEGIN_ERROR) logger::debug(F("Begin Failed"));
            else if (error == OTA_CONNECT_ERROR) logger::debug(F("Connect Failed"));
            else if (error == OTA_RECEIVE_ERROR) logger::debug(F("Receive Failed"));
            else if (error == OTA_END_ERROR) logger::debug(F("End Failed"));
        });

        ArduinoOTA.begin();
    }

    void handle() {
        ArduinoOTA.handle();
    }
}
