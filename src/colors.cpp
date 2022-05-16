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
#include "colors.hpp"

#include <iostream>

void print_lightgreen(std::string word) noexcept {
  std::cout << "\033[1;32m";
  std::cout << word;
  std::cout << "\033[0m";
}

void print_lightred(std::string word) noexcept {
  std::cout << "\033[1;31m";
  std::cout << word;
  std::cout << "\033[0m";
}

void print_yellow(std::string word) noexcept {
  std::cout << "\033[1;33m";
  std::cout << word;
  std::cout << "\033[0m";
}
