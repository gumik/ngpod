#include <gtk/gtk.h>
#include "ngpod-presenter.h"

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NGPOD_TYPE_PRESENTER, NgpodPresenterPrivate))

struct _NgpodPresenterPrivate
{
    GtkWidget *window;
    GtkStatusIcon *icon;

    const char *data;
    gsize data_length;
};

G_DEFINE_TYPE (NgpodPresenter, ngpod_presenter, G_TYPE_OBJECT);

/* private functions declarations */
static void ngpod_presenter_show_window (NgpodPresenter *self);
static void ngpod_presenter_hide_window (NgpodPresenter *self);
static void ngpod_presenter_show_tray (NgpodPresenter *self);
static void ngpod_presenter_hide_tray (NgpodPresenter *self);
static void status_icon_activate (GtkStatusIcon *icon, gpointer data);
static void destroy_event (GObject *object, gpointer data);

static void
ngpod_presenter_dispose (GObject *gobject)
{
    NgpodPresenter *self = NGPOD_PRESENTER (gobject);

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
    G_OBJECT_CLASS (ngpod_presenter_parent_class)->dispose (gobject);
}

static void
ngpod_presenter_finalize (GObject *gobject)
{
    NgpodPresenter *self = NGPOD_PRESENTER (gobject);
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    g_clear_object (&priv->window);
    g_clear_object (&priv->icon);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (ngpod_presenter_parent_class)->finalize (gobject);
}

static void
ngpod_presenter_class_init (NgpodPresenterClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = ngpod_presenter_dispose;
    gobject_class->finalize = ngpod_presenter_finalize;

    g_type_class_add_private (klass, sizeof (NgpodPresenterPrivate));
}

static void
ngpod_presenter_init (NgpodPresenter *self)
{
    NgpodPresenterPrivate *priv;
    self->priv = priv = GET_PRIVATE (self);

    priv->window = NULL;
    priv->icon = NULL;
    priv->data = NULL;
    priv->data_length = 0;
}

NgpodPresenter *
ngpod_presenter_new (void)
{
    GObject *object = g_object_new (NGPOD_TYPE_PRESENTER, NULL);
    return NGPOD_PRESENTER (object);
}

void
ngpod_presenter_notify (NgpodPresenter *self, const char *data, const gsize data_length)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    priv->data = data;
    priv->data_length = data_length;

    ngpod_presenter_show_tray (self);
}

/* private functions */

void
ngpod_presenter_show_window (NgpodPresenter *self)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    if (priv->window == NULL)
    {
        GtkBuilder *builder = gtk_builder_new ();
        gtk_builder_add_from_file (builder, "gui.glade", NULL);

        priv->window = GTK_WIDGET (gtk_builder_get_object (builder, "picture-dialog"));
        gtk_builder_connect_signals (builder, NULL);
        g_object_unref (builder);

        g_signal_connect (priv->window, "destroy", G_CALLBACK (destroy_event), &priv->window);

        GtkVBox *dialog_vbox = GTK_VBOX (gtk_bin_get_child (GTK_BIN (priv->window)));
        GList *children = gtk_container_get_children (GTK_CONTAINER (dialog_vbox));

        GtkImage *image = GTK_IMAGE (g_list_first (children)->data);

        GInputStream *stream = g_memory_input_stream_new_from_data (priv->data, priv->data_length, NULL);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);
        gtk_image_set_from_pixbuf (image, pixbuf);

        g_clear_object (&stream);
        g_clear_object (&pixbuf);
    }

    gtk_widget_set_visible (priv->window, TRUE);
}
void
ngpod_presenter_hide_window (NgpodPresenter *self)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    g_clear_object (&priv->window);
}

void
ngpod_presenter_show_tray (NgpodPresenter *self)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    if (priv->icon == NULL)
    {
        priv->icon = gtk_status_icon_new ();
        gtk_status_icon_set_from_file (priv->icon, "NG_Yellow_Frame.png");
        g_signal_connect (priv->icon, "activate", G_CALLBACK (status_icon_activate), self);
    }

    gtk_status_icon_set_visible(priv->icon, TRUE);
}

void
ngpod_presenter_hide_tray (NgpodPresenter *self)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    g_clear_object (&priv->icon);
}

void
status_icon_activate (GtkStatusIcon *icon, gpointer data)
{
    NgpodPresenter *self = (NgpodPresenter *) data;
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    ngpod_presenter_show_window (self);
}

void
destroy_event (GObject *object, gpointer data)
{
    GObject **pointer = (GObject **) data;
    *pointer = NULL;
}