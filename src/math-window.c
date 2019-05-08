/*
 * Copyright (C) 1987-2008 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright (C) 2008-2011 Robert Ancell.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "math-window.h"
#include "utility.h"

// gtk3 hack
	#ifndef GDK_KEY_F1
		#define GDK_KEY_F1 GDK_F1
	#endif

	#ifndef GDK_KEY_W
		#define GDK_KEY_W GDK_w
	#endif

	#ifndef GDK_KEY_Z
		#define GDK_KEY_Z GDK_z
	#endif

enum {
    PROP_0,
    PROP_EQUATION
};

struct MathWindowPrivate
{
    GtkWidget *menu_bar;
    MathEquation *equation;
    MathDisplay *display;
    MathButtons *buttons;
    MathPreferencesDialog *preferences_dialog;
    gboolean right_aligned;
    GtkWidget *mode_basic_menu_item;
    GtkWidget *mode_advanced_menu_item;
    GtkWidget *mode_financial_menu_item;
    GtkWidget *mode_programming_menu_item;
};

G_DEFINE_TYPE_WITH_PRIVATE (MathWindow, math_window, GTK_TYPE_WINDOW);

enum
{
    QUIT,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0, };

MathWindow *
math_window_new(MathEquation *equation)
{
    return g_object_new(math_window_get_type(),
            "equation", equation, NULL);
}

GtkWidget *math_window_get_menu_bar(MathWindow *window)
{
    return window->priv->menu_bar;
}

MathEquation *
math_window_get_equation(MathWindow *window)
{
    g_return_val_if_fail(window != NULL, NULL);
    return window->priv->equation;
}


MathDisplay *
math_window_get_display(MathWindow *window)
{
    g_return_val_if_fail(window != NULL, NULL);
    return window->priv->display;
}


MathButtons *
math_window_get_buttons(MathWindow *window)
{
    g_return_val_if_fail(window != NULL, NULL);
    return window->priv->buttons;
}


void
math_window_critical_error(MathWindow *window, const gchar *title, const gchar *contents)
{
    GtkWidget *dialog;

    g_return_if_fail(window != NULL);
    g_return_if_fail(title != NULL);
    g_return_if_fail(contents != NULL);

    dialog = gtk_message_dialog_new(NULL, 0,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_NONE,
                                    "%s", title);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
                                             "%s", contents);
    gtk_dialog_add_buttons(GTK_DIALOG(dialog), "gtk-quit", GTK_RESPONSE_ACCEPT, NULL);

    gtk_dialog_run(GTK_DIALOG(dialog));

    g_signal_emit(window, signals[QUIT], 0);
}

static void mode_changed_cb(GtkWidget *menu, MathWindow *window)
{
    int mode;

    if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu)))
    {
        return;
    }

    mode = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(menu), "calcmode"));
    math_buttons_set_mode(window->priv->buttons, mode);
}

static void show_preferences_cb(GtkMenuItem *menu, MathWindow *window)
{
    if (!window->priv->preferences_dialog)
    {
        window->priv->preferences_dialog = math_preferences_dialog_new(window->priv->equation);
        gtk_window_set_transient_for(GTK_WINDOW(window->priv->preferences_dialog), GTK_WINDOW(window));
    }

    gtk_window_present(GTK_WINDOW(window->priv->preferences_dialog));
}

static gboolean
key_press_cb(MathWindow *window, GdkEventKey *event)
{
    gboolean result;
    g_signal_emit_by_name(window->priv->display, "key-press-event", event, &result);

    if (math_buttons_get_mode (window->priv->buttons) == PROGRAMMING && (event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK) {
        switch(event->keyval)
        {
        /* Binary */
        case GDK_KEY_b:
            math_equation_set_base (window->priv->equation, 2);
            return TRUE;
        /* Octal */
        case GDK_KEY_o:
            math_equation_set_base (window->priv->equation, 8);
            return TRUE;
        /* Decimal */
        case GDK_KEY_d:
            math_equation_set_base (window->priv->equation, 10);
            return TRUE;
        /* Hexdecimal */
        case GDK_KEY_h:
            math_equation_set_base (window->priv->equation, 16);
            return TRUE;
        }
    }

    return result;
}

static void delete_cb(MathWindow *window, GdkEvent *event)
{
    g_signal_emit(window, signals[QUIT], 0);
}

static void copy_cb(GtkWidget *widget, MathWindow *window)
{
    math_equation_copy(window->priv->equation);
}

static void paste_cb(GtkWidget *widget, MathWindow *window)
{
    math_equation_paste(window->priv->equation);
}

