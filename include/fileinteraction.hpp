#ifndef FILEINTERACTION_HPP
#define FILEINTERACTION_HPP

#include <fstream>
#include <vector>

#include "turinga.hpp"

const size_t MAX_KEYLENGTH = 32;

#ifdef _WIN32
// when compiled on windows, use different way to get the filesize
// necessary because c++17 compilers are not yet available for windows
inline size_t file_size(const char* filename) {
  std::ifstream myfile(filename, std::ios::binary);
  myfile.seekg(0, std::ios::end);
  const size_t size = myfile.tellg();
  return size;
}
#else
// assuming we are running on Linux
#include <filesystem>
inline size_t file_size(const char* filename) {
  std::filesystem::path p{filename};
  return std::filesystem::file_size(p);
}
#endif

// ignore exactly one byte from the input file
inline void ignore_byte(std::istream & myfile) {
    Byte bin;
    myfile.read(reinterpret_cast<char*>(&bin),sizeof(bin));
}

void read_file(Data& bytes, const char* filename, const TuringaKey& key);
void write_file(const Data &bytes, const char* filename, const TuringaKey& key);

TuringaKey readTuringaKey(const char* filename);
void writeTuringaKey(const std::string filename, const TuringaKey& key);

Byte *loadRotors(const TuringaKey &key, const char* rotDirectory);

#endif // FILEINTERACTION_HPP
