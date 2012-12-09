#include "utils.h"
#include <boost/date_time/date_facet.hpp>
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include <glib.h>
#include <glibmm.h>
#include <sstream>

using namespace Glib;
using namespace std;
using namespace boost;

string GetPathFromDate(const posix_time::ptime& now, const string& dir)
{
    stringstream ss;
    gregorian::date_facet* facet = new gregorian::date_facet();
    facet->set_iso_extended_format();
    ss.imbue(locale(ss.getloc(), facet));
    ss << "pod-" << now.date() << ".png";
    filesystem::path path(dir);
    path /= ss.str();
    return path.string();
}

gregorian::date DateFromString (const std::string& sstr)
{
    static const char *months[] =
    {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };

    const gchar *str = sstr.c_str();
    gchar **substrs;
    uint count = regex_substr (str, "([^<]{3})[^<]* ([^<]*), ([^<]*)", &substrs);

    gint month;
    for (month = 0; month < 12; ++month)
    {
        if (g_strcmp0 (months[month], substrs[0]) == 0)
        {
            break;
        }
    }

    if (month >= 12) return gregorian::date();

    gregorian::date date(atoi(substrs[2]), month + 1, atoi (substrs[1]));
    regex_substr_free (&substrs, count);

    return date;
}

gint
regex_substr (const gchar *text, const gchar *regex_text, gchar ***result)
{
    GError *error = NULL;
    GRegex *regex = g_regex_new (regex_text, (GRegexCompileFlags)0, (GRegexMatchFlags)0, &error);
    g_return_val_if_fail (!error, 0);

    GMatchInfo *match_info;
    if (!g_regex_match (regex, text, (GRegexMatchFlags)0, &match_info))
    {
        return 0;
    }

    int match_count = g_match_info_get_match_count (match_info);
    if (match_count < 2)
    {
        return 0;
    }

    *result = g_new (gchar*, match_count - 1);

    int i;
    for (i = 0; i < match_count - 1; ++i)
    {
        gint start_pos;
        gint end_pos;
        g_match_info_fetch_pos (match_info, i + 1, &start_pos, &end_pos);
        (*result)[i] = g_strndup (text + start_pos, end_pos - start_pos);
    }

    g_regex_unref (regex);
    g_match_info_free (match_info);

    return match_count - 1;
}

void
regex_substr_free (gchar ***result, gint count)
{
    int i;
    for (i = 0; i < count; ++i)
    {
        g_free ((*result)[i]);
    }

    if (count)
    {
        g_free (*result);
    }
}

std::string StrReplace (const std::string& input, const std::string& search, const std::string& replace)
{
    string result;

    int prev = 0;
    int i = 0;
    while ((i = input.find(search, prev)) != string::npos)
    {
        result += input.substr(prev, i - prev);
        result += replace;
        prev = i + search.size();
    }

    result += input.substr(prev, i);

    return result;
}
