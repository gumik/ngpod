#ifndef __NGPOD_SETTINGS_H__
#define __NGPOD_SETTINGS_H__

#include <glib-object.h>

#define NGPOD_TYPE_SETTINGS                  (ngpod_settings_get_type ())
#define NGPOD_SETTINGS(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), NGPOD_TYPE_SETTINGS, NgpodSettings))
#define NGPOD_IS_SETTINGS(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NGPOD_TYPE_SETTINGS))
#define NGPOD_SETTINGS_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), NGPOD_TYPE_SETTINGS, NgpodSettingsClass))
#define NGPOD_IS_SETTINGS_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), NGPOD_TYPE_SETTINGS))
#define NGPOD_SETTINGS_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), NGPOD_TYPE_SETTINGS, NgpodSettingsClass))

typedef struct _NgpodSettings        NgpodSettings;
typedef struct _NgpodSettingsClass   NgpodSettingsClass;
typedef struct _NgpodSettingsPrivate NgpodSettingsPrivate;

struct _NgpodSettings
{
    GObject          parent_instance;
    NgpodSettingsPrivate *priv;
};

struct _NgpodSettingsClass
{
    GObjectClass parent_class;
};

/* used by NGPOD_TYPE_SETTINGS */
GType ngpod_settings_get_type (void);

NgpodSettings *ngpod_settings_new (void);

/*
 * Method definitions.
 */
gboolean ngpod_settings_initialize (NgpodSettings *self);

void ngpod_settings_set_last_date (NgpodSettings *self, const GDate *date);
GDate *ngpod_settings_get_last_date (NgpodSettings *self);

void ngpod_settings_set_dir (NgpodSettings *self, const gchar *dir);
gchar *ngpod_settings_get_dir (const NgpodSettings *self);

gchar *ngpod_settings_get_log_file (NgpodSettings *self);

GTimeSpan ngpod_settings_get_time_span (NgpodSettings *self);

#endif /* __NGPOD_SETTINGS_H__ */
