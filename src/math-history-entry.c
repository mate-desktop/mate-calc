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

#include "math-history-entry.h"

struct MathHistoryEntryPrivate
{
    MathEquation *equation;

    MPNumber *number;
    GtkWidget *equation_label;
    GtkWidget *answer_label;
};

G_DEFINE_TYPE_WITH_PRIVATE (MathHistoryEntry, math_history_entry, GTK_TYPE_LIST_BOX_ROW);

#define UI_HISTORY_ENTRY_RESOURCE_PATH "/org/mate/calculator/ui/history-entry.ui"

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
math_history_entry_insert_entry(MathHistoryEntry *history_entry, const gchar *equation, MPNumber *answer, MpSerializer *serializer)
{
    #define get_widget(x) GTK_WIDGET(gtk_builder_get_object(builder, x))

    GtkBuilder *builder;
    GtkWidget *grid;

    mp_set_from_mp(answer, history_entry->priv->number);

    builder = gtk_builder_new_from_resource(UI_HISTORY_ENTRY_RESOURCE_PATH);
    grid = get_widget("grid");
    gtk_container_add(GTK_CONTAINER(history_entry), grid);
    history_entry->priv->equation_label = get_widget("equation_label");
    history_entry->priv->answer_label = get_widget("answer_label");
    gtk_widget_set_tooltip_text(history_entry->priv->equation_label, equation);
    gtk_label_set_text(GTK_LABEL(history_entry->priv->equation_label), equation);
    gtk_builder_connect_signals(builder, history_entry);
    g_object_unref(builder);

    #undef get_widget

    math_history_entry_redisplay(history_entry, serializer);
}

void
math_history_entry_redisplay(MathHistoryEntry *history_entry, MpSerializer *serializer)
{
    gchar *answer = mp_serializer_to_string(serializer, history_entry->priv->number);

    gtk_widget_set_tooltip_text(history_entry->priv->answer_label, answer);
    gtk_label_set_text(GTK_LABEL(history_entry->priv->answer_label), answer);

    g_free(answer);
}

gchar *
math_history_entry_get_equation(MathHistoryEntry *history_entry)
{
    return gtk_widget_get_tooltip_text(history_entry->priv->equation_label);
}

static void
math_history_entry_finalize(GObject *object)
{
    MathHistoryEntry *self = MATH_HISTORY_ENTRY (object);

    mp_free(self->priv->number);

    G_OBJECT_CLASS (math_history_entry_parent_class)->finalize (object);
}

static void
math_history_entry_class_init(MathHistoryEntryClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = math_history_entry_finalize;
}

static void
math_history_entry_init(MathHistoryEntry *history_entry)
{
    history_entry->priv = math_history_entry_get_instance_private(history_entry);
    history_entry->priv->number = mp_new_ptr();
}
