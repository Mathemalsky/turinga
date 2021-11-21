#include "testrotate.hpp"

#include <cstring>
#include <iostream>

#include "constants.hpp"
#include "rotate.hpp"

static bool cmp(const Byte* a, const Byte* b, const unsigned int length) {
  for (unsigned int i = 0; i < length; ++i) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

void testRotate(TuringaKey key) {
  Byte* start    = (Byte*) malloc(MAX_KEYLENGTH);
  Byte* previous = (Byte*) malloc(MAX_KEYLENGTH);
  std::memcpy(start, key.rotorShifts, MAX_KEYLENGTH);
  std::memcpy(previous, key.rotorShifts, MAX_KEYLENGTH);
  for (size_t i = 0; i < 10000000; ++i) {
    rotate(key.rotorShifts);
    if (cmp(key.rotorShifts, previous, MAX_KEYLENGTH)) {
      std::cerr << "Fixpoint at iteration: " << i << "\n";
      free(start);
      free(previous);
      return;
    }
    else if (cmp(key.rotorShifts, start, MAX_KEYLENGTH)) {
      std::cerr << "Reached start value at iteration: " << i << "\n";
      free(start);
      free(previous);
      return;
    }
  }
  std::cerr << "Tes finished normal.\n";
}
