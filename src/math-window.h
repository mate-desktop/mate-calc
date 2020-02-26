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

#ifndef MATH_WINDOW_H
#define MATH_WINDOW_H

#include <glib-object.h>
#include "math-equation.h"
#include "math-display.h"
#include "math-buttons.h"
#include "math-preferences.h"

G_BEGIN_DECLS

#define MATH_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), math_window_get_type(), MathWindow))

typedef struct MathWindowPrivate MathWindowPrivate;

typedef struct
{
    GtkWindow parent_instance;
    MathWindowPrivate *priv;
} MathWindow;

typedef struct
{
    GtkWindowClass parent_class;

    void (*quit) (MathWindow *window);
} MathWindowClass;

GType math_window_get_type(void);

MathWindow *math_window_new(MathEquation *equation);

GtkWidget *math_window_get_menu_bar(MathWindow *window);

MathEquation *math_window_get_equation(MathWindow *window);

MathDisplay *math_window_get_display(MathWindow *window);

MathButtons *math_window_get_buttons(MathWindow *window);

gboolean math_window_get_show_history(MathWindow *window);

void math_window_set_show_history(MathWindow *window, gboolean visible);

void math_window_critical_error(MathWindow *window, const gchar *title, const gchar *contents);

G_END_DECLS

#endif /* MATH_WINDOW_H */
