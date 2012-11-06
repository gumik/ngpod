#include <gtk/gtk.h>
#include <glib.h>
#include "ngpod-presenter.h"
#include "utils.h"

using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace Gio;

namespace ngpod
{

Presenter::Presenter()
{
    data = NULL;
    data_length = 0;
    is_accepted = FALSE;
}

Presenter::~Presenter()
{
    window.reset();
    icon.reset();
}

void Presenter::Notify (const char *data, gsize data_length, const ustring& title, const ustring& description)
{
    this->data = data;
    this->data_length = data_length;
    this->title = title;
    this->description = description;

    ShowTray ();
}

void Presenter::Hide ()
{
    HideWindow ();
    HideTray ();
}

void Presenter::ShowError (const ustring& msg)
{
    Gtk::Dialog dialog(msg, window);
    dialog.run();
}

RefPtr<Builder> Presenter::GetBuilder ()
{
    GBytes *gui_bytes = g_resources_lookup_data ("/gumik/ngpod/gui.glade", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
    gsize data_length = 0;
    gconstpointer data = g_bytes_get_data (gui_bytes, &data_length);
    RefPtr<Builder> builder = Builder::create();
    ustring resources_string(static_cast<const char*>(data), data_length);
    builder->add_from_string(resources_string);
    g_bytes_unref (gui_bytes);
    return builder;
}

void Presenter::ShowWindow ()
{
    if (!window)
    {
        RefPtr<Builder> builder = GetBuilder ();

        window = builder->get_object("picture-dialog");

        RefPtr<Image> image = builder->get_object("image");
        RefPtr<Label> description_label = builder->get_object("description_label");

        accept_btn = builder->get_object("accept_button");
        deny_btn = builder->get_object("refuse_button");

        // g_signal_connect (priv->window, "destroy", G_CALLBACK (destroy_event), &priv->window);
        accept_btn->signal_clicked().connect(mem_fun(*this, &Presenter::AcceptButtonClickedCallback));
        deny_btn->signal_clicked().connect(mem_fun(*this, &Presenter::DenyButtonClickedCallback));
        // description_label->signal_size_allocate().connect(mem_fun(*this, &Presenter::LabelSizeAllocatedCallback));

        GInputStream *stream = g_memory_input_stream_new_from_data (data, data_length, NULL);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);
        int width = gdk_pixbuf_get_width (pixbuf);
        int height = gdk_pixbuf_get_height (pixbuf);
        gdouble factor = (gdouble) width / height;
        int dest_width = 640;
        int dest_height = (int) (640 / factor);
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, dest_width, dest_height, GDK_INTERP_HYPER);
        gtk_image_set_from_pixbuf (image->gobj(), scaled_pixbuf);

        g_clear_object (&stream);
        g_clear_object (&pixbuf);
        g_clear_object (&scaled_pixbuf);

        SetAdjustedDescription(description_label);
        window->set_title(title);
    }

    window->set_visible(true);
}

void Presenter::HideWindow ()
{
    if (window)
    {
        window.reset();
    }
}

void Presenter::ShowTray ()
{
    if (!icon)
    {
        icon = StatusIcon::create("ngpod");
        GInputStream *stream = g_resources_open_stream ("/gumik/ngpod/NG_Yellow_Frame.png", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);

        gtk_status_icon_set_from_pixbuf (icon->gobj(), pixbuf);
        icon->signal_activate().connect(mem_fun(*this, &Presenter::StatusIconActivateCallback));

        g_object_unref (pixbuf);
        g_object_unref (stream);
    }

    icon->set_visible(true);
}

void Presenter::HideTray ()
{
    if (icon)
    {
        icon->set_visible(false);
        icon.reset();
    }
}

void Presenter::StatusIconActivateCallback ()
{
    ShowWindow();
}

void Presenter::AcceptButtonClickedCallback ()
{
    is_accepted = true;
    signal_MadeChoice();
}

void Presenter::DenyButtonClickedCallback ()
{
    is_accepted = false;
    signal_MadeChoice();
}

// void Presenter::LabelSizeAllocatedCallback (Gtk::Allocation& allocation)
// {

//     gtk_widget_set_size_request (GTK_WIDGET (label), allocation->width, -1);
// }

void Presenter::SetAdjustedDescription (RefPtr<Label> description_label)
{
    char *d1 = g_str_replace (description.c_str(), "em>", "i>");
    char *d2 = g_str_replace (d1, "<br>", "");
    char *d3 = g_str_replace (d2, "strong>", "b>");

    description_label->set_markup(d3);

    g_free (d1);
    g_free (d2);
    g_free (d3);
}

}