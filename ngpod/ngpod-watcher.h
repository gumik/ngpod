#ifndef __NGPOD_WATCHER_H__
#define __NGPOD_WATCHER_H__

#include <glibmm/datetime.h>
#include "ngpod-downloader.h"

namespace ngpod
{

class Watcher
{
public:
    enum Status
    {
        INVALID,
        NOT_NEEDED,
        UPDATED
    };

    Watcher(Downloader& downloader, const boost::gregorian::date& last_date,
            const boost::posix_time::time_duration& time_span);
    virtual ~Watcher();
    Status Tick(const boost::posix_time::ptime& now);
    Status GetStatus() const { return status; }
    const Downloader& GetDownloader() const { return downloader; }
    void SetUrl(const std::string& url) { this->url = url; }
    const boost::gregorian::date& GetLastDate() const { return last_date; }

    sigc::signal<void> signal_UpdateFinished;

private:
    void DownloadFinishedCallback();
    bool IsDownloadNeeded (const boost::posix_time::ptime& now) const;

    Downloader& downloader;
    boost::gregorian::date last_date;
    boost::posix_time::time_duration time_span;
    Status status;
    std::string url;
};

}

#endif /* __NGPOD_WATCHER_H__ */
