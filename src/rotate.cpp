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

void print(const __m128i a, const __m128i b) {
  Byte* buffer = (Byte*) malloc(16);
  _mm_storeu_si128((__m128i*) buffer, a);
  for (size_t i = 0; i < 16; ++i) {
    std::cout << (size_t) buffer[i] << " ";
  }
  _mm_storeu_si128((__m128i*) buffer, b);
  for (size_t i = 0; i < 16; ++i) {
    std::cout << (size_t) buffer[i] << " ";
  }
  std::cout << std::endl;
}

void print(const __m256i a) {
  Byte* buffer = (Byte*) malloc(32);
  _mm256_storeu_si256((__m256i*) buffer, a);
  for (size_t i = 0; i < 32; ++i) {
    std::cout << (size_t) buffer[i] << " ";
  }
  std::cout << std::endl;
}

// rotates the wheels,
// wheel rotation is determined by a bent function on the current state of rotorShifts
void rotate(RotateArgs args) {
  /*
  for (size_t i = 0; i < MAX_KEYLENGTH; ++i) {
    std::cout << size_t(args.rotorShifts[i]) << " ";
  }
  std::cout << "\n";
  */
  /***************************************************************************************************
   *                                      AVX2 version
   **************************************************************************************************/
#if defined(__AVX2__)
// disable gcc warning -Woverflow
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
  // const __m256i reverse = _mm256_loadu_si256((__m256i*) args.reverseOrder);
  const __m256i low     = _mm256_set1_epi8(0b00001111);  // low bit mask
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

  // print(a, b);

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

  // print(x);
  // print(y);

  // invert using the lookup table x[i] := table[x[i]]
  x = _mm256_shuffle_epi8(table, x);

  // invert the order of y
  y = _mm256_permute2x128_si256(y, y, 0b00000001);  // swaps the first and the last 128 bits
  y = _mm256_shuffle_epi8(y, reverse);

  // take the "inner product" of x and y
  __m256i z = _mm256_and_si256(x, y);              // elementwise and
  z         = _mm256_shuffle_epi8(lookup_sum, z);  // sum over all elements using the lookup sum

  // set each position of z to 0 or rotor intervals and add to values
  z              = _mm256_and_si256(z, rotor_intervals);
  __m256i values = _mm256_set_m128i(b, a);  // concatinate a and b to obtain initial rotor shifts
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
  /*
  const __m128i even_mask = _mm_setr_epi8(
    0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00);
  const __m128i uneven_mask = _mm_setr_epi8(
    0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff);
  const __m128i even_low     = _mm_setr_epi8(0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0);
  const __m128i uneven_low   = _mm_setr_epi8(0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7);
  const __m128i even_high    = _mm_setr_epi8(8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15, 0);
  const __m128i uneven_high  = _mm_setr_epi8(0, 8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15);
  */
  const __m128i shuffle_high = _mm_setr_epi8(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);
// enable gcc warning -Woverflow
#pragma GCC diagnostic pop

  // load bytes from rotorShifts
  __m128i values1 = _mm_loadu_si128((__m128i*) args.rotorShifts);
  __m128i values2 = _mm_loadu_si128((__m128i*) args.rotorShifts + 1);

  /*
  __m128i a1 = _mm_shuffle_epi8 (values1, even_low);
  a1 = _mm_and_si128 (a1, even_mask);
  __m128i a2 = _mm_shuffle_epi8 (values1, uneven_low);
  a2 = _mm_and_si128 (a2, uneven_mask);
  __m128i a = _mm_add_epi8 (a1, a2);

  __m128i b1 = _mm_shuffle_epi8 (values1, even_low);
  b1 = _mm_and_si128 (b1, even_mask);
  __m128i b2 = _mm_shuffle_epi8 (values1, uneven_low);
  b2 = _mm_and_si128 (b2, uneven_mask);
  __m128i b = _mm_add_epi8 (b1, b2);
  */

  __m128i x1, x2, x3, x4, y1, y2, y3, y4;
  x1 = _mm_cvtepu8_epi16(values1);
  x3 = _mm_shuffle_epi8(values1, shuffle_high);
  x3 = _mm_cvtepu8_epi16(x3);

  // print(x1, x3);

  x2 = _mm_and_si128(x1, high_4_bits_mask);
  x1 = _mm_and_si128(x1, low_4_bits_mask);
  x4 = _mm_and_si128(x3, high_4_bits_mask);
  x3 = _mm_and_si128(x3, low_4_bits_mask);

  x2 = _mm_slli_epi32(x2, 4);  // bit shift from higher 4 bits to lower 4 bits
  x4 = _mm_slli_epi32(x4, 4);  // bit shift from higher 4 bits to lower 4 bits

  // print(x1, x3);
  // print(x2, x4);

  x1 = _mm_add_epi8(x1, x2);
  x2 = _mm_add_epi8(x3, x4);

  y1 = _mm_cvtepu8_epi16(values2);
  y3 = _mm_shuffle_epi8(values2, shuffle_high);
  y3 = _mm_cvtepu8_epi16(y3);

  y2 = _mm_and_si128(y1, high_4_bits_mask);
  y1 = _mm_and_si128(y1, low_4_bits_mask);
  y4 = _mm_and_si128(y3, high_4_bits_mask);
  y3 = _mm_and_si128(y3, low_4_bits_mask);

  y2 = _mm_slli_epi32(y2, 4);  // bit shift from higher 4 bits to lower 4 bits
  y4 = _mm_slli_epi32(y4, 4);  // bit shift from higher 4 bits to lower 4 bits

  y1 = _mm_add_epi8(y1, y2);
  y2 = _mm_add_epi8(y3, y4);

  // print(x1, y1);
  // print(x2, y2);

  // function pi(y)
  x1 = _mm_shuffle_epi8(table, x1);  // maps x1[i] := table[x1[i]]
  x2 = _mm_shuffle_epi8(table, x2);  // maps x2[i] := table[x2[i]]

  // print(x1, x2);

  // revert the order of the y
  y1 = _mm_shuffle_epi8(y1, reverseOrder);
  y2 = _mm_shuffle_epi8(y2, reverseOrder);

  // print(y2, y1);

  // the "inner product" of x and y
  __m128i z1 = _mm_and_si128(x1, y2);
  __m128i z2 = _mm_and_si128(x2, y1);
  z1         = _mm_shuffle_epi8(lookup_sum, z1);
  z2         = _mm_shuffle_epi8(lookup_sum, z2);

  /*
  // shuffle z alternating from z1 and z2
  __m128i z3 = _mm_shuffle_epi8(z1, even_low);
  __m128i z4 = _mm_shuffle_epi8(z1, uneven_low);
  z3         = _mm_and_si128(z3, even_mask);
  z4         = _mm_and_si128(z4, uneven_mask);
  z1         = _mm_add_epi8(z3, z4);

  z3 = _mm_shuffle_epi8(z1, even_high);
  z4 = _mm_shuffle_epi8(z1, uneven_high);
  z3 = _mm_and_si128(z3, even_mask);
  z4 = _mm_and_si128(z4, uneven_mask);
  z2 = _mm_add_epi8(z3, z4);
  */

  // multyply the indicatorvariable z with the shifts
  z1 = _mm_and_si128(z1, rotor_intervals_1);
  z2 = _mm_and_si128(z2, rotor_intervals_2);

  // print(z1, z2);

  // add rotorshift to rotors
  values1 = _mm_add_epi8(values1, z1);
  values2 = _mm_add_epi8(values2, z2);

  // save the maipulated rotorshifts
  _mm_storeu_si128((__m128i*) args.rotorShifts, values1);
  _mm_storeu_si128((__m128i*) args.rotorShifts + 1, values2);
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
    x[2 * i]     = args.rotorShifts[i] & 0b00001111;  // the rightmost bits
    x[2 * i + 1] = args.rotorShifts[i] >> 4;          // the leftmost bits
  }
  // Debug
  /*
  for (size_t i = 0; i < 2 * MAX_KEYLENGTH; ++i) {
    std::cout << size_t(x[i]) << " ";
  }
  std::cout << "\n\n";
  */

  for (size_t i = 0; i < MAX_KEYLENGTH; i++) {
    Byte val = table[x[i]];
    val &= x[2 * MAX_KEYLENGTH - 1 - i];  // bitwise xor
    args.rotorShifts[i] +=
      (2 * i + 1) * (__builtin_popcount(val) & 0b00000001);  // test val is uneven
    // std::cout << size_t((2 * i + 1) * (__builtin_popcount(val) & 0b00000001)) << " ";
  }
  // std::cout << std::endl;

  free(table);
  free(x);
#endif
  // Debug
  /*
  for (size_t i = 0; i < MAX_KEYLENGTH; ++i) {
    std::cout << size_t(args.rotorShifts[i]) << " ";
  }
  std::cout << "\n\n";
  */
}
