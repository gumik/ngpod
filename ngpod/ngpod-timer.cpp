#include "ngpod-timer.h"
#include "utils.h"

using namespace Glib;

namespace ngpod
{

Timer::Timer(Watcher& watcher, Settings& settings, Presenter& presenter,
    Wallpaper& wallpaper)
    : watcher(watcher)
    , wallpaper(wallpaper)
    , settings(settings)
    , presenter(presenter)
{
    watcher.signal_UpdateFinished.connect(sigc::mem_fun(*this, &Timer::WatcherFinishedCallback));
    presenter.signal_MadeChoice.connect(sigc::mem_fun(*this, &Timer::PresenterMadeChoiceCallback));
}

void Timer::Start()
{
    AddTimeout (1);
}

bool Timer::Tick ()
{
    DateTime now = DateTime::create_now_local();
    watcher.Tick(now);

    return false;
}

void Timer::WatcherFinishedCallback()
{
    Watcher::Status status = watcher.GetStatus();

    if (status == Watcher::NOT_NEEDED)
    {
        AddTimeout (NOT_NEEDED_TIMEOUT);
        log_message ("Timer", "NOT_NEEDED");
    }
    else if (status == Watcher::UPDATED)
    {
        const Downloader& downloader = watcher.GetDownloader();
        Downloader::Status downloader_status = downloader.GetStatus();

        switch (downloader_status)
        {
            case Downloader::FAILED:
                log_message ("Timer", "DOWNLOADER_FAILED");
                AddTimeout(FAILED_TIMEOUT);
                break;

            case Downloader::FAILED_GET_IMAGE:
                log_message ("Timer", "DOWNLOADER_FAILED_GET_IMAGE");
                AddTimeout(FAILED_TIMEOUT);
                break;

            case Downloader::SUCCESS_NO_IMAGE:
                log_message ("Timer", "DOWNLOADER_SUCCESS_NO_IMAGE");
                UpdateLastDateInSettings();
                AddTimeout(SUCCESS_NO_IMAGE_TIMEOUT);
                break;

            case Downloader::SUCCESS:
                log_message ("Timer", "DOWNLOADER_SUCCESS");

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
        log_message ("Timer", "unknown NgpodWatcher status");
    }
}

void Timer::AddTimeout (int seconds)
{
    signal_timeout().connect_seconds(sigc::mem_fun(*this, &Timer::Tick), seconds);
}

void Timer::UpdateLastDateInSettings ()
{
    Date last_date = watcher.GetLastDate();
    settings.SetLastDate(last_date);
}

void Timer::PresenterMadeChoiceCallback()
{
    if (presenter.IsAccepted())
    {
        GError *error = NULL;

        const Downloader& downloader = watcher.GetDownloader();
        const char * data = downloader.GetData();
        int data_length = downloader.GetDataLength();

        if (!wallpaper.SetFromData(data, data_length, &error))
        {
            GString *msg = g_string_new ("");
            g_string_printf (msg, "Error while setting wallpaper:\n%s", error->message);
            presenter.ShowError (msg->str);
            log_message ("Timer", msg->str);
            g_string_free (msg, TRUE);
            g_error_free (error);
        }
    }

    presenter.Hide();
    UpdateLastDateInSettings();
    AddTimeout(SUCCESS_TIMEOUT);
}

}