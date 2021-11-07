#include <cassert>
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <windows.h>  // maybe no longer necesarry
#endif

#include "colors.hpp"
#include "errors.hpp"
#include "fileinteraction.hpp"
#include "measurement.hpp"
#include "rotorgenerate.hpp"
#include "turinga.hpp"
#include "types.hpp"

void handleCrypt(const char* filename, const char* outputfilename, const char* rotDirectory, TuringaKey key) {
  Byte* rotors          = loadRotors(key, rotDirectory);
  const size_t fileSize = file_size(filename);
  Byte* data            = (Byte*) malloc(fileSize);
  Data bytes{data, fileSize};
  read_file(bytes, filename, key);
  encrypt(bytes, key, rotors);
  write_file(bytes, outputfilename, key);

  free(rotors);
  free(bytes.bytes);
  free(key.rotorShifts);
}

TuringaKey createStdKey() {
  std::string rotorNames = findRotors();
  if (rotorNames.length() == 0) {
    generateRotor(VALID_ROT_NAMES.c_str());
    rotorNames = VALID_ROT_NAMES;
  }
  return generateTuringaKey(STD_KEY_LENGTH, rotorNames);
}

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
          // get the Rotors by input
          availableRotors = argv[4];
        }
        const char* keyfile     = argv[2];
        std::string keyfilePath = std::string(keyfile);
        const size_t keylength  = atoi(argv[3]);
        TuringaKey key          = generateTuringaKey(keylength, availableRotors);
        writeTuringaKey(keyfilePath + ".key", key);
        key.direction = 1;
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
      else {
        generateRotor(argv[2]);
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
      assert((key.direction == 0 || key.direction == 1) && "the key ins't read correctly");
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
    else {
      if (testForExistence(argv[1])) {
        const char* filename = argv[1];
        TuringaKey key;
        if (testForExistence((STD_KEY_DIR + STD_KEY).c_str())) {
          key = readTuringaKey((STD_KEY_DIR + STD_KEY).c_str());
        }
        else {
          key = createStdKey();
          writeTuringaKey((STD_KEY_DIR + STD_KEY).c_str(), key);
          key.direction = 1;
          writeTuringaKey((STD_KEY_DIR + STD_KEY_INV).c_str(), key);
          key.direction = 0;
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
