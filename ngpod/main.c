#include <gtk/gtk.h>
//#include "config_win.h"
#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>

#include "ngpod-downloader.h"
#include "ngpod-watcher.h"
#include "ngpod-timer.h"
#include "ngpod-settings.h"
#include "ngpod-presenter.h"
#include "utils.h"

GMainLoop *main_loop;
NgpodDownloader *downloader;

static void
download_finished_event ()
{
    g_printf ("download_finished_event()\n");
    gboolean is_success = ngpod_downloader_is_success (downloader);
    g_printf ("is_success: %s\n", is_success ? "TRUE" : "FALSE");

    if (is_success)
    {
        const GDate *date = ngpod_downloader_get_date (downloader);
        const char *link = ngpod_downloader_get_link (downloader);
        const char *resolution = ngpod_downloader_get_resolution (downloader);
        //ngpod_downloader_get_data (NgpodDownloader *self);
        gsize data_length = ngpod_downloader_get_data_length (downloader);

        gchar *date_string = g_new (gchar, 100);
        g_date_strftime (date_string, 100, "%F", date);

        g_printf("date: %s\nlink: %s\nresolution: %s\ndata_length: %lu\n",
            date_string, link, resolution, data_length);
        g_free (date_string);
    }

    // g_object_unref (downloader);
    // g_main_loop_quit (main_loop);
}

static char *status_to_str[] =
{
    "NGPOD_WATCHER_STATUS_INVALID",
    "NGPOD_WATCHER_STATUS_NOT_NEEDED",              // There is too early to update
    "NGPOD_WATCHER_STATUS_FAILED",                  // Failed to download website
    "NGPOD_WATCHER_STATUS_SUCCESSFUL",              // Downloading website and image succesful
    "NGPOD_WATCHER_STATUS_SUCCESSFUL_NOT_NEEDED",   // Success but image is the same as previous
};

static void
watcher_finished_event (NgpodWatcher *watcher)
{
    g_print ("watcher_finished_event(), status: %s\n", status_to_str[ngpod_watcher_get_status (watcher)]);
}

static void
log_func (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
    GDateTime *now = g_date_time_new_now_local ();
    gchar *time_str = g_date_time_format (now, "%Y-%m-%d %H:%M:%S");
    g_print ("[%s] %s-%d: %s\n", time_str, log_domain, log_level, message);
    g_free (time_str);
}

int main (int argc, char **argv)
{
    // g_type_init ();

    // GMainContext *context = g_main_context_default ();
    // main_loop = g_main_loop_new (context, FALSE);

    // downloader = ngpod_downloader_new ();
    // g_signal_connect (downloader, "download-finished", download_finished_event, NULL);
    // // ngpod_downloader_start (downloader, NGPOD_DOWNLOADER_DEFAULT_URL);

    // GDate *last_date = g_date_new_dmy(24, 6, 2012);
    // NgpodWatcher *watcher = ngpod_watcher_new (downloader, last_date);
    // g_signal_connect (watcher, "update-finished", G_CALLBACK (watcher_finished_event), NULL);

    // GDateTime *now = g_date_time_new_now_local();
    // GDateTime *now_ = g_date_time_add_days (now, -1);
    // g_print ("ngpod_watcher_tick\n");
    // ngpod_watcher_tick (watcher, now_);
    // g_date_time_unref(now);
    // g_date_time_unref(now_);

    // g_main_loop_run (main_loop);

    gtk_init (&argc, &argv);

    g_log_set_default_handler (log_func, NULL);

    // NgpodDownloader *downloader = ngpod_downloader_new ();
    // NgpodSettings *settings = ngpod_settings_new ();
    // ngpod_settings_initialize (settings);
    // GDate *last_date = ngpod_settings_get_last_date (settings);
    // log_message ("main", "Last date: %d-%d-%d", g_date_get_year (last_date), g_date_get_month (last_date), g_date_get_day (last_date));
    // NgpodWatcher *watcher = ngpod_watcher_new (downloader, last_date);
    // NgpodTimer *timer = ngpod_timer_new (watcher, settings);

    // g_object_unref (downloader);
    // g_object_unref (settings);
    // g_object_unref (watcher);

    // ngpod_timer_start (timer);

    NgpodPresenter *presenter = ngpod_presenter_new ();

    GFile *file = g_file_new_for_path ("/home/gumik/temp/temp.jpg");
    GError *error = NULL;
    GFileInputStream *stream = g_file_read (file, NULL, &error);

    if (error != NULL)
    {
        g_print ("%s\n", error->message);
        g_error_free (error);
    }

    const char *data = g_malloc (10 * 1024 * 1024);
    gsize length = g_input_stream_read (G_INPUT_STREAM (stream), (gpointer) data, 10 * 1024 * 1024, NULL, NULL);
    g_print ("data length: %ld\n", length);

    ngpod_presenter_notify (presenter, data, length);

    gtk_main ();
}