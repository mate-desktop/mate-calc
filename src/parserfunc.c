#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "parserfunc.h"

/* Register error variables in ParserState structure. */
void
set_error(ParserState* state, gint errorno, const gchar *token)
{
    state->error = errorno;
    if(token)
        state->error_token = strdup(token);
}

/* Unused function pointer. This won't be called anytime. */
void*
pf_none(ParseNode* self)
{
    return NULL;
}

/* Set variable. */
void*
pf_set_var(ParseNode* self)
{
    MPNumber* val;
    val = (MPNumber *) (*(self->right->evaluate))(self->right);
    if(!val || !(self->state->set_variable))
    {
        if(val)
            mp_free(val);
        return NULL;
    }
    (*(self->state->set_variable))(self->state, self->left->token->string, val);
    return val;
}

/* Converts Number from one unit to other. */
void*
pf_convert_number(ParseNode* self)
{
    gchar* from;
    gchar* to;
    gint free_from = 0;
    gint free_to = 0;
    MPNumber tmp = mp_new();
    MPNumber* ans = mp_new_ptr();
    if(self->left->value)
    {
        from = (gchar*) self->left->value;
        free_from = 1;
    }
    else
        from = self->left->token->string;
    if(self->right->value)
    {
        to = (gchar*) self->right->value;
        free_to = 1;
    }
    else
        to = self->right->token->string;

    if(mp_set_from_string(self->left->left->token->string, self->state->options->base, &tmp) != 0)
    {
        mp_free(ans);
        ans = NULL;
        goto END_PF_CONVERT_NUMBER;
    }
    if(!(self->state->convert))
    {
        mp_free(ans);
        ans = NULL;
        goto END_PF_CONVERT_NUMBER;
    }
    if(!(*(self->state->convert))(self->state, &tmp, from, to, ans))
    {
        set_error(self->state, PARSER_ERR_UNKNOWN_CONVERSION, NULL);
        mp_free(ans);
        ans = NULL;
    }
END_PF_CONVERT_NUMBER:
    if(free_from)
    {
        g_free(self->left->value);
        self->left->value = NULL;
    }
    if(free_to)
    {
        g_free(self->right->value);
        self->right->value = NULL;
    }
    mp_clear(&tmp);
    return ans;
}

/* Conversion rate. */
void*
pf_convert_1(ParseNode* self )
{
    gchar* from;
    gchar* to;
    gint free_from = 0;
    gint free_to = 0;
    MPNumber tmp = mp_new();
    MPNumber* ans = mp_new_ptr();
    if(self->left->value)
    {
        from = (gchar*) self->left->value;
        free_from = 1;
    }
    else
        from = self->left->token->string;
    if(self->right->value)
    {
        to = (gchar*) self->right->value;
        free_to = 1;
    }
    else
        to = self->right->token->string;
    mp_set_from_integer(1, &tmp);
    if(!(self->state->convert))
    {
        mp_free(ans);
        return NULL;
    }
    if(!(*(self->state->convert))(self->state, &tmp, from, to, ans))
    {
        set_error(self->state, PARSER_ERR_UNKNOWN_CONVERSION, NULL);
        mp_free(ans);
        ans = NULL;
    }
    if(free_from)
    {
        g_free(self->left->value);
        self->left->value = NULL;
    }
    if(free_to)
    {
        g_free(self->right->value);
        self->right->value = NULL;
    }
    mp_clear(&tmp);
    return ans;
}

/* Join source unit and power. */
gchar*
pf_make_unit(gchar* source, gchar* power)
{
    return g_strjoin(NULL, source, power, NULL);
}

static gchar *
utf8_next_char(const gchar *c)
{
    c++;
    while((*c & 0xC0) == 0x80)
        c++;
    return(gchar *) c;
}

