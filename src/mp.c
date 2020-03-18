/*
 * Copyright (C) 1987-2008 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright (C) 2008-2011 Robert Ancell
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>

#include "mp.h"

char *mp_error = NULL;

/*  THIS ROUTINE IS CALLED WHEN AN ERROR CONDITION IS ENCOUNTERED, AND
 *  AFTER A MESSAGE HAS BEEN WRITTEN TO STDERR.
 */
void
mperr(const char *format, ...)
{
    char text[1024];
    va_list args;

    va_start(args, format);
    vsnprintf(text, 1024, format, args);
    va_end(args);

    if (mp_error)
        free(mp_error);
    mp_error = strdup(text);
}


const char *
mp_get_error()
{
    return mp_error;
}


void mp_clear_error()
{
    if (mp_error)
        free(mp_error);
    mp_error = NULL;
}


MPNumber
mp_new(void)
{
    MPNumber z;
    mpc_init2(z.num, PRECISION);
    return z;
}


MPNumber *
mp_new_ptr(void)
{
    MPNumber *z = malloc(sizeof(MPNumber));
    mpc_init2(z->num, PRECISION);
    return z;
}


void
mp_clear(MPNumber *z)
{
    if (z != NULL)
        mpc_clear(z->num);
}


void
mp_free(MPNumber *z)
{
    if (z != NULL)
    {
        mpc_clear(z->num);
        free(z);
    }
}


void
mp_get_eulers(MPNumber *z)
{
    /* e^1, since mpfr doesn't have a function to return e */
    mpfr_set_ui(mpc_realref(z->num), 1, MPFR_RNDN);
    mpfr_exp(mpc_realref(z->num), mpc_realref(z->num), MPFR_RNDN);
    mpfr_set_zero(mpc_imagref(z->num), 0);
}


void
mp_get_i(MPNumber *z)
{
    mpc_set_si_si(z->num, 0, 1, MPC_RNDNN);
}


void
mp_abs(const MPNumber *x, MPNumber *z)
{
    mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    mpc_abs(mpc_realref(z->num), x->num, MPC_RNDNN);
}


void
mp_arg(const MPNumber *x, MPAngleUnit unit, MPNumber *z)
{
    if (mp_is_zero(x))
    {
        /* Translators: Error display when attempting to take argument of zero */
        mperr(_("Argument not defined for zero"));
        mp_set_from_integer(0, z);
        return;
    }

    mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    mpc_arg(mpc_realref(z->num), x->num, MPC_RNDNN);
    convert_from_radians(z, unit, z);
    // MPC returns -π for the argument of negative real numbers if
    // their imaginary part is -0, we want +π for all real negative
    // numbers
    if (!mp_is_complex (x) && mp_is_negative (x))
        mpfr_abs(mpc_realref(z->num), mpc_realref(z->num), MPFR_RNDN);
}


void
mp_conjugate(const MPNumber *x, MPNumber *z)
{
    mpc_conj(z->num, x->num, MPC_RNDNN);
}


void
mp_real_component(const MPNumber *x, MPNumber *z)
{
    mpc_set_fr(z->num, mpc_realref(x->num), MPC_RNDNN);
}


void
mp_imaginary_component(const MPNumber *x, MPNumber *z)
{
    mpc_set_fr(z->num, mpc_imagref(x->num), MPC_RNDNN);
}

void
mp_add(const MPNumber *x, const MPNumber *y, MPNumber *z)
{
    mpc_add(z->num, x->num, y->num, MPC_RNDNN);
}


void
mp_add_integer(const MPNumber *x, int64_t y, MPNumber *z)
{
    mpc_add_si(z->num, x->num, y, MPC_RNDNN);
}

void
mp_subtract(const MPNumber *x, const MPNumber *y, MPNumber *z)
{
    mpc_sub(z->num, x->num, y->num, MPC_RNDNN);
}

void
mp_sgn(const MPNumber *x, MPNumber *z)
{
    mpc_set_si(z->num, mpfr_sgn(mpc_realref(x->num)), MPC_RNDNN);
}

void
mp_integer_component(const MPNumber *x, MPNumber *z)
{
    mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    mpfr_trunc(mpc_realref(z->num), mpc_realref(x->num));
}

