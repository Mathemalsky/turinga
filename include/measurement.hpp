#ifndef MEASUREMENT_HPP
#define MEASUREMENT_HPP

/*! \file measurement.hpp */

#include <chrono>
#include <stdlib.h>
#include <string>

/** acrynom for timepoint from std:.chrono */
using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

/*!
 * \brief start_time initialize the global variable startTime
 */
void start_time();

/*!
 * \brief current_duration computes the difference between the startTime and now
 * \details the time is rounded to microseconds
 * \return double that contains the time in seconds
 */
double current_duration();

/*!
 * \brief timestamp formats the elapsed time to be printed pretty to terminal
 * \details The timestamp output will look like this: [  0.256458s].
 * \param duration time in seconds
 * \return Time elapsed since program start formated in a std::string.
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

#endif  // MEASUREMENT_HPP
