#include <gio/gio.h>
#include <glibmm.h>
#include <string>
#include "ngpod-settings.h"
#include "Logger.h"

using namespace boost;
using namespace Glib;
using namespace std;

namespace ngpod
{

Settings::~Settings()
{
    g_key_file_free (key_file);
}

Settings::Settings()
    : log("Settings")
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

void Settings::SetLastDate (const gregorian::date& date)
{
    gint list[] = { date.year(), date.month(), date.day() };
    g_key_file_set_integer_list (key_file, "main", "last_date", list, 3);
    SaveSettings ();
}

gregorian::date Settings::GetLastDate() const
{
    gsize length = 0;
    gint *list = g_key_file_get_integer_list (key_file, "main", "last_date", &length, NULL);

    gregorian::date date;

    if (list == NULL || length < 3)
    {
        date = gregorian::date(1, 1, 1);
    }
    else
    {
        date = gregorian::date (list[0], list[1], list[2]);
        g_free (list);
    }

    return date;
}

void Settings::SetDir (const string& dir)
{
    g_key_file_set_string (key_file, "main", "dir", dir.c_str());
    SaveSettings();
}

string Settings::GetDir() const
{
    return g_key_file_get_string (key_file, "main", "dir", NULL);
}

string Settings::GetLogFile() const
{
    return g_key_file_get_string (key_file, "main", "log_file", NULL);
}

posix_time::time_duration Settings::GetTimeSpan() const
{
    gsize length = 0;
    gint *list = g_key_file_get_integer_list (key_file, "main", "time_span", &length, NULL);

    posix_time::time_duration time_span(0, 0, 0, 0);

    if (list != NULL && length == 2)
    {
        if (length == 2)
        {
            time_span = posix_time::time_duration(list[0], list[1], 0, 0);
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
            log << "Cannot create settings directory: " << error->message;
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
        log << "Cannot save settings: " << error->message;
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
            log << "Cannot save settings: " << error->message;
            g_error_free (error);
        }

        if (length != bytes_written)
        {
            log << "Written " << bytes_written << " bytes to settings file while should write " << length;
        }

        g_free (data);
    }

    g_clear_object (&os);
    g_object_unref (file);
}

}