static void undo_cb(GtkWidget *widget, MathWindow *window)
{
    math_equation_undo(window->priv->equation);
}

static void redo_cb(GtkWidget *widget, MathWindow *window)
{
    math_equation_redo(window->priv->equation);
}

static void help_cb(GtkWidget *widget, MathWindow *window)
{
    GError *error = NULL;

    gtk_show_uri_on_window(GTK_WINDOW(window),
                           "help:mate-calc",
                           gtk_get_current_event_time(),
                           &error);

    if (error != NULL)
    {
        GtkWidget *d;
        /* Translators: Error message displayed when unable to launch help browser */
        const char *message = _("Unable to open help file");

        d = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", message);
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(d), "%s", error->message);
        g_signal_connect(d, "response", G_CALLBACK(gtk_widget_destroy), NULL);
        gtk_window_present(GTK_WINDOW (d));

        g_error_free(error);
    }
}

static void about_cb(GtkWidget* widget, MathWindow* window)
{
    const char* authors[] = {
        "Rich Burridge <rich.burridge@sun.com>",
        "Robert Ancell <robert.ancell@gmail.com>",
        "Klaus Niederkrüger <kniederk@umpa.ens-lyon.fr>",
        NULL
    };

    const char* documenters[] = {
        N_("Sun Microsystems"),
        N_("MATE Documentation Team"),
        NULL
    };

    /* The license this software is under (GPL2+) */
    const char* license[] = {
        N_("MATE Calculator is free software; you can redistribute it and/or modify "
           "it under the terms of the GNU General Public License as published by "
           "the Free Software Foundation; either version 2 of the License, or "
           "(at your option) any later version."),
        N_("MATE Calculator is distributed in the hope that it will be useful, "
           "but WITHOUT ANY WARRANTY; without even the implied warranty of "
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
           "GNU General Public License for more details."),
        N_("You should have received a copy of the GNU General Public License "
           "along with MATE Calculator; if not, write to the Free Software Foundation, Inc., "
           "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.")
    };

    char *license_trans = g_strjoin ("\n\n", _(license[0]), _(license[1]), _(license[2]), NULL);
    const char **p;

    for (p = documenters; *p; ++p)
        *p = _(*p);

    gtk_show_about_dialog(GTK_WINDOW(window),
        "program-name", _("MATE Calculator"),
        "version", VERSION,
        "title", _("About MATE Calculator"),
        "copyright", _("Copyright \xc2\xa9 1986–2010 The GCalctool authors\n"
                       "Copyright \xc2\xa9 2011-2019 MATE developers"),
        "license", license_trans,
        "comments", _("Calculator with financial and scientific modes."),
        "authors", authors,
        "documenters", documenters,
        "translator_credits", _("translator-credits"),
        "wrap-license", TRUE,
        "website", "http://mate-desktop.org",
        "icon-name", "accessories-calculator",
        "logo-icon-name", "accessories-calculator",
        NULL);

    g_free (license_trans);
}

static void
scroll_changed_cb(GtkAdjustment *adjustment, MathWindow *window)
{
    if (window->priv->right_aligned)
        gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment));
}


static void
scroll_value_changed_cb(GtkAdjustment *adjustment, MathWindow *window)
{
    if (gtk_adjustment_get_value(adjustment) == gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment))
        window->priv->right_aligned = TRUE;
    else
        window->priv->right_aligned = FALSE;
}

static void button_mode_changed_cb(MathButtons *buttons, GParamSpec *spec, MathWindow *window)
{
    GtkWidget *menu;

    switch (math_buttons_get_mode(buttons))
    {
        default:
        case BASIC:
            menu = window->priv->mode_basic_menu_item;
            break;
        case ADVANCED:
            menu = window->priv->mode_advanced_menu_item;
            break;
        case FINANCIAL:
            menu = window->priv->mode_financial_menu_item;
            break;
        case PROGRAMMING:
            menu = window->priv->mode_programming_menu_item;
            break;
    }

    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu), TRUE);
    g_settings_set_enum(g_settings_var, "button-mode", math_buttons_get_mode(buttons));
}

static GtkWidget *add_menu(GtkWidget *menu_bar, const gchar *name)
{
    GtkWidget *menu_item;
    GtkWidget *menu;

    menu_item = gtk_menu_item_new_with_mnemonic(name);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);
    gtk_widget_show(menu_item);
    menu = gtk_menu_new();
    gtk_menu_set_reserve_toggle_size(GTK_MENU(menu),FALSE);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);

    return menu;
}

static void quit_cb(GtkWidget* widget, MathWindow* window)
{
    g_signal_emit(window, signals[QUIT], 0);
}

