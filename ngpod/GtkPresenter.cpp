#include "GtkPresenter.h"

#include <gtk/gtk.h>
#include <glib.h>

#include "utils.h"

using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace Gio;
using namespace std;

namespace ngpod
{

GtkPresenter::GtkPresenter()
    : logger("GtkPresenter")
{
    data = NULL;
    data_length = 0;
    is_accepted = FALSE;
}

GtkPresenter::~GtkPresenter()
{
    window.reset();
    icon.reset();
}

void GtkPresenter::Notify (const char *data, int data_length, const string& title, const string& description)
{
    this->data = data;
    this->data_length = data_length;
    this->title = title;
    this->description = description;

    ShowTray ();
}

void GtkPresenter::Hide ()
{
    HideWindow ();
    HideTray ();
}

void GtkPresenter::ShowError (const string& msg)
{
    Gtk::Dialog dialog(msg, window);
    dialog.run();
}

RefPtr<Builder> GtkPresenter::GetBuilder ()
{
    GBytes *gui_bytes = g_resources_lookup_data ("/gumik/ngpod/gui.glade", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
    gsize data_length = 0;
    gconstpointer data = g_bytes_get_data (gui_bytes, &data_length);
    RefPtr<Builder> builder = Builder::create();
    string resources_string(static_cast<const char*>(data));
    builder->add_from_string(resources_string);
    g_bytes_unref (gui_bytes);
    return builder;
}

void GtkPresenter::ShowWindow ()
{
    if (!window)
    {
        RefPtr<Builder> builder = GetBuilder ();

        window = RefPtr<Window>::cast_static(builder->get_object("picture-dialog"));

        RefPtr<Image> image = RefPtr<Image>::cast_static(builder->get_object("image"));
        RefPtr<Label> description_label = RefPtr<Label>::cast_static(builder->get_object("description-label"));

        accept_btn = RefPtr<Button>::cast_static(builder->get_object("accept_button"));
        deny_btn = RefPtr<Button>::cast_static(builder->get_object("refuse_button"));

        // g_signal_connect (priv->window, "destroy", G_CALLBACK (destroy_event), &priv->window);
        accept_btn->signal_clicked().connect(mem_fun(*this, &GtkPresenter::AcceptButtonClickedCallback));
        deny_btn->signal_clicked().connect(mem_fun(*this, &GtkPresenter::DenyButtonClickedCallback));
        // description_label->signal_size_allocate().connect(mem_fun(*this, &GtkPresenter::LabelSizeAllocatedCallback));

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

void GtkPresenter::HideWindow ()
{
    if (window)
    {
        window->set_visible(false);
        window.reset();
    }
}

void GtkPresenter::ShowTray ()
{
    if (!icon)
    {
        icon = StatusIcon::create("ngpod");
        GInputStream *stream = g_resources_open_stream ("/gumik/ngpod/NG_Yellow_Frame.png", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, NULL);

        gtk_status_icon_set_from_pixbuf (icon->gobj(), pixbuf);
        icon->signal_activate().connect(mem_fun(*this, &GtkPresenter::StatusIconActivateCallback));

        g_object_unref (pixbuf);
        g_object_unref (stream);
    }

    icon->set_visible(true);
}

void GtkPresenter::HideTray ()
{
    if (icon)
    {
        icon->set_visible(false);
        icon.reset();
    }
}

void GtkPresenter::StatusIconActivateCallback ()
{
    ShowWindow();
}

void GtkPresenter::AcceptButtonClickedCallback ()
{
    is_accepted = true;
    signal_MadeChoice();
}

void GtkPresenter::DenyButtonClickedCallback ()
{
    is_accepted = false;
    signal_MadeChoice();
}

// void GtkPresenter::LabelSizeAllocatedCallback (Gtk::Allocation& allocation)
// {

//     gtk_widget_set_size_request (GTK_WIDGET (label), allocation->width, -1);
// }

void GtkPresenter::SetAdjustedDescription (RefPtr<Label> description_label)
{
    string d1 = StrReplace(description, "em>", "i>");
    string d2 = StrReplace (d1, "<br>", "");
    string d3 = StrReplace (d2, "strong>", "b>");

    logger << "Adjusted description: " << d3;

    description_label->set_markup(d3);
}

}