/* Get value of variable. */
void*
pf_get_variable(ParseNode* self)
{
    gint result = 0;

    const gchar *c, *next;
    gchar *buffer;
    MPNumber value = mp_new();

    MPNumber t = mp_new();
    MPNumber* ans = mp_new_ptr();

    if(!(self->state->get_variable))
    {
        free(ans);
        return NULL;
    }

    /* If defined, then get the variable */
    if((*(self->state->get_variable))(self->state, self->token->string, ans))
    {
        return ans;
    }

    /* If has more than one character then assume a multiplication of variables */
    if(utf8_next_char(self->token->string)[0] != '\0')
    {
        result = 1;
        buffer = (gchar*) malloc(sizeof(gchar) * strlen(self->token->string));
        mp_set_from_integer(1, &value);
        for(c = self->token->string; *c != '\0'; c = next)
        {
            next = utf8_next_char(c);
            snprintf(buffer, next - c + 1, "%s", c);
            if(!(*(self->state->get_variable))(self->state, buffer, &t))
            {
                result = 0;
                break;
            }
            mp_multiply(&value, &t, &value);
        }
        free(buffer);
        if(result)
            mp_set_from_mp(&value, ans);
    }
    if(!result)
    {
        free (ans);
        ans = NULL;
        set_error(self->state, PARSER_ERR_UNKNOWN_VARIABLE, self->token->string);
    }
    return ans;
}

/* Get value of variable with power. */
void*
pf_get_variable_with_power(ParseNode* self)
{
    gint result = 0;
    gint pow;

    const gchar *c, *next;
    gchar *buffer;
    MPNumber value = mp_new();

    MPNumber t = mp_new();
    MPNumber* ans = mp_new_ptr();
    pow = super_atoi(((LexerToken*) self->value)->string);

    /* No need to free the memory. It is allocated and freed somewhere else. */
    self->value = NULL;

    if(!(self->state->get_variable))
    {
        free(ans);
        return NULL;
    }

    /* If defined, then get the variable */
    if((*(self->state->get_variable))(self->state, self->token->string, ans))
    {
        mp_xpowy_integer(ans, pow, ans);
        return ans;
    }

    /* If has more than one character then assume a multiplication of variables */
    if(utf8_next_char(self->token->string)[0] != '\0')
    {
        result = 1;
        buffer = (gchar*) malloc(sizeof(gchar) * strlen(self->token->string));
        mp_set_from_integer(1, &value);
        for(c = self->token->string; *c != '\0'; c = next)
        {
            next = utf8_next_char(c);
            snprintf(buffer, next - c + 1, "%s", c);
            if(!(*(self->state->get_variable))(self->state, buffer, &t))
            {
                result = 0;
                break;
            }

            /* If last term do power */
            if(*next == '\0')
                mp_xpowy_integer(&t, pow, &t);
            mp_multiply(&value, &t, &value);
        }
        free(buffer);
        if(result)
            mp_set_from_mp(&value, ans);
    }
    if(!result)
    {
        free(ans);
        ans = NULL;
        set_error(self->state, PARSER_ERR_UNKNOWN_VARIABLE, self->token->string);
    }
    return ans;
}

/* Apply function on child. */
void*
pf_apply_func(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!(self->state->get_function))
    {
        free(val);
        free(ans);
        return NULL;
    }
    if(!val)
    {
        free(ans);
        return NULL;
    }
    if(!(*(self->state->get_function))(self->state, self->token->string, val, ans))
    {
        free(val);
        free(ans);
        set_error(self->state, PARSER_ERR_UNKNOWN_FUNCTION, self->token->string);
        return NULL;
    }
    free(val);
    return ans;
}

