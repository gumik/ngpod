#include "utils.h"
#include <stdlib.h>
#include <glib.h>

// guint
// create_void_void_signal (const gchar *signal_name)
// {
//     return g_signal_newv (
//         signal_name,
//         G_TYPE_OBJECT,
//         G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
//         NULL /* closure */,
//         NULL /* accumulator */,
//         NULL /* accumulator data */,
//         g_cclosure_marshal_VOID__VOID,
//         G_TYPE_NONE /* return_type */,
//         0     /* n_params */,
//         NULL  /* param_types */);
// }

void
log_message (const gchar *class_name, const gchar *format, ...)
{
    va_list list;
    va_start (list, format);
    g_logv (class_name, G_LOG_LEVEL_MESSAGE, format, list);
    va_end (list);
}

// static const gchar *months[] =
// {
//     "Jan",
//     "Feb",
//     "Mar",
//     "Apr",
//     "May",
//     "Jun",
//     "Jul",
//     "Aug",
//     "Sep",
//     "Oct",
//     "Nov",
//     "Dec"
// };

Glib::Date date_from_string (Glib::ustring& sstr)
{
    // const gchar *str = sstr.c_str();
    // gchar **substrs;
    // guint count = regex_substr (str, "([^<]{3})[^<]* ([^<]*), ([^<]*)", &substrs);

    // gint month;
    // for (month = 0; month < 12; ++month)
    // {
    //     if (g_strcmp0 (months[month], substrs[0]) == 0)
    //     {
    //         break;
    //     }
    // }

    // if (month >= 12) return Glib::Date();

    Glib::Date date;
    // date.set_month(static_cast<Glib::Date::Month>(month + 1));
    // date.set_day(static_cast<Glib::Date::Day>(atoi (substrs[1])));
    // date.set_year(static_cast<Glib::Date::Year>(atoi (substrs[2])));

    // regex_substr_free (&substrs, count);

    return date;
}

// gchar *
// date_to_string (GDate *date)
// {
//     gchar *buf = g_malloc(16);
//     g_date_strftime (buf, 16, "%Y-%m-%d", date);
//     return buf;
// }

gint
regex_substr (const gchar *text, gchar *regex_text, gchar ***result)
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

gchar *
g_str_replace (const gchar *input, const gchar *search, const gchar *replace)
{
    gchar **tokens = g_strsplit (input, search, 0);
    gchar *replaced = g_strjoinv (replace, tokens);
    g_strfreev (tokens);
    return replaced;
}
