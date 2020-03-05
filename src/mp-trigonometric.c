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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <libintl.h>

#include "mp.h"

/* Convert x to radians */
void
convert_to_radians(const MPNumber *x, MPAngleUnit unit, MPNumber *z)
{
    int i;

    switch(unit) {
    default:
    case MP_RADIANS:
        mp_set_from_mp(x, z);
        return;

    case MP_DEGREES:
        i = 180;
        break;

    case MP_GRADIANS:
        i = 200;
        break;
    }
    mpfr_t scale;
    mpfr_init2(scale, PRECISION);
    mpfr_const_pi(scale, MPFR_RNDN);
    mpfr_div_si(scale, scale, i, MPFR_RNDN);
    mpc_mul_fr(z->num, x->num, scale, MPC_RNDNN);
    mpfr_clear(scale);
}

void
convert_from_radians(const MPNumber *x, MPAngleUnit unit, MPNumber *z)
{
    int i;

    switch(unit) {
    default:
    case MP_RADIANS:
        mp_set_from_mp(x, z);
        return;

    case MP_DEGREES:
        i = 180;
        break;

    case MP_GRADIANS:
        i = 200;
        break;
    }
    mpfr_t scale;
    mpfr_init2(scale, PRECISION);
    mpfr_const_pi(scale, MPFR_RNDN);
    mpfr_si_div(scale, i, scale, MPFR_RNDN);
    mpc_mul_fr(z->num, x->num, scale, MPC_RNDNN);
    mpfr_clear(scale);
}


void
mp_get_pi (MPNumber *z)
{
    mpfr_const_pi(mpc_realref(z->num), MPFR_RNDN);
    mpfr_set_zero(mpc_imagref(z->num), 0);
}


void
mp_sin(const MPNumber *x, MPAngleUnit unit, MPNumber *z)
{
    if (mp_is_complex(x))
        mp_set_from_mp(x, z);
    else
        convert_to_radians(x, unit, z);
    mpc_sin(z->num, z->num, MPC_RNDNN);    
}


void
mp_cos(const MPNumber *x, MPAngleUnit unit, MPNumber *z)
{
    if (mp_is_complex(x))
        mp_set_from_mp(x, z);
    else
        convert_to_radians(x, unit, z);
    mpc_cos(z->num, z->num, MPC_RNDNN); 
}


void
mp_tan(const MPNumber *x, MPAngleUnit unit, MPNumber *z)
{
    MPNumber x_radians = mp_new();
    MPNumber pi = mp_new();
    MPNumber t1 = mp_new();

    convert_to_radians(x, unit, &x_radians);
    mp_get_pi(&pi);
    mp_divide_integer(&pi, 2, &t1);
    mp_subtract(&x_radians, &t1, &t1);
    mp_divide(&t1, &pi, &t1);    

    if (mp_is_integer(&t1)) {
        /* Translators: Error displayed when tangent value is undefined */
        mperr(_("Tangent is undefined for angles that are multiples of π (180°) from π∕2 (90°)"));
        mp_set_from_integer(0, z);
        return;
    }

    if (mp_is_complex(x))
        mp_set_from_mp(x, z);
    else
        mp_set_from_mp(&x_radians, z);
    mpc_tan(z->num, z->num, MPC_RNDNN);
    mp_clear(&x_radians);
    mp_clear(&pi);
    mp_clear(&t1);
}


void
mp_asin(const MPNumber *x, MPAngleUnit unit, MPNumber *z)
{
    MPNumber x_max = mp_new();
    MPNumber x_min = mp_new();
    mp_set_from_integer(1, &x_max);
    mp_set_from_integer(-1, &x_min);

    if (mp_compare(x, &x_max) > 0 || mp_compare(x, &x_min) < 0)
    {
        /* Translators: Error displayed when inverse sine value is undefined */
        mperr(_("Inverse sine is undefined for values outside [-1, 1]"));
        mp_set_from_integer(0, z);
        return;
    }
    mpc_asin(z->num, x->num, MPC_RNDNN);
    if (!mp_is_complex(z))
        convert_from_radians(z, unit, z);
    mp_clear(&x_max);
    mp_clear(&x_min);
}