/* Apply function with +ve power. */
void*
pf_apply_func_with_power(ParseNode* self)
{
    MPNumber* val;
    MPNumber* tmp = mp_new_ptr();
    MPNumber* ans = mp_new_ptr();
    gint pow;
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!(self->state->get_function))
    {
        mp_free(tmp);
        mp_free(ans);
        mp_free(val);
        self->value = NULL;
        return NULL;
    }
    if(!val)
    {
        mp_free(tmp);
        mp_free(ans);
        self->value = NULL;
        return NULL;
    }
    if(!(*(self->state->get_function))(self->state, self->token->string, val, tmp))
    {
        mp_free(tmp);
        mp_free(ans);
        mp_free(val);
        self->value = NULL;
        set_error(self->state, PARSER_ERR_UNKNOWN_FUNCTION, self->token->string);
        return NULL;
    }
    pow = super_atoi(((LexerToken*) self->value)->string);
    mp_xpowy_integer(tmp, pow, ans);
    mp_free(val);
    mp_free(tmp);
    self->value = NULL;
    return ans;
}

/* Apply function with -ve power. */
void*
pf_apply_func_with_npower(ParseNode* self)
{
    MPNumber* val;
    MPNumber* tmp = mp_new_ptr();
    MPNumber* ans = mp_new_ptr();
    gint pow;
    gchar* inv_name;
    inv_name = (gchar*) malloc(sizeof(gchar) * strlen(self->token->string) + strlen("⁻¹") + 1);
    strcpy(inv_name, self->token->string);
    strcat(inv_name, "⁻¹");
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(tmp);
        free(inv_name);
        mp_free(ans);
        self->value = NULL;
        return NULL;
    }
    if(!(self->state->get_function))
    {
        mp_free(tmp);
        mp_free(ans);
        free(inv_name);
        self->value = NULL;
        return NULL;
    }
    if(!(*(self->state->get_function))(self->state, inv_name, val, tmp))
    {
        mp_free(tmp);
        mp_free(ans);
        mp_free(val);
        free(inv_name);
        self->value = NULL;
        set_error(self->state, PARSER_ERR_UNKNOWN_FUNCTION, self->token->string);
        return NULL;
    }
    pow = super_atoi(((LexerToken*) self->value)->string);
    mp_xpowy_integer(tmp, -pow, ans);
    mp_free(val);
    mp_free(tmp);
    free(inv_name);
    self->value = NULL;
    return ans;
}

/* Find nth root of child. */
void*
pf_do_nth_root(ParseNode* self)
{
    MPNumber* val;
    gint pow;
    MPNumber* ans = mp_new_ptr();
    pow = sub_atoi(((LexerToken*) self->value)->string);
    self->value = NULL;
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_root(val, pow, ans);
    mp_free(val);
    return ans;
}

/* Find sqrt of child. */
void*
pf_do_sqrt(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        free(ans);
        return NULL;
    }
    mp_sqrt(val, ans);
    free(val);
    return ans;
}

/* Find 3rd root of child. */
void*
pf_do_root_3(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_root(val, 3, ans);
    mp_free(val);
    return ans;
}

/* Find 4th root of child. */
void*
pf_do_root_4(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_root(val, 4, ans);
    mp_free(val);
    return ans;
}

/* Apply floor function. */
void*
pf_do_floor(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_floor(val, ans);
    mp_free(val);
    return ans;
}

/* Apply ceiling function. */
void* pf_do_ceiling (ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_ceiling(val, ans);
    mp_free(val);
    return ans;
}

/* Round off. */
void*
pf_do_round(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_round(val, ans);
    mp_free(val);
    return ans;
}

/* Fraction. */
void*
pf_do_fraction(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_fractional_part(val, ans);
    mp_free(val);
    return ans;
}

/* Absolute value. */
void*
pf_do_abs(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_abs(val, ans);
    mp_free(val);
    return ans;
}

/* Find x^y for x and y being MPNumber. */
void*
pf_do_x_pow_y(ParseNode* self)
{
    MPNumber* val;
    MPNumber* pow;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->left->evaluate))(self->left);
    pow = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val || !pow)
    {
        if(val)
            mp_free(val);
        if(pow)
            mp_free(pow);
        mp_free(ans);
        return NULL;
    }
    mp_xpowy(val, pow, ans);
    mp_free(val);
    mp_free(pow);
    return ans;
}

