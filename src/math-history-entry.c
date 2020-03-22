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

    GtkWidget *equation_label;
    GtkWidget *answer_label;
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

void answer_clicked_cb(GtkWidget *widget, GdkEventButton *eventbutton, MathHistoryEntry *history_entry);
G_MODULE_EXPORT
void
answer_clicked_cb (GtkWidget *widget, GdkEventButton *eventbutton, MathHistoryEntry *history_entry)
{
    const gchar *answer = gtk_widget_get_tooltip_text(history_entry->priv->answer_label);

    if (answer != NULL)
        math_equation_insert(history_entry->priv->equation, answer);
}

void equation_clicked_cb(GtkWidget *widget, GdkEventButton *eventbutton, MathHistoryEntry *history_entry);
G_MODULE_EXPORT
void
equation_clicked_cb (GtkWidget *widget, GdkEventButton *eventbutton, MathHistoryEntry *history_entry)
{
    const gchar *equation = gtk_label_get_text(GTK_LABEL(history_entry->priv->equation_label));

    if (equation != NULL)
        math_equation_set(history_entry->priv->equation, equation);
}

void
math_history_entry_insert_entry(MathHistoryEntry *history_entry, const gchar *equation, const gchar *answer_four_digits, const gchar *answer_nine_digits)
{
    GtkBuilder *builder = NULL;
    GtkWidget *grid;
    GError *error = NULL;

    builder = gtk_builder_new();
    if(gtk_builder_add_from_resource (builder, UI_HISTORY_ENTRY_RESOURCE_PATH, &error) == 0)
    {
        g_warning("Error loading history-entry UI: %s", error->message);
        g_clear_error(&error);
        return;
    }
    grid = GET_WIDGET(builder, "grid");
    gtk_container_add(GTK_CONTAINER(history_entry), grid);
    history_entry->priv->equation_label = GET_WIDGET(builder, "equation_label");
    history_entry->priv->answer_label = GET_WIDGET(builder, "answer_label");
    gtk_widget_set_tooltip_text(history_entry->priv->equation_label, equation);
    gtk_widget_set_tooltip_text(history_entry->priv->answer_label, answer_nine_digits);
    gtk_label_set_text(GTK_LABEL(history_entry->priv->equation_label), equation);
    gtk_label_set_text(GTK_LABEL(history_entry->priv->answer_label), answer_four_digits);
    gtk_builder_connect_signals(builder, history_entry);
    g_object_unref(builder);
}

static void
math_history_entry_class_init(MathHistoryEntryClass *klass)
{
}

static void
math_history_entry_init(MathHistoryEntry *history_entry)
{
    history_entry->priv = math_history_entry_get_instance_private(history_entry);
}
