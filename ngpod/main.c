#include <gtk/gtk.h>
//#include "config_win.h"
#include "ngpod-downloader.h"
#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h>

GMainLoop *main_loop;
NgpodDownloader *downloader;

void
download_finished_event ()
{
    g_printf ("download finished\n");
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

    g_object_unref (downloader);
    g_main_loop_quit (main_loop);
}

int main (int argc, char **argv)
 {
    //NgpodPictureWin *win;

    //gtk_init (&argc, &argv);

    //win = ngpod_picture_win_new ();

    //gtk_widget_show (GTK_WIDGET (win));

    //gtk_main();

    g_type_init ();

    GMainContext *context = g_main_context_default ();
    main_loop = g_main_loop_new (context, FALSE);

    downloader = ngpod_downloader_new ();
    g_signal_connect (downloader, "download-finished", G_CALLBACK (download_finished_event), NULL);
    ngpod_downloader_start (downloader);

    g_main_loop_run (main_loop);
}