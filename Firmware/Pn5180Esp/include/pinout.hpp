#pragma once

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_NANO)
    #define PIN_PN5180_NSS  10
    #define PIN_PN5180_BUSY 9
    #define PIN_PN5180_RST  7

#elif defined(ARDUINO_ARCH_ESP32)
    #define PIN_PN5180_NSS  16
    #define PIN_PN5180_BUSY 5
    #define PIN_PN5180_RST  17

#elif defined(ARDUINO_ARCH_ESP8266)
    #define PIN_PN5180_NSS  4
    #define PIN_PN5180_BUSY 16
    #define PIN_PN5180_RST  5
    #define PIN_WS2812B    D8

#else
    #error Please define your pinout here!
#endif