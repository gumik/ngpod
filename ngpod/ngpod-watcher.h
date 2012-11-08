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

    Watcher(Downloader& downloader, const Glib::Date& last_date, GTimeSpan time_span);
    virtual ~Watcher();
    Status Tick(const Glib::DateTime& now);
    Status GetStatus() const { return status; }
    const Downloader& GetDownloader() const { return downloader; }
    void SetUrl(const Glib::ustring& url) { this->url = url; }
    const Glib::Date& GetLastDate() const { return last_date; }

    sigc::signal<void> signal_UpdateFinished;

private:
    void DownloadFinishedCallback();
    bool IsDownloadNeeded (const Glib::DateTime& now) const;

    Downloader& downloader;
    Glib::Date last_date;
    GTimeSpan time_span;
    Status status;
    Glib::ustring url;
};

}

#endif /* __NGPOD_WATCHER_H__ */
