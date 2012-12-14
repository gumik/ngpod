#include "Application.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <iostream>

#include "config.h"
#include "Logger.h"

using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;
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

    date last_date = settings->GetLastDate();
    logger << "Last date: " << last_date.year() << "-" << last_date.month() << "-" << last_date.day();

    string dir = settings->GetDir();
    if (dir.empty())
    {
        cout << "Set dir in config file" << endl;
        return false;
    }
    logger << "Dir: " << dir;

    time_duration time_span = settings->GetTimeSpan();
    logger << "Time span: " << time_span;

    downloader.reset(new Downloader());
    watcher.reset(new Watcher(*downloader, last_date, time_span));
    presenter.reset(CreatePresenter());
    wallpaper.reset(new XfceWallpaper(dir));
    timer.reset(new Timer(*watcher, *settings, *presenter, *wallpaper));

    timer->Start();

    return true;
}

}