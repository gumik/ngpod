#ifndef __NGPOD_UTILS_H
#define __NGPOD_UTILS_H

#include <boost/date_time.hpp>
#include <glib-object.h>
#include <glib.h>
#include <glibmm/date.h>
#include <glibmm/datetime.h>

std::string GetPathFromDate(const boost::posix_time::ptime& now, const std::string& dir);
boost::gregorian::date DateFromString (const std::string& str);
std::string GetConfigDir();
gint regex_substr (const gchar *text, const gchar *regex_text, gchar ***result);
void regex_substr_free (gchar ***result, gint count);
std::string StrReplace (const std::string& input, const std::string& search, const std::string& replace);

#endif
