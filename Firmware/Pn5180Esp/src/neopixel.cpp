#include "../include/neopixel.hpp"

NeoPixel::NeoPixel(uint8_t pin, uint8_t numPixels) 
    : _pixels(numPixels, pin, NEO_GRB + NEO_KHZ800) {
}

void NeoPixel::begin() {
    _pixels.begin();
}

void NeoPixel::feedback(int r, int g, int b, int delayMs) {
    _pixels.setPixelColor(0, _pixels.Color(r, g, b));
    _pixels.show();
    delay(delayMs);
    _pixels.setPixelColor(0, _pixels.Color(0, 0, 0));
    _pixels.show();
}