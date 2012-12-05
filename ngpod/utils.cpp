#include "utils.h"
#include <stdlib.h>
#include <glib.h>
#include <glibmm.h>

using namespace Glib;
using namespace std;

string GetPathFromDate(const DateTime& now, const string& dir)
{
    string filename = now.format("pod-%F.png");
    return Glib::build_filename(dir, filename);
}

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

Glib::Date date_from_string (const std::string& sstr)
{
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

    if (month >= 12) return Glib::Date();

    Glib::Date date;
    date.set_month(static_cast<Glib::Date::Month>(month + 1));
    date.set_day(static_cast<Glib::Date::Day>(atoi (substrs[1])));
    date.set_year(static_cast<Glib::Date::Year>(atoi (substrs[2])));

    regex_substr_free (&substrs, count);

    return date;
}

std::string date_to_string (const Glib::Date& date)
{
    char buf[16];
    g_date_strftime (buf, 16, "%Y-%m-%d", date.gobj());
    return buf;
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
