#include "mainerror.hpp"

#include <iostream>
#include <string>

#include "colors.hpp"
#include "measurement.hpp"

void mainerror::set_func(std::string function) {
  p_function = function;
}

std::string mainerror::get_func() {
  return p_function;
}

file_not_found::file_not_found(std::string filename, std::string function) {
  p_filename = filename;
  set_func(function);
}

void file_not_found::report_error() {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "File <" << p_filename << "> in function <" << get_func() << "> not found.\n";
  exit(-1);
}

cannot_create_file::cannot_create_file(std::string filename, std::string function) {
  set_func(function);
  p_filename = filename;
}

void cannot_create_file::report_error() {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Couldn't create output file <" << p_filename << "> in function <" << get_func()
            << ">.\n";
  exit(-1);
}

inappropriate_number_of_arguments::inappropriate_number_of_arguments(
  unsigned int number, unsigned int expectet, std::string function) {
  set_func(function);
  p_number   = number;
  p_expectet = expectet;
}

void inappropriate_number_of_arguments::report_error() {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Inapropriate number of arguments in function <" << get_func() << ">. Got "
            << p_number << " but expectet " << p_expectet << ".\n";
  exit(-1);
}
