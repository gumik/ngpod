#include "GtkApplication.h"



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

void GtkApplication::LogFunc(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
    ngpod::Logger(log_domain) << message;
}

}