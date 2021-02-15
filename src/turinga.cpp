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

// rotates the wheels,
// each wheel rotates depending on the position of the prvious wheel
/*!
 * \brief rotate
 * \param rotorShifts
 * \param length
 */
void rotate(Byte* rotorShifts, const size_t length) {
  __m256i_u b = _mm256_setzero_si256();

  __m256i_u a = _mm256_loadu_si256((__m256i_u*) rotorShifts);

  Byte last = 1;
  b         = _mm256_insert_epi8(b, last, 0);
  last      = !((rotorShifts[0] + last) % 3);
  b         = _mm256_insert_epi8(b, last, 1);
  last      = !((rotorShifts[1] + last) % 5);
  b         = _mm256_insert_epi8(b, last, 2);
  last      = !((rotorShifts[2] + last) % 7);
  b         = _mm256_insert_epi8(b, last, 3);
  last      = !((rotorShifts[3] + last) % 9);
  b         = _mm256_insert_epi8(b, last, 4);
  last      = !((rotorShifts[4] + last) % 11);
  b         = _mm256_insert_epi8(b, last, 5);
  last      = !((rotorShifts[5] + last) % 13);
  b         = _mm256_insert_epi8(b, last, 6);
  last      = !((rotorShifts[6] + last) % 15);
  b         = _mm256_insert_epi8(b, last, 7);
  if (length > 8) {
    last = !((rotorShifts[7] + last) % 17);
    b    = _mm256_insert_epi8(b, last, 8);
    last = !((rotorShifts[8] + last) % 19);
    b    = _mm256_insert_epi8(b, last, 9);
    last = !((rotorShifts[9] + last) % 21);
    b    = _mm256_insert_epi8(b, last, 10);
    last = !((rotorShifts[10] + last) % 23);
    b    = _mm256_insert_epi8(b, last, 11);
    last = !((rotorShifts[11] + last) % 25);
    b    = _mm256_insert_epi8(b, last, 12);
    last = !((rotorShifts[12] + last) % 27);
    b    = _mm256_insert_epi8(b, last, 13);
    last = !((rotorShifts[13] + last) % 29);
    b    = _mm256_insert_epi8(b, last, 14);
    last = !((rotorShifts[14] + last) % 31);
    b    = _mm256_insert_epi8(b, last, 15);
    if (length > 16) {
      last = !((rotorShifts[15] + last) % 33);
      b    = _mm256_insert_epi8(b, last, 16);
      last = !((rotorShifts[16] + last) % 35);
      b    = _mm256_insert_epi8(b, last, 17);
      last = !((rotorShifts[17] + last) % 37);
      b    = _mm256_insert_epi8(b, last, 18);
      last = !((rotorShifts[18] + last) % 39);
      b    = _mm256_insert_epi8(b, last, 19);
      last = !((rotorShifts[19] + last) % 41);
      b    = _mm256_insert_epi8(b, last, 20);
      last = !((rotorShifts[20] + last) % 43);
      b    = _mm256_insert_epi8(b, last, 21);
      last = !((rotorShifts[21] + last) % 45);
      b    = _mm256_insert_epi8(b, last, 22);
      last = !((rotorShifts[22] + last) % 47);
      b    = _mm256_insert_epi8(b, last, 23);
      if (length > 24) {
        last = !((rotorShifts[23] + last) % 49);
        b    = _mm256_insert_epi8(b, last, 24);
        last = !((rotorShifts[24] + last) % 51);
        b    = _mm256_insert_epi8(b, last, 25);
        last = !((rotorShifts[25] + last) % 53);
        b    = _mm256_insert_epi8(b, last, 26);
        last = !((rotorShifts[26] + last) % 55);
        b    = _mm256_insert_epi8(b, last, 27);
        last = !((rotorShifts[27] + last) % 57);
        b    = _mm256_insert_epi8(b, last, 28);
        last = !((rotorShifts[28] + last) % 59);
        b    = _mm256_insert_epi8(b, last, 29);
        last = !((rotorShifts[29] + last) % 61);
        b    = _mm256_insert_epi8(b, last, 30);
        last = !((rotorShifts[30] + last) % 63);
        b    = _mm256_insert_epi8(b, last, 31);
      }
    }
  }

  a = _mm256_add_epi8(a, b);

  _mm256_storeu_si256((__m256i_u*) rotorShifts, a);

  rotorShifts[0] += !(rotorShifts[length - 1] % (2 * length + 1));
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

  end = std::round(bytes.size * (1 - ((1 - rotateRatio) / (2 - rotateRatio))));

  Byte* rotorShifts_cpy = (Byte*) malloc(MAX_KEYLENGTH);
  std::memcpy(rotorShifts_cpy, key.rotorShifts, MAX_KEYLENGTH);
  TuringaKey key_cpy{key.direction, key.length, key.rotorNames, rotorShifts_cpy, key.fileShift};

  std::thread thr(encrypt_block, std::ref(bytes), key_cpy, rotors, begin, end);

  for (size_t i = 0; i < end; ++i) {
    rotate(key.rotorShifts, key.length);
  }
  encrypt_block(bytes, key, rotors, end, bytes.size);

  thr.join();
  free(key_cpy.rotorShifts);

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
