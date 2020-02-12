#pragma once

#include <Arduino.h>

namespace logger {
    static Print& out = Serial;

    void setup(Print& p) {
        out = p;
    }

    void error(const __FlashStringHelper *text) {
        out.println(text);
    }

    void debug(const __FlashStringHelper *text) {
        out.println(text);
    }

    void debugf(const char *format, ...) {
        va_list arg;
        va_start(arg, format);
        char temp[64];
        char* buffer = temp;
        size_t len = vsnprintf(temp, sizeof(temp), format, arg);
        va_end(arg);
        if (len > sizeof(temp) - 1) {
            buffer = new char[len + 1];
            if (!buffer) {
                error(F("Not enough space for debugf string"));
                return;
            }
            va_start(arg, format);
            vsnprintf(buffer, len + 1, format, arg);
            va_end(arg);
        }
        len = out.write((const uint8_t*) buffer, len);
        if (buffer != temp) {
            delete[] buffer;
        }
    }
}