void
mp_fractional_component(const MPNumber *x, MPNumber *z)
{
    mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    mpfr_frac(mpc_realref(z->num), mpc_realref(x->num), MPFR_RNDN);
}


void
mp_fractional_part(const MPNumber *x, MPNumber *z)
{
    MPNumber f = mp_new();
    mp_floor(x, &f);
    mp_subtract(x, &f, z);
    mp_clear(&f);
}


void
mp_floor(const MPNumber *x, MPNumber *z)
{
    mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    mpfr_floor(mpc_realref(z->num), mpc_realref(x->num));
}


void
mp_ceiling(const MPNumber *x, MPNumber *z)
{
    mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    mpfr_ceil(mpc_realref(z->num), mpc_realref(x->num));
}


void
mp_round(const MPNumber *x, MPNumber *z)
{
    mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    mpfr_round(mpc_realref(z->num), mpc_realref(x->num));
}

int
mp_compare(const MPNumber *x, const MPNumber *y)
{
    return mpfr_cmp(mpc_realref(x->num), mpc_realref(y->num));
}

void
mp_divide(const MPNumber *x, const MPNumber *y, MPNumber *z)
{
    if (mp_is_zero(y))
    {
        /* Translators: Error displayed attempted to divide by zero */
        mperr(_("Division by zero is undefined"));
        mp_set_from_integer(0, z);
        return;
    }
    mpc_div(z->num, x->num, y->num, MPC_RNDNN);
}

void
mp_divide_integer(const MPNumber *x, int64_t y, MPNumber *z)
{
    MPNumber t1 = mp_new();

    mp_set_from_integer(y, &t1);
    mp_divide(x, &t1, z);
    mp_clear(&t1);
}

bool
mp_is_integer(const MPNumber *x)
{
    if (mp_is_complex(x))
        return false;

    return mpfr_integer_p(mpc_realref(x->num)) != 0;
}


bool
mp_is_positive_integer(const MPNumber *x)
{
    if (mp_is_complex(x))
        return false;
    else
        return mpfr_sgn(mpc_realref(x->num)) >= 0 && mp_is_integer(x);
}


bool
mp_is_natural(const MPNumber *x)
{
    if (mp_is_complex(x))
        return false;
    else
        return mpfr_sgn(mpc_realref(x->num)) > 0 && mp_is_integer(x);
}


bool
mp_is_complex(const MPNumber *x)
{
    return !mpfr_zero_p(mpc_imagref(x->num));
}


bool
mp_is_equal(const MPNumber *x, const MPNumber *y)
{
    int res = mpc_cmp(x->num, y->num);
    return MPC_INEX_RE(res) == 0 && MPC_INEX_IM(res) == 0;
}


void
mp_epowy(const MPNumber *x, MPNumber *z)
{
    mpc_exp(z->num, x->num, MPC_RNDNN);
}

bool
mp_is_zero (const MPNumber *x)
{
    int res = mpc_cmp_si_si(x->num, 0, 0);
    return MPC_INEX_RE(res) == 0 && MPC_INEX_IM(res) == 0;
}

bool
mp_is_negative(const MPNumber *x)
{
    return mpfr_sgn(mpc_realref(x->num)) < 0;
}


bool
mp_is_greater_equal(const MPNumber *x, const MPNumber *y)
{
    return mp_compare(x, y) >= 0;
}


bool
mp_is_greater_than(const MPNumber *x, const MPNumber *y)
{
    return mp_compare(x, y) > 0;
}


bool
mp_is_less_equal(const MPNumber *x, const MPNumber *y)
{
    return mp_compare(x, y) <= 0;
}

bool
mp_is_less_than(const MPNumber *x, const MPNumber *y)
{
    return mp_compare(x, y) < 0;
}

