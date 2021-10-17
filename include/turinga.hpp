#pragma once

/*! \file turinga.hpp */

/*!
 * \mainpage Turinga
 * \section sec1 What is Turinga?
 * Turinga is a simple program to encrypt and decrypt files. It's a polyalphabetic byte wise
 * substitution cipher. Hence it's a symmetric encryption scheme.
 * \section sec2 What are the requirements of Turinga?
 * Since the function rotate uses AVX2 your processor needs to support AVX2 and you have to compile
 * with -march=native
 * \section sec3 What should I care about using Turinga?
 * \subsection sec3_1 Security aspects
 * Don't use short keys! Use at least length 8!
 *
 * Don't encrypt data which is highly redundant! In that case security can by improved by a good
 * compression method.
 * \subsection sec3_2 Performance aspects
 * Due to the actual implementation of
 * rotate the best security/runtime ration is reached if the key length is a multiple of 8
 * \section sec4 Historical notes
 * The idea of the encryption scheme comes from the enigma invented independently
 * by a few people between 1915 and 1919. The name Turinga is inspired by the great british
 * mathematician Alan Turing, who was one of the experts in Bletchley Park that achieved to break
 * enigma during second world war. Their work is the foundation of the improvements applied to
 * enigma to obtain the cryptographic scheme used in Turinga.
 */

#include <cstddef>
#include <string>


#include "types.hpp"

using Byte  = unsigned char;     /**< To simplify expressions use the intuitive definition. */
using Bytes = std::vector<Byte>; /**< A vector of type Byte are Bytes. */

/*!
 * \struct Data
 * \brief The struct Data is a simple array and it's length.
 * \details It's intended to contain the complete information from a file.
 */
struct Data {
  Byte* bytes;       /**< an array of bytes (unsigned chars) */
  const size_t size; /**< the length of the array */
};

/*!
 * \struct TuringaKey
 * \brief The struct TuringaKey contains all information that determines the key for turinga.
 */
struct TuringaKey {
  int direction;       /**< It's value can be 0 or 1 where 0 stands for encrypt, 1 for decrypt. */
  const size_t length; /**< stores the number of rotors used in the key */
  const std::vector<char>
    rotorNames; /**< rotorNames It's a string that contains the names of the rotors. The i-th rotor
                   is represented by the i-th position in the string. Hence the names of the
                   rotors can only differ in one char */
  Byte* rotorShifts; /**< It's an array of bytes that stores current position of the wheels. This
                        will be changed by the rotate function after each encrypted Byte */
  const size_t fileShift; /**< the bytes will be shifted (mod filesize) by fileShift */
};

/*!
 * \brief changes the substitution rule after each byte
 * This function maps the first n := length Bytes of rotorShifts to a n byte vector which will re-
 * place the rotorShifts in the next step.
 * Let's consider this n Bytes as vector in \f$\mathbb{F}_2^{8n}\f$.
 * Now we cut this vector into 2n vectors of length 4.
 * So if r is the vector of the first n bytes represented as row vector of bits we define
 * \f{equation*}{ (v_0,...,v_{2n-1}) := r\f}
 * Furthermore we define a binary vector s of length n by
 * \f{equation*} s_i := <v_i, \pi(v_{2n-1-i})> \qquad i \in \{0,...,n-1\}\f}, where
 * \f$\pi\f$ is the bijective mapping from \f$\mathbb{F}_2^4\f$ to \f$\mathbb{F}_2^4\f$
 * which we obtain by considering the input vector as a coefficient vector of a binary
 * polynomial \f$g\f$ with \f$\deg(g) < 4\f$.
 * Then the output vector is the coeficcient vector of \f{equation*}{g^{-1}(x) \mod x^4 + x + 1\f}
 *
 * \authors Max, Jurek
 * \param fileShifts determines the current substitution
 * \param length number of rotors in use
 * \details This function replaces the rotation of wheels in enigma. This part is most critical for
 * security.
 */
void rotate(Bytes& fileShifts, const size_t length);

/*!
 * \brief generates a pair of keys for encryption and decryption
 * \param keylength length of the key in use
 * \param availableRotors a std::string that represents the rotors, that can be used
 * \return struct of type TuringaKey containing all infomation about the key
 */
TuringaKey generateTuringaKey(const size_t keylength, const std::string& availableRotors);

/*!
 * \brief encrypts or decrypts the data
 * \details The function iterates through the data and through the rotors and performs the
 * substitution for each rotor separately.
 * \param bytes data to be encrypted/ decrypted
 * \param key key used for encryption/ decryption
 * \param rotors stores the rotors (byte permutations) used
 */
void encrypt(Data& bytes, TuringaKey& key, const Byte* rotors);

/*!
 * \brief does the same as encrypt, but only from position begin to position end
 * \details This function is designed to split the data and encrypt/ decrypt it parallel in several
 * threads.
 * \param bytes data from were a block should be encrypted or decrypted
 * \param key key used for encryption/ decryption
 * \param rotors stores the rotors (byte permutations) used
 * \param begin position to begin the encryption/ decryption; Position begin is included.
 * \param end position to end the encryption/ decryption; Position end is excluded.
 */
void encrypt_block(
  Data& bytes, TuringaKey key, const Byte* rotors, const size_t begin, const size_t end);
