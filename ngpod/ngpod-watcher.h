#ifndef __NGPOD_WATCHER_H__
#define __NGPOD_WATCHER_H__

#include <glib-object.h>
#include "ngpod-downloader.h"

#define NGPOD_TYPE_WATCHER                  (ngpod_watcher_get_type ())
#define NGPOD_WATCHER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), NGPOD_TYPE_WATCHER, NgpodWatcher))
#define NGPOD_IS_WATCHER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NGPOD_TYPE_WATCHER))
#define NGPOD_WATCHER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), NGPOD_TYPE_WATCHER, NgpodWatcherClass))
#define NGPOD_IS_WATCHER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), NGPOD_TYPE_WATCHER))
#define NGPOD_WATCHER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), NGPOD_TYPE_WATCHER, NgpodWatcherClass))

typedef struct _NgpodWatcher        NgpodWatcher;
typedef struct _NgpodWatcherClass   NgpodWatcherClass;
typedef struct _NgpodWatcherPrivate NgpodWatcherPrivate;

typedef enum
{
    NGPOD_WATCHER_STATUS_INVALID,
    NGPOD_WATCHER_STATUS_NOT_NEEDED,  // There is too early to update
    NGPOD_WATCHER_STATUS_UPDATED      // Downloader was run to get new image
} NgpodWatcherStatus;

struct _NgpodWatcher
{
    GObject          parent_instance;
    NgpodWatcherPrivate *priv;
};

struct _NgpodWatcherClass
{
    GObjectClass parent_class;
};

/* used by NGPOD_TYPE_WATCHER */
GType ngpod_watcher_get_type (void);

/* Creates new instance of watcher.
 * watcher takes responsibility of freeing last_date */
NgpodWatcher *ngpod_watcher_new (NgpodDownloader *downloader, GDate *last_date, GTimeSpan time_span);

/*
 * Method definitions.
 */
NgpodWatcherStatus ngpod_watcher_tick (NgpodWatcher *self, GDateTime *now);
NgpodWatcherStatus ngpod_watcher_get_status (const NgpodWatcher *self);
const NgpodDownloader *ngpod_watcher_get_downloader (const NgpodWatcher *self);
void ngpod_watcher_set_url (NgpodWatcher *self, const gchar *url);
const GDate *ngpod_watcher_get_last_date (const NgpodWatcher *self);

#endif /* __NGPOD_WATCHER_H__ */
