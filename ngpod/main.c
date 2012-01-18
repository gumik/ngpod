#include <gtk/gtk.h>
//#include "config_win.h"
#include "ngpod-downloader.h"
#include <stdio.h>

int main (int argc, char **argv) 
 {
    //NgpodPictureWin *win;
  
    //gtk_init (&argc, &argv);

    //win = ngpod_picture_win_new ();
    
    //gtk_widget_show (GTK_WIDGET (win));
    
    //gtk_main();
    
    printf ("init\n");
    
    g_type_init ();
    
    /* regex test */
    /*GError *error = NULL;
    GRegex *regex = g_regex_new ("<p class=\"publication_time\">([^<]*)</p>", 0, 0, &error);
    g_return_if_fail (!error);  
    
    GMatchInfo *match_info;
    gchar *text = "<p class=\"publication_time\">January 18, 2012</p>";
    gboolean is_match = g_regex_match (regex, text, 0, &match_info);
    printf ("is_match: %d\n", is_match);
    gint match_count = g_match_info_get_match_count (match_info);
    printf ("match_count: %d\n", match_count);
    
    gint i;
    for (i = 0; i < match_count; ++i) 
    {
        gint start_pos;
        gint end_pos;
        g_match_info_fetch_pos (match_info, i, &start_pos, &end_pos);
        gchar *substr = g_strndup (text + start_pos, end_pos - start_pos);
        printf("match %d-%d: %s\n", start_pos, end_pos, substr);
        g_free (substr);
    }
    
    g_regex_unref (regex);
    g_match_info_free (match_info);*/
    
    GMainContext *context = g_main_context_default ();
    GMainLoop *main_loop = g_main_loop_new (context, FALSE);
    
    NgpodDownloader *downloader = ngpod_downloader_new ();
    ngpod_downloader_start (downloader);
    
    g_main_loop_run (main_loop);
}