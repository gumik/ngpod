#include "GtkApplication.h"
#include "GtkPresenter.h"
#include "GtkTimer.h"

namespace ngpod
{

GtkApplication::GtkApplication(int& argc, char**& argv)
    : Application(argc, argv)
{

}


void GtkApplication::InitGui()
{
    g_log_set_default_handler (LogFunc, NULL);
    app = Gtk::Application::create(argc, argv, "gumik.ngpod");
}

int GtkApplication::StartGui()
{
    app->hold();
    return app->run();
}

IPresenter* GtkApplication::CreatePresenter()
{
    return new GtkPresenter;
}

AbstractTimer* GtkApplication::CreateTimer(Watcher& watcher, Settings& settings,
    IPresenter& presenter, AbstractWallpaper& wallpaper)
{
    return new GtkTimer(watcher, settings, presenter, wallpaper);
}

void GtkApplication::LogFunc(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
    ngpod::Logger(log_domain) << message;
}

}