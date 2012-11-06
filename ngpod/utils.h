#ifndef __NGPOD_UTILS_H
#define __NGPOD_UTILS_H

#include <glib-object.h>
#include <glib.h>
#include <glibmm/date.h>

// guint create_void_void_signal (const gchar *signal_name);
void log_message (const gchar *class_name, const gchar *format, ...);
Glib::Date date_from_string (const Glib::ustring& str);
Glib::ustring date_to_string (const Glib::Date& date);
// gint regex_substr (const gchar *text, gchar *regex_text, gchar ***result);
// void regex_substr_free (gchar ***result, gint count);
 gchar *g_str_replace (const gchar *input, const gchar *search, const gchar *replace);

#endif
