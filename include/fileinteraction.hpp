/*
 * Turinga is a simple symmetric encryption scheme based on ideas from enigma.
 * Copyright (C) 2022  Mathemalsky, MilchRatchet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

/*! \file fileinteraction.hpp */

#include <fstream>
#include <string>

#include "constants.hpp"
#include "errors.hpp"
#include "types.hpp"

/** \def Assuming we are compiling with at least gcc 8.0 we can use std::filesystem and it's c++17
 * features. */
#if defined(__GNUC)
#include <features.h>
#if __GNUC_PREREQ(8, 0)
#include <filesystem>
/*!
 * \brief detects how large the file is
 * \param filename name of the considered file
 * \return a size_t that is the size of the file in bytes
 */
inline size_t file_size(const char* filename) {
  std::filesystem::path p{filename};
  return std::filesystem::file_size(p);
}
#else
/*!
 * \brief detects how large the file is
 * \param filename Name of the considered file.
 * \return size_t that is the size of the file in bytes.
 */
inline size_t file_size(const char* filename) {
  std::ifstream myfile(filename, std::ios::binary);
  if (!myfile) {
    throw FileNotFound(std::string(filename), "file_size");
  }
  myfile.seekg(0, std::ios::end);
  const size_t size = myfile.tellg();
  return size;
}
#endif
#else
/*!
 * \brief detects how large the file is
 * \param filename Name of the considered file.
 * \return A size_t that is the size of the file in bytes.
 */
inline size_t file_size(const char* filename) {
  std::ifstream myfile(filename, std::ios::binary);
  myfile.seekg(0, std::ios::end);
  const size_t size = myfile.tellg();
  return size;
}
#endif

/*!
 * \brief ignore_byte reads one byte from file an ignores it
 * \details This function is designed to ommit space or enter bytes which separetes data in file.
 * \param myfile determines from which file to read.
 */
inline void ignore_byte(std::istream& myfile) {
  Byte bin;
  myfile.read(reinterpret_cast<char*>(&bin), sizeof(bin));
}

/*!
 * \brief handleCrypt wraps up several function calls for encryption or decryption
 * \param filename file to be encrypted/ decrypted
 * \param outputfilename file where the output should be saved
 * \param rotDirectory directory which contains the rotorfiles used by the key
 * \param key defines the encryption
 */
void handleCrypt(const char* filename, const char* outputfilename, const char* rotDirectory, TuringaKey key);

/*!
 * \brief testForExistence tests a file of given name exists or not
 * \param filename name of the file to test
 * \return true if a file with the name filename exists, otherwise false
 */
bool testForExistence(const char* filename);

/*!
 * \brief findRotors detects the existence of rotorfiles
 * \param path specifies the directory to search for rotors
 * \return list of rotors found at the location given by path
 */
std::string findRotors(std::string path = STD_ROT_DIR);

/*!
 * \brief reads the content of file into bytes
 * \details In case the direction is encrypt the fileShift from TuringaKey will be applied.
 * \param bytes a struct of type Data storing the files information
 * \param filename name of the file to be read
 * \param key the TuringaKey which will be applied by reading the file
 */
void read_file(Data& bytes, const char* filename, const TuringaKey& key);

/*!
 * \brief writes the data from bytes to into a file
 * \details In case the direction is decrypt the invers of the fileShift from TuringaKey will be
 * applied.
 * \param bytes a struct of type Data storing the files information
 * \param filename name of the file to be written
 * \param key the TuringaKey which will be apllied by writing the file
 */
void write_file(const Data& bytes, const char* filename, const TuringaKey& key);

/*!
 * \brief reads a TuringaKey from given file
 * \param filename Name of the file where the TuringaKey is saved.
 * \return TuringaKey
 * \details The first bit of the first byte contains information about the direction. The other 7 bits encode the length
 * of the key. But note that the maximum keylength is 32.
 * The next 32 bytes contain the initial rotorshifts where the rotation starts.
 * The next bytes store the file shift.
 * Finally the last bytes specify the used rotors.
 */
TuringaKey readTuringaKey(const char* filename);

/*!
 * \brief writes a TuringaKey to file with given filename
 * \param filename name of the file where the TuringaKey should be saved
 * \param key the TuringaKey which should be written
 */
void writeTuringaKey(const std::string filename, const TuringaKey& key);

/*!
 * \brief loads the rotors from files in rotDirectory
 * \details If a rotor is used multiple times it's data will be loaded multiple times by this
 * function.
 * In case the direction is decrypt the invers rotors will be loaded and they will be loaded in
 * reverse order.
 * \param key determines which rotors should be loaded
 * \param rotDirectory specifies the directory where the rotor files are stored
 * \return an array which containes the data from the loaded rotors
 */
Byte* loadRotors(const TuringaKey& key, const char* rotDirectory);
