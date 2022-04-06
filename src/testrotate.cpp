#include "testrotate.hpp"

#include <cstring>
#include <iostream>

#include "constants.hpp"
#include "measurement.hpp"
#include "rotate.hpp"

void findCycle(const Byte* rotorShifts, size_t maxIter) {
  Byte* x = (Byte*) malloc(MAX_KEYLENGTH);
  Byte* y = (Byte*) malloc(MAX_KEYLENGTH);
  std::memcpy(x, rotorShifts, MAX_KEYLENGTH);
  std::memcpy(y, rotorShifts, MAX_KEYLENGTH);
  for (size_t iteration = 0; iteration < maxIter; ++iteration) {
    rotate(x);
    rotate(y);
    rotate(y);
    if (std::memcmp(x, y, MAX_KEYLENGTH) == 0) {
      std::cerr << "cycle length is a divisor of " << iteration + 1 << ".\n";
      break;
    }
    if ((iteration + 1) % (size_t) 1e7 == 0) {
      std::cout << timestamp(current_duration()) << "iterations checked: " << iteration + 1 << "\n";
    }
  }
  std::cout << timestamp(current_duration()) << "check done!";
}
