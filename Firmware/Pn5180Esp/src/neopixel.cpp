#include "../include/neopixel.hpp"

NeoPixel::NeoPixel(uint8_t pin, uint8_t numPixels) 
    : pixels(numPixels, pin, NEO_GRB + NEO_KHZ800) {
}

void NeoPixel::begin() {
    pixels.begin();
}

void NeoPixel::feedback(int r, int g, int b, int delayMs) {
    pixels.setPixelColor(0, pixels.Color(r, g, b));
    pixels.show();
    delay(delayMs);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
}