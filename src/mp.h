/*
 * Copyright (C) 1987-2008 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright (C) 2008-2011 Robert Ancell.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

/*  This maths library is based on the MP multi-precision floating-point
 *  arithmetic package originally written in FORTRAN by Richard Brent,
 *  Computer Centre, Australian National University in the 1970's.
 *
 *  It has been converted from FORTRAN into C using the freely available
 *  f2c translator, available via netlib on research.att.com.
 *
 *  The subsequently converted C code has then been tidied up, mainly to
 *  remove any dependencies on the libI77 and libF77 support libraries.
 *
 *  FOR A GENERAL DESCRIPTION OF THE PHILOSOPHY AND DESIGN OF MP,
 *  SEE - R. P. BRENT, A FORTRAN MULTIPLE-PRECISION ARITHMETIC
 *  PACKAGE, ACM TRANS. MATH. SOFTWARE 4 (MARCH 1978), 57-70.
 *  SOME ADDITIONAL DETAILS ARE GIVEN IN THE SAME ISSUE, 71-81.
 *  FOR DETAILS OF THE IMPLEMENTATION, CALLING SEQUENCES ETC. SEE
 *  THE MP USERS GUIDE.
 */

#ifndef MP_H
#define MP_H

#include <stdbool.h>
#include <stdint.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <mpfr.h>
#include <mpc.h>

/* If we're not using GNU C, elide __attribute__ */
#ifndef __GNUC__
#  define  __attribute__(x)  /*NOTHING*/
#endif

/* Precision of mpfr_t and mpc_t type objects */
#define PRECISION 1000

typedef struct
{
    mpc_t num;
} MPNumber;

typedef enum
{
    MP_RADIANS,
    MP_DEGREES,
    MP_GRADIANS
} MPAngleUnit;

/* Returns error string or NULL if no error */
// FIXME: Global variable
const char  *mp_get_error(void);

/* Clear any current error */
void        mp_clear_error(void);

void        mperr(const char *format, ...) __attribute__((format(printf, 1, 2)));

/* Returns initialized MPNumber object */
MPNumber    mp_new(void);

MPNumber    mp_new_from_integer(uint64_t x);

MPNumber*   mp_new_ptr(void);

void        mp_clear(MPNumber *z);

void        mp_free(MPNumber *z);

/* Returns:
 *  0 if x == y
 * <0 if x < y
 * >0 if x > y
 */
int    mp_compare(const MPNumber *x, const MPNumber *y);

/* Return true if the value is x == 0 */
bool   mp_is_zero(const MPNumber *x);

/* Return true if x < 0 */
bool   mp_is_negative(const MPNumber *x);

/* Return true if x is integer */
bool   mp_is_integer(const MPNumber *x);

/* Return true if x is a positive integer */
bool   mp_is_positive_integer(const MPNumber *x);

/* Return true if x is a natural number (an integer ≥ 0) */
bool   mp_is_natural(const MPNumber *x);

/* Return true if x has an imaginary component */
bool   mp_is_complex(const MPNumber *x);

/* Return true if x == y */
bool   mp_is_equal(const MPNumber *x, const MPNumber *y);

/* Return true if x ≥ y */
bool   mp_is_greater_equal(const MPNumber *x, const MPNumber *y);

/* Return true if x > y */
bool   mp_is_greater_than(const MPNumber *x, const MPNumber *y);

/* Return true if x ≤ y */
bool   mp_is_less_equal(const MPNumber *x, const MPNumber *y);

/* Return true if x < y */
bool   mp_is_less_than(const MPNumber *x, const MPNumber *y);

/* Sets z = |x| */
void   mp_abs(const MPNumber *x, MPNumber *z);

/* Sets z = Arg(x) */
void   mp_arg(const MPNumber *x, MPAngleUnit unit, MPNumber *z);

/* Sets z = ‾̅x */
void   mp_conjugate(const MPNumber *x, MPNumber *z);

