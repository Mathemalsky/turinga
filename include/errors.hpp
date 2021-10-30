#pragma once

/*! \file errors.hpp */

#include <stdexcept>
#include <string>

class TuringaError : public std::exception {
public:
  virtual void what() = 0;

protected:
  std::string p_func;
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
   * \param number counts the collected arguments
   * \param expected states the amount of expected arguments
   * \param function name of the function where the error occurs, typically main
   */
  InappropriateNumberOfArguments(unsigned int number, unsigned int expected, std::string function);
  /*!
   * \brief prints out the error message to the console
   * \details prints the number of argeuments got, the number of arguments expected and the function
   * where the error occurs
   */
  void what();

private:
  unsigned int p_number;   /**< \param p_filename number of arguments got */
  unsigned int p_expected; /**< \param p_filename number of arguments expected */
};

/*!
 * \class Mainerror
 * \brief All specific errors inherit from Mainerror class so they can be caught together.
 */
class Mainerror {
public:
  /*!
   * \brief virtual function that can be called to catch the errors
   */
  virtual void report_error() = 0;
  /*!
   * \brief writes into the private param p_function
   * \param function string which the param p_function should be set to
   */
  void func(const std::string function);
  /*!
   * \brief reads the private param p_function
   * \return std::string p_function
   */
  std::string func() const;
  virtual ~Mainerror() {
  }

private:
  std::string p_function; /**< \param string that contains the name where the error occurs */
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

void syntax();
void syntaxCrypt();
void syntaxGenerate();
void syntaxHelp();
