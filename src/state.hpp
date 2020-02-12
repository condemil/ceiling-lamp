#pragma once

#include <Arduino.h>
#include <math.h>
#include "config.hpp"

namespace state {
    static const uint8_t MIN_BRIGHTNESS = 0;
    static const uint8_t MAX_BRIGHTNESS = 255;
    static const uint16_t MIN_COLOR_TEMPERATURE = 153; // mired
    static const uint16_t MAX_COLOR_TEMPERATURE = 500; // mired
    static const uint16_t MIDDLE_COLOR_TEMPERATURE = (MIN_COLOR_TEMPERATURE + MAX_COLOR_TEMPERATURE) / 2;
    static const uint16_t MIN_PWM = 100;
    static const uint16_t MAX_PWM = PWMRANGE;

    static bool currentPower;
    static uint8_t currentBrightness = 255;
    static uint8_t currentBrightnessPercent = 100;
    static uint16_t currentColorTemperature = 327;
    static uint8_t currentWWCTPercent = 100;
    static uint8_t currentCWCTPercent = 100;
    static uint16_t debugWWPWM = 0;
    static uint16_t debugCWPWM = 0;

    void setup() {
        pinMode(config::WARM_WHITE_PIN, OUTPUT);
        pinMode(config::COLD_WHITE_PIN, OUTPUT);
        analogWrite(config::WARM_WHITE_PIN, 0);
        analogWrite(config::COLD_WHITE_PIN, 0);
        debugWWPWM = PWMRANGE;
        debugCWPWM = PWMRANGE;
    }

    uint8_t _toPercent(uint16_t value, uint16_t minValue, uint16_t maxValue, bool invert) {
        if (value <= minValue && !invert) return 0;
        if (value >= maxValue && !invert) return 100;

        if (value <= minValue && invert) return 100;
        if (value >= maxValue && invert) return 0;

        if (!invert) return (value - minValue) * 100 / (maxValue - minValue);
        return (maxValue - value) * 100 / float(maxValue - minValue);
    }

    uint16_t _fromPercent(uint16_t percent, uint16_t minValue, uint16_t maxValue) {
        if (percent == 0) return 0;
        return minValue + (percent * (maxValue - minValue) / 100);
    }

    void _set_pwm() {
        uint8_t warmWhitePercent = currentWWCTPercent * currentBrightnessPercent / 100;
        uint8_t coldWhitePercent = currentCWCTPercent * currentBrightnessPercent / 100;

        analogWrite(config::WARM_WHITE_PIN, _fromPercent(warmWhitePercent, MIN_PWM, MAX_PWM));
        analogWrite(config::COLD_WHITE_PIN, _fromPercent(coldWhitePercent, MIN_PWM, MAX_PWM));
        debugWWPWM = _fromPercent(warmWhitePercent, MIN_PWM, MAX_PWM);
        debugCWPWM = _fromPercent(coldWhitePercent, MIN_PWM, MAX_PWM);
    }

    void setPower(bool power) {
        if (power == currentPower) return;

        currentPower = power;

        if (power) {
            _set_pwm();
        } else {
            analogWrite(config::WARM_WHITE_PIN, 0);
            analogWrite(config::COLD_WHITE_PIN, 0);
        }
    }

    void setBrightness(uint8_t value) {
        if (value == currentBrightness) return;

        currentBrightness = min(value, (uint8_t)255);
        currentBrightnessPercent = _toPercent(currentBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS, false);
        _set_pwm();
    }

    void setColorTemperature(uint16_t value) {
        if (value == currentColorTemperature) return;

        value = min(value, (uint16_t)500);
        value = max(value, (uint16_t)153);
        currentColorTemperature = value;
        currentWWCTPercent = _toPercent(currentColorTemperature, MIDDLE_COLOR_TEMPERATURE, MAX_COLOR_TEMPERATURE, true);
        currentCWCTPercent = _toPercent(currentColorTemperature, MIN_COLOR_TEMPERATURE, MIDDLE_COLOR_TEMPERATURE, false);
        _set_pwm();
    }
}
