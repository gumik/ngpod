#ifndef __NGPOD_UTILS_H
#define __NGPOD_UTILS_H

#include <glib-object.h>
#include <glib.h>
#include <glibmm/date.h>
#include <glibmm/datetime.h>

std::string GetPathFromDate(const Glib::DateTime& now, const std::string& dir);
Glib::Date date_from_string (const std::string& str);
std::string date_to_string (const Glib::Date& date);
gint regex_substr (const gchar *text, const gchar *regex_text, gchar ***result);
void regex_substr_free (gchar ***result, gint count);
std::string StrReplace (const std::string& input, const std::string& search, const std::string& replace);

#endif
