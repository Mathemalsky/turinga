#pragma once

/*! \file rotate.hpp */

#include "types.hpp"

struct RotateArgs {
  Byte* rotorShifts;
  const size_t length;
};

/*!
 * \brief changes the substitution rule after each byte
 * This function maps the first n := length Bytes of rotorShifts to a n byte vector which will re-
 * place the rotorShifts in the next step.
 * Let's consider this n Bytes as vector in \f$\mathbb{F}_2^{8n}\f$.
 * Now we cut this vector into 2n vectors of length 4.
 * So if r is the vector of the first n bytes represented as row vector of bits we define
 * \f{equation*}{ (v_0,...,v_{2n-1}) := r\f}
 * Furthermore we define a binary vector s of length n by
 * \f{equation*} s_i := <v_i, \pi(v_{2n-1-i})> \qquad i \in \{0,...,n-1\}\f}, where
 * \f$\pi\f$ is the bijective mapping from \f$\mathbb{F}_2^4\f$ to \f$\mathbb{F}_2^4\f$
 * which we obtain by considering the input vector as a coefficient vector of a binary
 * polynomial \f$g\f$ with \f$\deg(g) < 4\f$.
 * Then the output vector is the coeficcient vector of \f{equation*}{g^{-1}(x) \textrm{mod} x^4 + x
 * + 1\f}
 *
 * \authors Max, Jurek
 * \param fileShifts determines the current substitution
 * \param length number of rotors in use
 * \details This function replaces the rotation of wheels in enigma. This part is most critical for
 * security.
 */
void rotate(RotateArgs args);
