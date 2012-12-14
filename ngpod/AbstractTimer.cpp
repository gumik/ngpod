#include "AbstractTimer.h"

#include <boost/date_time.hpp>
#include <glibmm.h>

#include "Logger.h"

using namespace Glib;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace std;

namespace ngpod
{

AbstractTimer::AbstractTimer(Watcher& watcher, Settings& settings, IPresenter& presenter,
    AbstractWallpaper& wallpaper)
    : watcher(watcher)
    , settings(settings)
    , presenter(presenter)
    , wallpaper(wallpaper)
    , log("AbstractTimer")
{
    watcher.signal_UpdateFinished.connect(sigc::mem_fun(*this, &AbstractTimer::WatcherFinishedCallback));
    presenter.signal_MadeChoice.connect(sigc::mem_fun(*this, &AbstractTimer::PresenterMadeChoiceCallback));
}

void AbstractTimer::Start()
{
    AddTimeout (1);
}

bool AbstractTimer::Tick ()
{
    ptime now = second_clock::local_time();
    watcher.Tick(now);

    return false;
}

void AbstractTimer::WatcherFinishedCallback()
{
    Watcher::Status status = watcher.GetStatus();

    if (status == Watcher::NOT_NEEDED)
    {
        AddTimeout (NOT_NEEDED_TIMEOUT);
        log << "NOT_NEEDED";
    }
    else if (status == Watcher::UPDATED)
    {
        const Downloader& downloader = watcher.GetDownloader();
        Downloader::Status downloader_status = downloader.GetStatus();

        switch (downloader_status)
        {
            case Downloader::FAILED:
                log << "DOWNLOADER_FAILED";
                AddTimeout(FAILED_TIMEOUT);
                break;

            case Downloader::FAILED_GET_IMAGE:
                log << "DOWNLOADER_FAILED_GET_IMAGE";
                AddTimeout(FAILED_TIMEOUT);
                break;

            case Downloader::SUCCESS_NO_IMAGE:
                log << "DOWNLOADER_SUCCESS_NO_IMAGE";
                UpdateLastDateInSettings();
                AddTimeout(SUCCESS_NO_IMAGE_TIMEOUT);
                break;

            case Downloader::SUCCESS:
                log << "DOWNLOADER_SUCCESS";

                presenter.Notify(
                    downloader.GetData(),
                    downloader.GetDataLength(),
                    downloader.GetTitle(),
                    downloader.GetDescription());
                break;
        }
    }
    else
    {
        log << "unknown NgpodWatcher status";
    }
}

void AbstractTimer::UpdateLastDateInSettings ()
{
    date last_date = watcher.GetLastDate();
    settings.SetLastDate(last_date);
}

void AbstractTimer::PresenterMadeChoiceCallback()
{
    if (presenter.IsAccepted())
    {
        const Downloader& downloader = watcher.GetDownloader();
        const char * data = downloader.GetData();
        int data_length = downloader.GetDataLength();

        AbstractWallpaper::Result result = wallpaper.SetFromData(data, data_length);
        if (!result.first)
        {
            string msg = "Error while setting wallpaper:\n%s" + result.second;
            presenter.ShowError (msg);
            log << msg;
        }
    }

    presenter.Hide();
    UpdateLastDateInSettings();
    AddTimeout(SUCCESS_TIMEOUT);
}

}