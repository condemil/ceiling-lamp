#include <Arduino.h>

#include "config.hpp"
#include "logger.hpp"
#include "mqtt.hpp"
#include "ota.hpp"
#include "state.hpp"
#include "wifi.hpp"

void setup() {
    logger::setup();
    logger::debugln(F("\nmain: started"));

    config::setup();
    wifi::setup();
    if (strlen(config::conf.syslog_host) != 0) {
        logger::setupSyslog(config::conf.syslog_host, config::conf.syslog_port, config::HOSTNAME, config::NAME);
    }
    mqtt::setup();
    state::setup();
    ota::setup();

    logger::debugln(F("main: setup is over"));
}

void loop() {
    wifi::handle();
    mqtt::handle();
    ota::handle();
}
