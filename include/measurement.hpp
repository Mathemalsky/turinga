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

/*! \file measurement.hpp */

#include <chrono>
#include <string>

/** time_point acrynom for timepoint from std:.chrono */
using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

/*!
 * \brief initializes the global variable startTime
 */
void start_time();

/*!
 * \brief computes the difference between the startTime and now
 * \details The time is rounded to microseconds.
 * \return double that contains the time in seconds
 */
double current_duration();

/*!
 * \brief formats the elapsed time to be printed pretty to terminal
 * \details The timestamp output will look like this: [  0.256458s].
 * \param duration time in seconds
 * \return time elapsed since program start formatted in a std::string.
 */
inline std::string timestamp(const double duration) {
  std::string timeStamp = "[";
  if (duration < 10) {
    timeStamp += " ";
  }
  timeStamp += std::to_string(duration);
  timeStamp += " s] ";
  return timeStamp;
}
