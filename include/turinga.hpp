#ifndef TURINGA_H
#define TURINGA_H

#include <stddef.h>
#include <string>
#include <vector>

using Bytes = std::vector<unsigned char>;
using Byte  = unsigned char;

/*!
 * \struct Data
 * \param bytes An array of bytes (unsigned chars)
 * \param size Length of the array
 */
struct Data {
  Byte* bytes;
  const size_t size;
};

/*!
 * \struct TuringaKey
 * \brief contains all information that determines the key for turinga
 * \param direction It's value can be 0 or 1 where 0 stands for encrypt, 1 for decrypt
 * \param length stores the number of rotors used in the key
 * \param rotorNames It's a string that contains the names of the rotors. The i-th rotor is
 * represented by the i-th position in the the string. Hence the names of the rotors can only differ
 * in ine char \param rotorShifts It's an array of bytes that stores current position of the wheels.
 * This will be changed by the rotate function after each encrypted Byte \param fileShift the bytes
 * aill be shiftet (mod filesize) by fileShift
 */
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
  Data& bytes, TuringaKey key, const Byte* rotors, const size_t begin, const size_t end);

#endif  // TURINGA_H
