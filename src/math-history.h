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
 */

#ifndef MATH_HISTORY_VIEW_H
#define MATH_HISTORY_VIEW_H

#include <glib-object.h>
#include <gtk/gtk.h>

#include "math-history-entry.h"
#include "math-display.h"
#include "math-equation.h"

G_BEGIN_DECLS

#define MATH_HISTORY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), math_history_get_type(), MathHistory))

typedef struct MathHistoryPrivate MathHistoryPrivate;

typedef struct
{
    GtkScrolledWindow parent_instance;
    MathHistoryPrivate *priv;
} MathHistory;

typedef struct
{
    GtkScrolledWindowClass parent_class;
} MathHistoryClass;

GType math_history_get_type(void);

MathHistory *
math_history_new(MathEquation *equation);

void
math_history_insert_entry(MathHistory *history, char *equation, MPNumber *answer, int number_base);

G_END_DECLS

#endif /* MATH_HISTORY_VIEW_H */
