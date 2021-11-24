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
#include "turinga.hpp"

#include <cmath>
#include <cstring>
#include <iostream>
#include <thread>

#include <csprng.hpp>

#include "constants.hpp"
#include "fileinteraction.hpp"
#include "measurement.hpp"
#include "rotate.hpp"
#include "rotorgenerate.hpp"

TuringaKey generateTuringaKey(const size_t keylength, const std::string& availableRotors) {
  Byte* rotorShifts           = (Byte*) malloc(MAX_KEYLENGTH);
  const size_t numberOfRotors = availableRotors.length();
  duthomhas::csprng random;
  for (size_t i = 0; i < MAX_KEYLENGTH; ++i) {
    rotorShifts[i] = random();
  }
  std::vector<char> rotorNames(keylength);
  for (char& rotorName : rotorNames) {
    rotorName = availableRotors[random() % numberOfRotors];
  }
  const size_t fileShift = random();
  const TuringaKey key{encryption, keylength, rotorNames, rotorShifts, fileShift};

  std::cout << timestamp(current_duration()) << "A key of length " << keylength << " has been generated.\n";
  return key;
}

TuringaKey createStdKey() {
  std::string rotorNames = findRotors();
  if (rotorNames.length() == 0) {
    generateRotor(VALID_ROT_NAMES.c_str());
    rotorNames = VALID_ROT_NAMES;
  }
  return generateTuringaKey(STD_KEY_LENGTH, rotorNames);
}

// encrypts/ decrypts the files
void encrypt(Data& bytes, TuringaKey& key, const Byte* rotors) {
  // maybe other numbers of threads would be more efficient
  const size_t threadcount = std::thread::hardware_concurrency();  // number of logical processors
  std::cout << timestamp(current_duration()) << threadcount << " logical processors detected.\n";

  size_t begin = 0, end;
  end          = bytes.size / threadcount;

  std::vector<std::thread> threads;
  std::vector<Byte*> rotorShiftsAry(threadcount);

  // allocate memory for copying
  for (size_t i = 0; i < threadcount; i++) {
    rotorShiftsAry[i] = (Byte*) malloc(MAX_KEYLENGTH);
  }

  std::memcpy(rotorShiftsAry[0], key.rotorShifts, MAX_KEYLENGTH);

  for (size_t i = 0; i < threadcount - 1; ++i) {
    // copy the key because rotate in encrypt changes rotorShifts
    std::memcpy(rotorShiftsAry[i + 1], rotorShiftsAry[i], MAX_KEYLENGTH);

    // start a thread
    threads.push_back(std::thread(
      encrypt_block, std::ref(bytes),
      TuringaKey{key.direction, key.length, key.rotorNames, rotorShiftsAry[i], key.fileShift}, rotors, begin, end));
    // prepair for next thread
    for (size_t j = begin; j < end; ++j) {  // rotate to start of next thread
      rotate(rotorShiftsAry[i + 1]);
    }
    begin = end;
    end += bytes.size / threadcount;
  }

  // encrypt the rest
  encrypt_block(
    bytes, TuringaKey{key.direction, key.length, key.rotorNames, rotorShiftsAry[threadcount - 1], key.fileShift},
    rotors, begin, bytes.size);

  // collect all threads
  for (std::thread& thr : threads) {
    thr.join();
  }

  // free all memory
  for (Byte*& rotShi : rotorShiftsAry) {
    free(rotShi);
  }

  if (key.direction == 0) {
    std::cout << timestamp(current_duration()) << "File has been encrypted.\n";
  }
  else {
    std::cout << timestamp(current_duration()) << "File has been decrypted.\n";
  }
}

void encrypt_block(Data& bytes, TuringaKey key, const Byte* rotors, const size_t begin, const size_t end) {
  const size_t keylength = key.length;

  // encryption
  if (key.direction == encryption) {
    for (size_t i = begin; i < end; ++i) {
      Byte tmp = bytes.bytes[i];
      for (size_t i = 0; i < keylength; ++i) {
        tmp = rotors[256 * i + ((tmp + key.rotorShifts[i]) % 256)];
      }
      bytes.bytes[i] = tmp;
      rotate(key.rotorShifts);
    }
  }

  // decryption
  else if (key.direction == decryption) {
    for (size_t i = begin; i < end; ++i) {
      Byte tmp = bytes.bytes[i];
      for (size_t i = 0; i < keylength; ++i) {
        tmp = rotors[256 * i + tmp] - key.rotorShifts[keylength - 1 - i];
      }
      bytes.bytes[i] = tmp;
      rotate(key.rotorShifts);
    }
  }
}
