/*
 * Turinga is a simple symmetric encryption scheme based on ideas from enigma.
 * Copyright (C) 2022 Mathemalsky, MilchRatchet
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
#include "errors.hpp"

#include <cstring>
#include <iostream>

#include "colors.hpp"
#include "constants.hpp"
#include "measurement.hpp"

/***********************************************************************************************************************
 *                                                 Error handling                                                      *
 **********************************************************************************************************************/
InappropriateNumberOfArguments::InappropriateNumberOfArguments(
  std::string function, unsigned int expected, unsigned int number)
  : p_expected(expected), p_number(number) {
  p_func = function;
}

const char* InappropriateNumberOfArguments::what() const noexcept {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Inapropriate number of arguments in function <" << p_func << ">. Got " << p_number << " but expectet "
            << p_expected << ".\n";
  std::cout << "Type " << EXECUTE << " <help> for syntax help.\n";
  exit(-1);
}

InvalidArgument::InvalidArgument(std::string function, std::string arg, std::string setting)
  : p_arg(arg), p_setting(setting) {
  p_func = function;
}

const char* InvalidArgument::what() const noexcept {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Argument <" << p_arg << "> is invaild " << p_setting << ".\n";
  std::cout << "Type " << EXECUTE << " <help> for syntax help.\n";
  exit(-1);
}

FileNotFound::FileNotFound(std::string function, std::string filename) : p_filename(filename) {
  p_func = function;
}

const char* FileNotFound::what() const noexcept {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "File <" << p_filename << "> in function <" << p_func << "> not found.\n";
  exit(-1);
}

CannotCreateFile::CannotCreateFile(std::string function, std::string filename) : p_filename(filename) {
  p_func = function;
}

const char* CannotCreateFile::what() const noexcept {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Couldn't create output file <" << p_filename << "> in function <" << p_func << ">.\n";
  exit(-1);
}

NoKey::NoKey(std::string function, std::string filename) : p_filename(filename) {
  p_func = function;
}

const char* NoKey::what() const noexcept {
  std::cout << timestamp(current_duration());
  print_lightred("ERROR: ");
  std::cout << "Couldn't find key file <" << p_filename << "> in function <" << p_func << ">.\n";
  exit(-1);
}

/***********************************************************************************************************************
 *                                                  syntax help                                                        *
 **********************************************************************************************************************/
void syntax() {
  std::cout << "Syntax\n======\n";
  std::cout << EXECUTE << " <argument1> <argument2> ...\n";
  std::cout << "Valid options are:\n";
  syntaxCrypt();
  syntaxGenerateKey();
  syntaxGenerateRotors();
  syntaxHelp();
}

void syntaxCrypt() {
  std::cout << "- " << EXECUTE << " crypt <input_file> <key> <rotors> <output_file>\n";
  std::cout << "    input_file  : path and filename (with ending) of the file to be "
               "encrypted/decrypted\n";
  std::cout << "    key         : path and filename (without ending) of the key used to be "
               "encrypt/decrypt\n";
  std::cout << "    rotors      : path to the directory where the rotor files are stored\n";
  std::cout << "    output_file : path and filename (with ending) to write the file into\n";
  std::cout << "- " << EXECUTE << " <input_file>\n";
  std::cout << "    input_file  : path and filename (with ending) of the file to be encrypted\n";
  std::cout << "                  <key> is assumed to be default, which is <" << STD_KEY_DIR << STD_KEY << ">.\n";
  std::cout << "                  If this file does not exist, a new pair of keys potentially "
               "using all rotors found in <"
            << STD_ROT_DIR << "> is generated in this directory.\n";
  std::cout << "                  If <" << STD_ROT_DIR
            << "> is empty all Rotors with valid names will be generated there in before generating the keys.\n";
  std::cout << "                  <output_file> is assumed to be <input_file> appended by the suffix <.tur>.\n";
  std::cout << "- " << EXECUTE << " -d <input_file>\n";
  std::cout << "    input_file  : path and filename (with ending) of the file to be decrypted\n";
  std::cout << "                  <key> is assumed to be default, which is <" << STD_KEY_DIR << STD_KEY_INV << ">.\n";
  std::cout << "                  <output_file> is assumed to be <input_file> without the suffix <.tur>.\n";
}

void syntaxGenerateKey() {
  std::cout << "- " << EXECUTE << " genKey <file> <length> <rotors>\n";
  std::cout << "    file        : path and filename (without ending) to write the files into\n";
  std::cout << "    length      : length of the key, recommended to use length 8 or larger\n";
  std::cout << "    rotors      : all names of available rotornames (1 character each) without spaces\n";
  std::cout << "- " << EXECUTE << " genKey <file> <length>\n";
  std::cout << "    file        : path and filename (without ending) to write the files into\n";
  std::cout << "    length      : length of the key, recommended to use length 8 or larger\n";
  std::cout << "                  Assumes <rotors> to be all rotors that can be found in <" << STD_ROT_DIR << ">.\n";
  std::cout << "                  If <" << STD_ROT_DIR << "> is empty generate all rotors with valid names in there.\n";
}

void syntaxGenerateRotors() {
  std::cout << "- " << EXECUTE << " genRot <rotor names> <seed>\n";
  std::cout << "    rotor names : string of all rotor names (1 character each) to be generated.\n";
  std::cout << "    seed        : unsigned integer used as seed for generation\n";
  std::cout << "- " << EXECUTE << " genRot <rotor names>\n";
  std::cout << "                  use seed 0 for generation\n";
  std::cout << "- " << EXECUTE << " genRot\n";
  std::cout << "                  generate all valid rotors with default seed 0\n";
  std::cout << "- " << EXECUTE << " genRot <rotor names> -r\n";
  std::cout << "                  use a random seed for generation\n";
  std::cout << "- " << EXECUTE << " genRot -r\n";
  std::cout << "                  generate all valid rotors with a random seed\n";
  std::cout << "- " << EXECUTE << " genRot -a <seed>\n";
  std::cout << "                  generate all valid rotors with given seed\n";
}

void syntaxHelp() {
  std::cout << "- " << EXECUTE << " help <command>\n";
  std::cout << "    command     : command you want to see detailed information about\n";
  std::cout << "                  options are: <crypt>, <genKey>, <genRot> and <help>\n";
}

/***********************************************************************************************************************
 *                                                     warning                                                         *
 **********************************************************************************************************************/
void readKeyWarning(size_t size, size_t expected) {
  if (size != expected) {
    std::cout << timestamp(current_duration());
    print_yellow("Warning: ");
    std::cout << "The key file read has size " << size << " but expected was size " << expected << ".\n";
    std::cout << "              Maybe it's a key file from incompatible version?\n";
    std::cout << "              Do you want to continue (Y/n)? ";
    std::string answer;
    std::getline(std::cin, answer);
    if (
      std::strcmp(answer.c_str(), "") == 0 || std::strcmp(answer.c_str(), "y") == 0
      || std::strcmp(answer.c_str(), "Y") == 0 || std::strcmp(answer.c_str(), "j") == 0
      || std::strcmp(answer.c_str(), "j") == 0) {
      return;
    }
    else if (std::strcmp(answer.c_str(), "n") == 0 || std::strcmp(answer.c_str(), "N") == 0) {
      exit(0);
    }
    else {
      throw InvalidArgument("readKeyWarning", answer, ". Valid options are y, Y, n, N, j, J, <nothing>");
    }
  }
}
