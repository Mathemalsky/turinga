#include "turinga.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "colors.hpp"
#include "fileinteraction.hpp"
#include "mainerror.hpp"
#include "measurement.hpp"

int main(int argc, char** argv) {
  start_time();
  try {
    if (argc == 5) {
      const char* filename     = argv[1];
      const char* keyfile      = argv[2];
      const char* rotDirectory = argv[3];
      const char* outputfile   = argv[4];
      TuringaKey key           = readTuringaKey(keyfile);
      assert((key.direction == 0 || key.direction == 1) && "the key ins't read correctly");
      Byte* rotors = loadRotors(key, rotDirectory);

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
    else if (argc == 4) {
      const char* keyfile               = argv[1];
      std::string keyfilePath           = std::string(keyfile);
      const size_t keylenght            = atoi(argv[2]);
      const std::string availableRotors = argv[3];
      TuringaKey key                    = generateTuringaKey(keylenght, availableRotors);
      writeTuringaKey(keyfilePath, key);
      key.direction = 1;
      writeTuringaKey(keyfilePath.substr(0, keyfilePath.length() - 4) + "_inv.txt", key);

      free(key.rotorShifts);
    }
    else {
      throw inappropriate_number_of_arguments(argc, 5, "main");
    }
  } catch (mainerror& error) {
    error.report_error();
  }
  std::cout << timestamp(current_duration());
  print_lightgreen("Done!\n");
  return 0;
}
