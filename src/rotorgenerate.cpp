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
#include "rotorgenerate.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "chacha.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "errors.hpp"
#include "measurement.hpp"
#include "types.hpp"

static Byte* order_256() {
  Byte* order = (Byte*) malloc(256);
  for (size_t i = 0; i < 256; ++i) {
    order[i] = i;
  }
  return order;
}

static Byte* permutaion(Byte* array, const unsigned int givenSeed) {
  // set up the chacha rng
  uint32_t seed[12];
  expandSeed(&seed[0], &givenSeed, sizeof(givenSeed));
  ChaCha rng;
  rng.init(seed);

  size_t a;
  for (size_t i = 0; i < 256; ++i) {
    a = rng.get() % (256 - i);
    std::swap(array[a], array[255 - i]);
  }
  return array;
}

static Byte* invert(const Byte* perm) {
  Byte* inv_perm = (Byte*) malloc(256);
  for (size_t i = 0; i < 256; ++i) {
    inv_perm[perm[i]] = i;
  }
  return inv_perm;
}

void generateRotor(const char* rotorNames, const unsigned int givenSeed) {
  Byte* perm     = permutaion(order_256(), givenSeed);
  Byte* inv_perm = invert(perm);
  std::string str_rotorNames(rotorNames);
  std::filesystem::create_directory(STD_ROT_DIR);
  for (size_t i = 0; i < str_rotorNames.length(); ++i) {
    std::string name = STD_ROT_DIR + "rotor_" + str_rotorNames[i];
    FILE* myfile     = fopen(name.c_str(), "wb");
    if (!myfile) {
      throw CannotCreateFile("generateRotor", name);
    }
    fwrite(perm, 1, 256, myfile);
    fclose(myfile);
    name += "_reverse";
    myfile = fopen(name.c_str(), "wb");
    if (!myfile) {
      throw CannotCreateFile("generateRotor", name);
    }
    fwrite(inv_perm, 1, 256, myfile);
    fclose(myfile);
  }
  free(perm);
  free(inv_perm);
  std::cout << timestamp(current_duration()) << "Rotors with the following names have been generated: <"
            << str_rotorNames << "> Used seed: " << givenSeed << "\n";
}
