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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <glib/gi18n.h>

#include "math-window.h"
#include "math-preferences.h"
#include "mp-equation.h"
#include "unit-manager.h"
#include "utility.h"

GSettings *g_settings_var = NULL;

static MathWindow *window;

static void
version(const gchar *progname)
{
    /* NOTE: Is not translated so can be easily parsed */
    fprintf(stderr, "%1$s %2$s\n", progname, VERSION);
}

static int
do_convert(const MPNumber *x, const char *x_units, const char *z_units, MPNumber *z, void *data)
{
    return unit_manager_convert_by_symbol(unit_manager_get_default(), x, x_units, z_units, z);
}

static void
solve(const char *equation)
{
    MPEquationOptions options;
    MPErrorCode error;
    MPNumber result = mp_new();
    char *result_str;

    memset(&options, 0, sizeof(options));
    options.base = 10;
    options.wordlen = 32;
    options.angle_units = MP_DEGREES;
    options.convert = do_convert;

    error = mp_equation_parse(equation, &options, &result, NULL);
    if(error == PARSER_ERR_MP) {
        fprintf(stderr, "Error: %s\n", mp_get_error());
        mp_clear(&result);
        exit(1);
    }
    else if(error != 0) {
        fprintf(stderr, "Error: %s\n", mp_error_code_to_string(error));
        mp_clear(&result);
        exit(1);
    }
    else {
        result_str = mp_serializer_to_string(mp_serializer_new(MP_DISPLAY_FORMAT_AUTOMATIC, 10, 9), &result);
        printf("%s\n", result_str);
        mp_clear(&result);
        exit(0);
    }
}

static void
usage(const gchar *progname, gboolean show_application, gboolean show_gtk)
{
    fprintf(stderr,
            /* Description on how to use mate-calc displayed on command-line */
            _("Usage:\n"
              "  %s — Perform mathematical calculations"), progname);

    fprintf(stderr,
            "\n\n");

    fprintf(stderr,
            /* Description on mate-calc command-line help options displayed on command-line */
            _("Help Options:\n"
              "  -v, --version                   Show release version\n"
              "  -h, -?, --help                  Show help options\n"
              "  --help-all                      Show all help options\n"
              "  --help-gtk                      Show GTK+ options"));
    fprintf(stderr,
            "\n\n");

    if (show_gtk) {
        fprintf(stderr,
                /* Description on mate-calc command-line GTK+ options displayed on command-line */
                _("GTK+ Options:\n"
                  "  --class=CLASS                   Program class as used by the window manager\n"
                  "  --name=NAME                     Program name as used by the window manager\n"
                  "  --screen=SCREEN                 X screen to use\n"
                  "  --sync                          Make X calls synchronous\n"
                  "  --gtk-module=MODULES            Load additional GTK+ modules\n"
                  "  --g-fatal-warnings              Make all warnings fatal"));
        fprintf(stderr,
                "\n\n");
    }

    if (show_application) {
        fprintf(stderr,
                /* Description on mate-calc application options displayed on command-line */
                _("Application Options:\n"
                  "  -s, --solve <equation>          Solve the given equation"));
        fprintf(stderr,
                "\n\n");
    }
}

