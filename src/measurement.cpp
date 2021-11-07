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
