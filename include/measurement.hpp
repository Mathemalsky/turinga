#ifndef MEASUREMENT_HPP
#define MEASUREMENT_HPP

#include <chrono>
#include <stdlib.h>
#include <string>

using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

void start_time();
double current_duration();

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