static void
get_options(int argc, char *argv[])
{
    int i;
    char *progname, *arg;

    progname = g_path_get_basename(argv[0]);

    for (i = 1; i < argc; i++) {
        arg = argv[i];

        if (strcmp(arg, "-v") == 0 ||
            strcmp(arg, "--version") == 0) {
            version(progname);
            g_free(progname);
            exit(0);
        }
        else if (strcmp(arg, "-h") == 0 ||
                 strcmp(arg, "-?") == 0 ||
                 strcmp(arg, "--help") == 0) {
            usage(progname, TRUE, FALSE);
            g_free(progname);
            exit(0);
        }
        else if (strcmp(arg, "--help-all") == 0) {
            usage(progname, TRUE, TRUE);
            g_free(progname);
            exit(0);
        }
        else if (strcmp(arg, "--help-gtk") == 0) {
            usage(progname, FALSE, TRUE);
            g_free(progname);
            exit(0);
        }
        else if (strcmp(arg, "-s") == 0 ||
            strcmp(arg, "--solve") == 0) {
            i++;
            if (i >= argc) {
                fprintf(stderr,
                        /* Error printed to stderr when user uses --solve argument without an equation */
                        _("Argument --solve requires an equation to solve"));
                fprintf(stderr, "\n");
                g_free(progname);
                exit(1);
            }
            else
                solve(argv[i]);
        }
        else {
            fprintf(stderr,
                    /* Error printed to stderr when user provides an unknown command-line argument */
                    _("Unknown argument '%s'"), arg);
            fprintf(stderr, "\n");
            usage(progname, TRUE, FALSE);
            g_free(progname);
            exit(1);
        }
    }

    g_free(progname);
}

static void
quit_cb(MathWindow *win)
{
    gtk_main_quit();
}

int main(int argc, char **argv)
{
    MathEquation *equation;
    MathButtons *buttons;
    int accuracy = 9, word_size = 64, base = 10;
    gboolean show_tsep = FALSE, show_zeroes = FALSE, show_hist = FALSE;
    MpDisplayFormat number_format;
    MPAngleUnit angle_units;
    ButtonMode button_mode;
    gchar *source_currency, *target_currency;
    gchar *source_units, *target_units;

    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    /* Seed random number generator. */
    srand48((long) time((time_t *) 0));

    gtk_init(&argc, &argv);

    g_settings_var = g_settings_new ("org.mate.calc");
    accuracy = g_settings_get_int(g_settings_var, "accuracy");
    word_size = g_settings_get_int(g_settings_var, "word-size");
    base = g_settings_get_int(g_settings_var, "base");
    show_tsep = g_settings_get_boolean(g_settings_var, "show-thousands");
    show_zeroes = g_settings_get_boolean(g_settings_var, "show-zeroes");
    show_hist = g_settings_get_boolean(g_settings_var, "show-history");
    number_format = g_settings_get_enum(g_settings_var, "number-format");
    angle_units = g_settings_get_enum(g_settings_var, "angle-units");
    button_mode = g_settings_get_enum(g_settings_var, "button-mode");
    source_currency = g_settings_get_string(g_settings_var, "source-currency");
    target_currency = g_settings_get_string(g_settings_var, "target-currency");
    source_units = g_settings_get_string(g_settings_var, "source-units");
    target_units = g_settings_get_string(g_settings_var, "target-units");

    equation = math_equation_new();
    math_equation_set_accuracy(equation, accuracy);
    math_equation_set_word_size(equation, word_size);
    math_equation_set_show_thousands_separators(equation, show_tsep);
    math_equation_set_show_trailing_zeroes(equation, show_zeroes);
    math_equation_set_number_format(equation, number_format);
    math_equation_set_angle_units(equation, angle_units);
    math_equation_set_source_currency(equation, source_currency);
    math_equation_set_target_currency(equation, target_currency);
    math_equation_set_source_units(equation, source_units);
    math_equation_set_target_units(equation, target_units);
    g_free(source_currency);
    g_free(target_currency);
    g_free(source_units);
    g_free(target_units);

    get_options(argc, argv);

    //gtk_window_set_default_icon_name("accessories-calculator");

    window = math_window_new(equation);
    buttons = math_window_get_buttons(window);
    g_signal_connect(G_OBJECT(window), "quit", G_CALLBACK(quit_cb), NULL);
    math_window_set_show_history(window, show_hist);
    math_buttons_set_programming_base(buttons, base);
    math_buttons_set_mode(buttons, button_mode); // FIXME: We load the basic buttons even if we immediately switch to the next type

    gtk_widget_show(GTK_WIDGET(window));
    gtk_main();

    return 0;
}
