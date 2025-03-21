#pragma once

#ifndef CARD_STORAGE_HPP
#define CARD_STORAGE_HPP

#include <ArduinoJson.h>
#include "pn15693.hpp"

enum class SaveCardResult {
    Success,
    Duplicate,
    Error
};

class CardStorage {
public:
    explicit CardStorage(const PN15693& pn15693);
    SaveCardResult saveCard() const;
    
private:
    static const char* CARDS_FILE;
    const PN15693& _pn15693;
};

#endif // CARD_STORAGE_HPP