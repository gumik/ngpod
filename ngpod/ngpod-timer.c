#include "ngpod-timer.h"
#include "utils.h"

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NGPOD_TYPE_TIMER, NgpodTimerPrivate))

struct _NgpodTimerPrivate
{
    NgpodWatcher *watcher;
    NgpodSettings *settings;
    NgpodPresenter *presenter;
    NgpodWallpaper *wallpaper;
};

G_DEFINE_TYPE (NgpodTimer, ngpod_timer, G_TYPE_OBJECT);

static const int NOT_NEEDED_TIMEOUT = 60;
static const int FAILED_TIMEOUT = 60;
static const int SUCCESS_TIMEOUT = 5 * 60;
static const int SUCCESS_NO_IMAGE_TIMEOUT = 15 * 60;

static gboolean ngpod_timer_tick (gpointer data);
static void watcher_finished_event (NgpodWatcher *watche, gpointer data);
static void add_timeout (NgpodTimer *self, gint seconds);
static void update_last_date_in_settings (NgpodTimer *self);
static void presenter_made_choice_event (NgpodPresenter *presenter, gpointer data);

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
    g_clear_object (&priv->settings);
    g_clear_object (&priv->presenter);
    g_clear_object (&priv->wallpaper);

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
ngpod_timer_new (
    NgpodWatcher *watcher,
    NgpodSettings *settings,
    NgpodPresenter *presenter,
    NgpodWallpaper *wallpaper)
{
    GObject *object = g_object_new (NGPOD_TYPE_TIMER, NULL);
    NgpodTimerPrivate *priv = GET_PRIVATE (object);

    priv->watcher = watcher;
    g_object_ref (watcher);
    g_signal_connect (watcher, "update-finished", G_CALLBACK (watcher_finished_event), object);

    priv->settings = settings;
    g_object_ref (settings);

    priv->presenter = presenter;
    g_object_ref (presenter);
    g_signal_connect (presenter, "made-choice", G_CALLBACK (presenter_made_choice_event), object);

    priv->wallpaper = wallpaper;
    g_object_ref (wallpaper);

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
    NgpodTimerPrivate *priv = GET_PRIVATE (self);
    NgpodWatcherStatus status = ngpod_watcher_get_status (watcher);

    if (status == NGPOD_WATCHER_STATUS_NOT_NEEDED)
    {
        add_timeout (self, NOT_NEEDED_TIMEOUT);
        log_message ("Timer", "NOT_NEEDED");
    }
    else if (status == NGPOD_WATCHER_STATUS_UPDATED)
    {
        const NgpodDownloader *downloader = ngpod_watcher_get_downloader (watcher);
        NgpodDownloaderStatus downloader_status = ngpod_downloader_get_status (downloader);

        switch (downloader_status)
        {
            case NGPOD_DOWNLOADER_STATUS_FAILED:
                log_message ("Timer", "DOWNLOADER_FAILED");
                add_timeout (self, FAILED_TIMEOUT);
                break;

            case NGPOD_DOWNLOADER_STATUS_FAILED_GET_IMAGE:
                log_message ("Timer", "DOWNLOADER_FAILED_GET_IMAGE");
                add_timeout (self, FAILED_TIMEOUT);
                break;

            case NGPOD_DOWNLOADER_STATUS_SUCCESS_NO_IMAGE:
                log_message ("Timer", "DOWNLOADER_SUCCESS_NO_IMAGE");
                update_last_date_in_settings (self);
                add_timeout (self, SUCCESS_NO_IMAGE_TIMEOUT);
                break;

            case NGPOD_DOWNLOADER_STATUS_SUCCESS:
                log_message ("Timer", "DOWNLOADER_SUCCESS");

                ngpod_presenter_notify (
                    priv->presenter,
                    ngpod_downloader_get_data (downloader),
                    ngpod_downloader_get_data_length (downloader));
                break;
        }
    }
    else
    {
        log_message ("Timer", "unknown NgpodWatcher status");
    }
}

static void
add_timeout (NgpodTimer *self, gint seconds)
{
    g_timeout_add_seconds (seconds, ngpod_timer_tick, (gpointer) self);
}

static void
update_last_date_in_settings (NgpodTimer *self)
{
    NgpodTimerPrivate *priv = GET_PRIVATE (self);

    const GDate *last_date = ngpod_watcher_get_last_date (priv->watcher);
    ngpod_settings_set_last_date (priv->settings, last_date);
}

static void
presenter_made_choice_event (NgpodPresenter *presenter, gpointer data)
{
    NgpodTimer *self = (NgpodTimer *) data;
    NgpodTimerPrivate *priv = GET_PRIVATE (self);

    if (ngpod_presenter_is_accepted (presenter))
    {
        GError *error = NULL;

        const NgpodDownloader *downloader = ngpod_watcher_get_downloader (priv->watcher);
        const char * data = ngpod_downloader_get_data (downloader);
        gsize data_length = ngpod_downloader_get_data_length (downloader);

        if (!ngpod_wallpaper_set_from_data(priv->wallpaper, data, data_length, &error))
        {
            GString *msg = g_string_new ("");
            g_string_printf (msg, "Error while setting wallpaper:\n%s", error->message);
            ngpod_presenter_show_error (priv->presenter, msg->str);
            log_message ("Timer", msg->str);
            g_string_free (msg, TRUE);
            g_error_free (error);
        }
    }

    ngpod_presenter_hide (presenter);
    update_last_date_in_settings (self);
    add_timeout (self, SUCCESS_TIMEOUT);
}