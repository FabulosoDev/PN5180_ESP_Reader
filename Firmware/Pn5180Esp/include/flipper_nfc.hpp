#pragma once

#ifndef FLIPPER_NFC_HPP
#define FLIPPER_NFC_HPP

#include <Arduino.h>
#include "pn15693.hpp"

class FlipperNfc {
private:
    const uint8_t* uid;
    const uint8_t* data;
    String bytesToHexString(const uint8_t* bytes, size_t length, bool reverse = false) const;

public:
    FlipperNfc(const PN15693& card);
    String create() const;
    String getFilename() const;
};

#endif // FLIPPER_NFC_HPP