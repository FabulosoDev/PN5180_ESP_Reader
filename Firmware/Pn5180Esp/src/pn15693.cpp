#include "../include/debug.hpp"
#include "../include/pn15693.hpp"
#include "../include/passwords.hpp"

PN15693::PN15693(uint8_t nss, uint8_t busy, uint8_t rst) 
    : _nfc15693(nss, busy, rst)
    , _isInitialized(false) {
}

bool PN15693::init() {
    uint8_t productVersion[2] = {255, 255};
    uint8_t firmwareVersion[2] = {255, 255};
    uint8_t eepromVersion[2] = {255, 255};
    
    _nfc15693.begin();

    Serial.println(F("----------------------------------"));
    Serial.println(F("PN5180 Hard-Reset..."));
    _nfc15693.reset();

    Serial.println(F("----------------------------------"));
    Serial.println(F("Reading product version..."));
    _nfc15693.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
    Serial.print(F("Product version="));
    Serial.print(productVersion[1]);
    Serial.print(".");
    Serial.println(productVersion[0]);

    if (0xff == productVersion[1]) // if product version 255, the initialization failed
    {
        Serial.println(F("PN5180 initialization failed"));
        Serial.flush();
        _isInitialized = false;
        return false;
    }
    
    Serial.println(F("----------------------------------"));
    Serial.println(F("Reading firmware version..."));
    _nfc15693.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
    Serial.print(F("Firmware version="));
    Serial.print(firmwareVersion[1]);
    Serial.print(".");
    Serial.println(firmwareVersion[0]);

    Serial.println(F("----------------------------------"));
    Serial.println(F("Reading EEPROM version..."));
    _nfc15693.readEEprom(EEPROM_VERSION, eepromVersion, sizeof(eepromVersion));
    Serial.print(F("EEPROM version="));
    Serial.print(eepromVersion[1]);
    Serial.print(".");
    Serial.println(eepromVersion[0]);

    Serial.println(F("----------------------------------"));
    Serial.println(F("Enable RF field..."));
    _nfc15693.setupRF();

    _isInitialized = true;
    return true;
}

//void PN15693::detectCardLoop() {
//    Serial.println(F("----------------------------------"));
//    Serial.print(F("Waiting for card"));
//    
//    uint8_t dotCount = 0;
//    while (true) {
//        uint32_t irqStatus = _nfc15693.getIRQStatus();
//        DEBUG_PRINT(F("IRQ-Status 0x"));
//        DEBUG_PRINTHEX(irqStatus);
//        DEBUG_PRINT(": [ ");
//        if (irqStatus & (1<< 0)) DEBUG_PRINT(F("RQ "));
//        if (irqStatus & (1<< 1)) DEBUG_PRINT(F("TX "));
//        if (irqStatus & (1<< 2)) DEBUG_PRINT(F("IDLE "));
//        if (irqStatus & (1<< 3)) DEBUG_PRINT(F("MODE_DETECTED "));
//        if (irqStatus & (1<< 4)) DEBUG_PRINT(F("CARD_ACTIVATED "));
//        if (irqStatus & (1<< 5)) DEBUG_PRINT(F("STATE_CHANGE "));
//        if (irqStatus & (1<< 6)) DEBUG_PRINT(F("RFOFF_DET "));
//        if (irqStatus & (1<< 7)) DEBUG_PRINT(F("RFON_DET "));
//        if (irqStatus & (1<< 8)) DEBUG_PRINT(F("TX_RFOFF "));
//        if (irqStatus & (1<< 9)) DEBUG_PRINT(F("TX_RFON "));
//        if (irqStatus & (1<<10)) DEBUG_PRINT(F("RF_ACTIVE_ERROR "));
//        if (irqStatus & (1<<11)) DEBUG_PRINT(F("TIMER0 "));
//        if (irqStatus & (1<<12)) DEBUG_PRINT(F("TIMER1 "));
//        if (irqStatus & (1<<13)) DEBUG_PRINT(F("TIMER2 "));
//        if (irqStatus & (1<<14)) DEBUG_PRINT(F("RX_SOF_DET "));
//        if (irqStatus & (1<<15)) DEBUG_PRINT(F("RX_SC_DET "));
//        if (irqStatus & (1<<16)) DEBUG_PRINT(F("TEMPSENS_ERROR "));
//        if (irqStatus & (1<<17)) DEBUG_PRINT(F("GENERAL_ERROR "));
//        if (irqStatus & (1<<18)) DEBUG_PRINT(F("HV_ERROR "));
//        if (irqStatus & (1<<19)) DEBUG_PRINT(F("LPCD "));
//        DEBUG_PRINTLN("]");
//
//        if (0 != (RX_SOF_DET_IRQ_STAT & irqStatus)) {
//            Serial.println(F("\n*** Card detected!"));
//            return;
//        }
//        Serial.print(F("."));
//        if (++dotCount >= 10) {
//            Serial.print(F("\nWaiting for card"));
//            dotCount = 0;
//        }
//
//        _nfc15693.reset();
//        _nfc15693.setupRF();
//        
//        delay(DETECT_CARD_INTERVAL_MS);
//    }
//}