void
mp_ln(const MPNumber *x, MPNumber *z)
{
    /* ln(0) undefined */
    if (mp_is_zero(x))
    {
        /* Translators: Error displayed when attempting to take logarithm of zero */
        mperr(_("Logarithm of zero is undefined"));
        mp_set_from_integer(0, z);
        return;
    }

    /* ln(-x) complex */
    /* FIXME: Make complex numbers optional */
    /*if (mp_is_negative(x)) {
        // Translators: Error displayed attempted to take logarithm of negative value
        mperr(_("Logarithm of negative values is undefined"));
        mp_set_from_integer(0, z);
        return;
    }*/

    mpc_log(z->num, x->num, MPC_RNDNN);
    // MPC returns -π for the imaginary part of the log of
    // negative real numbers if their imaginary part is -0, we want +π
    if (!mp_is_complex (x) && mp_is_negative (x))
        mpfr_abs(mpc_imagref(z->num), mpc_imagref(z->num), MPFR_RNDN);
}


void
mp_logarithm(int64_t n, const MPNumber *x, MPNumber *z)
{
    /* log(0) undefined */
    if (mp_is_zero(x)) 
    {
        /* Translators: Error displayed when attempting to take logarithm of zero */
        mperr(_("Logarithm of zero is undefined"));
        mp_set_from_integer(0, z);
        return;
    }

    /* logn(x) = ln(x) / ln(n) */
    MPNumber t1 = mp_new();
    mp_set_from_integer(n, &t1);
    mp_ln(&t1, &t1);
    mp_ln(x, z);
    mp_divide(z, &t1, z);
    mp_clear(&t1);
}

void
mp_multiply(const MPNumber *x, const MPNumber *y, MPNumber *z)
{
    mpc_mul(z->num, x->num, y->num, MPC_RNDNN);
}


void
mp_multiply_integer(const MPNumber *x, int64_t y, MPNumber *z)
{
    mpc_mul_si(z->num, x->num, (long) y, MPC_RNDNN);
}


void
mp_invert_sign(const MPNumber *x, MPNumber *z)
{
    mpc_neg(z->num, x->num, MPC_RNDNN);
}


void
mp_reciprocal(const MPNumber *x, MPNumber *z)
{
    mpc_set_si(z->num, 1, MPC_RNDNN);
    mpc_fr_div(z->num, mpc_realref(z->num), x->num, MPC_RNDNN);
}

void
mp_root(const MPNumber *x, int64_t n, MPNumber *z)
{
    uint64_t p;

    if (n < 0)
    {
        if (n == INT64_MIN)
            p = (uint64_t) INT64_MAX + 1;
        else
            p = -n;
    }
    else if (n > 0)
    {
        mp_set_from_mp(x, z);
        p = n;
    }
    else
    {   /* Translators: Error displayed when attempting to take zeroth root */
        mperr(_("The zeroth root of a number is undefined"));
        mp_set_from_integer(0, z);
        return;
    }
    if (!mp_is_complex(x) && (!mp_is_negative(x) || (p & 1) == 1))
    {
        mpfr_rootn_ui(mpc_realref(z->num), mpc_realref(z->num), (uint64_t) p, MPFR_RNDN);
        mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    }
    else
    {
        mpfr_t tmp;
        mpfr_init2(tmp, PRECISION);
        mpfr_set_ui(tmp, (uint64_t) p, MPFR_RNDN);
        mpfr_ui_div(tmp, 1, tmp, MPFR_RNDN);
        mpc_pow_fr(z->num, z->num, tmp, MPC_RNDNN);
        mpfr_clear(tmp);
    }
}


void
mp_sqrt(const MPNumber *x, MPNumber *z)
{
    mp_root(x, 2, z);
}


void
mp_factorial(const MPNumber *x, MPNumber *z)
{
    /* 0! == 1 */
    if (mp_is_zero(x))
    {
        mpc_set_si(z->num, 1, MPC_RNDNN);
        return;
    }
    if (!mp_is_natural(x))
    {
        /* Factorial Not defined for Complex or for negative numbers */
        if (mp_is_negative(x) || mp_is_complex(x))
        {  /* Translators: Error displayed when attempted take the factorial of a negative or complex number */
            mperr(_("Factorial is only defined for non-negative real numbers"));
            mp_set_from_integer(0, z);
            return;
        }
        MPNumber tmp = mp_new();
        mpfr_t tmp2;
        mpfr_init2(tmp2, PRECISION);
        mp_set_from_integer(1, &tmp);
        mp_add(&tmp, x, &tmp);

        /* Factorial(x) = Gamma(x+1) - This is the formula used to calculate Factorial of positive real numbers.*/
        mpfr_gamma(tmp2, mpc_realref(tmp.num), MPFR_RNDN);
        mpc_set_fr(z->num, tmp2, MPC_RNDNN);
        mp_clear(&tmp);
        mpfr_clear(tmp2);
    }
    else
    {
        /* Convert to integer - if couldn't be converted then the factorial would be too big anyway */
        int64_t value = mp_to_integer(x);
        mpfr_fac_ui(mpc_realref(z->num), value, MPFR_RNDN);
        mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    }
}


