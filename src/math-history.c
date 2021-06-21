/*
 * Copyright (C) 2020-2021 MATE developers
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 *
 *
 */

#include <string.h>
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>

#include "math-history.h"

struct MathHistoryPrivate
{
    MathEquation *equation;

    MpSerializer *serializer;
    gchar *last_equation;
    int current; /* 0 is the first entry, rows-1 the most recent entry */
    int rows;
    GtkWidget *listbox;
};

G_DEFINE_TYPE_WITH_PRIVATE(MathHistory, math_history, GTK_TYPE_SCROLLED_WINDOW);

MathHistory *
math_history_new(MathEquation *equation)
{
    MathHistory *history = g_object_new(math_history_get_type(), NULL);
    history->priv->equation = g_object_ref(equation);
    return history;
}

static void
scroll_bottom_cb(MathHistory *history, gpointer data)
{
    GtkAdjustment *adjustment;
    // TODO make this dynamic, do not hardcode listbox_height_request/number_of_rows
    int width, height;

    adjustment = gtk_list_box_get_adjustment(GTK_LIST_BOX(history->priv->listbox));
    gtk_widget_get_size_request(GTK_WIDGET(history), &width, &height);
    gtk_adjustment_set_page_size(adjustment, height / 3);
    gtk_adjustment_set_value (adjustment, gtk_adjustment_get_upper (adjustment)
                                          - gtk_adjustment_get_page_size (adjustment));
}

void
math_history_insert_entry (MathHistory *history, char *equation, MPNumber *answer)
{
    if (g_strcmp0(history->priv->last_equation, equation) == 0 && history->priv->rows >= 1)
    {
        return;
    }

    MathHistoryEntry *entry = math_history_entry_new(history->priv->equation);

    math_history_entry_insert_entry(entry, equation, answer, history->priv->serializer);

    gtk_list_box_insert(GTK_LIST_BOX(history->priv->listbox), GTK_WIDGET(entry), -1);
    gtk_widget_set_can_focus(GTK_WIDGET(entry), FALSE);
    gtk_widget_show(GTK_WIDGET(entry));

    g_free(history->priv->last_equation);

    history->priv->last_equation = g_strdup(equation);
    history->priv->rows++;
    history->priv->current = history->priv->rows;
    g_signal_emit_by_name(history, "row-added");
}

void
math_history_clear(MathHistory *history)
{
    GList *children, *iter;

    history->priv->rows = 0;
    history->priv->current = 0;
    children = gtk_container_get_children(GTK_CONTAINER(history->priv->listbox));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
          gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);
}

gpointer
math_history_get_entry_at(MathHistory *history, int index)
{
    if (index >= 0 && index < history->priv->rows)
        return MATH_HISTORY_ENTRY(gtk_list_box_get_row_at_index(GTK_LIST_BOX(history->priv->listbox), index));
    return NULL;
}

void
math_history_set_current(MathHistory *history, int value)
{
    history->priv->current = CLAMP (history->priv->current + value, 0, history->priv->rows - 1);
}

int
math_history_get_current(MathHistory *history)
{
    return history->priv->current;
}

void
math_history_set_serializer(MathHistory *history, MpSerializer *serializer)
{
    history->priv->serializer = serializer;

    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(history->priv->listbox));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
          math_history_entry_redisplay(MATH_HISTORY_ENTRY(iter->data), serializer);
    g_list_free(children);
}

static void
math_history_class_init(MathHistoryClass *klass)
{
    g_signal_new("row-added",
                 G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_FIRST,
                 0,
                 NULL,
                 NULL,
                 NULL,
                 G_TYPE_NONE,
                 0,
                 NULL);
}

static void
math_history_init(MathHistory *history)
{
    history->priv = math_history_get_instance_private(history);

    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(history), GTK_SHADOW_IN);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(history), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(history), GTK_CORNER_TOP_LEFT);

    history->priv->serializer = NULL;
    history->priv->current = 0;
    history->priv->rows = 0;
    history->priv->last_equation = g_strdup("");
    history->priv->listbox = gtk_list_box_new();

    gtk_list_box_set_selection_mode(GTK_LIST_BOX(history->priv->listbox), GTK_SELECTION_NONE);
    gtk_widget_show(GTK_WIDGET(history->priv->listbox));

    gtk_container_add(GTK_CONTAINER(history), history->priv->listbox);
    gtk_widget_set_valign(GTK_WIDGET(history->priv->listbox), GTK_ALIGN_END);
    gtk_widget_set_size_request(GTK_WIDGET(history), 100, 100);
    gtk_widget_set_can_focus(GTK_WIDGET(history), FALSE);

    g_signal_connect(history, "row-added", G_CALLBACK(scroll_bottom_cb), NULL);
}
