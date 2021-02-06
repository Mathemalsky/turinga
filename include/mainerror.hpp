#ifndef MAINERROR_HPP
#define MAINERROR_HPP

#include <string>

class mainerror {
public:
    virtual void report_error() = 0;
    void set_func(std::string function);
    std::string get_func();
    virtual ~mainerror() {}
private:
    std::string p_function;
};

class file_not_found : public mainerror {
public:
    file_not_found(std::string filename, std::string function);
    void report_error();
private:
    std::string p_filename;
};

class cannot_create_file : public mainerror {
public:
    cannot_create_file(std::string filename, std::string function);
    void report_error();
private:
    std::string p_filename;
};

class inappropriate_number_of_arguments : public mainerror {
public:
    inappropriate_number_of_arguments(unsigned int number, unsigned int expectet, std::string function);
    void report_error();
private:
    unsigned int p_number;
    unsigned int p_expectet;
};
#endif // MAINERROR_HPP
