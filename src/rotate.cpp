#include "rotate.hpp"

#include <cmath>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#if defined(__SSE3__) || defined(__AVX2__)
#include <immintrin.h>
#endif

#include <csprng.hpp>

#include "fileinteraction.hpp"
#include "mainerror.hpp"
#include "measurement.hpp"
#include "types.hpp"

#include <iostream>

// rotates the wheels,
// wheel rotation is determined by a bent function on the current state of rotorShifts
void rotate(RotateArgs args) {
  for (size_t i = 0; i < MAX_KEYLENGTH; ++i) {
    std::cout << size_t(args.rotorShifts[i]) << " ";
  }
  std::cout << "\n";
  /***************************************************************************************************
   *                                      AVX2 version
   **************************************************************************************************/
#if defined(__AVX2__) && 0
// disable gcc warning -Woverflow
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
  // const __m256i reverse = _mm256_loadu_si256((__m256i*) args.reverseOrder);
  const __m256i low     = _mm256_set1_epi8(0xff);  // low bit mask
  const __m256i reverse = _mm256_setr_epi8(
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4,
    3, 2, 1, 0);
  const __m256i table = _mm256_setr_epi8(
    0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110, 0b1111, 0b0010, 0b1100, 0b0101,
    0b1010, 0b0100, 0b0011, 0b1000, 0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110,
    0b1111, 0b0010, 0b1100, 0b0101, 0b1010, 0b0100, 0b0011, 0b1000);  // lookup table for inverting
  const __m256i lookup_sum = _mm256_setr_epi8(
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000,
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000);
  const __m256i rotor_intervals = _mm256_setr_epi8(
    1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49,
    51, 53, 55, 57, 59, 61, 63);  // lookup table: i-th entry is 2*i +1
// enable gcc warning -Woverflow
#pragma GCC diagnostic pop

  __m128i a = _mm_loadu_si128(((__m128i*) args.rotorShifts));      // load the first 16 byte ino a
  __m128i b = _mm_loadu_si128(((__m128i*) args.rotorShifts) + 1);  // load the last 16 byte into b

  // spread each byte into 2 (only in the lower 8 bit)
  __m256i x = _mm256_cvtepu8_epi16(a);
  __m256i y = _mm256_cvtepu8_epi16(b);

  // shift 4 bits to the left
  __m256i x2 = _mm256_slli_epi16(x, 4);
  __m256i y2 = _mm256_slli_epi16(y, 4);

  __m256i x1, y1;

  // take only the lower 4 bits of each byte
  x1 = _mm256_and_si256(x, low);
  x2 = _mm256_and_si256(x2, low);
  y1 = _mm256_and_si256(y, low);
  y2 = _mm256_and_si256(y2, low);

  // obtain x and y by adding
  x = _mm256_or_si256(x1, x2);
  y = _mm256_or_si256(y1, y2);

  // invert using the lookup table x[i] := table[x[i]]
  x = _mm256_shuffle_epi8(table, x);

  // invert the order of y
  y = _mm256_permute2x128_si256(y, y, 0b00000001);  // swaps the first and the last 128 bits
  y = _mm256_shuffle_epi8(y, reverse);

  // take the "inner product" of x and y
  __m256i z = _mm256_xor_si256(x, y);              // elementwise XOR
  z         = _mm256_shuffle_epi8(lookup_sum, z);  // sum over all elements using the lookup sum

  // set each position of z to 0 or rotor intervals and add to values
  z              = _mm256_and_si256(z, rotor_intervals);
  __m256i values = _mm256_set_m128i(a, b);  // concatinate a and b to obtain initial rotor shifts
  values         = _mm256_add_epi8(values, z);  // add z to the values

  _mm256_storeu_si256((__m256i*) args.rotorShifts, values);  // finally store rotor shifts
/***************************************************************************************************
 *                                      SSE version
 **************************************************************************************************/
#elif defined(__SSE3__) && 0
// disable gcc warning -Woverflow
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
  const __m128i low_4_bits_mask  = _mm_set1_epi8(0b00001111);
  const __m128i high_4_bits_mask = _mm_set1_epi8(0b11110000);

  // table for inverting polynomial  in GF(2) of degree <= 3 mod x^4 + x +1
  const __m128i reverseOrder = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
  const __m128i table        = _mm_setr_epi8(
    0b0000, 0b0001, 0b1001, 0b1110, 0b1101, 0b1011, 0b0111, 0b0110, 0b1111, 0b0010, 0b1100, 0b0101,
    0b1010, 0b0100, 0b0011, 0b1000);
  const __m128i lookup_sum = _mm_setr_epi8(
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b11111111,
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b11111111, 0b11111111, 0b00000000);
  const __m128i rotor_intervals_1 =
    _mm_setr_epi8(1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31);
  const __m128i rotor_intervals_2 =
    _mm_setr_epi8(33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63);

// enable gcc warning -Woverflow
#pragma GCC diagnostic pop

  __m128i values1 = _mm_loadu_si128((__m128i*) args.rotorShifts);  // load bytes from rotorShifts
  __m128i values2 =
    _mm_loadu_si128((__m128i*) args.rotorShifts + 1);  // load bytes from rotorShifts

  __m128i x1 = _mm_and_si128(values1, low_4_bits_mask);
  __m128i x2 = _mm_and_si128(values1, high_4_bits_mask);
  __m128i y1 = _mm_and_si128(values2, low_4_bits_mask);
  __m128i y2 = _mm_and_si128(values2, high_4_bits_mask);

  x2 = _mm_srli_epi32(x2, 4);  // bit shift from higher 4 bits to lower 4 bits
  y2 = _mm_srli_epi32(y2, 4);  // bit shift from higher 4 bits to lower 4 bits

  // function pi(y)
  x1 = _mm_shuffle_epi8(table, x1);  // maps x1[i] := table[x1[i]]
  x2 = _mm_shuffle_epi8(table, x2);  // maps x2[i] := table[x2[i]]

  // revert the order of the y
  y1 = _mm_shuffle_epi8(y1, reverseOrder);
  y2 = _mm_shuffle_epi8(y2, reverseOrder);

  // the "inner product" of x and y
  __m128i z1 = _mm_xor_si128(x1, y2);
  __m128i z2 = _mm_xor_si128(x2, y1);
  z1         = _mm_shuffle_epi8(lookup_sum, z1);
  z2         = _mm_shuffle_epi8(lookup_sum, z2);

  // multyply the indicatorvariable z with the shifts
  z1 = _mm_and_si128(z1, rotor_intervals_1);
  z2 = _mm_and_si128(z2, rotor_intervals_2);

  // add rotorshift to rotors
  values1 = _mm_add_epi8(values1, z1);
  values2 = _mm_add_epi8(z2, values2);

  // save the maipulated rotorshifts
  _mm_storeu_si128((__m128i*) args.rotorShifts, values1);
  _mm_storeu_si128((__m128i*) args.rotorShifts, values2);
#else
  /***************************************************************************************************
   *                                 standard version
   **************************************************************************************************/
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

  /*
  Byte* x = (Byte*) malloc(MAX_KEYLENGTH * 2);
  for (size_t i = 0; i < args.length / 2; i++) {
    x[2 * i]     = args.rotorShifts[i] >> 4;          // the rightmost bits
    x[2 * i + 1] = args.rotorShifts[i] & 0b00001111;  // the leftmost bits
  }
  for (size_t i = args.length / 2; i < args.length; i++) {
    x[2 * i]     = args.rotorShifts[MAX_KEYLENGTH - args.length + i] >> 4;
    x[2 * i + 1] = args.rotorShifts[MAX_KEYLENGTH - args.length + i] & 0b00001111;
  }

  for (size_t i = 0; i < args.length / 2; i++) {
    Byte val = table[x[i]];
    val ^= x[2 * args.length - 1 - i];  // bitwise xor
    args.rotorShifts[i] +=
      (2 * i + 1) * (__builtin_popcount(val) & 0b00000001);  // test val is uneven
  }
  for (size_t i = args.length / 2; i < args.length; i++) {
    Byte val = table[x[i]];
    val ^= x[2 * args.length - 1 - i];
    args.rotorShifts[MAX_KEYLENGTH - args.length + i] +=
      (2 * i + 1) * (__builtin_popcount(val) & 0b00000001);
  }
  */
  Byte* x = (Byte*) malloc(MAX_KEYLENGTH * 2);
  for (size_t i = 0; i < MAX_KEYLENGTH; i++) {
    x[2 * i]     = args.rotorShifts[i] >> 4;          // the rightmost bits
    x[2 * i + 1] = args.rotorShifts[i] & 0b00001111;  // the leftmost bits
  }

  for (size_t i = 0; i < MAX_KEYLENGTH; i++) {
    Byte val = table[x[i]];
    val ^= x[2 * MAX_KEYLENGTH - 1 - i];  // bitwise xor
    args.rotorShifts[i] +=
      (2 * i + 1) * (__builtin_popcount(val) & 0b00000001);  // test val is uneven
  }

  free(table);
  free(x);
#endif
  // Debug
  for (size_t i = 0; i < MAX_KEYLENGTH; ++i) {
    std::cout << size_t(args.rotorShifts[i]) << " ";
  }
  std::cout << "\n\n";
}
