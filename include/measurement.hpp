#pragma once

/*! \file measurement.hpp */

#include <chrono>
#include <stdlib.h>
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
