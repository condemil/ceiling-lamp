#pragma once

#include <ArduinoOTA.h>
#include <FS.h>

#include "config.hpp"
#include "logger.hpp"

namespace ota {
void setup() {
    ArduinoOTA.setPort(8266); // port defaults to 8266
    ArduinoOTA.setHostname(config::HOSTNAME); // hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setPassword(config::OTA_TOKEN);

    ArduinoOTA.onStart([]() {
        if (ArduinoOTA.getCommand() == U_FLASH) {
            logger::debugln(F("Start updating sketch"));
        } else {
            logger::debugln(F("Updating filesystem is unsupported"));
        }
    });

    ArduinoOTA.onEnd([]() { logger::debugln(F("\nEnd")); });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        logger::debugf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        logger::debugf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            logger::debugln(F("Auth Failed"));
        else if (error == OTA_BEGIN_ERROR)
            logger::debugln(F("Begin Failed"));
        else if (error == OTA_CONNECT_ERROR)
            logger::debugln(F("Connect Failed"));
        else if (error == OTA_RECEIVE_ERROR)
            logger::debugln(F("Receive Failed"));
        else if (error == OTA_END_ERROR)
            logger::debugln(F("End Failed"));
    });

    ArduinoOTA.begin();
}

void handle() {
    ArduinoOTA.handle();
}
} // namespace ota
