#include <gio/gio.h>
#include <gconf/gconf.h>
#include "ngpod-wallpaper.h"

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NGPOD_TYPE_WALLPAPER, NgpodWallpaperPrivate))

struct _NgpodWallpaperPrivate
{
    gchar *dir;
};

G_DEFINE_TYPE (NgpodWallpaper, ngpod_wallpaper, G_TYPE_OBJECT);

static gchar *get_path_from_date (GDateTime *now, const gchar *dir);

static void
ngpod_wallpaper_dispose (GObject *gobject)
{
    NgpodWallpaper *self = NGPOD_WALLPAPER (gobject);

    /*
    * In dispose, you are supposed to free all types referenced from this
    * object which might themselves hold a reference to self. Generally,
    * the most simple solution is to unref all members on which you own a
    * reference.
    */

    /* dispose might be called multiple times, so we must guard against
    * calling g_object_unref() on an invalid GObject.
    */
    /*if (self->priv->an_object)
    {
        g_object_unref (self->priv->an_object);
        self->priv->an_object = NULL;
    }*/

    /* Chain up to the parent class */
    G_OBJECT_CLASS (ngpod_wallpaper_parent_class)->dispose (gobject);
}

static void
ngpod_wallpaper_finalize (GObject *gobject)
{
    NgpodWallpaper *self = NGPOD_WALLPAPER (gobject);
    NgpodWallpaperPrivate *priv = GET_PRIVATE (self);

    g_free (priv->dir);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (ngpod_wallpaper_parent_class)->finalize (gobject);
}

static void
ngpod_wallpaper_class_init (NgpodWallpaperClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = ngpod_wallpaper_dispose;
    gobject_class->finalize = ngpod_wallpaper_finalize;

    g_type_class_add_private (klass, sizeof (NgpodWallpaperPrivate));
}

static void
ngpod_wallpaper_init (NgpodWallpaper *self)
{
    NgpodWallpaperPrivate *priv;
    self->priv = priv = GET_PRIVATE (self);
}

NgpodWallpaper *
ngpod_wallpaper_new (gchar *dir)
{
    GObject *object = g_object_new (NGPOD_TYPE_WALLPAPER, NULL);

    NgpodWallpaperPrivate *priv = GET_PRIVATE (object);
    priv->dir = dir;

    return NGPOD_WALLPAPER (object);
}

gboolean
ngpod_wallpaper_set_from_data (NgpodWallpaper *self, const char *data, gsize data_length, GError **error)
{
    NgpodWallpaperPrivate *priv = GET_PRIVATE (self);

    // Prepare path
    GDateTime *now = g_date_time_new_now_local ();
    gchar *path = get_path_from_date (now, priv->dir);
    g_date_time_unref (now);

    // Write data to file
    GFile *file = g_file_new_for_path (path);
    //g_free (path);

    GFileOutputStream *stream = g_file_create (file, G_FILE_CREATE_NONE, NULL, error);

    if (stream == NULL)
    {
        g_free (path);
        g_object_unref (file);
        return FALSE;
    }

    gsize bytes_written = 0;
    if (!g_output_stream_write_all (G_OUTPUT_STREAM (stream), data, data_length, &bytes_written, NULL, error))
    {
        g_free (path);
        g_object_unref (stream);
        return FALSE;
    }

    if (!g_output_stream_close (G_OUTPUT_STREAM (stream), NULL, error))
    {
        g_free (path);
        g_object_unref (stream);
        return FALSE;
    }

    g_object_unref (stream);

    // Change desktop background
    GConfEngine *conf = gconf_engine_get_default ();
    gchar *value = gconf_engine_get_string (conf, "/desktop/gnome/background/picture_filename", NULL);
    if (!gconf_engine_set_string (conf, "/desktop/gnome/background/picture_filename", path, error))
    {
        g_free (path);
        gconf_engine_unref (conf);
        return FALSE;
    }

    g_free (path);
    gconf_engine_unref (conf);

    return TRUE;
}

/* private methods */
static gchar *
get_path_from_date (GDateTime *now, const gchar *dir)
{
    gchar *filename = g_date_time_format (now, "pod-%F.png");
    gchar *path = g_build_filename (dir, filename, NULL);
    g_free (filename);
    return path;
}
