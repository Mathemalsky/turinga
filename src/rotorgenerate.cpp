#include "rotorgenerate.hpp"

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <time.h>

#include "colors.hpp"
#include "constants.hpp"
#include "measurement.hpp"
#include "types.hpp"

static Byte* order_256() {
  Byte* order = (Byte*) malloc(256);
  for (size_t i = 0; i < 256; ++i) {
    order[i] = i;
  }
  return order;
}

static Byte* permutaion(Byte* array) {
  size_t a;
  srand(time(NULL));
  for (size_t i = 0; i < 256; ++i) {
    a = rand() % (256 - i);
    std::swap(array[a], array[255 - i]);
  }
  return array;
}

static Byte* invert(const Byte* perm) {
  Byte* inv_perm = (Byte*) malloc(256);
  for (size_t i = 0; i < 256; ++i) {
    inv_perm[perm[i]] = i;
  }
  return inv_perm;
}

void generateRotor(const char* rotorNames) {
  Byte* perm     = permutaion(order_256());
  Byte* inv_perm = invert(perm);
  std::string str_rotorNames(rotorNames);
  if (str_rotorNames.length() <= 10) {
    print_yellow("WARNING: ");
    std::cout << "You're about to generate a very weak key!\n";
  }
  for (size_t i = 0; i < str_rotorNames.length(); ++i) {
    std::string name = STD_ROT_DIR + "rotor_" + str_rotorNames[i];
    FILE* myfile     = fopen(name.c_str(), "wb");
    fwrite(perm, 1, 256, myfile);
    fclose(myfile);
    name += "_reverse";
    myfile = fopen(name.c_str(), "wb");
    fwrite(inv_perm, 1, 256, myfile);
    fclose(myfile);
  }
  free(perm);
  free(inv_perm);
  std::cout << timestamp(current_duration())
            << "Rotors with the following names have been generated: <" << str_rotorNames << ">\n";
}
