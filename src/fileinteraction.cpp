#include "fileinteraction.hpp"

#include <cassert>
#include <iostream>
#include <stdlib.h>

#include "errors.hpp"
#include "measurement.hpp"

#include <iostream>

bool testForExistence(const char* filename) {
  bool result;
  FILE* file = fopen(filename, "rb");
  if (!file) {
    result = false;
  }
  else {
    result = true;
    fclose(file);
  }
  return result;
}

std::string findRotors(std::string path) {
  std::string rotorsFound = "";
  path += "rotor_";
  for (unsigned int i = 0; i < VALID_ROT_NAMES.size(); ++i) {
    std::string filename = path + VALID_ROT_NAMES[i];
    if (!testForExistence(filename.c_str())) {
      continue;
    }
    filename += "_reverse";
    if (!testForExistence(filename.c_str())) {
      continue;
    }
    rotorsFound += VALID_ROT_NAMES[i];
  }
  return rotorsFound;
}

void read_file(Data& bytes, const char* filename, const TuringaKey& key) {
  FILE* myfile = fopen(filename, "rb");
  if (!myfile) {
    throw FileNotFound("read_file", filename);
  }

  size_t size = 0;
  if (key.direction == 0) {
    int position = key.fileShift % bytes.size;
    size += fread(bytes.bytes + position, 1, bytes.size - position, myfile);
    size += fread(bytes.bytes, 1, position, myfile);
  }
  else {
    size = fread(bytes.bytes, 1, bytes.size, myfile);
  }
  fclose(myfile);
  assert(size == bytes.size && "Incomplete read of file!");
  std::cout << timestamp(current_duration()) << "File has been read from <" << filename << ">.\n";
}

void write_file(const Data& bytes, const char* filename, const TuringaKey& key) {
  FILE* myfile = fopen(filename, "wb");
  if (!myfile) {
    throw CannotCreateFile("write_file", filename);
  }
  if (key.direction == 0) {
    fwrite(bytes.bytes, 1, bytes.size, myfile);
  }
  else {
    int position = key.fileShift % bytes.size;
    fwrite(bytes.bytes + position, 1, bytes.size - position, myfile);
    fwrite(bytes.bytes, 1, position, myfile);
  }
  fclose(myfile);
  std::cout << timestamp(current_duration()) << "File has been written to <" << filename << ">.\n";
}

// read the Turinga key
TuringaKey readTuringaKey(const char* filename) {
  std::ifstream myfile(filename, std::ios::in);
  if (!myfile) {
    throw FileNotFound("readTuringaKey", filename);
  }

  int direction;
  size_t keylength;
  myfile >> direction;
  myfile >> keylength;

  std::vector<char> rotorNames(keylength);
  for (size_t i = 0; i < keylength; ++i) {
    myfile >> rotorNames[i];
  }

  size_t fileShift;
  myfile >> fileShift;

  ignore_byte(myfile);

  Byte* rotorShifts = (Byte*) malloc(MAX_KEYLENGTH);
  myfile.read((char*) rotorShifts, MAX_KEYLENGTH);
  assert(myfile.fail() == 0 && "Couldn't read correctly!");
  const TuringaKey key{direction, keylength, rotorNames, rotorShifts, fileShift};
  std::cout << timestamp(current_duration()) << "Turinga key has been read.\n";
  return key;
}

void writeTuringaKey(const std::string filename, const TuringaKey& key) {
  std::ofstream myfile(filename, std::ios::binary);
  if (!myfile) {
    throw CannotCreateFile("writeTuringaKey", filename);
  }
  myfile << key.direction << " " << key.length << " ";
  for (auto& character : key.rotorNames) {
    myfile << character;
  }
  myfile << " " << key.fileShift << std::endl;
  myfile.write((char*) key.rotorShifts, MAX_KEYLENGTH);
  assert(myfile.fail() == 0 && "Couldn't write correctly!");
  std::cout << timestamp(current_duration()) << "Turinga key has been written to <" << filename << ">.\n";
}

Byte* loadRotors(const TuringaKey& key, const char* rotDirectory) {
  std::string prefix(rotDirectory);
  prefix += "/rotor_";
  std::string suffix, filename;
  if (key.direction == 0) {
    suffix = "";
  }
  else if (key.direction == 1) {
    suffix = "_reverse";
  }

  Byte* wheels = (Byte*) malloc(256 * key.length);
  for (size_t i = 0; i < key.length; ++i) {
    if (key.direction == 0) {
      filename = prefix + key.rotorNames[i] + suffix;
    }
    else if (key.direction == 1) {
      filename = prefix + key.rotorNames[key.length - 1 - i] + suffix;
    }
    const char* new_filename = filename.c_str();
    FILE* myfile             = fopen(new_filename, "rb");
    if (!myfile) {
      throw FileNotFound("loadRotors", new_filename);
    }

// disable gcc warning -Wunsused-variable
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

    size_t size = fread(wheels + 256 * i, 1, 256, myfile);
    assert(size == 256 && "Rotrors must have size 256!");

// enable gcc warning -Wunused-variable
#pragma GCC diagnostic pop

    fclose(myfile);
  }

  std::cout << timestamp(current_duration()) << "Rotors have been loaded.\n";
  return wheels;
}
