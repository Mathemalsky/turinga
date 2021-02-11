#include <cstddef>
#include <cstring>
#include <fstream>
#include <immintrin.h>

using Byte = unsigned char;

void rotate(Byte* rotorShifts, const size_t length) {
	__m256i_u b = _mm256_setzero_si256();

  __m256i_u a = _mm256_loadu_si256((__m256i_u*) rotorShifts);

  Byte last = 1;
  b                  = _mm256_insert_epi8(b, last, 0);
  last               = !((rotorShifts[0] + last) % 3);
  b                  = _mm256_insert_epi8(b, last, 1);
  last               = !((rotorShifts[1] + last) % 5);
  b                  = _mm256_insert_epi8(b, last, 2);
  last               = !((rotorShifts[2] + last) % 7);
  b                  = _mm256_insert_epi8(b, last, 3);
  last               = !((rotorShifts[3] + last) % 9);
  b                  = _mm256_insert_epi8(b, last, 4);
  last               = !((rotorShifts[4] + last) % 11);
  b                  = _mm256_insert_epi8(b, last, 5);
  last               = !((rotorShifts[5] + last) % 13);
  b                  = _mm256_insert_epi8(b, last, 6);
  last               = !((rotorShifts[6] + last) % 15);
  b                  = _mm256_insert_epi8(b, last, 7);
  if(length>8) {
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
    if(length>16) {
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
      if (length>24) {
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

int main(int argc, char** argv) {
	const char* filename(argv[1]);
	size_t keylength = atoi(argv[2]);
	size_t iterations = atoi(argv[3]);

	Byte* rotorShifts = (Byte*)malloc(keylength);
	Byte* diff				= (Byte*)malloc(keylength);

	for(size_t i=0; i<keylength; ++i) {
		rotorShifts[i] = 0;
	}

	std::ofstream myfile(filename, std::ios::binary);
	for(size_t i=0; i<iterations; ++i) {
		std::memcpy(diff, rotorShifts, keylength);
		rotate(rotorShifts, keylength);
		for(size_t j=0; j<keylength; ++j) {
			diff[i] = rotorShifts[i] - diff[i];
		}
		myfile.write((char*)diff, keylength);
	}

	free(rotorShifts);
	free(diff);

	return 0;
}