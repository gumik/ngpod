#include "Application.h"

#include <boost/format.hpp>
#include <iostream>
#include <glibmm.h> // to remove

#include "config.h"
#include "Logger.h"

using namespace boost;
using namespace Glib; // to remove
using namespace std;

namespace ngpod
{

Application::Application(int& argc, char**& argv)
    : logger("main"), argc(argc), argv(argv)
{
}

int Application::Run()
{
    InitGui();
    if (!Init()) return 1;
    return StartGui();
}

bool Application::Init()
{
    cout << format("ngpod %1%.%2% - National Geographic Picture of the Day wallpaper downloader")
        % VERSION_MAJOR % VERSION_MINOR
        << endl;

    settings.reset(new Settings());
    settings->Initialize();

    string log_file = settings->GetLogFile();
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

    Date last_date = settings->GetLastDate();
    logger << "Last date: " << last_date.get_year() << "-" << last_date.get_month() << "-" << static_cast<int>(last_date.get_day());

    ustring dir = settings->GetDir();
    if (dir.empty())
    {
        g_print ("Set dir in config file\n");
        return false;
    }
    logger << "Dir: " << dir;

    GTimeSpan time_span = settings->GetTimeSpan();
    logger << "Time span: " << time_span / G_TIME_SPAN_HOUR << "h " << (time_span % G_TIME_SPAN_HOUR) / G_TIME_SPAN_MINUTE << "min";

    downloader.reset(new Downloader());
    watcher.reset(new Watcher(*downloader, last_date, time_span));
    presenter.reset(new Presenter());
    wallpaper.reset(new XfceWallpaper(dir));
    timer.reset(new Timer(*watcher, *settings, *presenter, *wallpaper));

    timer->Start();

    return true;
}

}