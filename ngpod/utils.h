#ifndef __NGPOD_UTILS_H
#define __NGPOD_UTILS_H

#include <glib-object.h>
#include <glib.h>

guint create_void_void_signal (const gchar *signal_name);
void log_message (const gchar *class_name, const gchar *format, ...);
GDate *date_from_string (gchar *str);
gchar *date_to_string (GDate *date);
gint regex_substr (const gchar *text, gchar *regex_text, gchar ***result);
void regex_substr_free (gchar ***result, gint count);

#endif
