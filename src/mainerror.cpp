#include "mainerror.hpp"

#include <iostream>
#include <string>

#include "colors.hpp"
#include "measurement.hpp"

void Mainerror::func(const std::string function) {
  p_function = function;
}

std::string Mainerror::func() const {
  return p_function;
}

File_not_found::File_not_found(std::string filename, std::string function) {
  p_filename = filename;
  func(function);
}

void File_not_found::report_error() {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "File <" << p_filename << "> in function <" << func() << "> not found.\n";
  exit(-1);
}

Cannot_create_file::Cannot_create_file(std::string filename, std::string function) {
  func(function);
  p_filename = filename;
}

void Cannot_create_file::report_error() {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Couldn't create output file <" << p_filename << "> in function <" << func()
            << ">.\n";
  exit(-1);
}

Inappropriate_number_of_arguments::Inappropriate_number_of_arguments(
  unsigned int number, unsigned int expectet, std::string function) {
  func(function);
  p_number   = number;
  p_expectet = expectet;
}

void Inappropriate_number_of_arguments::report_error() {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Inapropriate number of arguments in function <" << func() << ">. Got " << p_number
            << " but expectet " << p_expectet << ".\n";
  exit(-1);
}
