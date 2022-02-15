/*
 * Turinga is a simple symmetric encryption scheme based on ideas from enigma.
 * Copyright (C) 2022  Mathemalsky, MilchRatchet
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

#include <cstddef>
#include <cstdlib>

using Byte = unsigned char; /**< To simplify expressions use the intuitive definition. */

/*!
 * \brief enum direction specifies it's a key for encryption or decryption
 */
enum Direction { encryption = 0, decryption = 1 };

/*!
 * \struct Data
 * \brief The struct Data is a simple array and it's length.
 * \details It's intended to contain the complete information from a file.
 */
struct Data {
  Byte* bytes;       /**< an array of bytes (unsigned chars) */
  const size_t size; /**< the length of the array */
};

/*!
 * \struct TuringaKey
 * \brief The struct TuringaKey contains all information that determines the key for turinga.
 */
struct TuringaKey {
  Direction direction; /**< It's value can be 0 or 1 where 0 stands for encrypt, 1 for decrypt. */
  size_t length;       /**< stores the number of rotors used in the key */
  char* rotorNames;    /**< rotorNames It's a string that contains the names of the rotors.
                                      The i-th rotor is represented by the i-th position in the string.
                                      Hence the names of the rotors can only differ in one char */
  Byte* rotorShifts;   /**< It's an array of bytes that stores current position of the wheels. This
                          will be changed by the rotate function after each encrypted Byte */
  size_t fileShift;    /**< the bytes will be shifted (mod filesize) by fileShift */
};

/*!
 * \brief freeTuringaKey frees the memory allocated for the key
 * \param key specifies the key to be freed
 * \details rotorNames and rotorShifts are pointers which need to be freed in order avoid memory leaks
 */
inline void freeTuringaKey(TuringaKey key) {
  free(key.rotorNames);
  free(key.rotorShifts);
}
