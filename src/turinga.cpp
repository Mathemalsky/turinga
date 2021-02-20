#include "turinga.hpp"

#include <cassert>
#include <fstream>
#include <immintrin.h>
#include <iostream>
#include <cmath>
#include <stddef.h>
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

static char* reverse_global;

// rotates the wheels,
// each wheel rotates depending on the position of the prvious wheel
void rotate(Byte* rotorShifts, const size_t length) {
  __m256i low_4_bits_mask  = _mm256_set1_epi8(0b00001111);
  __m256i high_4_bits_mask = _mm256_set1_epi8(0b11110000);
  __m256i reverse          = _mm256_loadu_si256((__m256i*) reverse_global);
  __m256i table            = _mm256_setr_epi8(
    0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110, 0b1111, 0b0010, 0b1100, 0b0101,
    0b1010, 0b0100, 0b0011, 0b1000, 0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110,
    0b1111, 0b0010, 0b1100, 0b0101, 0b1010, 0b0100, 0b0011, 0b1000);
  __m256i rotor_intervals = _mm256_setr_epi8(
    1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49,
    51, 53, 55, 57, 59, 61, 63);
  __m256i lookup_sum = _mm256_setr_epi8(
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000,
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000);

  __m256i values = _mm256_loadu_si256((__m256i*) rotorShifts);
  __m256i y      = (length > 16) ? _mm256_permute2x128_si256(values, values, 0b00000001)
                                 : _mm256_permute2x128_si256(values, values, 0b00010000);

  y = _mm256_shuffle_epi8(y, reverse);

  y         = _mm256_and_si256(y, low_4_bits_mask);
  __m256i x = _mm256_and_si256(values, high_4_bits_mask);

  // function pi(y)
  y = _mm256_shuffle_epi8(table, y);

  x = _mm256_srli_epi32(x, 4);

  // function <x,y>
  y = _mm256_and_si256(x, y);
  y = _mm256_shuffle_epi8(lookup_sum, y);

  y      = _mm256_and_si256(y, rotor_intervals);
  values = _mm256_add_epi8(values, y);

  _mm256_storeu_si256((__m256i*) rotorShifts, values);
}

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

// encrypts/ decrypts the files
void encrypt(Data& bytes, TuringaKey& key, const Byte* rotors) {
  // const size_t threadcount = 2;
  // this is an empirical constant from kcachgrind measurement
  // it's ratio of time spend rotate and time spend in encryption
  // the second thread gives speed up up to 13% more would be close to useless
  const double rotateRatio = 45.92 / 53.71;
  size_t begin             = 0, end;

  char* reverse             = (char*) malloc(32);
  const char high_keylength = key.length >> 4;
  const char low_keylength  = key.length - (high_keylength << 4);
#define place(val) ((val) & -(0 < (val)))
  for (int i = 0; i < 16; i++) {
    reverse[i] = place(low_keylength - i - 1);
  }
  for (int i = 0; i < 16; i++) {
    reverse[16 + i] = place(high_keylength - i - 1);
  }
#undef place
  reverse_global = reverse;

  end = std::round(bytes.size * (1.0 - ((1.0 - rotateRatio) / (2.0 - rotateRatio))));

  Byte* rotorShifts_cpy = (Byte*) malloc(MAX_KEYLENGTH);
  std::memcpy(rotorShifts_cpy, key.rotorShifts, MAX_KEYLENGTH);
  TuringaKey key_cpy{key.direction, key.length, key.rotorNames, rotorShifts_cpy, key.fileShift};

  // std::thread thr(encrypt_block, std::ref(bytes), key_cpy, rotors, begin, end);

  for (size_t i = 0; i < end; ++i) {
    rotate(key.rotorShifts, key.length);
  }
  encrypt_block(bytes, key, rotors, end, bytes.size);

  // thr.join();
  free(key_cpy.rotorShifts);
  free(reverse);

  if (key.direction == 0) {
    std::cout << timestamp(current_duration()) << "File has been encrypted.\n";
  }
  else {
    std::cout << timestamp(current_duration()) << "File has been decrypted.\n";
  }
}

void encrypt_block(
  Data& bytes, TuringaKey key, const Byte* rotors, const size_t begin, const size_t end) {
  const size_t keylength = key.length;

  // encryption
  if (key.direction == 0) {
    for (size_t i = begin; i < end; ++i) {
      Byte tmp = bytes.bytes[i];
      for (size_t i = 0; i < keylength; ++i) {
        tmp = rotors[256 * i + ((tmp + key.rotorShifts[i]) % 256)];
      }
      bytes.bytes[i] = tmp;
      rotate(key.rotorShifts, keylength);
    }
  }

  // decryption
  else if (key.direction == 1) {
    for (size_t i = begin; i < end; ++i) {
      Byte tmp = bytes.bytes[i];
      for (size_t i = 0; i < keylength; ++i) {
        tmp = rotors[256 * i + tmp] - key.rotorShifts[keylength - 1 - i];
      }
      bytes.bytes[i] = tmp;
      rotate(key.rotorShifts, keylength);
    }
  }
}
