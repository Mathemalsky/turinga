#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

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

int main(int argc, char** argv) {
  start_time();
  try {
    if (argc <= 1) {
      throw InappropriateNumberOfArguments("main", 2, argc);
    }
    else if (std::strcmp(argv[1], "help") == 0) {
      if (argc == 2) {
        syntaxHelp();
      }
      else if (std::strcmp(argv[2], "crypt") == 0) {
        syntaxCrypt();
      }
      else if (std::strcmp(argv[2], "generate") == 0) {
        syntaxGenerate();
      }
      else {
        throw InvalidArgument("main", argv[2], "after <help>");
      }
    }
    else if (std::strcmp(argv[1], "genKey") == 0) {
      if (argc <= 3) {
        throw InappropriateNumberOfArguments("main", 5, argc);
      }
      else if (argc == 4) {
        // not yet implemented
        const std::string availableRotors = findRotors();
      }
      else {
        const char* keyfile               = argv[2];
        std::string keyfilePath           = std::string(keyfile);
        const size_t keylength            = atoi(argv[3]);
        const std::string availableRotors = argv[4];
        TuringaKey key                    = generateTuringaKey(keylength, availableRotors);
        writeTuringaKey(keyfilePath + ".key", key);
        key.direction = 1;
        writeTuringaKey(keyfilePath + "_inv.key", key);
        free(key.rotorShifts);
      }
    }
    else if (std::strcmp(argv[1], "genRot") == 0) {
      if (argc == 2) {
        throw InappropriateNumberOfArguments("main", 3, argc);
      }
      else {
        generateRotor(argv[3]);
      }
    }
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
      Byte* rotors          = loadRotors(key, rotDirectory);
      const size_t fileSize = file_size(filename);
      Byte* data            = (Byte*) malloc(fileSize);
      Data bytes{data, fileSize};
      read_file(bytes, filename, key);
      encrypt(bytes, key, rotors);
      write_file(bytes, outputfile, key);

      free(rotors);
      free(bytes.bytes);
      free(key.rotorShifts);
    }
    else {
      throw InvalidArgument("main", argv[1], "as first argument");
    }
  } catch (TuringaError& error) {
    error.what();
  }
  std::cout << timestamp(current_duration());
  print_lightgreen("Done!\n");
  return 0;
}