static GtkWidget *add_menu_item(GtkWidget *menu, GtkWidget *menu_item, GCallback callback, gpointer callback_data)
{
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    gtk_widget_show(menu_item);

    if (callback)
    {
        g_signal_connect(G_OBJECT(menu_item), "activate", callback, callback_data);
    }

    return menu_item;
}

static GtkWidget *radio_menu_item_new(GSList **group, const gchar *name)
{
    GtkWidget *menu_item = gtk_radio_menu_item_new_with_mnemonic(*group, name);

    *group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menu_item));

    return menu_item;
}

static GtkWidget *gtk_image_menu_item_new_from_icon (const gchar   *icon_name,
                                                     const gchar   *label_name,
                                                     GtkAccelGroup *accel_group)
{
    gchar *concat = g_strconcat (label_name, "     ", NULL);
    GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *icon = gtk_image_new_from_icon_name (icon_name, GTK_ICON_SIZE_MENU);
    GtkWidget *label = gtk_accel_label_new (concat);
    GtkWidget *menu_item = gtk_menu_item_new ();

    g_free (concat);

    gtk_container_add (GTK_CONTAINER (box), icon);

    gtk_label_set_use_underline (GTK_LABEL (label), TRUE);
    gtk_label_set_xalign (GTK_LABEL (label), 0.0);

    gtk_accel_label_set_accel_widget (GTK_ACCEL_LABEL (label), menu_item);

    gtk_box_pack_end (GTK_BOX (box), label, TRUE, TRUE, 0);

    gtk_container_add (GTK_CONTAINER (menu_item), box);

    gtk_widget_show_all (menu_item);

    return menu_item;
}

static void create_menu(MathWindow* window)
{
    GtkAccelGroup* accel_group;
    GtkWidget* menu;
    GtkWidget* menu_item;
    GSList* group = NULL;

    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    /* Calculator menu */
    #define CALCULATOR_MENU_LABEL _("_Calculator")
    /* Mode menu */
    #define MODE_MENU_LABEL _("_Mode")
    /* Help menu label */
    #define HELP_MENU_LABEL _("_Help")
    /* Basic menu label */
    #define MODE_BASIC_LABEL _("_Basic")
    /* Advanced menu label */
    #define MODE_ADVANCED_LABEL _("_Advanced")
    /* Financial menu label */
    #define MODE_FINANCIAL_LABEL _("_Financial")
    /* Programming menu label */
    #define MODE_PROGRAMMING_LABEL _("_Programming")
    /* Help>Contents menu label */
    #define HELP_CONTENTS_LABEL _("_Contents")

    menu = add_menu(window->priv->menu_bar, CALCULATOR_MENU_LABEL);
    menu_item = add_menu_item(menu, gtk_image_menu_item_new_from_icon("edit-copy",_("_Copy"), accel_group), G_CALLBACK(copy_cb), window);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_C, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    menu_item = add_menu_item(menu, gtk_image_menu_item_new_from_icon("edit-paste",_("_Paste"), accel_group), G_CALLBACK(paste_cb), window);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_V, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    menu_item = add_menu_item(menu, gtk_image_menu_item_new_from_icon("edit-undo",_("_Undo"), accel_group), G_CALLBACK(undo_cb), window);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_Z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    menu_item = add_menu_item(menu, gtk_image_menu_item_new_from_icon("edit-redo",_("_Redo"), accel_group), G_CALLBACK(redo_cb), window);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_Z, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    add_menu_item(menu, gtk_separator_menu_item_new(), NULL, NULL);
    add_menu_item(menu, gtk_image_menu_item_new_from_icon("preferences-desktop",_("_Preferences"), accel_group), G_CALLBACK(show_preferences_cb), window);
    add_menu_item(menu, gtk_separator_menu_item_new(), NULL, NULL);
    menu_item = add_menu_item(menu, gtk_image_menu_item_new_from_icon("application-exit",_("_Quit"), accel_group), G_CALLBACK(quit_cb), window);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_Q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_W, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    menu = add_menu(window->priv->menu_bar, MODE_MENU_LABEL);
    window->priv->mode_basic_menu_item = add_menu_item(menu, radio_menu_item_new(&group, MODE_BASIC_LABEL), G_CALLBACK(mode_changed_cb), window);
    g_object_set_data(G_OBJECT(window->priv->mode_basic_menu_item), "calcmode", GINT_TO_POINTER(BASIC));
    window->priv->mode_advanced_menu_item = add_menu_item(menu, radio_menu_item_new(&group, MODE_ADVANCED_LABEL), G_CALLBACK(mode_changed_cb), window);
    g_object_set_data(G_OBJECT(window->priv->mode_advanced_menu_item), "calcmode", GINT_TO_POINTER(ADVANCED));
    window->priv->mode_financial_menu_item = add_menu_item(menu, radio_menu_item_new(&group, MODE_FINANCIAL_LABEL), G_CALLBACK(mode_changed_cb), window);
    g_object_set_data(G_OBJECT(window->priv->mode_financial_menu_item), "calcmode", GINT_TO_POINTER(FINANCIAL));
    window->priv->mode_programming_menu_item = add_menu_item(menu, radio_menu_item_new(&group, MODE_PROGRAMMING_LABEL), G_CALLBACK(mode_changed_cb), window);
    g_object_set_data(G_OBJECT(window->priv->mode_programming_menu_item), "calcmode", GINT_TO_POINTER(PROGRAMMING));

    menu = add_menu(window->priv->menu_bar, HELP_MENU_LABEL);
    menu_item = add_menu_item(menu, gtk_image_menu_item_new_from_icon("help-browser", HELP_CONTENTS_LABEL, accel_group), G_CALLBACK(help_cb), window);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_F1, 0, GTK_ACCEL_VISIBLE);
    add_menu_item(menu, gtk_image_menu_item_new_from_icon("help-about",_("_About"), accel_group), G_CALLBACK(about_cb), window);
}

