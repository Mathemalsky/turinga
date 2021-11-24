/*
 * Turinga is a simple symmetric encryption scheme based on ideas from enigma.
 * Copyright (C) 2021  Mathemalsky, MilchRatchet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
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
