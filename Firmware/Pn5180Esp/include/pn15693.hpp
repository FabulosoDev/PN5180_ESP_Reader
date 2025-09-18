#pragma once

#ifndef PN15693_HPP
#define PN15693_HPP

#include <Arduino.h>
#include <PN5180ISO15693.h>

class PN15693 {
public:
    static const uint8_t BLOCK_NUM = 8;
    static const uint8_t BLOCK_SIZE = 4;
    static const uint8_t UID_SIZE = 8;
    static const uint8_t DATA_SIZE = BLOCK_NUM * BLOCK_SIZE;

    PN15693(uint8_t nss, uint8_t busy, uint8_t rst);
    bool init();

    bool unlockCard();
    bool readCard();

    const uint8_t* getData() const { return _dataBuffer; }
    const uint8_t* getUid() const { return _uid; }
    bool isInitialized() const { return _isInitialized; }

private:
    //static const uint16_t DETECT_CARD_INTERVAL_MS = 500;

    //void detectCardLoop();
    bool readUid();
    bool _isInitialized;
    PN5180ISO15693 _nfc15693;
    uint8_t _dataBuffer[DATA_SIZE];
    uint8_t _uid[UID_SIZE];
};

#endif // PN15693_HPP
