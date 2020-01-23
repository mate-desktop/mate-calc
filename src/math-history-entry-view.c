/*
 * Copyright (C) 2008-2011 Robert Ancell
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#include <string.h>
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>

#include "math-history-entry-view.h"

struct MathHistoryEntryViewPrivate
{
    MPNumber *answer; /* Stores answer in Number object */
    char *prev_equation; /* Stores equation to be entered in history-view */
    char *prev_answer; /* Stores answer to be entered in history-view */
    GtkWidget *equation_label;
    GtkWidget *answer_label;
    GtkWidget *eq_eventbox;
    GtkWidget *ans_eventbox;
    MathDisplay *display;
};

G_DEFINE_TYPE_WITH_PRIVATE (MathHistoryEntryView, math_history_entry_view, GTK_TYPE_LIST_BOX_ROW);

static gboolean
answer_clicked_cb (GtkWidget *widget, GdkEventButton *eventbutton, gpointer data)
{   /* Callback function for button-press-event on ans_eventbox */
    GtkEventBox *event = (GtkEventBox*) widget;
    MathHistoryEntryView *view = MATH_HISTORY_ENTRY_VIEW(data);
    if (event != NULL)
    {
        if (GTK_IS_BIN(event))
        {
            GtkWidget *ans_label = gtk_bin_get_child(GTK_BIN(event));
            char *prev_ans = gtk_widget_get_tooltip_text(ans_label);
            if (prev_ans  != NULL)
            {
                math_display_insert_text(view->priv->display, prev_ans); /* Replaces current equation by selected equation from history-view */
            }
        }
    }
    return TRUE;
}

static gboolean
equation_clicked_cb (GtkWidget *widget, GdkEventButton *eventbutton, gpointer data)
{   /* Callback function for button-press-event on eq_eventbox */
    GtkEventBox *event = (GtkEventBox*) widget;
    MathHistoryEntryView *view = MATH_HISTORY_ENTRY_VIEW(data);
    if (event != NULL)
    {
        if (GTK_IS_BIN(event))
        {
            GtkLabel *equation_label = (GtkLabel*) gtk_bin_get_child(GTK_BIN(event));
            const char *prev_equation = gtk_label_get_text(equation_label);
            if (prev_equation  != NULL)
            {
                math_display_display_text (view->priv->display, prev_equation); /* Appends selected answer from history-view to current equation in editbar */
            }
        }
    }
    return TRUE;
}

MathHistoryEntryView *
math_history_entry_view_new(char *equation, MPNumber *number, MathDisplay *display, int number_base)
{   /* Creates a new history-view entry */
    MathHistoryEntryView *view = g_object_new(math_history_entry_view_get_type(), NULL);

    view->priv->display = display;
    view->priv->answer = number;
    view->priv->prev_equation = equation;
	MpSerializer *serializer = mp_serializer_new(MP_DISPLAY_FORMAT_AUTOMATIC, number_base, 9);
    MpSerializer *ans_serializer = mp_serializer_new(MP_DISPLAY_FORMAT_AUTOMATIC, number_base, 4);
    view->priv->prev_answer = mp_serializer_to_string(serializer, view->priv->answer);
    char *answer_text = mp_serializer_to_string(ans_serializer, view->priv->answer);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_insert_column(GTK_GRID(grid), 0);
    gtk_grid_insert_column(GTK_GRID(grid), 1);
    gtk_grid_insert_column(GTK_GRID(grid), 2);
    gtk_grid_insert_column(GTK_GRID(grid), 3);
    gtk_grid_insert_row(GTK_GRID(grid), 0);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_add(GTK_CONTAINER(view), grid);
    view->priv->equation_label = gtk_label_new("");
    view->priv->answer_label = gtk_label_new("");
    view->priv->eq_eventbox = gtk_event_box_new();
    view->priv->ans_eventbox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(view->priv->eq_eventbox), view->priv->equation_label);
    gtk_container_add(GTK_CONTAINER(view->priv->ans_eventbox), view->priv->answer_label);
    gtk_widget_set_events(view->priv->eq_eventbox, GDK_BUTTON_PRESS_MASK);
    gtk_widget_set_events(view->priv->ans_eventbox, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(view->priv->eq_eventbox, "button_press_event", G_CALLBACK(equation_clicked_cb), view); /* Calls onclick_equation on clicking on equation_label */
    g_signal_connect(view->priv->ans_eventbox, "button_press_event", G_CALLBACK(answer_clicked_cb), view); /* Calls onclick_answer on clicking on answer_label */
    gtk_widget_set_size_request(view->priv->equation_label, 10, 10);
    gtk_widget_set_size_request(view->priv->answer_label, 10, 10);
    gtk_label_set_selectable(GTK_LABEL(view->priv->equation_label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(view->priv->answer_label), TRUE);
    gtk_event_box_set_above_child (GTK_EVENT_BOX(view->priv->eq_eventbox), TRUE);
    gtk_event_box_set_above_child (GTK_EVENT_BOX(view->priv->ans_eventbox), TRUE);
    gtk_widget_set_tooltip_text (view->priv->equation_label, view->priv->prev_equation); /* Sets tooltip for equation_label */
    gtk_widget_set_tooltip_text(view->priv->answer_label, view->priv->prev_answer); /* Sets tooltip for answer_label */
    gtk_label_set_ellipsize(GTK_LABEL(view->priv->equation_label), PANGO_ELLIPSIZE_END); /* Ellipsizes the equation when its size is greater than the size of the equation_label */
    gtk_label_set_ellipsize(GTK_LABEL(view->priv->answer_label), PANGO_ELLIPSIZE_END); /* Elipsizes the answer when its size is greater the than size of answer_label */
    gtk_label_set_text(GTK_LABEL(view->priv->equation_label), view->priv->prev_equation);
    char* final_answer = g_strconcat("= ", answer_text, NULL);
    gtk_label_set_text(GTK_LABEL(view->priv->answer_label), final_answer);
    gtk_grid_attach(GTK_GRID(grid), view->priv->eq_eventbox, 0, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), view->priv->ans_eventbox, 3, 0, 1, 1);
    gtk_label_set_xalign(GTK_LABEL(view->priv->equation_label), 0); /* Aligns equation_label to the left */
    gtk_label_set_xalign(GTK_LABEL(view->priv->answer_label), 1); /* Aligns answer_label to the right */
    PangoAttrList *list = pango_attr_list_new ();
    PangoFontDescription *font = pango_font_description_new ();
    pango_font_description_set_weight (font, PANGO_WEIGHT_BOLD);
    PangoAttribute *weight = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
    pango_attr_list_insert(list, weight);
    gtk_label_set_attributes(GTK_LABEL(view->priv->answer_label), list); /* Sets font weight of the text on answer_label to BOLD */

    gtk_widget_show_all(grid);
    g_free(final_answer);

    return view;
}

static void
math_history_entry_view_class_init(MathHistoryEntryViewClass *klass)
{
}

static void
math_history_entry_view_init(MathHistoryEntryView *view)
{
    view->priv = math_history_entry_view_get_instance_private(view);
}
