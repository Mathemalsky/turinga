#include "rotate.hpp"

#include <cmath>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#include <csprng.hpp>

#include "fileinteraction.hpp"
#include "mainerror.hpp"
#include "measurement.hpp"
#include "types.hpp"

#ifdef __AVX2__
#include <immintrin.h>
// rotates the wheels,
// wheel rotation is determined by a bent function on the current state of rotorShifts
void rotate(RotateArgs args) {
// disable gcc warning -Woverflow
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
  const __m256i reverse = _mm256_loadu_si256((__m256i*) args.reverseOrder);
  __m256i low           = _mm256_set1_epi8(0xff);
  const __m256i table   = _mm256_setr_epi8(
    0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110, 0b1111, 0b0010, 0b1100, 0b0101,
    0b1010, 0b0100, 0b0011, 0b1000, 0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110,
    0b1111, 0b0010, 0b1100, 0b0101, 0b1010, 0b0100, 0b0011, 0b1000);
  const __m256i lookup_sum = _mm256_setr_epi8(
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000,
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000);
  const __m256i rotor_intervals = _mm256_setr_epi8(
    1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49,
    51, 53, 55, 57, 59, 61, 63);
// enable gcc warning -Woverflow
#pragma GCC diagnostic pop

  __m128i a = _mm_loadu_si128(((__m128i*) args.rotorShifts));
  __m128i b = _mm_loadu_si128(((__m128i*) args.rotorShifts) + 1);

  __m256i x = _mm256_cvtepu8_epi16(a);
  __m256i y = _mm256_cvtepu8_epi16(b);

  __m256i x1 = x;
  __m256i x2 = _mm256_slli_epi16(x, 4);
  __m256i y1 = y;
  __m256i y2 = _mm256_slli_epi16(y, 4);

  x1 = _mm256_and_si256(x1, low);
  x2 = _mm256_and_si256(x2, low);
  y1 = _mm256_and_si256(y1, low);
  y2 = _mm256_and_si256(y2, low);

  x = _mm256_or_si256(x1, x2);
  y = _mm256_or_si256(y1, y2);

  x = _mm256_shuffle_epi8(table, x);
  y = _mm256_shuffle_epi8(y, reverse);

  __m256i z = _mm256_xor_si256(x, y);
  z         = _mm256_shuffle_epi8(lookup_sum, z);
  z         = _mm256_and_si256(z, rotor_intervals);

  __m256i values = _mm256_set_m128i(a, b);
  values         = _mm256_add_epi8(values, z);

  _mm256_storeu_si256((__m256i*) args.rotorShifts, values);

  // 0,1,...,31

  /*
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
  */
}
/*
// encrypts/ decrypts the files
void encrypt(Data& bytes, TuringaKey& key, const Byte* rotors) {
  // count downwards from keylength-1 to 0 and fill with zeros
  char* reverseOrder = (char*) malloc(32);
#define place(val) ((val) & -(0 < (val)))
  for (size_t i = 0; i < 32; ++i) {
    reverseOrder[i] = place(key.length - i - 1);
  }
#undef place
*/
#elif defined(__SSE3__)
// disable gcc warning -Woverflow
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
const __m128i low_4_bits_mask  = _mm_set1_epi8(0b00001111);
const __m128i high_4_bits_mask = _mm_set1_epi8(0b11110000);

// table for inverting polynomial  in GF(2) of degree <= 3 mod x^4 + x +1
const __m128i table = _mm_setr_epi8(
  0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110, 0b1111, 0b0010, 0b1100, 0b0101,
  0b1010, 0b0100, 0b0011, 0b1000);
const __m256i rotor_intervals = _mm256_setr_epi8(
  1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51,
  53, 55, 57, 59, 61, 63);
const __m256i lookup_sum = _mm256_setr_epi8(
  0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
  0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000,
  0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
  0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000);

// enable gcc warning -Woverflow
#pragma GCC diagnostic pop

__m128i values1 = _mm_loadu_si128((__m128i*) rotorShifts);      // load bytes from rotorShifts
__m128i values2 = _mm_loadu_si128((__m128i*) rotorShifts + 1);  // load bytes from rotorShifts

__m128i x1 = _mm_and_si128(values1, low_4_bits_mask);
__m128i x2 = _mm_and_si128(values1, high_4_bits_mask);
__m128i y1 = _mm_and_si128(values2, low_4_bits_mask);
__m128i y2 = _mm_and_si128(values2, high_4_bits_mask);

x2 = _mm_srli_epi32(x2, 4);  // bit shift from higher 4 bits to lower 4 bits
y2 = _mm_srli_epi32(y2, 4);  // bit shift from higher 4 bits to lower 4 bits

// function pi(y)
x1 = _mm_shuffle_epi8(table, x1);  // maps x1[i] := table[x1[i]]
x2 = _mm_shuffle_epi8(table, x2);  // maps x2[i] := table[x2[i]]

#else

// rotates the wheels,
// wheel rotation is determined by a bent function on the current state of rotorShifts
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

  Byte* x = (Byte*) malloc(MAX_KEYLENGTH * 2);
  for (size_t i = 0; i < length; i++) {
    x[2 * i]     = rotorShifts[i] >> 4;          // the rightmost bits
    x[2 * i + 1] = rotorShifts[i] & 0b00001111;  // the leftmost bits
  }

  for (size_t i = 0; i < length; i++) {
    Byte val = table[x[i]];
    val ^= x[2 * length - 1 - i];  // bitwise xor
    rotorShifts[i] +=
      (2 * i + 1) * (__builtin_popcount(val) & 0b00000001);  // test if val is uneven
  }

  // Debug
  /*
  for (size_t i = 0; i < length; ++i) {
    std::cout << size_t(rotorShifts[i]) << " ";
  }
  std::cout << "\n";
  */

  free(table);
  free(x);
}

#endif
