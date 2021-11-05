#include "colors.hpp"

#include <iostream>

void print_lightgreen(std::string word) {
  std::cout << "\033[1;32m";
  std::cout << word;
  std::cout << "\033[0m";
}

void print_lightred(std::string word) {
  std::cout << "\033[1;31m";
  std::cout << word;
  std::cout << "\033[0m";
}

void print_yellow(std::string word) {
  std::cout << "\033[1;33m";
  std::cout << word;
  std::cout << "\033[0m";
}