void
mp_modulus_divide(const MPNumber *x, const MPNumber *y, MPNumber *z)
{
    if (!mp_is_integer(x) || !mp_is_integer(y))
    {  /* Translators: Error displayed when attemping to do a modulus division on non-integer numbers */
        mperr(_("Modulus division is only defined for integers"));
        mp_set_from_integer(0, z);
        return;
    }

    mp_set_from_mp(x, z);
    mp_divide(x, y, z);
    mp_floor(z, z);
    mp_multiply(z, y, z);
    mp_subtract(x, z, z);

    MPNumber t1 = mp_new();
    mp_set_from_integer(0, &t1);
    if ((mp_compare(y, &t1) < 0 && mp_compare(z, &t1) > 0) ||
        (mp_compare(y, &t1) > 0 && mp_compare(z, &t1) < 0))
        mp_add(z, y, z);

    mp_clear(&t1);
}


void
mp_xpowy(const MPNumber *x, const MPNumber *y, MPNumber *z)
{
    /* 0^-n invalid */
    if (mp_is_zero(x) && mp_is_negative(y))
    {   /* Translators: Error displayed when attempted to raise 0 to a negative exponent */
        mperr(_("The power of zero is undefined for a negative exponent"));
        mp_set_from_integer(0, z);
        return;
    }

    if (!mp_is_complex(x) && !mp_is_complex(y) && !mp_is_integer(y))
    {
        MPNumber reciprocal = mp_new();
        mp_reciprocal(y, &reciprocal);
        if (mp_is_integer(&reciprocal))
        {
            mp_root(x, mp_to_integer(&reciprocal), z);
            mp_clear(&reciprocal);
            return;
        }
        mp_clear(&reciprocal);
    }

    mpc_pow(z->num, x->num, y->num, MPC_RNDNN);
}


void
mp_xpowy_integer(const MPNumber *x, int64_t n, MPNumber *z)
{
    /* 0^-n invalid */
    if (mp_is_zero(x) && n < 0)
    {   /* Translators: Error displayed when attempted to raise 0 to a negative re_exponent */
        mperr(_("The power of zero is undefined for a negative exponent"));
        mp_set_from_integer(0, z);
        return;
    }

    mpc_pow_si(z->num, x->num, (long) n, MPC_RNDNN);
}

void
mp_erf(const MPNumber *x, MPNumber *z)
{
    if (mp_is_complex(x))
    {   /* Translators: Error displayed when error function (erf) value is undefined */
        mperr(_("The error function is only defined for real numbers"));
        mp_set_from_integer(0, z);
        return;
    }

    mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    mpfr_erf(mpc_realref(z->num), mpc_realref(x->num), MPFR_RNDN);
}

void
mp_zeta(const MPNumber *x, MPNumber *z)
{
    MPNumber one = mp_new();

    mp_set_from_integer(1, &one);
    if (mp_is_complex(x) || mp_compare(x, &one) == 0)
    {   /* Translators: Error displayed when zeta function value is undefined */
        mperr(_("The Riemann zeta function is only defined for real numbers ≠1"));
        mp_set_from_integer(0, z);
        mp_clear(&one);
        return;
    }

    mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    mpfr_zeta(mpc_realref(z->num), mpc_realref(x->num), MPFR_RNDN);

    mp_clear(&one);
}

