/*
 * Copyright (C) 2020 MATE developers
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 *
 *
 * Authors: Johannes Unruh <johannes.unruh@fau.de>
 *
 */

#ifndef MATH_HISTORY_VIEW_H
#define MATH_HISTORY_VIEW_H

#include <glib-object.h>
#include <gtk/gtk.h>

#include "math-history-entry-view.h"
#include "math-display.h"
#include "math-equation.h"

G_BEGIN_DECLS

#define MATH_HISTORY_VIEW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), math_history_view_get_type(), MathHistoryView))

typedef struct MathHistoryViewPrivate MathHistoryViewPrivate;

typedef struct
{
    GtkBox parent_instance;
    MathHistoryViewPrivate *priv;
} MathHistoryView;

typedef struct
{
    GtkBoxClass parent_class;
} MathHistoryViewClass;

GType math_history_view_get_type(void);

MathHistoryView *
math_history_view_new(MathDisplay *display, GtkWidget *box);

void
math_history_insert_entry(MathHistoryView *view, char *equation, MPNumber *answer, int number_base);

G_END_DECLS

#endif /* MATH_HISTORY_VIEW_H */
