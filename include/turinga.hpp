#ifndef TURINGA_H
#define TURINGA_H

/*!
 * \mainpage Turinga
 * \section sec1 What is Turinga?
 * Turinga is a simple program to encrypt and decrypt files.
 * \section sec2 What are the requirements of Turinga?
 * Since the function rotate uses AVX2 your processor needs to support AVX2 and you have to compile
 * with -march=native
 * \section sec3 What should I care about using Turinga?
 * \subsection sec3_1 Security aspects
 *  Don't use short keys! Use at least length 8!
 *
 * Don't encrypt data which is highly redundant. In that case security can by improved by a good
 * compression method
 * \subsection sec3_2 Performance aspects
 * Due to the actual implementation of
 * rotate the best security/runtime ration is reached if the key length is a multiple of 8
 * \section sec4 Historical notes
 * The idea of the encryption scheme comes from the enigma invented independently
 * by a few peole in 1919. The name Turinga is inspired by the great british mathematician Alan
 * Turing, who was one of the experts in Bletchley Park that achieved to break enigma during second
 * world war. Their work are the foundation of the improvements applied to enigma to obtain the
 * cryptographic scheme used in Turinga.
 */

#include <stddef.h>
#include <string>
#include <vector>

using Bytes = std::vector<unsigned char>;
using Byte  = unsigned char;

/*!
 * \struct Data
 * \brief The struct Data is a simple array and it's length.
 * \details It's intended to contain the complete information from a file.
 */
struct Data {
  Byte* bytes;       /**< an array of bytes (unsigned chars) */
  const size_t size; /**< the length of the arry */
};

/*!
 * \struct TuringaKey
 * \brief The struct TuringaKey contains all information that determines the key for turinga
 */
struct TuringaKey {
  int direction;       /**< It's value can be 0 or 1 where 0 stands for encrypt, 1 for decrypt. */
  const size_t length; /**< stores the number of rotors used in the key */
  const std::vector<char>
    rotorNames; /**< rotorNames It's a string that contains the names of the rotors. The i-th rotor
                   is represented by the i-th position in the the string. Hence the names of the
                   rotors can only differ in ine char */
  Byte* rotorShifts; /**< It's an array of bytes that stores current position of the wheels. This
                        will be changed by the rotate function after each encrypted Byte */
  const size_t fileShift; /**< the bytes will be shiftet (mod filesize) by fileShift */
};

void rotate(Bytes& fileShifts, const size_t length);
TuringaKey generateTuringaKey(const size_t keylength, const std::string& availableRotors);
void encrypt(Data& bytes, TuringaKey& key, const Byte* rotors);
void encrypt_block(
  Data& bytes, TuringaKey key, const Byte* rotors, const size_t begin, const size_t end);

#endif  // TURINGA_H
