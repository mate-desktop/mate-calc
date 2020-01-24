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

#include <string.h>
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>

#include "math-history-view.h"

struct MathHistoryViewPrivate
{
    int no_ofitems; /* No of entries in history-view listbox */

    GtkWidget *listbox;
    GtkWidget *scroll_window;
    GtkWidget *main_box;
    MathDisplay *display;
};

G_DEFINE_TYPE_WITH_PRIVATE(MathHistoryView, math_history_view, GTK_TYPE_BOX);

static void
scroll_changed_cb(GtkAdjustment *adjustment, MathHistoryView *view)
{
    gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment));
}

static gboolean
check_history(MathHistoryView *view, char *equation, char *answer)
{ /* Checks if the last inserted calculation is the same as the current calculation to be inserted in history-view */
    if (view->priv->no_ofitems == 0)
    {
        return FALSE; /* returns false for the first entry */
    }

    char *current_equation = equation;
    char *ans = answer;
    GtkListBoxRow *row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(view->priv->listbox), view->priv->no_ofitems - 1);
    GtkGrid *grid = GTK_GRID(gtk_bin_get_child(GTK_BIN(row)));
    if (grid != NULL)
    {
        GtkEventBox *ans_eventbox = GTK_EVENT_BOX(g_list_nth_data(gtk_container_get_children(GTK_CONTAINER(grid)), 0));
        char *prev_ans = gtk_widget_get_tooltip_text(GTK_WIDGET(gtk_bin_get_child(GTK_BIN(ans_eventbox)))); /* retrieves previous equation */
        GtkEventBox *eq_eventbox = GTK_EVENT_BOX(g_list_nth_data(gtk_container_get_children(GTK_CONTAINER(grid)), 1));
        char *prev_equation = gtk_widget_get_tooltip_text(GTK_WIDGET(gtk_bin_get_child(GTK_BIN(eq_eventbox)))); /* retrieves previous answer */
        if ((view->priv->no_ofitems >= 1) && (strcmp(prev_ans, ans)==0) && (strcmp(current_equation, prev_equation)==0))
            return TRUE; /* returns true if last entered equation and answer is the same as the current equation and answer */
    }
    return FALSE;
}

void
math_history_insert_entry (MathHistoryView *view, char *equation, MPNumber *answer, int number_base)
{   /* Inserts a new entry into the history-view listbox */
    char *prev_eq = equation;
    MpSerializer *serializer = mp_serializer_new(MP_DISPLAY_FORMAT_AUTOMATIC, number_base, 9);
    char *ans = mp_serializer_to_string(serializer, answer);
    gboolean check = check_history (view, prev_eq, ans);
    if (!check)
    {
        MathHistoryEntryView *entry = math_history_entry_view_new (prev_eq, answer, view->priv->display, number_base);
        if (entry != NULL)	
        {
            gtk_container_add(GTK_CONTAINER(view->priv->listbox), GTK_WIDGET(entry));
            gtk_widget_show_all(GTK_WIDGET(entry));	
            view->priv->no_ofitems += 1;
        }	
    }
}

MathHistoryView *
math_history_view_new(MathDisplay *display, GtkWidget *box)
{
    MathHistoryView *view = g_object_new(math_history_view_get_type(), NULL);
    view->priv->display = display;
    view->priv->main_box = box;
    view->priv->listbox = gtk_list_box_new();    
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(view->priv->listbox), GTK_SELECTION_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(view->priv->listbox), 5);
    view->priv->scroll_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(view->priv->scroll_window), GTK_SHADOW_ETCHED_OUT);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(view->priv->scroll_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW(view->priv->scroll_window), GTK_CORNER_TOP_LEFT);
    gtk_container_add(GTK_CONTAINER(view->priv->scroll_window), view->priv->listbox);
    gtk_widget_set_size_request(view->priv->scroll_window, 100, 100);
    gtk_box_pack_start(GTK_BOX(view->priv->main_box), view->priv->scroll_window, TRUE, TRUE, 0);
    g_signal_connect(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(view->priv->scroll_window)), "changed", G_CALLBACK(scroll_changed_cb), view);
    gtk_widget_show_all(view->priv->main_box);
    return view;
}

static void
math_history_view_class_init(MathHistoryViewClass *klass)
{
}

static void
math_history_view_init(MathHistoryView *view)
{
    view->priv = math_history_view_get_instance_private(view);
    view->priv->no_ofitems = 0;
}
