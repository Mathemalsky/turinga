#pragma once

/*! \file errors.hpp */

#include <stdexcept>
#include <string>

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
  void what();

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
  void what();

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
  void what();

private:
  std::string
    p_filename; /**< \param p_filename string that contains the name of the missing file */
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
  void what();

private:
  std::string p_filename; /**< \param p_filename string that contains the name of the file
                             struggling to create */
};

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
 * \brief syntaxGenerate
 * \details explaines the arguments needed for key generation and their order
 */
void syntaxGenerate();
/*!
 * \brief syntaxHelp prints a hint how syntax
 * \details explaines how to get only specific syntax advices
 */
void syntaxHelp();
