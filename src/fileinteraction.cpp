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
#include "fileinteraction.hpp"

#include <cassert>
#include <iostream>
#include <stdlib.h>

#include "errors.hpp"
#include "measurement.hpp"
#include "turinga.hpp"

#include <iostream>

void handleCrypt(const char* filename, const char* outputfilename, const char* rotDirectory, TuringaKey key) {
  Byte* rotors          = loadRotors(key, rotDirectory);
  const size_t fileSize = file_size(filename);
  Byte* data            = (Byte*) malloc(fileSize);
  Data bytes{data, fileSize};
  read_file(bytes, filename, key);
  encrypt(bytes, key, rotors);
  write_file(bytes, outputfilename, key);

  free(rotors);
  free(bytes.bytes);
  freeTuringaKey(key);
}

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
  if (key.direction == encryption) {
    int position = key.fileShift % bytes.size;
    size += fread(bytes.bytes + position, 1, bytes.size - position, myfile) * sizeof(Byte);
    size += fread(bytes.bytes, 1, position, myfile) * sizeof(Byte);
  }
  else {
    size = fread(bytes.bytes, 1, bytes.size, myfile) * sizeof(Byte);
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
  if (key.direction == encryption) {
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
  FILE* myfile = fopen(filename, "rb");
  if (!myfile) {
    throw FileNotFound("readTuringaKey", filename);
  }

  unsigned int size = 0;
  Byte init;
  size += fread(&init, sizeof(Byte), 1, myfile) * sizeof(Byte);
  Byte dir       = init & 0b10000000;
  Byte keylength = init & 0b01111111;

  Direction direction = (dir == 0) ? encryption : decryption;

  char* rotorNames = (char*) malloc(keylength);
  size += fread(rotorNames, sizeof(char), keylength, myfile) * sizeof(char);

  size_t fileShift;
  size += fread(&fileShift, sizeof(size_t), 1, myfile) * sizeof(size_t);

  Byte* rotorShifts = (Byte*) malloc(MAX_KEYLENGTH);
  size += fread(rotorShifts, sizeof(Byte), MAX_KEYLENGTH, myfile) * sizeof(Byte);

  readKeyWarning(size, 1 + MAX_KEYLENGTH + sizeof(size_t) + (unsigned int) keylength);

  fclose(myfile);
  const TuringaKey key{direction, keylength, rotorNames, rotorShifts, fileShift};
  std::cout << timestamp(current_duration()) << "Turinga key has been read.\n";
  return key;
}

void writeTuringaKey(const std::string filename, const TuringaKey& key) {
  FILE* myfile = fopen(filename.c_str(), "wb");
  if (!myfile) {
    throw CannotCreateFile("writeTuringaKey", filename);
  }
  Byte init = (key.direction == encryption) ? 0b00000000 : 0b10000000;
  init += key.length;  // assemble infor for direction and length into one byte
  fwrite(&init, sizeof(Byte), 1, myfile);
  fwrite(key.rotorNames, sizeof(char), key.length, myfile);
  fwrite(&key.fileShift, sizeof(size_t), 1, myfile);
  fwrite(key.rotorShifts, sizeof(Byte), MAX_KEYLENGTH, myfile);
  fclose(myfile);
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
    if (key.direction == encryption) {
      filename = prefix + key.rotorNames[i] + suffix;
    }
    else if (key.direction == decryption) {
      filename = prefix + key.rotorNames[key.length - 1 - i] + suffix;
    }
    const char* new_filename = filename.c_str();
    FILE* myfile             = fopen(new_filename, "rb");
    if (!myfile) {
      throw FileNotFound("loadRotors", new_filename);
    }

    size_t size = fread(wheels + 256 * i, 1, 256, myfile);
    assert(size == 256 && "Rotors must have size 256!");
    fclose(myfile);
  }

  std::cout << timestamp(current_duration()) << "Rotors have been loaded.\n";
  return wheels;
}