GList*
mp_factorize(const MPNumber *x)
{
    GList *list = NULL;
    MPNumber *factor = g_slice_alloc0(sizeof(MPNumber));
    MPNumber value = mp_new();
    MPNumber tmp = mp_new();
    MPNumber divisor = mp_new();
    MPNumber root = mp_new();
    MPNumber int_max = mp_new();
    mpc_init2(factor->num, PRECISION);

    mp_abs(x, &value);

    if (mp_is_zero(&value))
    {
        mp_set_from_mp(&value, factor);
        list = g_list_append(list, factor);
        return list;
    }

    mp_set_from_integer(1, &tmp);
    if (mp_is_equal(&value, &tmp))
    {
        mp_set_from_mp(x, factor);
        list = g_list_append(list, factor);
        return list;
    }

    /* If value < 2^64-1, call for factorize_uint64 function which deals in integers */
    uint64_t num = 1;
    num = num << 63;
    num += (num - 1);
    mp_set_from_unsigned_integer(num, &int_max);
    if (mp_is_less_equal(x, &int_max))
    {
        list = mp_factorize_unit64(mp_to_unsigned_integer(&value));
        if (mp_is_negative(x))
            mp_invert_sign(list->data, list->data);
        mp_clear(&int_max);
        return list;
    }

    mp_set_from_integer(2, &divisor);
    while (TRUE)
    {
        mp_divide(&value, &divisor, &tmp);
        if (mp_is_integer(&tmp))
        {
            mp_set_from_mp(&tmp, &value);
            mp_set_from_mp(&divisor, factor);
            list = g_list_append(list, factor);
            factor = g_slice_alloc0(sizeof(MPNumber));
            mpc_init2(factor->num, PRECISION);
        }
        else
            break;
    }

    mp_set_from_integer(3, &divisor);
    mp_sqrt(&value, &root);
    while (mp_is_less_equal(&divisor, &root))
    {
        mp_divide(&value, &divisor, &tmp);
        if (mp_is_integer(&tmp))
        {
            mp_set_from_mp(&tmp, &value);
            mp_sqrt(&value, &root);
            mp_set_from_mp(&divisor, factor);
            list = g_list_append(list, factor);
            factor = g_slice_alloc0(sizeof(MPNumber));
            mpc_init2(factor->num, PRECISION);
        } 
        else
        {
            mp_add_integer(&divisor, 2, &tmp);
            mp_set_from_mp(&tmp, &divisor);
        }
    }

    mp_set_from_integer(1, &tmp);
    if (mp_is_greater_than(&value, &tmp))
    {
        mp_set_from_mp(&value, factor);
        list = g_list_append(list, factor);
    } 
    else 
    {
        mpc_clear(factor->num);
        g_slice_free(MPNumber, factor);
    }

    if (mp_is_negative(x))
        mp_invert_sign(list->data, list->data);

    /* MPNumbers in GList will be freed in math_equation_factorize_real */
    mp_clear(&value); mp_clear(&tmp); mp_clear(&divisor); mp_clear(&root);

    return list;
}

GList*
mp_factorize_unit64(uint64_t n)
{
    GList *list = NULL;
    MPNumber *factor = g_slice_alloc0(sizeof(MPNumber));
    MPNumber tmp = mp_new();
    mpc_init2(factor->num, PRECISION);

    mp_set_from_unsigned_integer(2, &tmp);
    while (n % 2 == 0)
    {
        n /= 2;
        mp_set_from_mp(&tmp, factor);
        list = g_list_append(list, factor);
        factor = g_slice_alloc0(sizeof(MPNumber));
        mpc_init2(factor->num, PRECISION);
    }

    for (uint64_t divisor = 3; divisor <= n / divisor; divisor +=2)
    {
        while (n % divisor == 0)
        {
            n /= divisor;
            mp_set_from_unsigned_integer(divisor, factor);
            list = g_list_append(list, factor);
            factor = g_slice_alloc0(sizeof(MPNumber));
            mpc_init2(factor->num, PRECISION);
        }
    }

    if (n > 1)
    {
        mp_set_from_unsigned_integer(n, factor);
        list = g_list_append(list, factor);
    }
    else 
    {
        mpc_clear(factor->num);
        g_slice_free(MPNumber, factor);
    }
    mp_clear(&tmp);

    return list;
}
