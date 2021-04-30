#pragma once

/*! \file encrypt_avx2.hpp */

#ifndef __AVX2__
#include "turinga.hpp"

/*!
 * \brief changes the substitution rule after each byte
 * \details This function replaces the rotation of wheels in enigma. This part is most critical for
 * security.
 * \authors Max, Jurek
 * \param fileShifts determines the current substitution
 * \param length number of rotors in use
 */
void rotate(Bytes& fileShifts, const size_t length);

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

#endif
