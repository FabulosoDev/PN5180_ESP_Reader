#pragma once

#ifndef FLIPPER_NFC_HPP
#define FLIPPER_NFC_HPP

#include <Arduino.h>
#include "pn15693.hpp"

class FlipperNfc {
public:
    FlipperNfc(const PN15693& card);
    String create() const;
    String getFilename() const;

private:
    const uint8_t* _uid;
    const uint8_t* _data;
    String bytesToHexString(const uint8_t* bytes, size_t length, bool reverse = false) const;
};

#endif // FLIPPER_NFC_HPP