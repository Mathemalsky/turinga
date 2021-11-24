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
#include "measurement.hpp"

#include <cmath>

using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
time_point startTime{};

void start_time() {
  startTime = std::chrono::high_resolution_clock::now();
}

// returns time in seconds since the programm has started
double current_duration() {
  const time_point splitTime = std::chrono::high_resolution_clock::now();
  double timeSpan            = std::chrono::duration_cast<std::chrono::microseconds>(splitTime - startTime).count();
  timeSpan                   = double(std::round(timeSpan)) / 1000000;
  return timeSpan;
}
