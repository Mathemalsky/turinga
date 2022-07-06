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

#include "chacha.hpp"

#include <cmath>
#include <cstring>

// implementation related to:
// https://stackoverflow.com/questions/776508/best-practices-for-circular-shift-rotate-operations-in-c
static inline uint32_t rotleft(const uint32_t integer, unsigned int count) {
  const unsigned int cutof = 31;
  count &= cutof;  // shifts larger than 31 bits are not needed in a 32 bit integer
  return (integer << count | integer >> ((-count) & cutof));
}

static inline void quaterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
  a += b;
  d ^= a;
  d = rotleft(d, 16);

  c += d;
  b ^= c;
  b = rotleft(b, 12);

  a += b;
  d ^= a;
  d = rotleft(d, 8);

  c += d;
  b ^= c;
  b = rotleft(b, 7);
}

void ChaCha::init(const uint32_t* seed) {
  std::memcpy(&pState[0], &expand_32_byte_k, 16);  // copy the constant in the key
  std::memcpy(&pState[4], seed, 48);               // 48 = 12 * 4 Byte
}

uint32_t ChaCha::get() {
  uint32_t workingState[16];
  std::memcpy(&workingState[0], &pState[0], 64);

  for (unsigned int i = 0; i < DOUBLE_ROUNDS; ++i) {
    // uneven rounds
    quaterRound(workingState[0], workingState[4], workingState[8], workingState[12]);
    quaterRound(workingState[1], workingState[5], workingState[9], workingState[13]);
    quaterRound(workingState[2], workingState[6], workingState[10], workingState[14]);
    quaterRound(workingState[3], workingState[7], workingState[11], workingState[15]);

    // even rounds
    quaterRound(workingState[0], workingState[5], workingState[10], workingState[15]);
    quaterRound(workingState[1], workingState[6], workingState[11], workingState[12]);
    quaterRound(workingState[2], workingState[7], workingState[8], workingState[13]);
    quaterRound(workingState[3], workingState[4], workingState[9], workingState[14]);
  }

  // add original state to current
  for (unsigned int i = 0; i < 16; ++i) {
    workingState[i] += pState[i];
  }

  // increase counter by one
  ++pState[12];

  // return, theoreticly all 64 byte
  return workingState[0];
}

void expandSeed(uint32_t* seed, const uint32_t* src, unsigned int length) {
  for (unsigned int i = 0; i < 12; i += length) {
    std::memcpy(seed + i, src, 4 * std::min(length, 12 - i));
  }
}
