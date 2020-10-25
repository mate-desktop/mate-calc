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

MPNumber
mp_new_from_unsigned_integer(ulong x)
{
    MPNumber z;
    mpc_init2(z.num, PRECISION);
    mpc_set_ui(z.num, x, MPC_RNDNN);
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
mp_add_integer(const MPNumber *x, long y, MPNumber *z)
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
mp_divide_integer(const MPNumber *x, long y, MPNumber *z)
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
mp_logarithm(long n, const MPNumber *x, MPNumber *z)
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
mp_multiply_integer(const MPNumber *x, long y, MPNumber *z)
{
    mpc_mul_si(z->num, x->num, y, MPC_RNDNN);
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
mp_root(const MPNumber *x, long n, MPNumber *z)
{
    ulong p;

    if (n < 0)
    {
        mpc_ui_div(z->num, 1, x->num, MPC_RNDNN);

        if (n == LONG_MIN)
            p = (ulong) LONG_MAX + 1;
        else
            p = (ulong) -n;
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
        mpfr_rootn_ui(mpc_realref(z->num), mpc_realref(z->num), p, MPFR_RNDN);
        mpfr_set_zero(mpc_imagref(z->num), MPFR_RNDN);
    }
    else
    {
        mpfr_t tmp;
        mpfr_init2(tmp, PRECISION);
        mpfr_set_ui(tmp, p, MPFR_RNDN);
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
        ulong value = mp_to_unsigned_integer(x);
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

    MPNumber t1 = mp_new();
    MPNumber t2 = mp_new();

    mp_divide(x, y, &t1);
    mp_floor(&t1, &t1);
    mp_multiply(&t1, y, &t2);
    mp_subtract(x, &t2, z);

    mp_set_from_integer(0, &t1);
    if ((mp_compare(y, &t1) < 0 && mp_compare(z, &t1) > 0) ||
        (mp_compare(y, &t1) > 0 && mp_compare(z, &t1) < 0))
        mp_add(z, y, z);

    mp_clear(&t1);
    mp_clear(&t2);
}

void
mp_modular_exponentiation(const MPNumber *x, const MPNumber *y, const MPNumber *p, MPNumber *z)
{
    MPNumber base_value = mp_new();
    MPNumber exp_value = mp_new();
    MPNumber ans = mp_new();
    MPNumber two = mp_new();
    MPNumber tmp = mp_new();

    mp_set_from_integer(1, &ans);
    mp_set_from_integer(2, &two);
    mp_abs(y, &exp_value);

    if (mp_is_negative(y))
        mp_reciprocal(x, &base_value);
    else
        mp_set_from_mp(x, &base_value);

    while (!mp_is_zero(&exp_value))
    {
        mp_modulus_divide(&exp_value, &two, &tmp);

        bool is_even = mp_is_zero(&tmp);
        if (!is_even)
        {
            mp_multiply(&ans, &base_value, &ans);
            mp_modulus_divide(&ans, p, &ans);
        }
        mp_multiply(&base_value, &base_value, &base_value);
        mp_modulus_divide(&base_value, p, &base_value);
        mp_divide_integer(&exp_value, 2, &exp_value);
        mp_floor(&exp_value, &exp_value);
    }

    mp_modulus_divide(&ans, p, z);

    mp_clear(&base_value);
    mp_clear(&exp_value);
    mp_clear(&ans);
    mp_clear(&two);
    mp_clear(&tmp);
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
mp_xpowy_integer(const MPNumber *x, long n, MPNumber *z)
{
    /* 0^-n invalid */
    if (mp_is_zero(x) && n < 0)
    {   /* Translators: Error displayed when attempted to raise 0 to a negative re_exponent */
        mperr(_("The power of zero is undefined for a negative exponent"));
        mp_set_from_integer(0, z);
        return;
    }

    mpc_pow_si(z->num, x->num, n, MPC_RNDNN);
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

/***********************************************************************/
/**                          FACTORIZATION                            **/
/***********************************************************************/


/**
 * mp_is_pprime uses the Miller-Rabin primality test to decide
 * whether or not a number is probable prime.
 * For special values of @n and @rounds it can be deterministic,
 * but in general the probability of declaring @n as prime if it
 * is not is at most 4^(-@rounds).
 * @n has to be odd.
 * Returns TRUE if @n is probable prime and FALSE otherwise.
 */
static bool
mp_is_pprime(MPNumber *n, ulong rounds)
{
    MPNumber tmp = mp_new();
    MPNumber two = mp_new_from_unsigned_integer(2);
    ulong l = 0;
    bool is_pprime = TRUE;

    /* Write t := n-1 = 2^l * q with q odd */
    MPNumber q = mp_new();
    MPNumber t = mp_new();
    mp_add_integer(n, -1, &t);
    mp_set_from_mp(&t, &q);
    do
    {
        mp_divide_integer(&q, 2, &q);
        mp_modulus_divide(&q, &two, &tmp);
        l++;
    } while (mp_is_zero(&tmp));

    /* @rounds Miller-Rabin tests to bases a = 2,3,...,@rounds+1 */
    MPNumber one = mp_new_from_unsigned_integer(1);
    MPNumber a = mp_new_from_unsigned_integer(1);
    MPNumber b = mp_new();

    for (ulong i = 1; (i < mp_to_integer(&t)) && (i <= rounds+1); i++)
    {
        mp_add_integer(&a, 1, &a);
        mp_modular_exponentiation(&a, &q, n, &b);
        if (mp_compare(&one, &b) == 0 || mp_compare(&t, &b) == 0)
        {
            continue;
        }

        bool is_witness = FALSE;
        for (long j = 1; j < l; j++)
        {
            mp_modular_exponentiation(&b, &two, n, &b);
            if (mp_compare(&b, &t) == 0)
            {
                is_witness = TRUE;
                break;
            }
        }

        if (!is_witness)
        {
            is_pprime = FALSE;
            break;
        }
    }

    mp_clear(&t);
    mp_clear(&q);
    mp_clear(&a);
    mp_clear(&b);
    mp_clear(&one);
    mp_clear(&two);
    mp_clear(&tmp);

    return is_pprime;
}

/**
 * Sets z = gcd(a,b) where gcd(a,b) is the greatest common divisor of @a and @b.
 */
static void
mp_gcd (const MPNumber *a, const MPNumber *b, MPNumber *z)
{
    MPNumber null = mp_new_from_unsigned_integer(0);
    MPNumber t1 = mp_new();
    MPNumber t2 = mp_new();

    mp_set_from_mp(a, z);
    mp_set_from_mp(b, &t2);

    while (mp_compare(&t2, &null) != 0)
    {
        mp_set_from_mp(&t2, &t1);
        mp_modulus_divide(z, &t2, &t2);
        mp_set_from_mp(&t1, z);
    }

    mp_clear(&null);
    mp_clear(&t1);
    mp_clear(&t2);
}

/**
 * mp_pollard_rho searches a divisor of @n using Pollard's rho algorithm.
 * @i is the start value of the pseudorandom sequence which is generated
 * by the polynomial x^2+1 mod n.
 *
 * Returns TRUE if a divisor was found and stores the divisor in z.
 * Returns FALSE otherwise.
 */
static bool
mp_pollard_rho (const MPNumber *n, ulong i, MPNumber *z)
{
    MPNumber one = mp_new_from_unsigned_integer(1);
    MPNumber two = mp_new_from_unsigned_integer(2);
    MPNumber x = mp_new_from_unsigned_integer(i);
    MPNumber y = mp_new_from_unsigned_integer(2);
    MPNumber d = mp_new_from_unsigned_integer(1);

    while (mp_compare(&d, &one) == 0)
    {
        mp_modular_exponentiation(&x, &two, n, &x);
        mp_add(&x, &one, &x);

        mp_modular_exponentiation(&y, &two, n, &y);
        mp_add(&y, &one, &y);

        mp_modular_exponentiation(&y, &two, n, &y);
        mp_add(&y, &one, &y);

        mp_subtract(&x, &y,z);
        mp_abs(z, z);
        mp_gcd(z, n, &d);
    }

    if (mp_compare(&d, n) == 0)
    {
        mp_clear(&one);
        mp_clear(&two);
        mp_clear(&x);
        mp_clear(&y);
        mp_clear(&d);

        return FALSE;
    }
    else
    {
        mp_set_from_mp(&d, z);

        mp_clear(&one);
        mp_clear(&two);
        mp_clear(&x);
        mp_clear(&y);
        mp_clear(&d);

        return TRUE;
    }
}

/**
 * find_big_prime_factor acts as driver function for mp_pollard_rho which
 * is run as long as a prime factor is found.
 * On success sets @z to a prime factor of @n.
 */
static void
find_big_prime_factor (const MPNumber *n, MPNumber *z)
{
    MPNumber tmp = mp_new();
    ulong i = 2;

    while (TRUE)
    {
        while (mp_pollard_rho (n, i, &tmp) == FALSE)
        {
            i++;
        }

        if (!mp_is_pprime(&tmp, 50))
        {
            mp_divide(n, &tmp, &tmp);
        }
        else
            break;
    }

    mp_set_from_mp(&tmp, z);
    mp_clear(&tmp);
}

/**
 * mp_factorize tries to factorize the value of @x.
 * If @x < 2^64 it calls mp_factorize_unit64 which deals in integers
 * and should be fast enough for most values.
 * If @x > 2^64 the approach to find factors of @x is as follows:
 *   - Try to divide @x by prime numbers 2,3,5,7,.. up to min(2^13, sqrt(x))
 *   - Use Pollard rho to find prime factors > 2^13.
 * Returns a pointer to a GList with all prime factors of @x which needs to
 * be freed.
 */
GList*
mp_factorize(const MPNumber *x)
{
    GList *list = NULL;
    MPNumber *factor = g_slice_alloc0(sizeof(MPNumber));
    mpc_init2(factor->num, PRECISION);

    MPNumber value = mp_new();
    mp_abs(x, &value);

    if (mp_is_zero(&value))
    {
        mp_set_from_mp(&value, factor);
        list = g_list_append(list, factor);
        mp_clear(&value);
        return list;
    }

    MPNumber tmp = mp_new();
    mp_set_from_integer(1, &tmp);
    if (mp_is_equal(&value, &tmp))
    {
        mp_set_from_mp(x, factor);
        list = g_list_append(list, factor);
        mp_clear(&value);
        mp_clear(&tmp);
        return list;
    }

    /* If value < 2^64-1, call for factorize_uint64 function which deals in integers */
    uint64_t num = 1;
    num = num << 63;
    num += (num - 1);
    MPNumber int_max = mp_new();
    mp_set_from_unsigned_integer(num, &int_max);
    if (mp_is_less_equal(x, &int_max))
    {
        list = mp_factorize_unit64(mp_to_unsigned_integer(&value));
        if (mp_is_negative(x))
            mp_invert_sign(list->data, list->data);
        mp_clear(&value);
        mp_clear(&tmp);
        mp_clear(&int_max);
        return list;
    }

    MPNumber divisor = mp_new_from_unsigned_integer(2);
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

    MPNumber root = mp_new();
    mp_sqrt(&value, &root);
    uint64_t max_trial_division = (uint64_t) (1 << 10);
    uint64_t iter = 0;
    while (mp_is_less_equal(&divisor, &root) && (iter++ < max_trial_division))
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
            mp_add_integer(&divisor, 2, &divisor);
        }
    }

    while (!mp_is_pprime(&value, 50))
    {
        find_big_prime_factor (&value, &divisor);

        mp_divide(&value, &divisor, &tmp);
        if (mp_is_integer(&tmp))
        {
            mp_set_from_mp(&tmp, &value);
            mp_set_from_mp(&divisor, factor);
            list = g_list_append(list, factor);
            factor = g_slice_alloc0(sizeof(MPNumber));
            mpc_init2(factor->num, PRECISION);
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

    mp_clear(&value);
    mp_clear(&tmp);
    mp_clear(&divisor);
    mp_clear(&root);

    return list;
}

GList*
mp_factorize_unit64(uint64_t n)
{
    GList *list = NULL;
    MPNumber *factor = g_slice_alloc0(sizeof(MPNumber));
    mpc_init2(factor->num, PRECISION);

    MPNumber tmp = mp_new();
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
