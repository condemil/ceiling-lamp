#pragma once

#include <Arduino.h>
#include <FS.h>

#include "logger.hpp"

void print_file_contents(const char *name) {
    File f = SPIFFS.open(name, "r");
    if (!f) {
        logger::errorln(F("helpers: failed to open file to read"));
        return;
    }

    while (f.available()) {
        Serial.write(f.read());
    }

    f.close();

    Serial.println();
}

class elapsedMillis // modified version of https://github.com/pfeerick/elapsedMillis
{
  private:
    uint16_t ms;

  public:
    elapsedMillis(void) {
        ms = millis();
    }
    operator uint16_t() const {
        return millis() - ms;
    }
    elapsedMillis &operator=(uint16_t val) {
        ms = millis() - val;
        return *this;
    }
};
