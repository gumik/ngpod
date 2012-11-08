#include <gtk/gtk.h>
#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#include "ngpod-downloader.h"
#include "ngpod-watcher.h"
#include "ngpod-timer.h"
#include "ngpod-settings.h"
#include "ngpod-presenter.h"
#include "config.h"
#include "utils.h"

using namespace ngpod;
using namespace Glib;

GMainLoop *main_loop;

static ustring log_file;
static GFileOutputStream *os = NULL;

static void log_to_file (const gchar *msg, gsize length)
{
    if (!log_file.empty())
    {
        GError *error = NULL;

        if (os == NULL)
        {
            GFile *file = g_file_new_for_path (log_file.c_str());
            os = g_file_append_to (file, G_FILE_CREATE_NONE, NULL, &error);

            if (os == NULL)
            {
                g_print ("Cannot open log file: %s\n", error->message);
                g_error_free (error);
                g_object_unref (file);
                return;
            }

            g_object_unref (file);
        }

        gsize bytes_written = 0;
        if (!g_output_stream_write_all (G_OUTPUT_STREAM (os), msg, length, &bytes_written, NULL, &error))
        {
            g_print ("Cannot write to log file: %s\n", error->message);
            g_error_free (error);
            return;
        }

        if (!g_output_stream_flush (G_OUTPUT_STREAM (os), NULL, &error))
        {
            g_print ("Cannot flush log file: %s\n", error->message);
            g_error_free (error);
            return;
        }
    }
}

static void
log_func (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
    GDateTime *now = g_date_time_new_now_local ();
    gchar *time_str = g_date_time_format (now, "%Y-%m-%d %H:%M:%S");
    GString *msg = g_string_new ("");

    g_string_printf (msg, "[%s] %s-%d: %s\n", time_str, log_domain, log_level, message);
    g_print ("%s", msg->str);

    log_to_file (msg->str, msg->len);

    g_free (time_str);
    g_string_free (msg, TRUE);
}

int main (int argc, char **argv)
{
    Glib::RefPtr<Gtk::Application> app =
    Gtk::Application::create(argc, argv,
      "gumik.ngpod");

    g_print ("ngpod %d.%d - National Geographic Picture of the Day wallpaper downloader\n",
        VERSION_MAJOR, VERSION_MINOR);

    // gtk_init (&argc, &argv);

    g_log_set_default_handler (log_func, NULL);

    Settings settings;
    settings.Initialize();

    log_file = settings.GetLogFile();
    if (!log_file.empty())
    {
        log_message ("main", "Log file: %s", log_file.c_str());
    }

    Date last_date = settings.GetLastDate();
    log_message ("main", "Last date: %d-%d-%d", last_date.get_year(), last_date.get_month(), last_date.get_day());

    ustring dir = settings.GetDir();
    if (dir.empty())
    {
        g_print ("Set dir in config file\n");
        return 1;
    }
    log_message ("main", "Dir: %s", dir.c_str());

    GTimeSpan time_span = settings.GetTimeSpan();
    log_message ("main", "Time span: %dh %dmin", time_span / G_TIME_SPAN_HOUR, (time_span % G_TIME_SPAN_HOUR) / G_TIME_SPAN_MINUTE);

    Downloader downloader;
    Watcher watcher(downloader, last_date, time_span);
    Presenter presenter;
    Wallpaper wallpaper(dir);
    ngpod::Timer timer(watcher, settings, presenter, wallpaper);

    timer.Start();

    app->run();
}