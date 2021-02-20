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

#ifndef MATH_HISTORY_ENTRY_VIEW_H
#define MATH_HISTORY_ENTRY_VIEW_H

#include <glib-object.h>
#include <gtk/gtk.h>

#include "math-display.h"
#include "math-history.h"

G_BEGIN_DECLS

#define MATH_HISTORY_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), math_history_entry_get_type(), MathHistoryEntry))

typedef struct MathHistoryEntryPrivate MathHistoryEntryPrivate;

typedef struct
{
    GtkListBoxRow parent_instance;
    MathHistoryEntryPrivate *priv;
} MathHistoryEntry;

typedef struct
{
    GtkListBoxRowClass parent_class;
} MathHistoryEntryClass;

GType math_history_entry_get_type(void);

MathHistoryEntry *
math_history_entry_new(MathEquation *equation);

void
math_history_entry_insert_entry(MathHistoryEntry *history_entry, const gchar *equation, MPNumber *answer, MpSerializer *serializer);

void 
math_history_entry_redisplay(MathHistoryEntry *history_entry, MpSerializer *serializer);

gchar *
math_history_entry_get_equation(MathHistoryEntry *history_entry);

G_END_DECLS

#endif /* MATH_HISTORY_ENTRY_VIEW_H */
