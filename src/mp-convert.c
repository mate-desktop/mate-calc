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
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <langinfo.h>

#include "mp.h"

void
mp_set_from_mp(const MPNumber *x, MPNumber *z)
{
    mpc_set(z->num, x->num, MPC_RNDNN);
}


void
mp_set_from_double(double dx, MPNumber *z)
{
    mpc_set_d(z->num, dx, MPC_RNDNN);
}


void
mp_set_from_integer(int64_t x, MPNumber *z)
{
    mpc_set_si(z->num, x, MPC_RNDNN);
}


void
mp_set_from_unsigned_integer(uint64_t x, MPNumber *z)
{
    mpc_set_ui(z->num, x, MPC_RNDNN);
}


void
mp_set_from_fraction(int64_t numerator, int64_t denominator, MPNumber *z)
{
    if (denominator < 0) {
        numerator = -numerator;
        denominator = -denominator;
    }

    mp_set_from_integer(numerator, z);
    if (denominator != 1)
        mp_divide_integer(z, denominator, z);
}


void
mp_set_from_polar(const MPNumber *r, MPAngleUnit unit, const MPNumber *theta, MPNumber *z)
{
    MPNumber x = mp_new();
    MPNumber y = mp_new();

    mp_cos(theta, unit, &x);
    mp_multiply(&x, r, &x);
    mp_sin(theta, unit, &y);
    mp_multiply(&y, r, &y);
    mp_set_from_complex(&x, &y, z);

    mp_clear(&x);
    mp_clear(&y);
}

void
mp_set_from_complex(const MPNumber *x, const MPNumber *y, MPNumber *z)
{
    mpc_set_fr_fr(z->num, mpc_realref(x->num), mpc_realref(y->num), MPC_RNDNN);
}

void
mp_set_from_random(MPNumber *z)
{
    mp_set_from_double(drand48(), z);
}

int64_t
mp_to_integer(const MPNumber *x)
{
    return mpfr_get_si(mpc_realref(x->num), MPFR_RNDN);
}


uint64_t
mp_to_unsigned_integer(const MPNumber *x)
{
    return mpfr_get_ui(mpc_realref(x->num), MPFR_RNDN);
}

float
mp_to_float(const MPNumber *x)
{
    return mpfr_get_flt(mpc_realref(x->num), MPFR_RNDN);
}

double
mp_to_double(const MPNumber *x)
{
    return mpfr_get_d(mpc_realref(x->num), MPFR_RNDN);
}

