#pragma once

#ifndef FLIPPER_NFC_HPP
#define FLIPPER_NFC_HPP

#include <Arduino.h>
#include "pn15693.hpp"

class FlipperNfc {
public:
    FlipperNfc(const PN15693& card);
    FlipperNfc(const String& uidStr, const String& dataStr);
    String create() const;
    String getFilename() const;

private:
    uint8_t _uid[PN15693::UID_SIZE];
    uint8_t _data[PN15693::DATA_SIZE];
    String bytesToHexString(const uint8_t* bytes, size_t length, bool reverse = false) const;
    void hexStringToBytes(const String& hex, uint8_t* bytes, size_t size) const;
};

#endif // FLIPPER_NFC_HPP