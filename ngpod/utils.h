#ifndef __NGPOD_UTILS_H
#define __NGPOD_UTILS_H

#include <glib-object.h>
#include <glib.h>
#include <glibmm/date.h>
#include <glibmm/datetime.h>

Glib::ustring GetPathFromDate(const Glib::DateTime& now, const Glib::ustring& dir);
Glib::Date date_from_string (const Glib::ustring& str);
Glib::ustring date_to_string (const Glib::Date& date);
gint regex_substr (const gchar *text, const gchar *regex_text, gchar ***result);
void regex_substr_free (gchar ***result, gint count);
Glib::ustring StrReplace (const Glib::ustring& input, const Glib::ustring& search, const Glib::ustring& replace);

#endif
