#include <Arduino.h>

#include "config.hpp"
#include "logger.hpp"
#include "mqtt.hpp"
#include "ota.hpp"
#include "state.hpp"
#include "wifi.hpp"

elapsedMillis debugTimer;

void setup() {
    logger::setup();
    logger::debugln(F("\nmain: started"));

    config::setup();
    wifi::setup();
    mqtt::setup();
    state::setup();
    ota::setup();

    logger::debugln(F("main: setup is over"));
}

void loop() {
    wifi::handle();
    mqtt::handle();
    ota::handle();

    if (debugTimer >= 1000) {
        debugTimer = 0; // reset timer
        Serial.print(state::currentBrightness);
        Serial.print(" ");
        Serial.print(state::currentBrightnessPercent);
        Serial.print(" ");
        Serial.print(state::currentColorTemperature);
        Serial.print(" ");
        Serial.print(state::currentWWCTPercent);
        Serial.print(" ");
        Serial.print(state::currentCWCTPercent);
        Serial.print(" ");
        Serial.print(state::debugWWPWM);
        Serial.print(" ");
        Serial.print(state::debugCWPWM);
        Serial.println();
    }
}
