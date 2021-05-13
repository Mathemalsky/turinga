
#ifndef __AVX2__
#include "encrypt.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <vector>

#include <csprng.hpp>

#include "fileinteraction.hpp"
#include "mainerror.hpp"
#include "measurement.hpp"
#include "turinga.hpp"

using Byte  = unsigned char;
using Bytes = std::vector<unsigned char>;

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
    x[2 * i]     = rotorShifts[i] & 0b00001111;  // the rightmost bits WE MAY HAVE TO SWAP?
    x[2 * i + 1] = rotorShifts[i] >> 4;          // the leftmost bits
  }

  for (size_t i = 0; i < length; i++) {
    Byte val = table[x[i]];
    val ^= x[2 * length - 1 - i];  // bitwise xor
    rotorShifts[i] += (2 * i + 1) * (__builtin_popcount(val) & 0b00000001);
  }

  // Debug
  /*
  for (size_t i = 0; i < length; ++i) {
    std::cout << size_t(rotorShifts[i]) << " ";
  }
  std::cout << "\n";
  */

  free(table);
}

// encrypts/ decrypts the files
void encrypt(Data& bytes, TuringaKey& key, const Byte* rotors) {
  // maybe other numbers of threads would be more efficient
  const size_t threadcount = std::thread::hardware_concurrency();  // number of logical processors
  std::cout << timestamp(current_duration()) << threadcount << " logical processors detected.\n";

  size_t begin = 0, end;
  end          = bytes.size / threadcount;

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
      rotors, begin, end));
    // prepair for next thread
    for (size_t j = begin; j < end; ++j) {  // rotate to start of next thread
      rotate(rotorShiftsAry[i + 1], key.length);
    }
    begin = end;
    end += bytes.size / threadcount;
  }

  // encrypt the rest
  encrypt_block(
    bytes,
    TuringaKey{key.direction, key.length, key.rotorNames, rotorShiftsAry[threadcount - 1],
               key.fileShift},
    rotors, begin, bytes.size);

  // collect all threads
  for (std::thread& thr : threads) {
    thr.join();
  }

  // free all memory
  for (Byte*& rotShi : rotorShiftsAry) {
    free(rotShi);
  }

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

#endif
