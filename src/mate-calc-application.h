/*
 * mate-calc-application.h - GtkApplication for MATE Calculator
 *
 * Copyright (C) 2026 MATE Desktop Team
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#ifndef MATE_CALC_APPLICATION_H
#define MATE_CALC_APPLICATION_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MATE_CALC_TYPE_APPLICATION (mate_calc_application_get_type())
G_DECLARE_FINAL_TYPE(MateCalcApplication, mate_calc_application, MATE_CALC, APPLICATION, GtkApplication)

/**
 * mate_calc_application_new:
 *
 * Creates a new MateCalcApplication instance.
 *
 * Returns: (transfer full): A new MateCalcApplication
 */
MateCalcApplication *mate_calc_application_new(void);

/**
 * mate_calc_application_get_settings:
 * @app: A MateCalcApplication
 *
 * Gets the GSettings object for the calculator.
 *
 * Returns: (transfer none): The GSettings object
 */
GSettings *mate_calc_application_get_settings(MateCalcApplication *app);

G_END_DECLS

#endif /* MATE_CALC_APPLICATION_H */
