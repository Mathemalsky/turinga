#pragma once

/*! \file rotate.hpp */

#include "types.hpp"

/*!
 * \brief changes the substitution rule after each byte
 * \details This function replaces the rotation of wheels in enigma. This part is most critical for
 * security.
 * \authors Max, Jurek
 * \param fileShifts determines the current substitution
 * \param length number of rotors in use
 */
void rotate(Byte* fileShifts, const size_t length);
