#ifndef MAINERROR_HPP
#define MAINERROR_HPP

#include <string>

/*!
 * \brief All specific errors inherrit from Mainerror class so they can be catched together
 * \param p_function string that contains the name where the error occurs
 */
class Mainerror {
public:
  /*!
   * \brief report_error virtual function that can be called to catch ther errors
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
  std::string p_function;
};

/*!
 * \brief The class File_not_found is designed to handle errors when accesing files while reading
 * \param p_filename string that contains the name of the missing file
 */
class File_not_found : public Mainerror {
public:
  /*!
   * \brief File_not_found
   * \param filename name of the file which wasn't found
   * \param function the name of the function where the error occurs as string
   */
  File_not_found(std::string filename, std::string function);
  void report_error();

private:
  std::string p_filename;
};

/*!
 * \brief The class Cannot_create_file is designed to handle errors when accesing files while
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
  std::string p_filename;
};

/*!
 * \brief The Inappropriate_number_of_arguments class is designed to handle errors in case the
 * number of given arguments differs from the expected number of arguments
 * \param p_number number of collected arguments
 * \param p_expect expected number of arguments
 */
class Inappropriate_number_of_arguments : public Mainerror {
public:
  /*!
   * \brief Inappropriate_number_of_arguments
   * \param number counts the collected arguments
   * \param expectet states the amount of expected arguments
   * \param function name of the function where the error occurs, typically main
   */
  Inappropriate_number_of_arguments(
    unsigned int number, unsigned int expectet, std::string function);
  void report_error();

private:
  unsigned int p_number;
  unsigned int p_expectet;
};
#endif  // MAINERROR_HPP
