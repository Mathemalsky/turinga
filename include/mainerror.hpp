#ifndef MAINERROR_HPP
#define MAINERROR_HPP

#include <string>

class Mainerror {
public:
  virtual void report_error() = 0;
  void set_func(std::string function);
  std::string get_func();
  virtual ~Mainerror() {
  }

private:
  std::string p_function;
};

class File_not_found : public Mainerror {
public:
  File_not_found(std::string filename, std::string function);
  void report_error();

private:
  std::string p_filename;
};

class Cannot_create_file : public Mainerror {
public:
  Cannot_create_file(std::string filename, std::string function);
  void report_error();

private:
  std::string p_filename;
};

class Inappropriate_number_of_arguments : public Mainerror {
public:
  Inappropriate_number_of_arguments(
    unsigned int number, unsigned int expectet, std::string function);
  void report_error();

private:
  unsigned int p_number;
  unsigned int p_expectet;
};
#endif  // MAINERROR_HPP
