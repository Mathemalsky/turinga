/*
 * Turinga is a simple symmetric encryption scheme based on ideas from enigma.
 * Copyright (C) 2021  Mathemalsky, MilchRatchet
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
/*! \file colors.hpp */

#ifdef _WIN32
#include <windows.h>
void enable_windows_virtual_terminal_sequence() {
  HANDLE hOut  = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
}
#else
#define enable_windows_virtual_terminal_sequence()
#endif /* _WIN32 */

#include <string>

void print_lightgreen(std::string word) noexcept; /**< \brief prints word in light green to console */
void print_lightred(std::string word) noexcept;   /**< \brief prints word in light red to console */
void print_yellow(std::string word) noexcept;     /**< \brief prints word in yellow to console */
