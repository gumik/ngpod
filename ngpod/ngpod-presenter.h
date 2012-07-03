#ifndef __NGPOD_PRESENTER_H__
#define __NGPOD_PRESENTER_H__

#include <glib-object.h>

#define NGPOD_TYPE_PRESENTER                  (ngpod_presenter_get_type ())
#define NGPOD_PRESENTER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), NGPOD_TYPE_PRESENTER, NgpodPresenter))
#define NGPOD_IS_PRESENTER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NGPOD_TYPE_PRESENTER))
#define NGPOD_PRESENTER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), NGPOD_TYPE_PRESENTER, NgpodPresenterClass))
#define NGPOD_IS_PRESENTER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), NGPOD_TYPE_PRESENTER))
#define NGPOD_PRESENTER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), NGPOD_TYPE_PRESENTER, NgpodPresenterClass))

typedef struct _NgpodPresenter        NgpodPresenter;
typedef struct _NgpodPresenterClass   NgpodPresenterClass;
typedef struct _NgpodPresenterPrivate NgpodPresenterPrivate;

struct _NgpodPresenter
{
    GObject          parent_instance;
    NgpodPresenterPrivate *priv;
};

struct _NgpodPresenterClass
{
    GObjectClass parent_class;
};

/* used by NGPOD_TYPE_PRESENTER */
GType ngpod_presenter_get_type (void);

NgpodPresenter *ngpod_presenter_new (void);

/*
 * Method definitions.
 */
void ngpod_presenter_notify (NgpodPresenter *self, const char *data, const gsize data_length);

#endif /* __NGPOD_PRESENTER_H__ */