void
mp_acos(const MPNumber *x, MPAngleUnit unit, MPNumber *z)
{
    MPNumber x_max = mp_new();
    MPNumber x_min = mp_new();
    mp_set_from_integer(1, &x_max);
    mp_set_from_integer(-1, &x_min);

    if (mp_compare(x, &x_max) > 0 || mp_compare(x, &x_min) < 0)
    {
        /* Translators: Error displayed when inverse sine value is undefined */
        mperr(_("Inverse cosine is undefined for values outside [-1, 1]"));
        mp_set_from_integer(0, z);
        return;
    }
    mpc_acos(z->num, x->num, MPC_RNDNN);
    if (!mp_is_complex(z))
        convert_from_radians(z, unit, z);
    mp_clear(&x_max);
    mp_clear(&x_min);
}


void
mp_atan(const MPNumber *x, MPAngleUnit unit, MPNumber *z)
{
    MPNumber i = mp_new();
    MPNumber minus_i = mp_new();
    mpc_set_si_si(i.num, 0, 1, MPC_RNDNN);
    mpc_set_si_si(minus_i.num, 0, -1, MPC_RNDNN);

    /* Check x != i and x != -i */
    if (mp_is_equal(x, &i) || mp_is_equal(x, &minus_i))
    {
        /* Translators: Error displayed when inverse sine value is undefined */
        mperr(_("Arctangent function is undefined for values i and -i"));
        mp_set_from_integer(0, z);
        return;
    }
    mpc_atan(z->num, x->num, MPC_RNDNN);
    if (!mp_is_complex(z))
        convert_from_radians(z, unit, z);
    mp_clear(&i);
    mp_clear(&minus_i);
}


void
mp_sinh(const MPNumber *x, MPNumber *z)
{
    mpc_sinh(z->num, x->num, MPC_RNDNN);
}


void
mp_cosh(const MPNumber *x, MPNumber *z)
{
    mpc_cosh(z->num, x->num, MPC_RNDNN);
}


void
mp_tanh(const MPNumber *x, MPNumber *z)
{
    mpc_tanh(z->num, x->num, MPC_RNDNN);
}


void
mp_asinh(const MPNumber *x, MPNumber *z)
{
    mpc_asinh(z->num, x->num, MPC_RNDNN);
}


void
mp_acosh(const MPNumber *x, MPNumber *z)
{
    MPNumber t = mp_new();

    /* Check x >= 1 */
    mp_set_from_integer(1, &t);
    if (mp_is_less_than(x, &t))
    {
        /* Translators: Error displayed when inverse hyperbolic cosine value is undefined */
        mperr(_("Inverse hyperbolic cosine is undefined for values less than one"));
        mp_set_from_integer(0, z);
        return;
    }

    mpc_acosh(z->num, x->num, MPC_RNDNN);
    mp_clear(&t);
}


void
mp_atanh(const MPNumber *x, MPNumber *z)
{
    MPNumber x_max = mp_new();
    MPNumber x_min = mp_new();
    mp_set_from_integer(1, &x_max);
    mp_set_from_integer(-1, &x_min);

    if (mp_compare(x, &x_max) >= 0 || mp_compare(x, &x_min) <= 0)
    {
         /* Translators: Error displayed when inverse hyperbolic tangent value is undefined */
        mperr(_("Inverse hyperbolic tangent is undefined for values outside (-1, 1)"));
        mp_set_from_integer(0, z);
        return;
    }
    mpc_atanh(z->num, x->num, MPC_RNDNN);
    mp_clear(&x_max);
    mp_clear(&x_min);
}
