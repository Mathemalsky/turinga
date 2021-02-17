#ifndef MAINERROR_HPP
#define MAINERROR_HPP

/*! \file mainerror.hpp */

#include <string>

/*!
 * \class Mainerror
 * \brief All specific errors inherit from Mainerror class so they can be caught together
 */
class Mainerror {
public:
  /*!
   * \brief report_error virtual function that can be called to catch the errors
   */
  virtual void report_error() = 0;
  /*!
   * \brief func writes into the private param p_function
   * \param function string which the param p_function should be set to
   */
  void func(const std::string function);
  /*!
   * \brief func reads the private param p_function
   * \return std::string p_function
   */
  std::string func() const;
  virtual ~Mainerror() {
  }

private:
  std::string p_function; /**< \param string that contains the name where the error occurs */
};

/*!
 * \class File_not_found
 * \brief The class File_not_found is designed to handle errors when accessing files while reading
 */
class File_not_found : public Mainerror {
public:
  /*!
   * \brief File_not_found
   * \param filename name of the file which wasn't found
   * \param function the name of the function where the error occurs as string
   */
  File_not_found(std::string filename, std::string function);
  /*!
   * \brief report_error prints out the error message to the console
   * \details prints the name of the missing file and the name of the function where the error
   * occured
   */
  void report_error();

private:
  std::string
    p_filename; /**< \param p_filename string that contains the name of the missing file */
};

/*!
 * \class Cannot_create_file
 * \brief The class Cannot_create_file is designed to handle errors when accessing files while
 * writing
 * \param p_filename string that contains the name of the file
 */
class Cannot_create_file : public Mainerror {
public:
  /*!
   * \brief Cannot_create_file
   * \param filename name of the file failed to create
   * \param function the name of the function where the error occurs as string
   */
  Cannot_create_file(std::string filename, std::string function);
  void report_error();

private:
  std::string p_filename; /**< \param p_filename string that contains the name of the file
                             struggling to create */
};

/*!
 * \class Inappropriate_number_of_arguments
 * \brief The class Inappropriate_number_of_arguments is designed to handle errors in case the
 * number of given arguments differs from the expected number of arguments
 * \param p_number number of collected arguments
 * \param p_expect expected number of arguments
 */
class Inappropriate_number_of_arguments : public Mainerror {
public:
  /*!
   * \brief Inappropriate_number_of_arguments
   * \param number counts the collected arguments
   * \param expected states the amount of expected arguments
   * \param function name of the function where the error occurs, typically main
   */
  Inappropriate_number_of_arguments(
    unsigned int number, unsigned int expected, std::string function);
  void report_error();

private:
  unsigned int p_number;   /**< \param p_filename number of arguments got */
  unsigned int p_expected; /**< \param p_filename number of arguments expected */
};
#endif  // MAINERROR_HPP
