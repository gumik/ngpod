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

typedef enum
{
	NGPOD_DOWNLOADER_STATUS_SUCCESS,
	NGPOD_DOWNLOADER_STATUS_SUCCESS_NO_IMAGE,
	NGPOD_DOWNLOADER_STATUS_FAILED,
	NGPOD_DOWNLOADER_STATUS_FAILED_GET_IMAGE
} NgpodDownloaderStatus;

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
 * Static fields
 */
const gchar *NGPOD_DOWNLOADER_DEFAULT_URL;

/*
 * Method definitions.
 */
void ngpod_downloader_start (NgpodDownloader *self, const gchar *url);
const GDate* ngpod_downloader_get_date (const NgpodDownloader *self);
const gchar* ngpod_downloader_get_link (const NgpodDownloader *self);
const gchar* ngpod_downloader_get_resolution (NgpodDownloader *self);
const char* ngpod_downloader_get_data (const NgpodDownloader *self);
gsize ngpod_downloader_get_data_length (const NgpodDownloader *self);
const gchar *ngpod_downloader_get_title (const NgpodDownloader *self);
const gchar *ngpod_downloader_get_description (const NgpodDownloader *self);
gboolean ngpod_downloader_is_success (const NgpodDownloader *self);
NgpodDownloaderStatus ngpod_downloader_get_status (const NgpodDownloader *self);

#endif /* __NGPOD_DOWNLOADER_H__ */
