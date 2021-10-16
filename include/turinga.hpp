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
