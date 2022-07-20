/*
 * Turinga is a simple symmetric encryption scheme based on ideas from enigma.
 * Copyright (C) 2022 Mathemalsky, MilchRatchet
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

/*! \file chacha.hpp */

#include <cstdint>
#include <cstdlib>

#include <csprng.hpp>

/*!
 * class ChaCha
 * \brief implemnts the csprng ChaCha
 * \details compare implementation to: BERNSTEIN, Daniel J., et al. ChaCha, a variant of Salsa20. In: Workshop record of
 * SASC. 2008. S. 3-5.
 */
class ChaCha {
private:
  uint32_t pState[16];                     /**< compile time fixed value according to Bernsteins algorithm **/
  const unsigned int DOUBLE_ROUNDS   = 10; /**< constant for number of rounds **/
  const uint32_t expand_32_byte_k[4] = {
    0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}; /**< nothing in the sleeve number **/

public:
  ChaCha() = default;

  /*!
   * \brief init sets the initial state
   * \details The first 4 * 4 bytes are set to the nothing in the sleeve constant defined by "expand 32-byte k".
   * The following 12 * 4 bytes are set to the given seed.
   * \param seed has to be 48 byte to initialize the state of the chacha algorithm
   */
  void init(const uint32_t* seed);

  /*!
   * \brief get computes the 64 bytes by computing 2 * DOUBLE_ROUNDS rounds and ticks the counter once
   * \return in contrary to normal use of chacha only 1 32-bit integer is returned
   */
  uint32_t get();

  ~ChaCha() = default;
};

/*!
 * \brief expandSeed
 * \param seed pointer of 48 byte length were the expanded seed is written to
 * \param src given seed will be repeated until it's length is 48 byte
 */
void expandSeed(uint32_t* seed, const uint64_t src);

/*!
 * \brief generateSeed invokes the csprng library in order to get a seed
 * \return a 64-bit integer as seed
 */
inline unsigned long generateSeed() {
  duthomhas::csprng random;
  return random();
}