bool PN15693::unlockCard() {
    Serial.println(F("----------------------------------"));

    // code for unlocking an ICODE SLIX2 protected tag
    for (uint8_t (&password)[4]: passwords) {
        _nfc15693.reset();
        _nfc15693.setupRF();

        ISO15693ErrorCode errorCode = _nfc15693.disablePrivacyMode(password);
        if (ISO15693_EC_OK == errorCode) {
            Serial.println(F("Unlock successful"));
            return true;
        }
    }
    Serial.println(F("Unlock failed"));
    return false;
}

bool PN15693::readUid() {
    Serial.println(F("----------------------------------"));

    // try to read ISO15693 inventory
    ISO15693ErrorCode errorCode = _nfc15693.getInventory(_uid);
    if (ISO15693_EC_OK == errorCode) {
#ifdef DEBUG
        String response = "";
        for (int i = 0; i < 8; i++) {
            response = response + (_uid[7 - i] < 0x10 ? "0" : "");
            response = response + String(_uid[7 - i], HEX);
        }
        response.toUpperCase();
        Serial.print(F("UID: "));
        Serial.println(response);
#endif
        return true;
    }

    memset(_uid, 0, UID_SIZE);

    Serial.print(F("Error in getInventory: "));
    Serial.println(_nfc15693.strerror(errorCode));
    return false;
}

bool PN15693::readCard() {
    //detectCardLoop();

    //if (!unlockCard()) return false;

    memset(_dataBuffer, 0, DATA_SIZE);

    if (!readUid()) return false;

    Serial.println(F("----------------------------------"));

    uint8_t numBlocks, blockSize;
    ISO15693ErrorCode errorCode = _nfc15693.getSystemInfo(_uid, &blockSize, &numBlocks);
    if (ISO15693_EC_OK != errorCode) {
        Serial.print(F("Error in getSystemInfo: "));
        Serial.println(_nfc15693.strerror(errorCode));
        return false;
    }

    DEBUG_PRINT(F("System Info retrieved: blockSize="));
    DEBUG_PRINT(blockSize);
    DEBUG_PRINT(F(", numBlocks="));
    DEBUG_PRINTLN(numBlocks);

    if (blockSize != BLOCK_SIZE) {
        Serial.print(F("Error: Block size is "));
        Serial.println(blockSize);
        return false;
    }

    if (blockSize != BLOCK_SIZE) {
        Serial.print(F("Error: Block size is "));
        Serial.println(blockSize);
        return false;
    }

    uint8_t blockData[blockSize];
    for (int no = 0; no < numBlocks; no++) {
        ISO15693ErrorCode errorCode = _nfc15693.readSingleBlock(_uid, no, blockData, blockSize);
        if (ISO15693_EC_OK != errorCode) {
            memset(_dataBuffer, 0, DATA_SIZE);

            Serial.print(F("Error in readSingleBlock #"));
            Serial.print(no);
            Serial.print(": ");
            Serial.println(_nfc15693.strerror(errorCode));
            return false;
        }

        DEBUG_PRINT(F("Read block #"));
        DEBUG_PRINT(no);
        DEBUG_PRINT(": ");

        for (int i = 0; i < blockSize; i++) {
            if ((no * blockSize + i) < DATA_SIZE) {
                _dataBuffer[no * blockSize + i] = blockData[i];
            }
            if (blockData[i] < 16) DEBUG_PRINT("0");
            DEBUG_PRINTHEX(blockData[i]);
        }
        DEBUG_PRINTLN();
    }
    return true;
}