/* Find x^y for MPNumber x and integer y. */
void*
pf_do_x_pow_y_int(ParseNode* self)
{
    MPNumber* val;
    gint pow;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->left->evaluate))(self->left);
    pow = super_atoi(self->right->token->string);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_xpowy_integer(val, pow, ans);
    mp_free(val);
    return ans;
}

/* Find factorial. */
void*
pf_do_factorial(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_factorial(val, ans);
    mp_free(val);
    return ans;
}

/* Apply unary minus. */
void*
pf_unary_minus(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_invert_sign(val, ans);
    mp_free(val);
    return ans;
}

/* Divide. */
void*
pf_do_divide(ParseNode* self)
{
    MPNumber* left;
    MPNumber* right;
    MPNumber* ans = mp_new_ptr();
    left = (MPNumber*) (*(self->left->evaluate))(self->left);
    right = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!left || !right)
    {
        if(left)
            mp_free(left);
        if(right)
            mp_free(right);
        mp_free(ans);
        return NULL;
    }
    mp_divide(left, right, ans);
    mp_free(left);
    mp_free(right);
    return ans;
}

/* Modulus. */
void*
pf_do_mod(ParseNode* self)
{
    MPNumber* left;
    MPNumber* right;
    MPNumber* ans = mp_new_ptr();
    left = (MPNumber*) (*(self->left->evaluate))(self->left);
    right = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!left || !right)
    {
        if(left)
            mp_free(left);
        if(right)
            mp_free(right);
        free(ans);
        return NULL;
    }
    if (self->left->evaluate == pf_do_x_pow_y)
    {
        MPNumber* base_value = (MPNumber*) (*(self->left->left->evaluate))(self->left->left);
        MPNumber* exponent = (MPNumber*) (*(self->left->right->evaluate))(self->left->right);
        if(!base_value || !exponent)
        {
            if(base_value)
                mp_free(base_value);
            if(exponent)
                mp_free(exponent);
            free(ans);
            return NULL;
        }
        mp_modular_exponentiation(base_value, exponent, right, ans);
        mp_free(base_value);
        mp_free(exponent);
    }
    else
        mp_modulus_divide(left, right, ans);

    mp_free(left);
    mp_free(right);
    return ans;
}

/* Multiply two numbers. */
void*
pf_do_multiply(ParseNode* self)
{
    MPNumber* left;
    MPNumber* right;
    MPNumber* ans = mp_new_ptr();
    left = (MPNumber*) (*(self->left->evaluate))(self->left);
    right = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!left || !right)
    {
        if(left)
            mp_free(left);
        if(right)
            mp_free(right);
        mp_free(ans);
        return NULL;
    }
    mp_multiply(left, right, ans);
    mp_free(left);
    mp_free(right);
    return ans;
}

/* Subtract two numbers. */
void*
pf_do_subtract(ParseNode* self)
{
    MPNumber* left;
    MPNumber* right;
    MPNumber* ans = mp_new_ptr();
    left = (MPNumber*) (*(self->left->evaluate))(self->left);
    right = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!left || !right)
    {
        if(left)
            mp_free(left);
        if(right)
            mp_free(right);
        free(ans);
        return NULL;
    }
    mp_subtract(left, right, ans);
    mp_free(left);
    mp_free(right);
    return ans;
}

/* Add two numbers. */
void*
pf_do_add(ParseNode* self)
{
    MPNumber* left;
    MPNumber* right;
    MPNumber* ans = mp_new_ptr();
    left = (MPNumber*) (*(self->left->evaluate))(self->left);
    right = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!left || !right)
    {
        if(left)
            mp_free(left);
        if(right)
            mp_free(right);
        free(ans);
        return NULL;
    }
    mp_add(left, right, ans);
    mp_free(left);
    mp_free(right);
    return ans;
}

