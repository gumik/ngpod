#include <gtk/gtk.h>
#include "ngpod-presenter.h"
#include "utils.h"

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NGPOD_TYPE_PRESENTER, NgpodPresenterPrivate))

struct _NgpodPresenterPrivate
{
    GtkWidget *window;
    GtkStatusIcon *icon;
    GtkButton *accept_btn;
    GtkButton *deny_btn;

    gboolean is_accepted;

    const char *data;
    gsize data_length;
};

enum
{
    MADE_CHOICE,
    SIGNALS_NUM
};

static guint signals[SIGNALS_NUM];

G_DEFINE_TYPE (NgpodPresenter, ngpod_presenter, G_TYPE_OBJECT);

/* private functions declarations */
static void ngpod_presenter_show_window (NgpodPresenter *self);
static void ngpod_presenter_hide_window (NgpodPresenter *self);
static void ngpod_presenter_show_tray (NgpodPresenter *self);
static void ngpod_presenter_hide_tray (NgpodPresenter *self);
static void status_icon_activate (GtkStatusIcon *icon, gpointer data);
static void destroy_event (GObject *object, gpointer data);
static void button_clicked_event (GtkButton *btn, gpointer data);

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

    signals[MADE_CHOICE] = create_void_void_signal ("made-choice");

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
    priv->is_accepted = FALSE;
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

void
ngpod_presenter_hide (NgpodPresenter *self)
{
    ngpod_presenter_hide_window (self);
    ngpod_presenter_hide_tray (self);
}

gboolean
ngpod_presenter_is_accepted (NgpodPresenter *self)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);
    return priv->is_accepted;
}

/* private functions */

static void
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

        GtkVBox *dialog_vbox = GTK_VBOX (gtk_bin_get_child (GTK_BIN (priv->window)));
        GList *children = gtk_container_get_children (GTK_CONTAINER (dialog_vbox));

        GtkImage *image = GTK_IMAGE (g_list_first (children)->data);

        GtkHButtonBox *button_box = GTK_HBUTTON_BOX (g_list_first (children)->next->data);
        children = gtk_container_get_children (GTK_CONTAINER (button_box));
        priv->accept_btn = GTK_BUTTON (g_list_first (children)->data);
        priv->deny_btn = GTK_BUTTON (g_list_first (children)->next->data);

        g_signal_connect (priv->window, "destroy", G_CALLBACK (destroy_event), &priv->window);
        g_signal_connect (priv->accept_btn, "clicked", G_CALLBACK (button_clicked_event), self);
        g_signal_connect (priv->deny_btn, "clicked", G_CALLBACK (button_clicked_event), self);

        GInputStream *stream = g_memory_input_stream_new_from_data (priv->data, priv->data_length, NULL);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);
        int width = gdk_pixbuf_get_width (pixbuf);
        int height = gdk_pixbuf_get_height (pixbuf);
        gdouble factor = (gdouble) width / height;
        int dest_width = 640;
        int dest_height = (int) (640 / factor);
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, dest_width, dest_height, GDK_INTERP_HYPER);
        gtk_image_set_from_pixbuf (image, scaled_pixbuf);

        g_clear_object (&stream);
        g_clear_object (&pixbuf);
        g_clear_object (&scaled_pixbuf);
    }

    gtk_widget_set_visible (priv->window, TRUE);
}

static void
ngpod_presenter_hide_window (NgpodPresenter *self)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    if (priv->window != NULL)
    {
        gtk_widget_destroy (priv->window);
        priv->window = NULL;
    }
}

static void
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

static void
ngpod_presenter_hide_tray (NgpodPresenter *self)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    if (priv->icon != NULL)
    {
        gtk_status_icon_set_visible (priv->icon, FALSE);
        g_object_unref (priv->icon);
        priv->icon = NULL;
    }
}

static void
status_icon_activate (GtkStatusIcon *icon, gpointer data)
{
    NgpodPresenter *self = (NgpodPresenter *) data;
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    ngpod_presenter_show_window (self);
}

static void
destroy_event (GObject *object, gpointer data)
{
    GObject **pointer = (GObject **) data;
    *pointer = NULL;
}

static void
button_clicked_event (GtkButton *btn, gpointer data)
{
    NgpodPresenter *self = (NgpodPresenter *) data;
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    priv->is_accepted = (btn == priv->accept_btn);

    g_signal_emit (self, signals[MADE_CHOICE], 0);
}