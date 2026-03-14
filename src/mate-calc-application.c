/*
 * mate-calc-application.c - GtkApplication for MATE Calculator
 *
 * Copyright (C) 2026 MATE Desktop Team
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#include <config.h>

#include <string.h>
#include <glib/gi18n.h>

#include "mate-calc-application.h"
#include "math-window.h"
#include "mp-equation.h"
#include "unit-manager.h"

/* Global settings variable for backward compatibility with existing code */
GSettings *g_settings_var = NULL;

struct _MateCalcApplication
{
    GtkApplication  parent_instance;

    GSettings      *settings;
};

G_DEFINE_TYPE(MateCalcApplication, mate_calc_application, GTK_TYPE_APPLICATION)

static void
mate_calc_application_startup(GApplication *application)
{
    MateCalcApplication *app = MATE_CALC_APPLICATION(application);

    G_APPLICATION_CLASS(mate_calc_application_parent_class)->startup(application);

    /* Initialize settings */
    app->settings = g_settings_new("org.mate.calc");
    g_settings_var = app->settings;
}

static void
mate_calc_application_activate(GApplication *application)
{
    MateCalcApplication *app = MATE_CALC_APPLICATION(application);
    MathEquation *equation;
    GtkWindow *window;
    MathButtons *buttons;
    gint accuracy, word_size, base;
    gboolean show_tsep, show_zeroes, show_hist;
    MpDisplayFormat number_format;
    MPAngleUnit angle_units;
    ButtonMode button_mode;
    gchar *source_currency, *target_currency;
    gchar *source_units, *target_units;

    accuracy = g_settings_get_int(app->settings, "accuracy");
    word_size = g_settings_get_int(app->settings, "word-size");
    base = g_settings_get_int(app->settings, "base");
    show_tsep = g_settings_get_boolean(app->settings, "show-thousands");
    show_zeroes = g_settings_get_boolean(app->settings, "show-zeroes");
    show_hist = g_settings_get_boolean(app->settings, "show-history");
    number_format = g_settings_get_enum(app->settings, "number-format");
    angle_units = g_settings_get_enum(app->settings, "angle-units");
    button_mode = g_settings_get_enum(app->settings, "button-mode");
    source_currency = g_settings_get_string(app->settings, "source-currency");
    target_currency = g_settings_get_string(app->settings, "target-currency");
    source_units = g_settings_get_string(app->settings, "source-units");
    target_units = g_settings_get_string(app->settings, "target-units");

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

    window = GTK_WINDOW(math_window_new(equation));
    gtk_application_add_window(GTK_APPLICATION(app), window);

    buttons = math_window_get_buttons(MATH_WINDOW(window));

    math_window_set_show_history(MATH_WINDOW(window), show_hist);
    math_buttons_set_programming_base(buttons, base);
    math_buttons_set_mode(buttons, button_mode);

    gtk_widget_show(GTK_WIDGET(window));
}

static void
mate_calc_application_shutdown(GApplication *application)
{
    MateCalcApplication *app = MATE_CALC_APPLICATION(application);

    g_settings_var = NULL;
    g_clear_object(&app->settings);

    G_APPLICATION_CLASS(mate_calc_application_parent_class)->shutdown(application);
}

static int
do_convert(const MPNumber *x, const char *x_units, const char *z_units, MPNumber *z, void *data)
{
    return unit_manager_convert_by_symbol(unit_manager_get_default(), x, x_units, z_units, z);
}

static gint
mate_calc_application_handle_local_options(GApplication *application,
                                            GVariantDict *options)
{
    if (g_variant_dict_contains(options, "version"))
    {
        g_print("%s %s\n", "mate-calc", VERSION);
        return 0;
    }

    const gchar *solve_expr = NULL;
    if (g_variant_dict_lookup(options, "solve", "&s", &solve_expr))
    {
        /* Solve mode - handled locally without initializing the full GUI */
        MPEquationOptions mp_options;
        MPErrorCode error;
        MPNumber result = mp_new();
        char *result_str;

        memset(&mp_options, 0, sizeof(mp_options));
        mp_options.base = 10;
        mp_options.wordlen = 32;
        mp_options.angle_units = MP_DEGREES;
        mp_options.convert = do_convert;

        error = mp_equation_parse(solve_expr, &mp_options, &result, NULL);

        if (error == PARSER_ERR_MP)
        {
            g_printerr("Error: %s\n", mp_get_error());
            mp_clear(&result);
            return 1;
        }
        else if (error != 0)
        {
            g_printerr("Error: %s\n", mp_error_code_to_string(error));
            mp_clear(&result);
            return 1;
        }
        else
        {
            MpSerializer *serializer = mp_serializer_new(MP_DISPLAY_FORMAT_AUTOMATIC, 10, 9);
            result_str = mp_serializer_to_string(serializer, &result);
            g_print("%s\n", result_str);
            g_free(result_str);
            g_object_unref(serializer);
            mp_clear(&result);
            return 0;
        }
    }

    /* Let the default handler continue */
    return -1;
}

static gint
mate_calc_application_command_line(GApplication            *application,
                                    GApplicationCommandLine *command_line)
{
    /* Activate normal GUI mode */
    g_application_activate(application);

    return 0;
}

static void
mate_calc_application_init(MateCalcApplication *app)
{
    /* Command line options */
    const GOptionEntry options[] = {
        { "version", 'v', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, NULL,
          N_("Show release version"), NULL },
        { "solve", 's', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, NULL,
          N_("Solve the given equation"), N_("EQUATION") },
        { NULL }
    };

    g_application_add_main_option_entries(G_APPLICATION(app), options);
}

static void
mate_calc_application_class_init(MateCalcApplicationClass *klass)
{
    GApplicationClass *app_class = G_APPLICATION_CLASS(klass);

    app_class->startup = mate_calc_application_startup;
    app_class->activate = mate_calc_application_activate;
    app_class->shutdown = mate_calc_application_shutdown;
    app_class->handle_local_options = mate_calc_application_handle_local_options;
    app_class->command_line = mate_calc_application_command_line;
}

MateCalcApplication *
mate_calc_application_new(void)
{
    return g_object_new(MATE_CALC_TYPE_APPLICATION,
                        "application-id", "org.mate.Calculator",
                        "flags", G_APPLICATION_HANDLES_COMMAND_LINE,
                        NULL);
}

GSettings *
mate_calc_application_get_settings(MateCalcApplication *app)
{
    g_return_val_if_fail(MATE_CALC_IS_APPLICATION(app), NULL);
    return app->settings;
}
