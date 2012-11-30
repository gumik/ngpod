#include <boost/format.hpp>
#include <glibmm.h>
#include <iostream>

#include "ngpod-downloader.h"
#include "ngpod-watcher.h"
#include "ngpod-timer.h"
#include "ngpod-settings.h"
#include "ngpod-presenter.h"
#include "XfceWallpaper.h"
#include "config.h"
#include "Logger.h"

using namespace boost;
using namespace Glib;
using namespace ngpod;
using namespace std;

namespace
{

Logger logger("main");

static void
log_func (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
    Logger(log_domain) << message;
}

}

int main (int argc, char **argv)
{
    Glib::RefPtr<Gtk::Application> app =
    Gtk::Application::create(argc, argv,
      "gumik.ngpod");

    cout << format("ngpod %1%.%2% - National Geographic Picture of the Day wallpaper downloader")
        % VERSION_MAJOR % VERSION_MINOR
        << endl;

    g_log_set_default_handler (log_func, NULL);

    Settings settings;
    settings.Initialize();

    string log_file = settings.GetLogFile();
    if (!log_file.empty())
    {
        if (Log::SetFile(log_file))
        {
            logger << "Log file:" << log_file;
        }
        else
        {
            logger << "Cannot log to file: " << log_file;
        }
    }

    Date last_date = settings.GetLastDate();
    logger << "Last date: " << last_date.get_year() << "-" << last_date.get_month() << "-" << static_cast<int>(last_date.get_day());

    ustring dir = settings.GetDir();
    if (dir.empty())
    {
        g_print ("Set dir in config file\n");
        return 1;
    }
    logger << "Dir: " << dir;

    GTimeSpan time_span = settings.GetTimeSpan();
    logger << "Time span: " << time_span / G_TIME_SPAN_HOUR << "h " << (time_span % G_TIME_SPAN_HOUR) / G_TIME_SPAN_MINUTE << "min";

    Downloader downloader;
    Watcher watcher(downloader, last_date, time_span);
    Presenter presenter;
    XfceWallpaper wallpaper(dir);
    ngpod::Timer timer(watcher, settings, presenter, wallpaper);

    timer.Start();

    app->hold();
    app->run();
}