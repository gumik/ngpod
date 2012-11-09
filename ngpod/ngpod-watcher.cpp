#include "ngpod-watcher.h"
#include "utils.h"

using namespace Glib;

namespace ngpod
{

Watcher::Watcher(Downloader& downloader, const Glib::Date& last_date, GTimeSpan time_span)
    : downloader(downloader),
      last_date(last_date),
      time_span(time_span),
      status(INVALID),
      url(Downloader::DEFAULT_URL)
{
    downloader.signal_DownloadFinished.connect(sigc::mem_fun(*this, &Watcher::DownloadFinishedCallback));
}

Watcher::~Watcher()
{
}

Watcher::Status Watcher::Tick(const DateTime& now)
{
    if (IsDownloadNeeded(now))
    {
        status = INVALID;
        downloader.Start(url);
    }
    else
    {
        status = NOT_NEEDED;
        signal_UpdateFinished();
    }

    return status;
}

void Watcher::DownloadFinishedCallback ()
{
    Downloader::Status downloader_status = downloader.GetStatus();
    const Date& date = downloader.GetDate();

    switch (downloader_status)
    {
        case Downloader::SUCCESS:
            if (date > last_date)
            {
                last_date = date;
            }
            break;

        case Downloader::SUCCESS_NO_IMAGE:
            last_date = date;
            break;

        default:
            break;
    }

    status = Watcher::UPDATED;
    signal_UpdateFinished();
}

bool Watcher::IsDownloadNeeded(const DateTime& now) const
{
    gint last_year = last_date.get_year();
    gint last_month = last_date.get_month();
    gint last_day = last_date.get_day();

    DateTime last_date_time = DateTime::create_local(last_year, last_month, last_day, 0, 0, 0);
    DateTime last_date_time_added = last_date_time.add_days(1).add(time_span);

    GTimeSpan diff = last_date_time_added.difference(now);

    return diff <= 0;
}

}