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
    NGPOD_WATCHER_STATUS_NOT_NEEDED,              // There is too early to update
    NGPOD_WATCHER_STATUS_FAILED,                  // Failed to download website
    NGPOD_WATCHER_STATUS_SUCCESSFUL,              // Downloading website and image succesful
    NGPOD_WATCHER_STATUS_SUCCESSFUL_NOT_NEEDED,   // Success but image is the same as previous
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
NgpodWatcher *ngpod_watcher_new (NgpodDownloader *downloader, GDate *last_date);

/*
 * Method definitions.
 */
NgpodWatcherStatus ngpod_watcher_tick(NgpodWatcher *self, GDateTime *now);
NgpodWatcherStatus ngpod_watcher_get_status (NgpodWatcher *self);
void ngpod_watcher_set_url (NgpodWatcher *self, const gchar *url);

#endif /* __NGPOD_WATCHER_H__ */