/* Sets z = Re(x) */
void   mp_real_component(const MPNumber *x, MPNumber *z);

/* Sets z = Im(x) */
void   mp_imaginary_component(const MPNumber *x, MPNumber *z);

/* Sets z = −x */
void   mp_invert_sign(const MPNumber *x, MPNumber *z);

/* Sets z = x + y */
void   mp_add(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z = x + y */
void   mp_add_integer(const MPNumber *x, int64_t y, MPNumber *z);

/* Sets z = x − y */
void   mp_subtract(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z = x × y */
void   mp_multiply(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z = x × y */
void   mp_multiply_integer(const MPNumber *x, int64_t y, MPNumber *z);

/* Sets z = x ÷ y */
void   mp_divide(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z = x ÷ y */
void   mp_divide_integer(const MPNumber *x, int64_t y, MPNumber *z);

/* Sets z = 1 ÷ x */
void   mp_reciprocal(const MPNumber *, MPNumber *);

/* Sets z = sgn(x) */
void   mp_sgn(const MPNumber *x, MPNumber *z);

void   mp_integer_component(const MPNumber *x, MPNumber *z);

/* Sets z = x mod 1 */
void   mp_fractional_component(const MPNumber *x, MPNumber *z);

/* Sets z = {x} */
void   mp_fractional_part(const MPNumber *x, MPNumber *z);

/* Sets z = ⌊x⌋ */
void   mp_floor(const MPNumber *x, MPNumber *z);

/* Sets z = ⌈x⌉ */
void   mp_ceiling(const MPNumber *x, MPNumber *z);

/* Sets z = [x] */
void   mp_round(const MPNumber *x, MPNumber *z);

/* Sets z = ln x */
void   mp_ln(const MPNumber *x, MPNumber *z);

/* Sets z = log_n x */
void   mp_logarithm(int64_t n, const MPNumber *x, MPNumber *z);

/* Sets z = π */
void   mp_get_pi(MPNumber *z);

/* Sets z = e */
void   mp_get_eulers(MPNumber *z);

/* Sets z = i (√−1) */
void   mp_get_i(MPNumber *z);

/* Sets z = n√x */
void   mp_root(const MPNumber *x, int64_t n, MPNumber *z);

/* Sets z = √x */
void   mp_sqrt(const MPNumber *x, MPNumber *z);

/* Sets z = x! */
void   mp_factorial(const MPNumber *x, MPNumber *z);

/* Sets z = x mod y */
void   mp_modulus_divide(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z = x ^ y mod p */
void mp_modular_exponentiation(const MPNumber *x, const MPNumber *y, const MPNumber *p, MPNumber *z);

/* Sets z = x^y */
void   mp_xpowy(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z = x^y */
void   mp_xpowy_integer(const MPNumber *x, int64_t y, MPNumber *z);

/* Sets z = e^x */
void   mp_epowy(const MPNumber *x, MPNumber *z);

/* Returns a list of all prime factors in x as MPNumbers */
GList* mp_factorize(const MPNumber *x);

GList* mp_factorize_unit64 (uint64_t n);

/* Sets z = x */
void   mp_set_from_mp(const MPNumber *x, MPNumber *z);

/* Sets z = x */
void   mp_set_from_float(float x, MPNumber *z);

/* Sets z = x */
void   mp_set_from_double(double x, MPNumber *z);

/* Sets z = x */
void   mp_set_from_integer(int64_t x, MPNumber *z);

/* Sets z = x */
void   mp_set_from_unsigned_integer(uint64_t x, MPNumber *z);

/* Sets z = numerator ÷ denominator */
void   mp_set_from_fraction(int64_t numerator, int64_t denominator, MPNumber *z);

/* Sets z = r(cos theta + i sin theta) */
void   mp_set_from_polar(const MPNumber *r, MPAngleUnit unit, const MPNumber *theta, MPNumber *z);

/* Sets z = x + iy */
void   mp_set_from_complex(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z to be a uniform random number in the range [0, 1] */
void   mp_set_from_random(MPNumber *z);

/* Sets z from a string representation in 'text'.
 * Returns true on success.
 */
bool   mp_set_from_string(const char *text, int default_base, MPNumber *z);

/* Returns x as a native single-precision floating point number */
float  mp_to_float(const MPNumber *x);

/* Returns x as a native double-precision floating point number */
double mp_to_double(const MPNumber *x);

/* Returns x as a native integer */
int64_t mp_to_integer(const MPNumber *x);

/* Returns x as a native unsigned integer */
uint64_t mp_to_unsigned_integer(const MPNumber *x);

/* Sets z = sin x */
void   mp_sin(const MPNumber *x, MPAngleUnit unit, MPNumber *z);

/* Sets z = cos x */
void   mp_cos(const MPNumber *x, MPAngleUnit unit, MPNumber *z);

/* Sets z = tan x */
void   mp_tan(const MPNumber *x, MPAngleUnit unit, MPNumber *z);

/* Sets z = sin⁻¹ x */
void   mp_asin(const MPNumber *x, MPAngleUnit unit, MPNumber *z);

/* Sets z = cos⁻¹ x */
void   mp_acos(const MPNumber *x, MPAngleUnit unit, MPNumber *z);

/* Sets z = tan⁻¹ x */
void   mp_atan(const MPNumber *x, MPAngleUnit unit, MPNumber *z);

/* Sets z = sinh x */
void   mp_sinh(const MPNumber *x, MPNumber *z);

/* Sets z = cosh x */
void   mp_cosh(const MPNumber *x, MPNumber *z);

/* Sets z = tanh x */
void   mp_tanh(const MPNumber *x, MPNumber *z);

/* Sets z = sinh⁻¹ x */
void   mp_asinh(const MPNumber *x, MPNumber *z);

/* Sets z = cosh⁻¹ x */
void   mp_acosh(const MPNumber *x, MPNumber *z);

/* Sets z = tanh⁻¹ x */
void   mp_atanh(const MPNumber *x, MPNumber *z);

/* Sets z to the value of the error function of x */
void   mp_erf(const MPNumber *x, MPNumber *z);

/* Sets z to the value of the Riemann Zeta function of x */
void   mp_zeta(const MPNumber *x, MPNumber *z);

/* Returns true if x is cannot be represented in a binary word of length 'wordlen' */
bool   mp_is_overflow(const MPNumber *x, int wordlen);

/* Sets z = boolean AND for each bit in x and z */
void   mp_and(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z = boolean OR for each bit in x and z */
void   mp_or(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z = boolean XOR for each bit in x and z */
void   mp_xor(const MPNumber *x, const MPNumber *y, MPNumber *z);

/* Sets z = boolean XNOR for each bit in x and z for word of length 'wordlen' */
void   mp_xnor(const MPNumber *x, const MPNumber *y, int wordlen, MPNumber *z);

/* Sets z = boolean NOT for each bit in x and z for word of length 'wordlen' */
void   mp_not(const MPNumber *x, int wordlen, MPNumber *z);

/* Sets z = x shifted by 'count' bits.  Positive shift increases the value, negative decreases */
void   mp_shift(const MPNumber *x, int count, MPNumber *z);

/* Sets z to be the ones complement of x for word of length 'wordlen' */
void   mp_ones_complement(const MPNumber *x, int wordlen, MPNumber *z);

/* Sets z to be the twos complement of x for word of length 'wordlen' */
void   mp_twos_complement(const MPNumber *x, int wordlen, MPNumber *z);

void convert_to_radians(const MPNumber *x, MPAngleUnit unit, MPNumber *z);

void convert_from_radians(const MPNumber *x, MPAngleUnit unit, MPNumber *z);

#endif /* MP_H */
