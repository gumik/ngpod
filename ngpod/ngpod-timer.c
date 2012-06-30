#include "ngpod-timer.h"

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NGPOD_TYPE_TIMER, NgpodTimerPrivate))

struct _NgpodTimerPrivate
{
    NgpodWatcher *watcher;
};

G_DEFINE_TYPE (NgpodTimer, ngpod_timer, G_TYPE_OBJECT);

static const int NOT_NEEDED_TIMEOUT = 1;
static const int FAILED_TIMEOUT = 1;
static const int SUCCESS_TIMEOUT = 1;
//static const int SUCCESS_NOT_NEED_TIMEOUT = 1;

static gboolean ngpod_timer_tick (gpointer data);
static void watcher_finished_event (NgpodWatcher *watche, gpointer data);
static void add_timeout (NgpodTimer *self, gint seconds);

static void
ngpod_timer_dispose (GObject *gobject)
{
    NgpodTimer *self = NGPOD_TIMER (gobject);

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
  G_OBJECT_CLASS (ngpod_timer_parent_class)->dispose (gobject);
}

static void
ngpod_timer_finalize (GObject *gobject)
{
    NgpodTimer *self = NGPOD_TIMER (gobject);
    NgpodTimerPrivate *priv = GET_PRIVATE (self);

    g_clear_object (&priv->watcher);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (ngpod_timer_parent_class)->finalize (gobject);
}

static void
ngpod_timer_class_init (NgpodTimerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = ngpod_timer_dispose;
    gobject_class->finalize = ngpod_timer_finalize;

    g_type_class_add_private (klass, sizeof (NgpodTimerPrivate));
}

static void
ngpod_timer_init (NgpodTimer *self)
{
    NgpodTimerPrivate *priv;
    self->priv = priv = GET_PRIVATE (self);
}

NgpodTimer *
ngpod_timer_new (NgpodWatcher *watcher)
{
    GObject *object = g_object_new (NGPOD_TYPE_TIMER, NULL);
    NgpodTimerPrivate *priv = GET_PRIVATE (object);

    priv->watcher = watcher;
    g_object_ref (watcher);
    g_signal_connect (watcher, "update-finished", G_CALLBACK (watcher_finished_event), object);

    return NGPOD_TIMER (object);
}

void
ngpod_timer_start (NgpodTimer *self)
{
    add_timeout (self, 1);
}

/* private functions */
static gboolean
ngpod_timer_tick (gpointer data)
{
    NgpodTimer *self = (NgpodTimer *) data;
    NgpodTimerPrivate *priv = GET_PRIVATE (self);

    GDateTime *now = g_date_time_new_now_local();
    ngpod_watcher_tick (priv->watcher, now);
    g_date_time_unref (now);

    return FALSE;
}

static void
watcher_finished_event (NgpodWatcher *watcher, gpointer data)
{
    NgpodTimer *self = (NgpodTimer *) data;
    NgpodWatcherStatus status = ngpod_watcher_get_status (watcher);

    if (status == NGPOD_WATCHER_STATUS_NOT_NEEDED)
    {
        add_timeout (self, NOT_NEEDED_TIMEOUT);
    }
    else if (status == NGPOD_WATCHER_STATUS_UPDATED)
    {
        const NgpodDownloader *downloader = ngpod_watcher_get_downloader (watcher);
        NgpodDownloaderStatus downloader_status = ngpod_downloader_get_status (downloader);

        switch (downloader_status)
        {
            case NGPOD_DOWNLOADER_STATUS_FAILED:
                add_timeout (self, FAILED_TIMEOUT);
                break;

            case NGPOD_DOWNLOADER_STATUS_FAILED_GET_IMAGE:
                add_timeout (self, FAILED_TIMEOUT);
                break;

            case NGPOD_DOWNLOADER_STATUS_SUCCESS_NO_IMAGE:
                add_timeout (self, SUCCESS_TIMEOUT);
                break;

            case NGPOD_DOWNLOADER_STATUS_SUCCESS:
                // show window
                // add_timeout (self, SUCCESS_TIMEOUT);
                break;
        }
    }
    else
    {
        // log error
    }
}

static void
add_timeout (NgpodTimer *self, gint seconds)
{
    g_timeout_add_seconds (seconds, ngpod_timer_tick, (gpointer) self);
}