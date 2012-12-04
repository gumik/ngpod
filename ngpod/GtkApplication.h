#ifndef __NGPOD_GTKAPPLICATION_H__
#define __NGPOD_GTKAPPLICATION_H__

#include <gtkmm.h>

#include "Application.h"

namespace ngpod
{

class GtkApplication : public Application
{
public:
    GtkApplication(int& argc, char**& argv);

protected:
    /*override*/ void InitGui();
    /*override*/ int StartGui();

private:
    static void LogFunc(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data);

    Glib::RefPtr<Gtk::Application> app;
};

}

#endif /* __NGPOD_GTKAPPLICATION_H__ */