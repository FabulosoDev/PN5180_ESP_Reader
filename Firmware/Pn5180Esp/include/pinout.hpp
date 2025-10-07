#pragma once

#if defined(BOARD_D1_MINI)
    #define PIN_PN5180_NSS  4
    #define PIN_PN5180_BUSY 16
    #define PIN_PN5180_RST  5
    #define PIN_WS2812B    D8

#elif defined(BOARD_D1_MINI_ESP32)
    #define PIN_PN5180_NSS  21
    #define PIN_PN5180_BUSY 26
    #define PIN_PN5180_RST  22
    #define PIN_WS2812B     15

#elif defined(BOARD_ESP32C3_SUPERMINI)
    #define PIN_PN5180_NSS  0
    #define PIN_PN5180_BUSY 1
    #define PIN_PN5180_RST  10
    #define PIN_WS2812B     3

#else
    #error Please define your pinout here!
#endif