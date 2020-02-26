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

#include <string.h>
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>

#include "math-history.h"

struct MathHistoryPrivate
{
    MathEquation *equation;

    MathHistoryEntry *entry;
    char *prev_equation;
    int items_count; /* Number of entries in history listbox */
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

static gboolean
check_history(MathHistory *history, char *equation)
{ /* Checks if the last inserted calculation is the same as the current calculation to be inserted in history */
    if (history->priv->items_count >= 1 && g_strcmp0(equation, history->priv->prev_equation)==0)
        return TRUE; /* returns true if last entered equation is the same as the current equation */
    else
        return FALSE;
}

void
math_history_insert_entry (MathHistory *history, char *equation, MPNumber *answer, int number_base)
{   /* Inserts a new entry into the history listbox */
    history->priv->entry = math_history_entry_new(history->priv->equation);
    gboolean check = check_history (history, equation);
    history->priv->prev_equation = g_strdup(equation);
    if (!check)
    {
        math_history_entry_insert_entry(history->priv->entry, equation, answer, number_base);
        if (history->priv->entry != NULL)	
        {
            gtk_list_box_insert(GTK_LIST_BOX(history->priv->listbox), GTK_WIDGET(history->priv->entry), -1);
            gtk_widget_set_can_focus(GTK_WIDGET(history->priv->entry), FALSE);
            gtk_widget_show_all(GTK_WIDGET(history->priv->entry));	
            history->priv->items_count++;
        }
    }
    g_signal_emit_by_name(history, "row-added");
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

    history->priv->items_count = 0;
    history->priv->prev_equation = "";
    history->priv->listbox = gtk_list_box_new();    
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(history->priv->listbox), GTK_SELECTION_NONE);
    gtk_widget_show(GTK_WIDGET(history->priv->listbox));

    gtk_container_add(GTK_CONTAINER(history), history->priv->listbox);
    gtk_widget_set_valign(GTK_WIDGET(history->priv->listbox), GTK_ALIGN_END);
    gtk_widget_set_size_request(GTK_WIDGET(history), 100, 100);
    gtk_widget_set_can_focus(GTK_WIDGET(history), FALSE);

    g_signal_connect(history, "row-added", G_CALLBACK(scroll_bottom_cb), NULL);
}