static int
char_val(char **c, int base)
{
    int i, j, value, offset;
    const char *digits[][10] = {{"٠", "١", "٢", "٣", "٤", "٥", "٦", "٧", "٨", "٩"},
                                {"〇", "〡", "〢", "〣", "〤", "〥", "〦", "〧", "〨", "〩"},
                                {"۰", "۱", "۲", "۳", "۴", "۵", "۶", "۷", "۸", "۹"},
                                {"߀", "߁", "߂", "߃", "߄", "߅", "߆", "߇", "߈", "߉"},
                                {"०", "१", "२", "३", "४", "५", "६", "७", "८", "९"},
                                {"০", "১", "২", "৩", "৪", "৫", "৬", "৭", "৮", "৯"},
                                {"੦", "੧", "੨", "੩", "੪", "੫", "੬", "੭", "੮", "੯"},
                                {"૦", "૧", "૨", "૩", "૪", "૫", "૬", "૭", "૮", "૯"},
                                {"୦", "୧", "୨", "୩", "୪", "୫", "୬", "୭", "୮", "୯"},
                                {"௦", "௧", "௨", "௩", "௪", "௫", "௬", "௭", "௮", "௯"},
                                {"౦", "౧", "౨", "౩", "౪", "౫", "౬", "౭", "౮", "౯"},
                                {"೦", "೧", "೨", "೩", "೪", "೫", "೬", "೭", "೮", "೯"},
                                {"൦", "൧", "൨", "൩", "൪", "൫", "൬", "൭", "൮", "൯"},
                                {"๐", "๑", "๒", "๓", "๔", "๕", "๖", "๗", "๘", "๙"},
                                {"໐", "໑", "໒", "໓", "໔", "໕", "໖", "໗", "໘", "໙"},
                                {"༠", "༡", "༢", "༣", "༤", "༥", "༦", "༧", "༨", "༩"},
                                {"၀", "၁", "၂", "၃", "၄", "၅", "၆", "၇", "၈", "၉"},
                                {"႐", "႑", "႒", "႓", "႔", "႕", "႖", "႗", "႘", "႙"},
                                {"០", "១", "២", "៣", "៤", "៥", "៦", "៧", "៨", "៩"},
                                {"᠐", "᠑", "᠒", "᠓", "᠔", "᠕", "᠖", "᠗", "᠘", "᠙"},
                                {"᥆", "᥇", "᥈", "᥉", "᥊", "᥋", "᥌", "᥍", "᥎", "᥏"},
                                {"᧐", "᧑", "᧒", "᧓", "᧔", "᧕", "᧖", "᧗", "᧘", "᧙"},
                                {"᭐", "᭑", "᭒", "᭓", "᭔", "᭕", "᭖", "᭗", "᭘", "᭙"},
                                {"᮰", "᮱", "᮲", "᮳", "᮴", "᮵", "᮶", "᮷", "᮸", "᮹"},
                                {"᱀", "᱁", "᱂", "᱃", "᱄", "᱅", "᱆", "᱇", "᱈", "᱉"},
                                {"᱐", "᱑", "᱒", "᱓", "᱔", "᱕", "᱖", "᱗", "᱘", "᱙"},
                                {"꘠", "꘡", "꘢", "꘣", "꘤", "꘥", "꘦", "꘧", "꘨", "꘩"},
                                {"꣐", "꣑", "꣒", "꣓", "꣔", "꣕", "꣖", "꣗", "꣘", "꣙"},
                                {"꤀", "꤁", "꤂", "꤃", "꤄", "꤅", "꤆", "꤇", "꤈", "꤉"},
                                {"꩐", "꩑", "꩒", "꩓", "꩔", "꩕", "꩖", "꩗", "꩘", "꩙"},
                                {"𐒠", "𐒡", "𐒢", "𐒣", "𐒤", "𐒥", "𐒦", "𐒧", "𐒨", "𐒩"},
                                {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};

    if (**c >= '0' && **c <= '9') {
        value = **c - '0';
        offset = 1;
    } else if (**c >= 'a' && **c <= 'f') {
        value = **c - 'a' + 10;
        offset = 1;
    } else if (**c >= 'A' && **c <= 'F') {
        value = **c - 'A' + 10;
        offset = 1;
    } else {
        for (i = 0; digits[i][0]; i++) {
            for (j = 0; j < 10; j++) {
                if (strncmp(*c, digits[i][j], strlen(digits[i][j])) == 0)
                    break;
            }
            if (j != 10)
                break;
        }
        if (digits[i][0] == NULL)
            return -1;
        value = j;
        offset = strlen(digits[i][j]);
    }
    if (value >= base)
       return -1;

    *c += offset;

    return value;
}


static int
ends_with(const char *start, const char *end, const char *word)
{
    size_t word_len = strlen(word);

    if (word_len > end - start)
        return 0;

    return strncmp(end - word_len, word, word_len) == 0;
}


// FIXME: Doesn't handle errors well (e.g. trailing space)
static bool
set_from_sexagesimal(const char *str, int length, MPNumber *z)
{
    int degrees = 0, minutes = 0;
    char seconds[length+1];
    int n_matched;

    seconds[0] = '\0';
    n_matched = sscanf(str, "%d°%d'%s\"", &degrees, &minutes, seconds);

    if (n_matched < 1)
        return true;
    MPNumber t = mp_new();
    mp_set_from_integer(degrees, z);
    if (n_matched > 1) {
        mp_set_from_integer(minutes, &t);
        mp_divide_integer(&t, 60, &t);
        mp_add(z, &t, z);
    }
    if (n_matched > 2) {
        mp_set_from_string(seconds, 10, &t);
        mp_divide_integer(&t, 3600, &t);
        mp_add(z, &t, z);
    }
    mp_clear(&t);

    return false;
}


bool
mp_set_from_string(const char *str, int default_base, MPNumber *z)
{
    int i, base, negate = 0, multiplier = 0, base_multiplier = 1;
    const char *c, *end;
    gboolean has_fraction = FALSE;

    const char *base_digits[]   = {"₀", "₁", "₂", "₃", "₄", "₅", "₆", "₇", "₈", "₉", NULL};
    const char *fractions[]     = {"½", "⅓", "⅔", "¼", "¾", "⅕", "⅖", "⅗", "⅘", "⅙", "⅚", "⅛", "⅜", "⅝", "⅞", NULL};
    int numerators[]            = { 1,   1,   2,   1,   3,   1,   2,   3,   4,   1,   5,   1,   3,   5,   7};
    int denominators[]          = { 2,   3,   3,   4,   4,   5,   5,   5,   5,   6,   6,   8,   8,   8,   8};

    if (strstr(str, "°"))
        return set_from_sexagesimal(str, strlen(str), z);

    /* Find the base */
    end = str;
    while (*end != '\0')
        end++;
    base = 0;
    while (1) {
        for (i = 0; base_digits[i] != NULL; i++) {
            if (ends_with(str, end, base_digits[i])) {
                base += i * base_multiplier;
                end -= strlen(base_digits[i]);
                base_multiplier *= 10;
                break;
            }
        }
        if (base_digits[i] == NULL)
            break;
    }
    if (base_multiplier == 1)
        base = default_base;

    /* Check if this has a sign */
    c = str;
    if (*c == '+') {
        c++;
    } else if (*c == '-') {
        negate = 1;
        c++;
    } else if (strncmp(c, "−", strlen("−")) == 0) {
        negate = 1;
        c += strlen("−");
    }

    /* Convert integer part */
    mp_set_from_integer(0, z);
    while ((i = char_val((char **)&c, base)) >= 0) {
        if (i > base)
            return true;
        mp_multiply_integer(z, base, z);
        mp_add_integer(z, i, z);
    }

    /* Look for fraction characters, e.g. ⅚ */
    for (i = 0; fractions[i] != NULL; i++) {
        if (ends_with(str, end, fractions[i])) {
            end -= strlen(fractions[i]);
            break;
        }
    }
    if (fractions[i] != NULL) {
        MPNumber fraction = mp_new();
        mp_set_from_fraction(numerators[i], denominators[i], &fraction);
        mp_add(z, &fraction, z);
        mp_clear(&fraction);
    }

    if (*c == '.') {
        has_fraction = TRUE;
        c++;
    }

    /* Convert fractional part */
    if (has_fraction) {
        MPNumber numerator = mp_new();
        MPNumber denominator = mp_new();

        mp_set_from_integer(0, &numerator);
        mp_set_from_integer(1, &denominator);
        while ((i = char_val((char **)&c, base)) >= 0) {
            mp_multiply_integer(&denominator, base, &denominator);
            mp_multiply_integer(&numerator, base, &numerator);
            mp_add_integer(&numerator, i, &numerator);
        }
        mp_divide(&numerator, &denominator, &numerator);
        mp_add(z, &numerator, z);
        mp_clear(&numerator);
        mp_clear(&denominator);
    }

    if (c != end) {
        return true;
    }

    if (multiplier != 0) {
        MPNumber t = mp_new();
        mp_set_from_integer(10, &t);
        mp_xpowy_integer(&t, multiplier, &t);
        mp_multiply(z, &t, z);
        mp_clear(&t);
    }

    if (negate == 1)
        mp_invert_sign(z, z);

    return false;
}
