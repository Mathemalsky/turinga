#pragma once

/*! \file constants.hpp */

#include <cstddef>
#include <string>

/** Limit key length a priori to dertermine space allocation at compile time. */
const size_t MAX_KEYLENGTH        = 32;
const std::string PROJECTNAME     = "turinga21";
const std::string STD_KEY         = "stdkey.key";
const std::string STD_KEY_INV     = "stdkeyinv.key";
const std::string STD_KEY_DIR     = "keys/";
const unsigned int STD_KEY_LENGTH = 10;
const std::string STD_ROT_DIR     = "rotors/";
const std::string VALID_ROT_NAMES = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
