/*
 * Copyright (C) 2009 Rich Burridge
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <locale.h>
#include <termios.h>

#include "mp-equation.h"
#include "mp-serializer.h"

#define MAXLINE 1024

static MpSerializer *result_serializer;

static void
solve(const char *equation)
{
    int ret;
    MPEquationOptions options;
    MPNumber z;
    gchar *result_str = NULL;
    
    memset(&options, 0, sizeof(options));
    options.base = 10;
    options.wordlen = 32;
    options.angle_units = MP_DEGREES;
    
    ret = mp_equation_parse(equation, &options, &z, NULL);

    if (ret == PARSER_ERR_MP)
        fprintf(stderr, "Error %s\n", mp_get_error());
    else if (ret)        
        fprintf(stderr, "Error %d\n", ret);
    else {
        result_str = mp_serializer_to_string(result_serializer, &z);
        printf("%s\n", result_str);
    }
    g_free(result_str);
}


/* Adjust user input equation string before solving it. */
static void
str_adjust(char *str)
{
    int i, j = 0;

    str[strlen(str)-1] = '\0';        /* Remove newline at end of string. */
    for (i = 0; str[i] != '\0'; i++) {        /* Remove whitespace. */
        if (str[i] != ' ' && str[i] != '\t')
            str[j++] = str[i];
    }
    str[j] = '\0';
    if (j > 0 && str[j-1] == '=')     /* Remove trailing '=' (if present). */
        str[j-1] = '\0';
}

static char*
readline()
{
    size_t after_size  = 512;
    size_t before_size = 512;
    size_t after_ptr  = 0;
    size_t before_ptr = 0;
    char *after_buf  = (char *) malloc(after_size * sizeof(char));
    char *before_buf = (char *) malloc(before_size * sizeof(char));
    char *line = NULL;
    char c;
    long i;
    int ci;
    
    struct termios saved_stty;
    struct termios stty;
    tcgetattr(0, &saved_stty);
    tcgetattr(0, &stty);
    stty.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSAFLUSH, &stty);
    
    while ((ci = getchar()) >= 0) {
        c = (char)ci;
        if ((c >= ' ' && c != 127) || (c & 0x80) || c < 0) {
	    if (before_ptr == before_size) {
	        char *tmp = (char *) malloc(2 * before_size * sizeof(char));
		for (i = 0; i < before_size; i++)
		    *(tmp + i) = *(before_buf + i);
		free(before_buf);
		before_buf = tmp;
		before_size <<= 1;
	    }
	    *(before_buf + before_ptr++) = c;
	    printf("%c", c);
	    for (i = (long)after_ptr - 1; i >= 0; i--)
	        printf("%c", *(after_buf + i));
	    if (after_ptr > 0)
	        printf("\033[%luD", after_ptr);
	    fflush(stdout);
	}
	else if (c == 4)
	    break;
	else if (c == '\n') {
	    line = (char *) malloc((before_ptr + after_ptr + 2) * sizeof(char));
	    for (i = 0; i < before_ptr; i++)
	        *(line + i) = *(before_buf + i);
	    line += before_ptr;
	    for (i = (long)after_ptr - 1; i >= 0; i--)
	        *(line + i) = *(after_buf + i);
	    *(line + after_ptr) = '\n';
	    *(line + after_ptr + 1) = 0;
	    line -= before_ptr;
	    printf("\n");
	    break;
	}
	else if (c == '\b' || c == 127) {
	    if (before_ptr == 0)
	        printf("\a");
	    else {
	        before_ptr--;
		printf("\033[D");
		for (i = (long)after_ptr - 1; i >= 0; i--)
                    printf("%c", *(after_buf + i));
		printf("  \033[%luD", after_ptr + 2);
	    }
	    fflush(stdout);
	}
	else if (c == '\033') {
	    if ((c = getchar()) < 0)
	        break;
	    if (c != '[' && c != 'O')
	        continue;
	    if (c != 'O')
	        if ((c = getchar()) < 0)
		    break;
	    switch (c)
	    {
	    case 'C':
	        if (after_ptr == 0)
		    printf("\a");
		else {
		    *(before_buf + before_ptr++) = *(after_buf + --after_ptr);
		    printf("\033[C");
	        }
	        break;
	    case 'D':
	        if (before_ptr == 0)
		    printf("\a");
		else {
		    if (after_ptr == after_size) {
		        char *tmp = (char *) malloc(2 * after_size * sizeof(char));
			for (i = 0; i < after_size; i++)
			    *(tmp + i) = *(after_buf + i);
			free(after_buf);
			after_buf = tmp;
			after_size <<= 1;
		    }
		    *(after_buf + after_ptr++) = *(before_buf + --before_ptr);
		    printf("\033[D");
	        }
	        break;
	    case 'O':
	    case '1':
	    case '4':
	    case '3':
	        {
		    char e;
		    if ((e = getchar()) < 0)
		        break;
		    if (c == 'O' && e == 'F')
		        c = '4';
		    else if (c == 'O' && e == 'H')
		        c = '1';
		    else if (e != '~' || c == 'O')
		        continue;
		    if (c == '1' && before_ptr > 0) {
		        size_t n = before_ptr;
			while ((before_ptr)) {
			    if (after_ptr == after_size) {
			        char *tmp = (char *) malloc(2 * after_size * sizeof(char));
				for (i = 0; i < after_size; i++)
				    *(tmp + i) = *(after_buf + i);
				free(after_buf);
				after_buf = tmp;
				after_size <<= 1;
			    }
			    *(after_buf + after_ptr++) = *(before_buf + --before_ptr);
			}
			printf("\033[%luD", n);
		    }
		    else if (c == '4' && after_ptr > 0) {
		        size_t n = after_ptr;
			while ((after_ptr))
			    *(before_buf + before_ptr++) = *(after_buf + --after_ptr);
			printf("\033[%luC", n);
		    }
		    else if (c == '3') {
		        if (after_ptr == 0)
			    printf("\a");
			else {
			    after_ptr--;
			    for (i = (long)after_ptr - 1; i >= 0; i--)
			        printf("%c", *(after_buf + i));
			    printf("  \033[%luD", after_ptr + 2);
			}
		    }
		}
		break;
	    }
	    fflush(stdout);
	}
    }
    
    tcsetattr(0, TCSAFLUSH, &saved_stty);
    
    free(after_buf);
    free(before_buf);
    return line;
}


int
main(int argc, char **argv)
{
    char *equation;

    /* Seed random number generator. */
    srand48((long) time((time_t *) 0));
    
    g_type_init ();
    setlocale(LC_ALL, "");
    
    result_serializer = mp_serializer_new(MP_DISPLAY_FORMAT_AUTOMATIC, 10, 9);

    while (1) {
        printf("> ");
        equation = readline();

        if (equation != NULL)
            str_adjust(equation);

        if (equation == NULL || strcmp(equation, "exit") == 0 || strcmp(equation, "quit") == 0 || strlen(equation) == 0) {
	    if (equation != NULL)
	        free(equation);
            break;
	}

        solve(equation);
	free(equation);
    }

    return 0;
}

