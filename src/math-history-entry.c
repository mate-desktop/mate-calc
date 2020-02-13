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

#include "math-history-entry.h"

struct MathHistoryEntryPrivate
{
    MathEquation *equation;

    MPNumber *answer; /* Stores answer in MPNumber object */
    char *equation_string; /* Stores equation to be entered in history-entry */
    char *answer_string; /* Stores answer to be entered in history-entry */
    GtkWidget *equation_label;
    GtkWidget *answer_label;
    GtkWidget *equation_eventbox;
    GtkWidget *answer_eventbox;
};

G_DEFINE_TYPE_WITH_PRIVATE (MathHistoryEntry, math_history_entry, GTK_TYPE_LIST_BOX_ROW);

#define UI_HISTORY_ENTRY_RESOURCE_PATH "/org/mate/calculator/ui/history-entry.ui"
#define GET_WIDGET(ui, name) \
          GTK_WIDGET(gtk_builder_get_object(ui, name))

MathHistoryEntry *
math_history_entry_new(MathEquation *equation)
{
    MathHistoryEntry *history_entry = g_object_new(math_history_entry_get_type(), NULL);
    history_entry->priv->equation = g_object_ref(equation);
    return history_entry;
}

void answer_clicked_cb(GtkWidget *widget, GdkEventButton *eventbutton, gpointer data);
G_MODULE_EXPORT
void
answer_clicked_cb (GtkWidget *widget, GdkEventButton *eventbutton, gpointer data)
{   /* Callback function for button-press-event on answer_eventbox */
    GtkEventBox *event = (GtkEventBox*) widget;
    MathHistoryEntry *history_entry = MATH_HISTORY_ENTRY(data);
    if (event != NULL)
    {
        if (GTK_IS_BIN(event))
        {
            GtkWidget *answer_label = gtk_bin_get_child(GTK_BIN(event));
            char *answer = gtk_widget_get_tooltip_text(answer_label);
            if (answer != NULL)
                math_equation_insert(history_entry->priv->equation, answer);
        }
    }
}
void equation_clicked_cb(GtkWidget *widget, GdkEventButton *eventbutton,  gpointer data);
G_MODULE_EXPORT
void
equation_clicked_cb (GtkWidget *widget, GdkEventButton *eventbutton, gpointer data)
{   /* Callback function for button-press-event on equation_eventbox */
    GtkEventBox *event = (GtkEventBox*) widget;
    MathHistoryEntry *history_entry = MATH_HISTORY_ENTRY(data);
    if (event != NULL)
    {
        if (GTK_IS_BIN(event))
        {
            GtkLabel *equation_label = (GtkLabel*) gtk_bin_get_child(GTK_BIN(event));
            const char *equation = gtk_label_get_text(equation_label);
            if (equation  != NULL)
                math_equation_set(history_entry->priv->equation, equation);
        }
    }
}

void
math_history_entry_insert_entry(MathHistoryEntry *history_entry, char *equation, MPNumber *number, int number_base)
{
    GtkBuilder *builder = NULL;
    GtkWidget *grid;
    GError *error = NULL;
    char *final_answer;
    MpSerializer *serializer_nine = mp_serializer_new(MP_DISPLAY_FORMAT_AUTOMATIC, number_base, 9);
    MpSerializer *serializer_four = mp_serializer_new(MP_DISPLAY_FORMAT_AUTOMATIC, number_base, 4);

    history_entry->priv->answer = number;
    history_entry->priv->equation_string = equation;
    history_entry->priv->answer_string = mp_serializer_to_string(serializer_nine, history_entry->priv->answer);
    final_answer = mp_serializer_to_string(serializer_four, history_entry->priv->answer);
    
    builder = gtk_builder_new();
    if(gtk_builder_add_from_resource (builder, UI_HISTORY_ENTRY_RESOURCE_PATH, &error) == 0)
    {
        g_print("gtk_builder_add_from_resource FAILED\n");
        g_print("%s\n",error->message);
        return;
    }
    grid = GET_WIDGET(builder, "grid");
    gtk_container_add(GTK_CONTAINER(history_entry), grid);
    history_entry->priv->equation_label = GET_WIDGET(builder, "equation_label");
    history_entry->priv->answer_label = GET_WIDGET(builder, "answer_label");
    history_entry->priv->equation_eventbox = GET_WIDGET(builder, "equation_eventbox");
    history_entry->priv->answer_eventbox = GET_WIDGET(builder, "answer_eventbox");
    gtk_widget_set_tooltip_text(history_entry->priv->equation_label, history_entry->priv->equation_string); 
    gtk_widget_set_tooltip_text(history_entry->priv->answer_label, history_entry->priv->answer_string); 
    gtk_label_set_text(GTK_LABEL(history_entry->priv->equation_label), history_entry->priv->equation_string);
    gtk_label_set_text(GTK_LABEL(history_entry->priv->answer_label), final_answer);
    gtk_builder_connect_signals(builder, history_entry);
}

static void
math_history_entry_class_init(MathHistoryEntryClass *klass)
{
}

static void
math_history_entry_init(MathHistoryEntry *history_entry)
{
    history_entry->priv = math_history_entry_get_instance_private(history_entry);
    history_entry->priv->equation_string = "";
    history_entry->priv->answer_string = "";
}
