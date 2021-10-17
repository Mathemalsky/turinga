#include "turinga.hpp"

#include <cmath>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <csprng.hpp>

#include "fileinteraction.hpp"
#include "mainerror.hpp"
#include "measurement.hpp"
#include "rotate.hpp"
#include "types.hpp"

using Byte  = unsigned char;
using Bytes = std::vector<unsigned char>;

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
  Byte* reverseOrder = (Byte*) malloc(32);
#define place(val) ((val) & -(0 < (val)))
  for (size_t i = 0; i < 32; ++i) {
    reverseOrder[i] = place(key.length - i - 1);
  }
#undef place

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
      rotors, begin, end, reverseOrder));
    // prepair for next thread
    RotateArgs args{rotorShiftsAry[i + 1], key.length, reverseOrder};
    for (size_t j = begin; j < end; ++j) {  // rotate to start of next thread
      rotate(args);
    }
    begin = end;
    end += bytes.size / threadcount;
  }

  // encrypt the rest
  encrypt_block(
    bytes,
    TuringaKey{
      key.direction, key.length, key.rotorNames, rotorShiftsAry[threadcount - 1], key.fileShift},
    rotors, begin, bytes.size, reverseOrder);

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
  const Byte* reverseOrder) {
  const size_t keylength = key.length;
  RotateArgs args{key.rotorShifts, keylength, reverseOrder};

  // encryption
  if (key.direction == 0) {
    for (size_t i = begin; i < end; ++i) {
      Byte tmp = bytes.bytes[i];
      for (size_t i = 0; i < keylength; ++i) {
        tmp = rotors[256 * i + ((tmp + key.rotorShifts[i]) % 256)];
      }
      bytes.bytes[i] = tmp;
      rotate(args);
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
      rotate(args);
    }
  }
}
