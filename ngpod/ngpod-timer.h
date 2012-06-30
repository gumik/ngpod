#ifndef __NGPOD_TIMER_H__
#define __NGPOD_TIMER_H__

#include <glib-object.h>
#include "ngpod-watcher.h"

#define NGPOD_TYPE_TIMER                  (ngpod_timer_get_type ())
#define NGPOD_TIMER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), NGPOD_TYPE_TIMER, NgpodTimer))
#define NGPOD_IS_TIMER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NGPOD_TYPE_TIMER))
#define NGPOD_TIMER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), NGPOD_TYPE_TIMER, NgpodTimerClass))
#define NGPOD_IS_TIMER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), NGPOD_TYPE_TIMER))
#define NGPOD_TIMER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), NGPOD_TYPE_TIMER, NgpodTimerClass))

typedef struct _NgpodTimer        NgpodTimer;
typedef struct _NgpodTimerClass   NgpodTimerClass;
typedef struct _NgpodTimerPrivate NgpodTimerPrivate;

struct _NgpodTimer
{
    GObject          parent_instance;
    NgpodTimerPrivate *priv;
};

struct _NgpodTimerClass
{
    GObjectClass parent_class;
};

/* used by NGPOD_TYPE_TIMER */
GType ngpod_timer_get_type (void);

NgpodTimer *ngpod_timer_new (NgpodWatcher *watcher);

/*
 * Method definitions.
 */
void ngpod_timer_start (NgpodTimer *self);

#endif /* __NGPOD_TIMER_H__ */
