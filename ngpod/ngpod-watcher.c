#include "ngpod-watcher.h"
#include "utils.h"

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NGPOD_TYPE_WATCHER, NgpodWatcherPrivate))

enum
{
    UPDATE_FINISHED,
    SIGNALS_NUM
};

struct _NgpodWatcherPrivate
{
    NgpodDownloader *downloader;
    GDate *last_date;
    NgpodWatcherStatus status;
    const gchar *url;
};

static guint signals[SIGNALS_NUM];

G_DEFINE_TYPE (NgpodWatcher, ngpod_watcher, G_TYPE_OBJECT);

static void download_finished_event(NgpodDownloader *downloader, gpointer data);
static gboolean is_download_needed (NgpodWatcher *self, GDateTime *now);
static void update_last_date (NgpodWatcher *self, const GDate *date);

static void
ngpod_watcher_dispose (GObject *gobject)
{
    NgpodWatcher *self = NGPOD_WATCHER (gobject);
    NgpodWatcherPrivate *priv = GET_PRIVATE (self);

    g_clear_object (&priv->downloader);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (ngpod_watcher_parent_class)->dispose (gobject);
}

static void
ngpod_watcher_finalize (GObject *gobject)
{
    NgpodWatcher *self = NGPOD_WATCHER (gobject);
    NgpodWatcherPrivate *priv = GET_PRIVATE (self);

    g_date_free (priv->last_date);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (ngpod_watcher_parent_class)->finalize (gobject);
}

static void
ngpod_watcher_class_init (NgpodWatcherClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = ngpod_watcher_dispose;
    gobject_class->finalize = ngpod_watcher_finalize;

    g_type_class_add_private (klass, sizeof (NgpodWatcherPrivate));

    /* signals */
    signals[UPDATE_FINISHED] = create_void_void_signal ("update-finished");
}

static void
ngpod_watcher_init (NgpodWatcher *self)
{
    NgpodWatcherPrivate *priv;
    self->priv = priv = GET_PRIVATE (self);

    priv->url = NGPOD_DOWNLOADER_DEFAULT_URL;
    priv->downloader = NULL;
    priv->last_date = NULL;
    priv->status = NGPOD_WATCHER_STATUS_INVALID;
}

NgpodWatcher *
ngpod_watcher_new (NgpodDownloader *downloader, GDate *last_date)
{
    GObject *object = g_object_new (NGPOD_TYPE_WATCHER, NULL);
    NgpodWatcherPrivate *priv = GET_PRIVATE (object);

    priv->downloader = downloader;
    priv->last_date = last_date;

    g_object_ref (priv->downloader);

    g_signal_connect (priv->downloader, "download-finished", G_CALLBACK (download_finished_event), object);

    return NGPOD_WATCHER (object);
}

/* public methods */

NgpodWatcherStatus
ngpod_watcher_tick(NgpodWatcher *self, GDateTime *now)
{
    NgpodWatcherPrivate *priv = GET_PRIVATE (self);

    if (is_download_needed(self, now))
    {
        priv->status = NGPOD_WATCHER_STATUS_INVALID;
        ngpod_downloader_start (priv->downloader, priv->url);
    }
    else
    {
        priv->status = NGPOD_WATCHER_STATUS_NOT_NEEDED;
        g_signal_emit (self, signals[UPDATE_FINISHED], 0);
    }

    return priv->status;
}

NgpodWatcherStatus
ngpod_watcher_get_status (NgpodWatcher *self)
{
    NgpodWatcherPrivate *priv = GET_PRIVATE (self);
    return priv->status;
}

void
ngpod_watcher_set_url (NgpodWatcher *self, const gchar *url)
{
    NgpodWatcherPrivate *priv = GET_PRIVATE (self);
    priv->url = url;
}

/* private functions */

static void
download_finished_event (NgpodDownloader *downloader, gpointer data)
{
    NgpodWatcher *self = data;
    NgpodWatcherPrivate *priv = GET_PRIVATE (self);

    NgpodWatcherStatus status = NGPOD_WATCHER_STATUS_INVALID;
    NgpodDownloaderStatus downloader_status = ngpod_downloader_get_status (downloader);
    const GDate *date = ngpod_downloader_get_date (downloader);

    switch (downloader_status)
    {
        case NGPOD_DOWNLOADER_STATUS_SUCCESS:
            if (g_date_compare (date, priv->last_date) <= 0)
            {
                status = NGPOD_WATCHER_STATUS_SUCCESSFUL_NOT_NEEDED;
            }
            else
            {
                status = NGPOD_WATCHER_STATUS_SUCCESSFUL;
                update_last_date (self, date);
            }
            break;

        case NGPOD_DOWNLOADER_STATUS_SUCCESS_NO_IMAGE:
            update_last_date (self, date);
            status = NGPOD_WATCHER_STATUS_SUCCESSFUL_NOT_NEEDED;
            break;

        case NGPOD_DOWNLOADER_STATUS_FAILED:
        case NGPOD_DOWNLOADER_STATUS_FAILED_GET_IMAGE:
            status = NGPOD_WATCHER_STATUS_FAILED;
            break;
    }

    priv->status = status;
    g_signal_emit (self, signals[UPDATE_FINISHED], 0);
}

static gboolean
is_download_needed(NgpodWatcher *self, GDateTime *now)
{
    NgpodWatcherPrivate *priv = GET_PRIVATE (self);

    GDate *last_date = priv->last_date;

    gint last_year = g_date_get_year (last_date);
    gint last_month = g_date_get_month (last_date);
    gint last_day = g_date_get_day (last_date);

    GDateTime *last_date_time = g_date_time_new_local (last_year, last_month, last_day, 0, 0, 0);
    GDateTime *last_date_time_added = g_date_time_add_days (last_date_time, 1);

    GTimeSpan diff = g_date_time_difference (last_date_time_added, now);

    g_date_time_unref (last_date_time);
    g_date_time_unref (last_date_time_added);

    return diff <= 0;
}

static void
update_last_date (NgpodWatcher *self, const GDate *date)
{
    NgpodWatcherPrivate *priv = GET_PRIVATE (self);

    g_date_set_dmy (
        priv->last_date,
        g_date_get_day (date),
        g_date_get_month (date),
        g_date_get_year (date));
}