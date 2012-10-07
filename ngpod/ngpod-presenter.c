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
    const char *title;
    const char *description;
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
static GtkBuilder *get_builder (void);
static void status_icon_activate (GtkStatusIcon *icon, gpointer data);
static void destroy_event (GObject *object, gpointer data);
static void button_clicked_event (GtkButton *btn, gpointer data);
static void set_adjusted_description (NgpodPresenter *self, GtkLabel *description_label);

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
    priv->title = NULL;
    priv->description = NULL;
    priv->is_accepted = FALSE;
}

NgpodPresenter *
ngpod_presenter_new (void)
{
    GObject *object = g_object_new (NGPOD_TYPE_PRESENTER, NULL);
    return NGPOD_PRESENTER (object);
}

void
ngpod_presenter_notify (NgpodPresenter *self, const char *data, const gsize data_length, const gchar *title, const gchar *description)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    priv->data = data;
    priv->data_length = data_length;
    priv->title = title;
    priv->description = description;

    ngpod_presenter_show_tray (self);
}

void
ngpod_presenter_hide (NgpodPresenter *self)
{
    ngpod_presenter_hide_window (self);
    ngpod_presenter_hide_tray (self);
}

void
ngpod_presenter_show_error (NgpodPresenter *self, const gchar *msg)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);
    GtkWindow *parent = priv->window != NULL ? GTK_WINDOW (priv->window) : NULL;

    GtkWidget *dialog = gtk_message_dialog_new (
        parent,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_CLOSE,
        "%s", msg);

    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
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
        GtkBuilder *builder = get_builder ();

        priv->window = GTK_WIDGET (gtk_builder_get_object (builder, "picture-dialog"));

        GtkImage *image = GTK_IMAGE (gtk_builder_get_object (builder, "image"));
        GtkLabel *description_label = GTK_LABEL (gtk_builder_get_object (builder, "description-label"));

        priv->accept_btn = GTK_BUTTON (gtk_builder_get_object (builder, "accept_button"));
        priv->deny_btn = GTK_BUTTON (gtk_builder_get_object (builder, "refuse_button"));

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
        g_object_unref (builder);

        set_adjusted_description (self, description_label);
        gtk_window_set_title (GTK_WINDOW (priv->window), priv->title);
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
        GInputStream *stream = g_resources_open_stream ("/gumik/ngpod/NG_Yellow_Frame.png", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);

        gtk_status_icon_set_from_pixbuf (priv->icon, pixbuf);
        g_signal_connect (priv->icon, "activate", G_CALLBACK (status_icon_activate), self);

        g_object_unref (pixbuf);
        g_object_unref (stream);
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

static GtkBuilder *
get_builder ()
{
    GBytes *gui_bytes = g_resources_lookup_data ("/gumik/ngpod/gui.glade", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
    gsize data_length = 0;
    gconstpointer data = g_bytes_get_data (gui_bytes, &data_length);
    GtkBuilder *builder = gtk_builder_new ();
    gtk_builder_add_from_string (builder, data, data_length, NULL);
    g_bytes_unref (gui_bytes);
    gtk_builder_connect_signals (builder, NULL);
    return builder;
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

static void
set_adjusted_description (NgpodPresenter *self, GtkLabel *description_label)
{
    NgpodPresenterPrivate *priv = GET_PRIVATE (self);

    gchar *d1 = g_str_replace (priv->description, "em>", "i>");
    gchar *d2 = g_str_replace (d1, "<br>", "");
    gchar *d3 = g_str_replace (d2, "strong>", "b>");

    gtk_label_set_markup (description_label, d3);

    g_free (d1);
    g_free (d2);
    g_free (d3);
}