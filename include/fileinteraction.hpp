#ifndef FILEINTERACTION_HPP
#define FILEINTERACTION_HPP

/*! \file fileinteraction.hpp */

#include <fstream>
#include <vector>

#include "turinga.hpp"

/** Limit key length a priori to dertermine space allocation at compile time. */
const size_t MAX_KEYLENGTH = 32;

#ifdef _WIN32
/**< \def When compiled on windows, use different way to get the filesize. This is because
 * std::filesystem contains c++17 features not yet supportet by windows compilers.
 */
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
/** \def Assuming we are running on Linux we can use std::filesystem and it's c++17 features. */
#else
#include <experimental/filesystem>
/*!
 * \brief detects how large the file is
 * \param filename name of the considered file
 * \return a size_t that is the size of the file in bytes
 */
inline size_t file_size(const char* filename) {
  std::experimental::filesystem::path p{filename};
  return std::experimental::filesystem::file_size(p);
}
#endif

/*!
 * \brief reads one byte from file an ignore it
 * \details This function is designed to ommit space or enter bytes which separetes data in file.
 * \param myfile determines from which file to read.
 */
inline void ignore_byte(std::istream& myfile) {
  Byte bin;
  myfile.read(reinterpret_cast<char*>(&bin), sizeof(bin));
}

/*!
 * \brief reads the content of file into bytes
 * \details In case the direction is encrypt the fileShift from TuringaKey will be applied.
 * \param bytes a struct of type Data storing the files information
 * \param filename name of the file to be read
 * \param key the TuringaKey which will be apllied by reading the file
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
 */
TuringaKey readTuringaKey(const char* filename);

/*!
 * \brief writes a TuringaKey to file with given filename
 * \param filename name of the file where the TuringaKey should be saved
 * \param key the TuringaKey wich should be written
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

#endif  // FILEINTERACTION_HPP
