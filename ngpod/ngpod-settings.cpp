#include <gio/gio.h>
#include <glibmm.h>
#include "ngpod-settings.h"
#include "utils.h"

using namespace Glib;

namespace ngpod
{

Settings::~Settings()
{
    g_key_file_free (key_file);
}

Settings::Settings()
{
    SETTINGS_DIR = build_filename (g_get_user_config_dir (), "gumik");
    SETTINGS_PATH = build_filename (SETTINGS_DIR, "ngpod-gtk.ini");

    key_file = g_key_file_new ();
}

bool Settings::Initialize()
{
    if (!CreateConfigDir())
    {
        return false;
    }

    g_key_file_load_from_file (
        key_file,
        SETTINGS_PATH.c_str(),
        G_KEY_FILE_NONE, NULL);

    return true;
}

void Settings::SetLastDate (const Date& date)
{
    gint list[] = { date.get_year(), date.get_month(), date.get_day() };
    g_key_file_set_integer_list (key_file, "main", "last_date", list, 3);
    SaveSettings ();
}

Date Settings::GetLastDate() const
{
    gsize length = 0;
    gint *list = g_key_file_get_integer_list (key_file, "main", "last_date", &length, NULL);

    Date date;

    if (list == NULL || length < 3)
    {
        date = Date(Date::MONDAY, Date::JANUARY, 1);
    }
    else
    {
        date = Date (static_cast<Date::Day>(list[2]), static_cast<Date::Month>(list[1]), list[0]);
        g_free (list);
    }

    return date;
}

void Settings::SetDir (const ustring& dir)
{
    g_key_file_set_string (key_file, "main", "dir", dir.c_str());
    SaveSettings();
}

ustring Settings::GetDir() const
{
    return g_key_file_get_string (key_file, "main", "dir", NULL);
}

ustring Settings::GetLogFile() const
{
    return g_key_file_get_string (key_file, "main", "log_file", NULL);
}

GTimeSpan Settings::GetTimeSpan() const
{
    gsize length = 0;
    gint *list = g_key_file_get_integer_list (key_file, "main", "time_span", &length, NULL);

    GTimeSpan time_span = 0;

    if (list != NULL && length == 2)
    {
        if (length == 2)
        {
            time_span = G_TIME_SPAN_HOUR * list[0] + G_TIME_SPAN_MINUTE * list[1];
        }

        g_free (list);
    }

    return time_span;
}

bool Settings::CreateConfigDir()
{
    GError *error = NULL;
    GFile *dir = g_file_new_for_path (SETTINGS_DIR.c_str());
    g_file_make_directory_with_parents (dir, NULL, &error);

    gboolean status = TRUE;

    if (error != NULL)
    {
        if (error->code != G_IO_ERROR_EXISTS)
        {
            status = FALSE;
            log_message ("Settings", "Cannot create settings directory: %s", error->message);
        }

        g_error_free (error);
    }

    g_object_unref (dir);

    return status;
}

void Settings::SaveSettings()
{
    GError *error = NULL;
    GFile *file = g_file_new_for_path (SETTINGS_PATH.c_str());
    GFileOutputStream *os = g_file_replace (file, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &error);

    if (error != NULL)
    {
        log_message ("Settings", "Cannot save settings: %s", error->message);
        g_error_free (error);
    }
    else
    {
        gsize length = 0;
        gchar *data = g_key_file_to_data (key_file, &length, NULL);

        error = NULL;
        gsize bytes_written = 0;
        g_output_stream_write_all (G_OUTPUT_STREAM (os), data, length, &bytes_written, NULL, &error);

        if (error != NULL)
        {
            log_message ("Settings", "Cannot save settings: %s", error->message);
            g_error_free (error);
        }

        if (length != bytes_written)
        {
            log_message ("Settings", "Written %d bytes to settings file while should write %d",
                bytes_written, length);
        }

        g_free (data);
    }

    g_clear_object (&os);
    g_object_unref (file);
}

}