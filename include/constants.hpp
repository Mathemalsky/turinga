#pragma once

/*! \file constants.hpp */

#include <cstddef>
#include <string>

/** Limit key length a priori to dertermine space allocation at compile time. */
const size_t MAX_KEYLENGTH    = 32;
const std::string PROJECTNAME = "turinga21";
const std::string STD_ROT_DIR = "Rotors/";
const std::string STD_KEY     = "key";
const std::string STD_KEY_INV = "keyinv";
const std::string STD_KEY_DIR = "keys/";
