#pragma once

#include <cstddef>
#include <vector>

using Byte  = unsigned char;     /**< To simplify expressions use the intuitive definition. */
using Bytes = std::vector<Byte>; /**< A vector of type Byte are Bytes. */

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
  int direction;       /**< It's value can be 0 or 1 where 0 stands for encrypt, 1 for decrypt. */
  const size_t length; /**< stores the number of rotors used in the key */
  const std::vector<char>
    rotorNames; /**< rotorNames It's a string that contains the names of the rotors. The i-th rotor
                   is represented by the i-th position in the string. Hence the names of the
                   rotors can only differ in one char */
  Byte* rotorShifts; /**< It's an array of bytes that stores current position of the wheels. This
                        will be changed by the rotate function after each encrypted Byte */
  const size_t fileShift; /**< the bytes will be shifted (mod filesize) by fileShift */
};
