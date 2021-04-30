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

// rotates the wheels,
// wheel rotation is determined by a bent function on the current state of rotorShifts
#ifdef __AVX2__
void rotate(Byte* rotorShifts, const size_t length, const char* __restrict__ reverseOrder) {
// disable gcc warning -Woverflow
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
  const __m256i low_4_bits_mask  = _mm256_set1_epi8(0b00001111);
  const __m256i high_4_bits_mask = _mm256_set1_epi8(0b11110000);
  const __m256i reverse          = _mm256_loadu_si256((__m256i*) reverseOrder);

  // table for inverting polynomial  in GF(2) of degree <= 3 mod x^4 + x +1
  const __m256i table = _mm256_setr_epi8(
    0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110, 0b1111, 0b0010, 0b1100, 0b0101,
    0b1010, 0b0100, 0b0011, 0b1000, 0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110,
    0b1111, 0b0010, 0b1100, 0b0101, 0b1010, 0b0100, 0b0011, 0b1000);
  const __m256i rotor_intervals = _mm256_setr_epi8(
    1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49,
    51, 53, 55, 57, 59, 61, 63);
  const __m256i lookup_sum = _mm256_setr_epi8(
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000,
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000);

// enable gcc warning -Woverflow
#pragma GCC diagnostic pop

  __m256i values = _mm256_loadu_si256((__m256i*) rotorShifts);  // load bytes from rotorShifts

  // because shuffle can only permutate inside the lower and inside the upper 128 bit swap in case
  // key is longer than 16 bit
  __m256i y = (length > 16) ? _mm256_permute2x128_si256(values, values, 0b00000001)
                            : _mm256_permute2x128_si256(values, values, 0b00010000);

  y = _mm256_shuffle_epi8(y, reverse);  // maps y_i := y[reverse_i]

  y         = _mm256_and_si256(y, low_4_bits_mask);
  __m256i x = _mm256_and_si256(values, high_4_bits_mask);

  // function pi(y)
  y = _mm256_shuffle_epi8(table, y);  // maps y_i := table[y_i]
  x = _mm256_srli_epi32(x, 4);        // bit shift from higher 4 bits to lower 4 bits

  // function <x,y>
  y = _mm256_and_si256(x, y);              // bitwise add x and y
  y = _mm256_shuffle_epi8(lookup_sum, y);  // maps y_i := lookupsum[y_i] for 0 <= i < 16 and y_i :=
                                           // lookupsum[16 + y_i] for 16 <= i < 32

  // bitwise and to simulate multiplication of 0,1 with 0,...,255
  y      = _mm256_and_si256(y, rotor_intervals);
  values = _mm256_add_epi8(values, y);

  _mm256_storeu_si256((__m256i*) rotorShifts, values);  // save to rotorShifts
}

#else
void rotate(Byte* rotorShifts, const size_t length) {
  // table for inverting polynomial  in GF(2) of degree <= 3 mod x^4 + x +1
  Byte* table = (Byte*) malloc(16);
  table[0]    = 0b0000;
  table[1]    = 0b0001;
  table[2]    = 0b1001;
  table[3]    = 0b1110;
  table[4]    = 0b1101;
  table[5]    = 0b1011;
  table[6]    = 0b0111;
  table[7]    = 0b0110;
  table[8]    = 0b1111;
  table[9]    = 0b0010;
  table[10]   = 0b1100;
  table[11]   = 0b0101;
  table[12]   = 0b1010;
  table[13]   = 0b0100;
  table[14]   = 0b0011;
  table[15]   = 0b1000;

  // lookup table for the sum of the binary digits mod 2
  Byte* lookup_sum = (Byte*) malloc(16);
  lookup_sum[0]    = 0;
  lookup_sum[1]    = 1;
  lookup_sum[2]    = 1;
  lookup_sum[3]    = 0;
  lookup_sum[4]    = 1;
  lookup_sum[5]    = 0;
  lookup_sum[6]    = 0;
  lookup_sum[7]    = 1;
  lookup_sum[8]    = 1;
  lookup_sum[9]    = 0;
  lookup_sum[10]   = 0;
  lookup_sum[11]   = 1;
  lookup_sum[12]   = 0;
  lookup_sum[13]   = 1;
  lookup_sum[14]   = 1;
  lookup_sum[15]   = 0;

  Byte* x = (Byte*) malloc(MAX_KEYLENGTH * 2);
  for (size_t i = 0; i < length; i++) {
    x[i]     = (rotorShifts[i] << 4) >> 4;  // the rightmost bits
    x[i + 1] = rotorShifts[i] >> 4;         // the leftmost bits
  }
  for (size_t i = 0; i < length; i++) {
    Byte val = table[x[i]];
    val ^= x[2 * length - 1 - i];  // bitwise xor
    val = (val << 4) >> 4;         // shouldn't be nesecarry
    rotorShifts[i] += (2 * i + 1) * lookup_sum[val];
  }

  free(table);
  free(lookup_sum);
}
#endif

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
  // count downwards from keylength-1 to 0 and fill with zeros
  char* reverseOrder = (char*) malloc(32);
#define place(val) ((val) & -(0 < (val)))
  for (size_t i = 0; i < 32; ++i) {
    reverseOrder[i] = place(key.length - i - 1);
  }
#undef place

  const size_t threadcount = std::thread::hardware_concurrency();  // number of logical processors
  size_t begin             = 0, end;
  end                      = bytes.size / threadcount;

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
      TuringaKey{key.direction, key.length, key.rotorNames, rotorShiftsAry[i], key.fileShift},
      rotors, begin, end, std::ref(reverseOrder)));
    // prepair for next thread
    for (size_t j = begin; j < end; ++j) {  // rotate to start of next thread
      rotate(rotorShiftsAry[i + 1], key.length, reverseOrder);
    }
    begin = end;
    end += bytes.size / threadcount;
  }

  // encrypt the rest
  end = bytes.size;
  encrypt_block(
    bytes,
    TuringaKey{key.direction, key.length, key.rotorNames, rotorShiftsAry[threadcount - 1],
               key.fileShift},
    rotors, end, bytes.size, reverseOrder);

  // collect all threads
  for (std::thread& thr : threads) {
    thr.join();
  }

  // free all memory
  for (Byte*& rotShi : rotorShiftsAry) {
    free(rotShi);
  }
  free(reverseOrder);

  if (key.direction == 0) {
    std::cout << timestamp(current_duration()) << "File has been encrypted.\n";
  }
  else {
    std::cout << timestamp(current_duration()) << "File has been decrypted.\n";
  }
}

void encrypt_block(
  Data& bytes, TuringaKey key, const Byte* rotors, const size_t begin, const size_t end,
  const char* __restrict__ reverseOrder) {
  const size_t keylength = key.length;

  // encryption
  if (key.direction == 0) {
    for (size_t i = begin; i < end; ++i) {
      Byte tmp = bytes.bytes[i];
      for (size_t i = 0; i < keylength; ++i) {
        tmp = rotors[256 * i + ((tmp + key.rotorShifts[i]) % 256)];
      }
      bytes.bytes[i] = tmp;
      rotate(key.rotorShifts, keylength, reverseOrder);
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
      rotate(key.rotorShifts, keylength, reverseOrder);
    }
  }
}
