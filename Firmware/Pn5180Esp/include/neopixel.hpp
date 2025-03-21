#pragma once

#ifndef NEOPIXEL_HPP
#define NEOPIXEL_HPP

#include <Adafruit_NeoPixel.h>

class NeoPixel {
public:
    NeoPixel(uint8_t pin, uint8_t numPixels = 1);
    void begin();
    void feedback(int r, int g, int b, int delayMs);

private:
    Adafruit_NeoPixel _pixels;
};

#endif // NEOPIXEL_HPP