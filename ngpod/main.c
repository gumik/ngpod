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
log_func (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
    GDateTime *now = g_date_time_new_now_local ();
    gchar *time_str = g_date_time_format (now, "%Y-%m-%d %H:%M:%S");
    g_print ("[%s] %s-%d: %s\n", time_str, log_domain, log_level, message);
    g_free (time_str);
}

int main (int argc, char **argv)
{
    gtk_init (&argc, &argv);

    g_log_set_default_handler (log_func, NULL);

    NgpodSettings *settings = ngpod_settings_new ();
    ngpod_settings_initialize (settings);

    GDate *last_date = ngpod_settings_get_last_date (settings);
    log_message ("main", "Last date: %d-%d-%d", g_date_get_year (last_date), g_date_get_month (last_date), g_date_get_day (last_date));

    gchar *dir = ngpod_settings_get_dir (settings);
    if (dir == NULL)
    {
        g_print ("Set dir in config file\n");
        return 1;
    }
    log_message ("main", "Dir: %s", dir);

    NgpodDownloader *downloader = ngpod_downloader_new ();
    NgpodWatcher *watcher = ngpod_watcher_new (downloader, last_date);
    NgpodPresenter *presenter = ngpod_presenter_new ();
    NgpodWallpaper *wallpaper = ngpod_wallpaper_new (dir);
    NgpodTimer *timer = ngpod_timer_new (watcher, settings, presenter, wallpaper);

    g_object_unref (downloader);
    g_object_unref (settings);
    g_object_unref (watcher);
    g_object_unref (wallpaper);

    ngpod_timer_start (timer);

    gtk_main ();
}