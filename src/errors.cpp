#include "errors.hpp"

#include <iostream>

#include "colors.hpp"
#include "constants.hpp"
#include "measurement.hpp"

InappropriateNumberOfArguments::InappropriateNumberOfArguments(
  unsigned int number, unsigned int expected, std::string function) {
  p_number   = number;
  p_expected = expected;
  p_func     = function;
}

void InappropriateNumberOfArguments::what() {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Inapropriate number of arguments in function <" << p_func << ">. Got " << p_number
            << " but expectet " << p_expected << ".\n";
  std::cout << "Type <./" << PROJECTNAME << " help> for syntax help.\n";
  exit(-1);
}

FileNotFound::FileNotFound(std::string filename, std::string function) {
  p_filename = filename;
  p_func     = function;
}

void FileNotFound::what() {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "File <" << p_filename << "> in function <" << p_func << "> not found.\n";
  exit(-1);
}

CannotCreateFile::CannotCreateFile(std::string filename, std::string function) {
  p_func     = function;
  p_filename = filename;
}

void CannotCreateFile::what() {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Couldn't create output file <" << p_filename << "> in function <" << p_func
            << ">.\n";
  exit(-1);
}

void syntax() {
  std::cout << "Syntax\n ======\n";
  std::cout << "./" << PROJECTNAME << "<argument1> <argument2> ...\n";
  std::cout << "Valid options are:\n";
  syntaxCrypt();
  syntaxGenerate();
  syntaxHelp();
}

void syntaxCrypt() {
  std::cout << "- ./" << PROJECTNAME << " <input_file> <key> <rotors> <output_file>\n";
  std::cout << "    input_filename  : path and filename (with ending) of the file to be "
               "encrypted/decrypted\n";
  std::cout << "    key             : path and filename (with ending) of the key used to be "
               "encrypt/decrypt\n";
  std::cout << "    rotors          : path to the directory where the rotor files are stored\n";
  std::cout << "    output_filename : path and filename (with ending) to write the file into\n";
}

void syntaxGenerate() {
  std::cout << "- ./" << PROJECTNAME << " <file> <length> <rotors>\n";
  std::cout << "    file            : path and filename (without ending) to write the files into\n";
  std::cout << "    length          : length of the key, recommended to use length 8 or larger\n";
  std::cout << "    rotors          : all Names of Rotornames (1 character each) without spaces\n";
}

void syntaxHelp() {
  std::cout << "- ./" << PROJECTNAME << " <command>\n";
  std::cout << "    command         : command you want to see detailed information about\n";
  std::cout << "                      options are: <crypt>, <generate> and <help>\n";
}
