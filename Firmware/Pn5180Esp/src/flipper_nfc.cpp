#include "../include/flipper_nfc.hpp"

FlipperNfc::FlipperNfc(const PN15693& pn15693)
    : _uid(pn15693.getUid()), _data(pn15693.getData()) {
}

String FlipperNfc::bytesToHexString(const uint8_t* bytes, size_t length, bool reverse) const {
    String result;

    if (reverse) {
        for (int i = length - 1; i >= 0; i--) {
            if (i < length - 1) result += " ";
            char hex[3];
            sprintf(hex, "%02X", bytes[i]);
            result += hex;
        }
    } else {
        for (size_t i = 0; i < length; i++) {
            if (i > 0) result += " ";
            char hex[3];
            sprintf(hex, "%02X", bytes[i]);
            result += hex;
        }
    }

    return result;
}

String FlipperNfc::create() const {
    String formattedUid = bytesToHexString(_uid, PN15693::UID_SIZE, true);
    String formattedData = bytesToHexString(_data, PN15693::DATA_SIZE);

    return String(F("Filetype: Flipper NFC device\n"
        "Version: 4\n"
        "# Device type can be ISO14443-3A, ISO14443-3B, ISO14443-4A, ISO14443-4B, ISO15693-3, FeliCa, NTAG/Ultralight, Mifare Classic, Mifare DESFire, SLIX, ST25TB\n"
        "Device type: SLIX\n"
        "# UID is common for all formats\n"
        "UID: ")) + formattedUid + F("\n"
        "# ISO15693-3 specific data\n"
        "# Data Storage Format Identifier\n"
        "DSFID: 00\n"
        "# Application Family Identifier\n"
        "AFI: 00\n"
        "# IC Reference - Vendor specific meaning\n"
        "IC Reference: 03\n"
        "# Lock Bits\n"
        "Lock DSFID: false\n"
        "Lock AFI: false\n"
        "# Number of memory blocks, valid range = 1..256\n"
        "Block Count: 8\n"
        "# Size of a single memory block, valid range = 01...20 (hex)\n"
        "Block Size: 04\n"
        "Data Content: ") + formattedData + F("\n"
        "# Block Security Status: 01 = locked, 00 = not locked\n"
        "Security Status: 00 00 00 00 00 00 00 00\n"
        "# SLIX specific data\n"
        "# SLIX capabilities field affects emulation modes. Possible options: Default, AcceptAllPasswords\n"
        "Capabilities: Default\n"
        "# Passwords are optional. If a password is omitted, a default value will be used\n"
        "Password Privacy: 7F FD 6E 5B\n"
        "Password Destroy: 0F 0F 0F 0F\n"
        "Password EAS: 00 00 00 00\n"
        "Privacy Mode: false\n"
        "# SLIX Lock Bits\n"
        "Lock EAS: false");
}

String FlipperNfc::getFilename() const {
    String hexString;

    for (size_t i = 0; i < 4; i++) {
        char hex[3];
        sprintf(hex, "%02X", _uid[i]);
        hexString += hex;
    }
    return hexString;
}