#ifndef __NGPOD_PRESENTER_H__
#define __NGPOD_PRESENTER_H__

#include <glibmm.h>
#include <gtkmm.h>

namespace ngpod
{

class Presenter
{
public:
    Presenter();
    ~Presenter();

    void Notify(const char* data, gsize data_length, const Glib::ustring& title,
                const Glib::ustring& description);
    void Hide();
    void ShowError(const Glib::ustring& msg);
    bool IsAccepted() const { return is_accepted; }

    Glib::SignalProxy0<void> signal_MadeChoice();

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
    Glib::ustring title;
    Glib::ustring description;
};

}

#endif /* __NGPOD_PRESENTER_H__ */
