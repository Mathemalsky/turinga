/*
 * Turinga is a simple symmetric encryption scheme based on ideas from enigma.
 * Copyright (C) 2022  Mathemalsky, MilchRatchet
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

/*! \file rotate.hpp */

#include "types.hpp"

/*!
 * \brief changes the substitution rule after each byte
 * This function maps the first n := length Bytes of rotorShifts to a n byte vector which will re-
 * place the rotorShifts in the next step.
 * Let's consider this n Bytes as vector in \f$\mathbb{F}_2^{8n}\f$.
 * Now we cut this vector into 2n vectors of length 4.
 * So if \f$r\f$ is the vector of the \f$n\f$ bytes represented as row vector of bits we define
 * \f{equation*}{ (v_0,...,v_{2n-1}) := r\f}. (Each \f$v_i\f$ is a vector of 4 bits length)
 * Furthermore we define a binary vector s of length n by
 * \f{equation*} s_i := <v_i, \pi(v_{2n-1-i})> \qquad i \in \{0,...,n-1\}\f}, where
 * \f$\pi\f$ is the bijective mapping from \f$\mathbb{F}_2^4\f$ to \f$\mathbb{F}_2^4\f$
 * which we obtain by considering the input vector as a coefficient vector of a binary
 * polynomial \f$g\f$ with \f$\deg(g) < 4\f$.
 * Then the output vector is the coefficient vector of \f{equation*}\pi(g) \equiv {g^{-1}(x) \textrm{ mod } x^4 + x
 * + 1\f} if g is not the 0 polynomial otherwiese zero.
 * If \f$s_0 = 1\f$ shift the i-th rotor by \f$(2i +1) s_i\f$ else shift it by \f$(2i+1) (1-s_i)\f$
 *
 * \authors Max, Jurek
 * \param rotorShifts determines the current substitution
 * \details This function replaces the rotation of wheels in enigma. This part is most critical for
 * security.
 */
void rotate(Byte* rotorShifts);
