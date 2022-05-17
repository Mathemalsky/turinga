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

/*! \file measurement.hpp */

#include <chrono>
#include <cmath>
#include <string>

/** time_point acrynom for timepoint from std:.chrono */
using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

extern time_point START_TIME; /**< global variable which holds the time when the program started **/

/*!
 * \brief initializes the global variable startTime
 */
inline void start_time() noexcept {
  START_TIME = std::chrono::high_resolution_clock::now();
}

/*!
 * \brief computes the difference between the startTime and now
 * \details time is rounded to microseconds.
 * \return double that contains the time in seconds
 */
inline double current_duration() noexcept {
  const time_point splitTime = std::chrono::high_resolution_clock::now();
  double timeSpan            = std::chrono::duration_cast<std::chrono::microseconds>(splitTime - START_TIME).count();
  timeSpan                   = double(std::round(timeSpan)) / 1000000;
  return timeSpan;
}

/*!
 * \brief formats the elapsed time to be printed pretty to terminal
 * \details The timestamp output will look like this: [  0.256458s].
 * \param duration time in seconds
 * \return time elapsed since program start formatted in a std::string.
 */
inline std::string timestamp(const double duration) noexcept {
  std::string timeStamp = "[";
  if (duration < 10) {
    timeStamp += " ";
  }
  timeStamp += std::to_string(duration);
  timeStamp += " s] ";
  return timeStamp;
}
