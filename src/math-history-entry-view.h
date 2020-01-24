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

#ifndef MATH_HISTORY_ENTRY_VIEW_H
#define MATH_HISTORY_ENTRY_VIEW_H

#include <glib-object.h>
#include <gtk/gtk.h>

#include "math-display.h"
#include "math-history-view.h"

G_BEGIN_DECLS

#define MATH_HISTORY_ENTRY_VIEW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), math_history_entry_view_get_type(), MathHistoryEntryView))

typedef struct MathHistoryEntryViewPrivate MathHistoryEntryViewPrivate;

typedef struct
{
    GtkListBoxRow parent_instance;
    MathHistoryEntryViewPrivate *priv;
} MathHistoryEntryView;

typedef struct
{
    GtkListBoxRowClass parent_class;
} MathHistoryEntryViewClass;

GType math_history_entry_view_get_type(void);

MathHistoryEntryView *
math_history_entry_view_new(char *equation, MPNumber *number, MathDisplay *display, int number_base);

G_END_DECLS

#endif /* MATH_HISTORY_ENTRY_VIEW_H */
