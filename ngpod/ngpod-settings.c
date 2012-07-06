#include <gio/gio.h>
#include "ngpod-settings.h"
#include "utils.h"

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NGPOD_TYPE_SETTINGS, NgpodSettingsPrivate))

struct _NgpodSettingsPrivate
{
    GKeyFile *key_file;
};

G_DEFINE_TYPE (NgpodSettings, ngpod_settings, G_TYPE_OBJECT);

static const gchar *SETTINGS_PATH;
static const gchar *SETTINGS_DIR;

static gboolean create_config_dir ();
static void save_settings (NgpodSettings *self);

static void
ngpod_settings_dispose (GObject *gobject)
{
    NgpodSettings *self = NGPOD_SETTINGS (gobject);

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
    G_OBJECT_CLASS (ngpod_settings_parent_class)->dispose (gobject);
}

static void
ngpod_settings_finalize (GObject *gobject)
{
    NgpodSettings *self = NGPOD_SETTINGS (gobject);
    NgpodSettingsPrivate *priv = GET_PRIVATE (self);

    g_key_file_free (priv->key_file);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (ngpod_settings_parent_class)->finalize (gobject);
}

static void
ngpod_settings_class_init (NgpodSettingsClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = ngpod_settings_dispose;
    gobject_class->finalize = ngpod_settings_finalize;

    g_type_class_add_private (klass, sizeof (NgpodSettingsPrivate));

    SETTINGS_DIR = g_build_filename (g_get_user_config_dir (), "gumik", NULL);
    SETTINGS_PATH = g_build_filename (SETTINGS_DIR, "ngpod-gtk.ini", NULL);
}

static void
ngpod_settings_init (NgpodSettings *self)
{
    NgpodSettingsPrivate *priv;
    self->priv = priv = GET_PRIVATE (self);

    priv->key_file = g_key_file_new ();
}

NgpodSettings *
ngpod_settings_new (void)
{
    GObject *object = g_object_new (NGPOD_TYPE_SETTINGS, NULL);
    return NGPOD_SETTINGS (object);
}

gboolean
ngpod_settings_initialize (NgpodSettings *self)
{
    NgpodSettingsPrivate *priv = GET_PRIVATE (self);

    if (!create_config_dir ())
    {
        return FALSE;
    }

    g_key_file_load_from_file (
        priv->key_file,
        SETTINGS_PATH,
        G_KEY_FILE_NONE, NULL);

    return TRUE;
}

void
ngpod_settings_set_last_date (NgpodSettings *self, const GDate *date)
{
    NgpodSettingsPrivate *priv = GET_PRIVATE (self);

    gint list[] = { g_date_get_year (date), g_date_get_month (date), g_date_get_day (date) };
    g_key_file_set_integer_list (priv->key_file, "main", "last_date", list, 3);
    save_settings (self);
}

GDate *
ngpod_settings_get_last_date (NgpodSettings *self)
{
    NgpodSettingsPrivate *priv = GET_PRIVATE (self);

    gsize length = 0;
    gint *list = g_key_file_get_integer_list (priv->key_file, "main", "last_date", &length, NULL);

    GDate *date = NULL;

    if (list == NULL || length < 3)
    {
        date = g_date_new_dmy (1, 1, 1);
    }
    else
    {
        date = g_date_new_dmy (list[2], list[1], list[0]);
        g_free (list);
    }

    return date;
}

void
ngpod_settings_set_dir (NgpodSettings *self, const gchar *dir)
{
    NgpodSettingsPrivate *priv = GET_PRIVATE (self);

    g_key_file_set_string (priv->key_file, "main", "dir", dir);
    save_settings (self);
}

gchar *
ngpod_settings_get_dir (const NgpodSettings *self)
{
    NgpodSettingsPrivate *priv = GET_PRIVATE (self);
    return g_key_file_get_string (priv->key_file, "main", "dir", NULL);
}

gchar *
ngpod_settings_get_log_file (NgpodSettings *self)
{
    NgpodSettingsPrivate *priv = GET_PRIVATE (self);
    return g_key_file_get_string (priv->key_file, "main", "log_file", NULL);
}

/* private functions */

static gboolean
create_config_dir ()
{
    GError *error = NULL;
    GFile *dir = g_file_new_for_path (SETTINGS_DIR);
    g_file_make_directory_with_parents (dir, NULL, &error);

    gboolean status = TRUE;

    if (error != NULL)
    {
        if (error->code != G_IO_ERROR_EXISTS)
        {
            status = FALSE;
            log_message ("Settings", "Cannot create settings directory: %s", error->message);
        }

        g_error_free (error);
    }

    g_object_unref (dir);

    return status;
}

static void
save_settings (NgpodSettings *self)
{
    NgpodSettingsPrivate *priv = GET_PRIVATE (self);
    GError *error = NULL;
    GFile *file = g_file_new_for_path (SETTINGS_PATH);
    GFileOutputStream *os = g_file_replace (file, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &error);

    if (error != NULL)
    {
        log_message ("Settings", "Cannot save settings: %s", error->message);
        g_error_free (error);
    }
    else
    {
        gsize length = 0;
        gchar *data = g_key_file_to_data (priv->key_file, &length, NULL);

        error = NULL;
        gsize bytes_written = 0;
        g_output_stream_write_all (G_OUTPUT_STREAM (os), data, length, &bytes_written, NULL, &error);

        if (error != NULL)
        {
            log_message ("Settings", "Cannot save settings: %s", error->message);
            g_error_free (error);
        }

        if (length != bytes_written)
        {
            log_message ("Settings", "Written %d bytes to settings file while should write %d",
                bytes_written, length);
        }

        g_free (data);
    }

    g_clear_object (&os);
    g_object_unref (file);
}