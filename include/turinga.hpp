#ifndef TURINGA_H
#define TURINGA_H

#include <stddef.h>
#include <string>
#include <vector>

using Bytes = std::vector<unsigned char>;
using Byte  = unsigned char;

struct Data {
  unsigned char* bytes;
  const size_t size;
};

struct TuringaKey {
  int direction;
  const size_t length;
  const std::vector<char> rotorNames;
  Byte* rotorShifts;
  const size_t fileShift;
};

void rotate(Bytes& fileShifts, const size_t length);
TuringaKey generateTuringaKey(const size_t keylength, const std::string& availableRotors);
void encrypt(Data& bytes, TuringaKey& key, const Byte* rotors);
void encrypt_block(
  Data& bytes, TuringaKey key, const Byte* rotors, const size_t begin, const size_t end,
  const char* __restrict__ reverseOrder);

#endif  // TURINGA_H