static void
create_gui(MathWindow *window)
{
    GtkWidget *main_vbox, *vbox;
    GtkWidget *scrolled_window;

    main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);
    gtk_widget_show(main_vbox);

    window->priv->menu_bar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(main_vbox), window->priv->menu_bar, TRUE, TRUE, 0);
    gtk_widget_show(window->priv->menu_bar);

    create_menu(window);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 6);
    gtk_box_pack_start(GTK_BOX(main_vbox), vbox, TRUE, TRUE, 0);  
    gtk_widget_show(vbox);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(scrolled_window), TRUE, TRUE, 0);
    g_signal_connect(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window)), "changed", G_CALLBACK(scroll_changed_cb), window);
    g_signal_connect(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window)), "value-changed", G_CALLBACK(scroll_value_changed_cb), window);
    window->priv->right_aligned = TRUE;
    gtk_widget_show(scrolled_window);

    window->priv->display = math_display_new_with_equation(window->priv->equation);
    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(window->priv->display));
    gtk_widget_show(GTK_WIDGET(window->priv->display));

    window->priv->buttons = math_buttons_new(window->priv->equation);
    g_signal_connect(window->priv->buttons, "notify::mode", G_CALLBACK(button_mode_changed_cb), window);
    button_mode_changed_cb(window->priv->buttons, NULL, window);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(window->priv->buttons), TRUE, TRUE, 0);
    gtk_widget_show(GTK_WIDGET(window->priv->buttons));
}


static void
math_window_set_property(GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
    MathWindow *self;

    self = MATH_WINDOW(object);

    switch (prop_id) {
    case PROP_EQUATION:
        self->priv->equation = g_value_get_object(value);
        create_gui(self);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


static void
math_window_get_property(GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
    MathWindow *self;

    self = MATH_WINDOW(object);

    switch (prop_id) {
    case PROP_EQUATION:
        g_value_set_object(value, self->priv->equation);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


static void
math_window_class_init(MathWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->get_property = math_window_get_property;
    object_class->set_property = math_window_set_property;

    g_object_class_install_property(object_class,
                                    PROP_EQUATION,
                                    g_param_spec_object("equation",
                                                        "equation",
                                                        "Equation being calculated",
                                                        math_equation_get_type(),
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    signals[QUIT] = g_signal_new("quit",
                                 G_TYPE_FROM_CLASS (klass),
                                 G_SIGNAL_RUN_LAST,
                                 G_STRUCT_OFFSET (MathWindowClass, quit),
                                 NULL, NULL,
                                 g_cclosure_marshal_VOID__VOID,
                                 G_TYPE_NONE, 0);
}


static void
math_window_init(MathWindow *window)
{
    window->priv = math_window_get_instance_private (window);
    gtk_window_set_title(GTK_WINDOW(window),
                         /* Title of main window */
                         _("Calculator"));
    gtk_window_set_icon_name(GTK_WINDOW(window), "accessories-calculator");
    gtk_window_set_role(GTK_WINDOW(window), "mate-calc");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    g_signal_connect_after(G_OBJECT(window), "key-press-event", G_CALLBACK(key_press_cb), NULL);
    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(delete_cb), NULL);
}
