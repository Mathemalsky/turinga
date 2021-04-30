#include "turinga.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <immintrin.h>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <vector>

#include <csprng.hpp>

#include "fileinteraction.hpp"
#include "mainerror.hpp"
#include "measurement.hpp"

using Byte  = unsigned char;
using Bytes = std::vector<unsigned char>;

TuringaKey generateTuringaKey(const size_t keylength, const std::string& availableRotors) {
  std::vector<char> rotorNames(keylength);
  Byte* rotorShifts           = (Byte*) malloc(MAX_KEYLENGTH);
  const size_t numberOfRotors = availableRotors.length();
  duthomhas::csprng random;
  for (size_t i = 0; i < keylength; ++i) {
    rotorShifts[i] = random();
    rotorNames[i]  = availableRotors[random() % numberOfRotors];
  }
  const size_t fileShift = random();
  const TuringaKey key{0, keylength, rotorNames, rotorShifts, fileShift};

  std::cout << timestamp(current_duration()) << "A key of length " << keylength
            << " has been generated.\n";
  return key;
}
