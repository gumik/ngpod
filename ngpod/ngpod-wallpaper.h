#ifndef __NGPOD_WALLPAPER_H__
#define __NGPOD_WALLPAPER_H__

#include <glib-object.h>

#define NGPOD_TYPE_WALLPAPER                  (ngpod_wallpaper_get_type ())
#define NGPOD_WALLPAPER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), NGPOD_TYPE_WALLPAPER, NgpodWallpaper))
#define NGPOD_IS_WALLPAPER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NGPOD_TYPE_WALLPAPER))
#define NGPOD_WALLPAPER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), NGPOD_TYPE_WALLPAPER, NgpodWallpaperClass))
#define NGPOD_IS_WALLPAPER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), NGPOD_TYPE_WALLPAPER))
#define NGPOD_WALLPAPER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), NGPOD_TYPE_WALLPAPER, NgpodWallpaperClass))

typedef struct _NgpodWallpaper        NgpodWallpaper;
typedef struct _NgpodWallpaperClass   NgpodWallpaperClass;
typedef struct _NgpodWallpaperPrivate NgpodWallpaperPrivate;

struct _NgpodWallpaper
{
    GObject          parent_instance;
    NgpodWallpaperPrivate *priv;
};

struct _NgpodWallpaperClass
{
    GObjectClass parent_class;
};

/* used by NGPOD_TYPE_WALLPAPER */
GType ngpod_wallpaper_get_type (void);

NgpodWallpaper *ngpod_wallpaper_new (gchar *dir);

/*
 * Method definitions.
 */
gboolean ngpod_wallpaper_set_from_data (NgpodWallpaper *self, const char *data, gsize data_length, GError **error);

#endif /* __NGPOD_WALLPAPER_H__ */
