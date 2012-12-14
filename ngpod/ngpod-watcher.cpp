#include "ngpod-watcher.h"
#include "utils.h"

using namespace boost::gregorian;
using namespace boost::posix_time;

namespace ngpod
{

Watcher::Watcher(Downloader& downloader, const date& last_date,
                 const time_duration& time_span)
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

Watcher::Status Watcher::Tick(const ptime& now)
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
    const date& date = boost::posix_time::second_clock::local_time().date();

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

bool Watcher::IsDownloadNeeded(const ptime& now) const
{
    ptime date(last_date + days(1), time_span);
    return date < now;
}

}