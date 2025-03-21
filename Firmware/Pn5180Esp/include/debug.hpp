#pragma once

#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "globals.hpp"

#ifdef DEBUG
#define DEBUG_PRINT(msg) Serial.print(msg)
#define DEBUG_PRINTHEX(msg) Serial.print(msg, HEX)
#define DEBUG_PRINTLN(msg) Serial.println(msg)
#else
#define DEBUG_PRINT(msg)
#define DEBUG_PRINTHEX(msg)
#define DEBUG_PRINTLN(msg)
#endif

#endif // DEBUG_HPP