#ifndef __NGPOD_DOWNLOADER_H__
#define __NGPOD_DOWNLOADER_H__

#include <glib-object.h>

#define NGPOD_TYPE_DOWNLOADER                  (ngpod_downloader_get_type ())
#define NGPOD_DOWNLOADER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), NGPOD_TYPE_DOWNLOADER, NgpodDownloader))
#define NGPOD_IS_DOWNLOADER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NGPOD_TYPE_DOWNLOADER))
#define NGPOD_DOWNLOADER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), NGPOD_TYPE_DOWNLOADER, NgpodDownloaderClass))
#define NGPOD_IS_DOWNLOADER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), NGPOD_TYPE_DOWNLOADER))
#define NGPOD_DOWNLOADER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), NGPOD_TYPE_DOWNLOADER, NgpodDownloaderClass))

typedef struct _NgpodDownloader        NgpodDownloader;
typedef struct _NgpodDownloaderClass   NgpodDownloaderClass;
typedef struct _NgpodDownloaderPrivate NgpodDownloaderPrivate;

struct _NgpodDownloader
{
    GObject          parent_instance;
    NgpodDownloaderPrivate *priv;
};

struct _NgpodDownloaderClass
{
    GObjectClass parent_class;
};

/* used by NGPOD_TYPE_DOWNLOADER */
GType ngpod_downloader_get_type (void);
NgpodDownloader *ngpod_downloader_new (void);

/*
 * Method definitions.
 */
void ngpod_downloader_start (NgpodDownloader *self);
const GDate* ngpod_downloader_get_date (NgpodDownloader *self);
const gchar* ngpod_downloader_get_link (NgpodDownloader *self);
const gchar* ngpod_downloader_get_resolution (NgpodDownloader *self);
const char* ngpod_downloader_get_data (NgpodDownloader *self);
gsize ngpod_downloader_get_data_length (NgpodDownloader *self);
gboolean ngpod_downloader_is_success (NgpodDownloader *self);

#endif /* __NGPOD_DOWNLOADER_H__ */
