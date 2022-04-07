/*
 * Turinga is a simple symmetric encryption scheme based on ideas from enigma.
 * Copyright (C) 2021  Mathemalsky, MilchRatchet
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
#pragma once

/*! \file errors.hpp */

#include <stdexcept>
#include <string>

/***********************************************************************************************************************
 *                                                 Error handling                                                      *
 **********************************************************************************************************************/
/*!
 * \class TuringaError
 * \brief All specific errors inherit from TuringaError class so they can be caught together.
 */
class TuringaError : public std::exception {
public:
  /*!
   * \brief virtual function that can be called to catch the errors
   */
  virtual void what() = 0;

protected:
  std::string p_func; /**< \param string that contains the name where the error occurs */
};

/*!
 * \class InappropriateNumberOfArguments
 * \brief The class InappropriateNumberOfArguments is designed to handle errors in case the
 * number of given arguments differs from the expected number of arguments.
 * \param p_number number of collected arguments
 * \param p_expect expected number of arguments
 */
class InappropriateNumberOfArguments : public TuringaError {
public:
  /*!
   * \brief InappropriateNumberOfArguments
   * \param function name of the function where the error occurs, typically main
   * \param expected states the amount of expected arguments
   * \param number counts the collected arguments
   */
  InappropriateNumberOfArguments(std::string function, unsigned int expected, unsigned int number);
  /*!
   * \brief prints out the error message to the console
   * \details prints the number of arguments got, the number of arguments expected and the function
   * where the error occurs and a syntax hint
   */
  void what() override;

private:
  unsigned int p_expected; /**< \param p_expected number of arguments expected */
  unsigned int p_number;   /**< \param p_number number of collected arguments */
};

/*!
 * \class InvalidArgument
 * \brief The class InvalidArgument is designed to handle errors in case a given argument does not
 * match an argument that the programm can process within this context
 * \param p_arg argument provided to turinga
 * \param p_setting context of the argument
 */
class InvalidArgument : public TuringaError {
public:
  /*!
   * \brief InvalidArgument
   * \param function name of the function where the error occurs, typically main
   * \param arg holds the argument that could not be parsed correctly
   * \param setting gives a description of the context of arg
   */
  InvalidArgument(std::string function, std::string arg, std::string setting);
  /*!
   * \brief what prints out the error message to the console
   * \details prints the argument the description of context and a syntax hint
   */
  void what() override;

private:
  std::string p_arg;     /**< \param p_arg argument provided to turinga */
  std::string p_setting; /**< \param p_setting context of the argument */
};

/*!
 * \class FileNotFound
 * \brief The class FileNotFound is designed to handle errors when accessing files while reading.
 */
class FileNotFound : public TuringaError {
public:
  /*!
   * \brief FileNotFound
   * \param filename name of the file which wasn't found
   * \param function the name of the function where the error occurs as string
   */
  FileNotFound(std::string filename, std::string function);
  /*!
   * \brief prints out the error message to the console
   * \details prints the name of the missing file and the name of the function where the error
   * occured
   */
  void what() override;

private:
  std::string p_filename; /**< \param p_filename string that contains the name of the missing file */
};

/*!
 * \class CannotCreateFile
 * \brief The class CannotCreateFile is designed to handle errors when accessing files while
 * writing.
 * \param p_filename string that contains the name of the file
 */
class CannotCreateFile : public TuringaError {
public:
  /*!
   * \brief CannotCreateFile
   * \param filename name of the file failed to create
   * \param function the name of the function where the error occurs as string
   */
  CannotCreateFile(std::string filename, std::string function);
  /*!
   * \brief prints out the error message to the console
   * \details prints the name of the file which makes problems and the name of the function where
   * the error occured
   */
  void what() override;

private:
  std::string p_filename; /**< \param p_filename string that contains the name of the file
                             struggling to create */
};

/*!
 * \class NoKey
 * \brief The class NoKey s designed to handle the error of not existing keys
 * \param p_filename string that contains the name of the file where turinga was looking for the key
 */
class NoKey : public TuringaError {
public:
  /*!
   * \brief NoKey
   * \param function the name of the function where the error occurs as string
   * \param filename name of the missing key file
   */
  NoKey(std::string function, std::string filename);
  /*!
   * \brief prints out the error message to the console
   * \details prints the name of the file of the missing key and the name of the function where the
   * error occured
   */
  void what() override;

private:
  std::string p_filename;
};

/***********************************************************************************************************************
 *                                                  syntax help                                                        *
 **********************************************************************************************************************/
/*!
 * \brief syntax prints detailed sytanx advices
 * \details calls syntaxCrypt, syntaxGenerate and syntaxHelp
 */
void syntax();
/*!
 * \brief syntaxCrypt prints detailed syntax advices for encrypting/ decrypting files
 * \details explaines the arguments needed for encryption and their order
 */
void syntaxCrypt();
/*!
 * \brief syntaxGenerateKey prints detailed syntax advices for generating keys
 * \details explaines the arguments needed for key generation and their order
 */
void syntaxGenerateKey();
/*!
 * \brief syntaxGenerateRotors prints detailed syntax advices for genrating rotors
 * \details explaines the arguments needed for rotor generation
 */
void syntaxGenerateRotors();
/*!
 * \brief syntaxHelp prints a hint how syntax
 * \details explaines how to get only specific syntax advices
 */
void syntaxHelp();

/***********************************************************************************************************************
 *                                                     warning                                                         *
 **********************************************************************************************************************/
void readKeyWarning(size_t size, size_t expected);
