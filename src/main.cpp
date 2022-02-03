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
#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>

#include "colors.hpp"
#include "errors.hpp"
#include "fileinteraction.hpp"
#include "measurement.hpp"
#include "rotorgenerate.hpp"
#include "turinga.hpp"
#include "types.hpp"

#include "testrotate.hpp"

int main(int argc, char** argv) {
  start_time();
  try {
    if (argc <= 1) {
      throw InappropriateNumberOfArguments("main", 2, argc);
    }
    // print syntax hints
    else if (std::strcmp(argv[1], "help") == 0) {
      if (argc == 2) {
        syntax();
      }
      else if (std::strcmp(argv[2], "crypt") == 0) {
        syntaxCrypt();
      }
      else if (std::strcmp(argv[2], "genKey") == 0) {
        syntaxGenerateKey();
      }
      else if (std::strcmp(argv[2], "genRot") == 0) {
        syntaxGenerateRotors();
      }
      else {
        throw InvalidArgument("main", argv[2], "after <help>");
      }
    }
    // generate a key
    else if (std::strcmp(argv[1], "genKey") == 0) {
      if (argc <= 3) {
        throw InappropriateNumberOfArguments("main", 5, argc);
      }
      else {
        std::string availableRotors;
        if (argc == 4) {
          // get the rotors
          availableRotors = findRotors();
          if (availableRotors.length() == 0) {
            generateRotor(VALID_ROT_NAMES.c_str());
            availableRotors = VALID_ROT_NAMES;
          }
        }
        else {
          // get the rotors by input
          availableRotors = argv[4];
        }
        const char* keyfile     = argv[2];
        std::string keyfilePath = std::string(keyfile);
        const size_t keylength  = atoi(argv[3]);
        TuringaKey key          = generateTuringaKey(keylength, availableRotors);
        writeTuringaKey(keyfilePath + ".key", key);
        key.direction = decryption;
        writeTuringaKey(keyfilePath + "_inv.key", key);
        free(key.rotorShifts);
        if (keylength < 8) {
          print_yellow("WARNING: ");
          std::cout << "You generated a short key, which may be insecure.\n";
        }
      }
    }
    // generate Rotors
    else if (std::strcmp(argv[1], "genRot") == 0) {
      if (argc == 2) {
        throw InappropriateNumberOfArguments("main", 3, argc);
      }
      else if (argc == 3) {
        // generate rotors using the default seed 0
        generateRotor(argv[2]);
      }
      else if (std::strcmp(argv[3], "-r") == 0) {
        // generate rotors using a random seed
        generateRotor(argv[2], time(NULL));
      }
      else {
        // generate rotors using a given seed
        generateRotor(argv[2], std::atoi(argv[3]));
      }
    }
    // encrypt or decrypt
    else if (std::strcmp(argv[1], "crypt") == 0) {
      if (argc <= 5) {
        throw InappropriateNumberOfArguments("main", 6, argc);
      }
      const char* filename     = argv[2];
      const char* keyfile      = argv[3];
      const char* rotDirectory = argv[4];
      const char* outputfile   = argv[5];
      TuringaKey key           = readTuringaKey(keyfile);
      assert((key.direction == encryption || key.direction == decryption) && "the key ins't read correctly");
      handleCrypt(filename, outputfile, rotDirectory, key);
    }
    // decrypt file
    else if (std::strcmp(argv[1], "-d") == 0) {
      const char* filename = argv[2];
      if (!testForExistence((STD_KEY_DIR + STD_KEY_INV).c_str())) {
        throw NoKey("main", STD_KEY_DIR + STD_KEY_INV);
      }
      TuringaKey key = readTuringaKey((STD_KEY_DIR + STD_KEY_INV).c_str());
      std::string outputfilename(filename);
      outputfilename = outputfilename.substr(0, outputfilename.length() - 4);
      handleCrypt(filename, outputfilename.c_str(), STD_ROT_DIR.c_str(), key);
    }
    // testing
    else if (std::strcmp(argv[1], "-t") == 0) {
      TuringaKey key;
      if (testForExistence((STD_KEY_DIR + STD_KEY).c_str())) {
        key = readTuringaKey((STD_KEY_DIR + STD_KEY).c_str());
      }
      else {
        key = createStdKey();
        writeTuringaKey((STD_KEY_DIR + STD_KEY).c_str(), key);
        key.direction = decryption;
        writeTuringaKey((STD_KEY_DIR + STD_KEY_INV).c_str(), key);
        key.direction = encryption;
      }
      testRotate(key);
    }
    else {
      if (testForExistence(argv[1])) {
        const char* filename = argv[1];
        TuringaKey key;
        if (testForExistence((STD_KEY_DIR + STD_KEY).c_str())) {
          key = readTuringaKey((STD_KEY_DIR + STD_KEY).c_str());
        }
        else {
          std::filesystem::create_directory(STD_KEY_DIR);
          key = createStdKey();
          writeTuringaKey((STD_KEY_DIR + STD_KEY).c_str(), key);
          key.direction = decryption;
          writeTuringaKey((STD_KEY_DIR + STD_KEY_INV).c_str(), key);
          key.direction = encryption;
        }
        std::string outputfilename(filename);
        outputfilename += ".tur";
        handleCrypt(filename, outputfilename.c_str(), STD_ROT_DIR.c_str(), key);
      }
      else {
        throw InvalidArgument("main", argv[1], "as first argument");
      }
    }
  } catch (TuringaError& error) {
    error.what();
  }
  std::cout << timestamp(current_duration());
  print_lightgreen("Done!\n");
  return 0;
}
