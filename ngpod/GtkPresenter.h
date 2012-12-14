#ifndef __NGPOD_GTKPRESENTER_H__
#define __NGPOD_GTKPRESENTER_H__

#include <glibmm.h>
#include <gtkmm.h>

#include "IPresenter.h"
#include "Logger.h"

namespace ngpod
{

class GtkPresenter : public IPresenter
{
public:
    GtkPresenter();
    ~GtkPresenter();

    /*override*/ void Notify(const char* data, int data_length, const std::string& title,
                const std::string& description);
    /*override*/ void Hide();
    /*override*/ void ShowError(const std::string& msg);
    /*override*/ bool IsAccepted() const { return is_accepted; }

private:
    void ShowWindow ();
    void HideWindow ();
    void ShowTray ();
    void HideTray ();
    Glib::RefPtr<Gtk::Builder> GetBuilder();
    void StatusIconActivateCallback ();
    // void DestroyEvent ();
    void AcceptButtonClickedCallback ();
    void DenyButtonClickedCallback ();
    void LabelSizeAllocatedCallback (Gtk::Allocation& allocation);
    void SetAdjustedDescription (Glib::RefPtr<Gtk::Label> description_label);

    Glib::RefPtr<Gtk::Window> window;
    Glib::RefPtr<Gtk::StatusIcon> icon;
    Glib::RefPtr<Gtk::Button> accept_btn;
    Glib::RefPtr<Gtk::Button> deny_btn;

    bool is_accepted;

    const char *data;
    gsize data_length;
    std::string title;
    std::string description;
    Logger logger;
};

}

#endif /* __NGPOD_GTKPRESENTER_H__ */