/* Add (x) Percentage to value. */
void*
pf_do_add_percent(ParseNode* self)
{
    MPNumber* ans = mp_new_ptr();
    MPNumber* val;
    MPNumber* per;
    val = (MPNumber*) (*(self->left->evaluate))(self->left);
    per = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val || !per)
    {
        if(val)
            mp_free(val);
        if(per)
            mp_free(per);
        mp_free(ans);
        return NULL;
    }
    mp_add_integer(per, 100, per);
    mp_divide_integer(per, 100, per);
    mp_multiply(val, per, ans);
    mp_free(val);
    mp_free(per);
    return ans;
}

/* Subtract (x) Percentage to value. */
void*
pf_do_subtract_percent(ParseNode* self)
{
    MPNumber* ans = mp_new_ptr();
    MPNumber* val;
    MPNumber* per;
    val = (MPNumber*) (*(self->left->evaluate))(self->left);
    per = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val || !per)
    {
        if(val)
            mp_free(val);
        if(per)
            mp_free(per);
        free(ans);
        return NULL;
    }
    mp_add_integer(per, -100, per);
    mp_divide_integer(per, -100, per);
    mp_multiply(val, per, ans);
    mp_free(val);
    mp_free(per);
    return ans;
}

/* Converts a constant into percentage. */
void*
pf_do_percent(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    mp_divide_integer(val, 100, ans);
    mp_free(val);
    return ans;
}

/* NOT. */
void*
pf_do_not(ParseNode* self)
{
    MPNumber* val;
    MPNumber* ans = mp_new_ptr();
    val = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!val)
    {
        mp_free(ans);
        return NULL;
    }
    if(!mp_is_overflow(val, self->state->options->wordlen))
    {
        set_error(self->state, PARSER_ERR_OVERFLOW, NULL);
        mp_free(ans);
        mp_free(val);
        return NULL;
    }
    mp_not(val, self->state->options->wordlen, ans);
    mp_free(val);
    return ans;
}

/* AND. */
void*
pf_do_and(ParseNode* self)
{
    MPNumber* left;
    MPNumber* right;
    MPNumber* ans = mp_new_ptr();
    left = (MPNumber*) (*(self->left->evaluate))(self->left);
    right = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!left || !right)
    {
        if(left)
            mp_free(left);
        if(right)
            mp_free(right);
        mp_free(ans);
        return NULL;
    }
    mp_and(left, right, ans);
    mp_free(left);
    mp_free(right);
    return ans;
}

/* OR. */
void*
pf_do_or(ParseNode* self)
{
    MPNumber* left;
    MPNumber* right;
    MPNumber* ans = mp_new_ptr();
    left = (MPNumber*) (*(self->left->evaluate))(self->left);
    right = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!left || !right)
    {
        if(left)
            mp_free(left);
        if(right)
            mp_free(right);
        mp_free(ans);
        return NULL;
    }
    mp_or(left, right, ans);
    mp_free(left);
    mp_free(right);
    return ans;
}

/* XOR. */
void*
pf_do_xor(ParseNode* self)
{
    MPNumber* left;
    MPNumber* right;
    MPNumber* ans = mp_new_ptr();
    left = (MPNumber*) (*(self->left->evaluate))(self->left);
    right = (MPNumber*) (*(self->right->evaluate))(self->right);
    if(!left || !right)
    {
        if(left)
            mp_free(left);
        if(right)
            mp_free(right);
        free(ans);
        return NULL;
    }
    mp_xor(left, right, ans);
    mp_free(left);
    mp_free(right);
    return ans;
}

/* Constant value. Convert into MPNumber and return. */
void*
pf_constant(ParseNode* self)
{
    MPNumber* ans = mp_new_ptr();
    if(mp_set_from_string(self->token->string, self->state->options->base, ans) != 0)
    {
        /* This should never happen, as l_check_if_number() has already passed the string once. */
        /* If the code reaches this point, something is really wrong. X( */
        mp_free(ans);
        set_error(self->state, PARSER_ERR_INVALID, self->token->string);
        return NULL;
    }
    return ans;
}

