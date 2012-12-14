#ifndef __NGPOD_ABSTRACTTIMER_H__
#define __NGPOD_ABSTRACTTIMER_H__

#include "ngpod-watcher.h"
#include "ngpod-settings.h"
#include "IPresenter.h"
#include "AbstractWallpaper.h"
#include "Logger.h"

namespace ngpod
{

class AbstractTimer
{
public:
    AbstractTimer(Watcher& watcher, Settings& settings, IPresenter& presenter,
        AbstractWallpaper& wallpaper);
    void Start();

protected:
    virtual void AddTimeout (int seconds) = 0;
    bool Tick ();

private:
    void WatcherFinishedCallback ();
    void UpdateLastDateInSettings ();
    void PresenterMadeChoiceCallback ();

    Watcher& watcher;
    Settings& settings;
    IPresenter& presenter;
    AbstractWallpaper& wallpaper;
    Logger log;

    static const int NOT_NEEDED_TIMEOUT = 60;
    static const int FAILED_TIMEOUT = 60;
    static const int SUCCESS_TIMEOUT = 5 * 60;
    static const int SUCCESS_NO_IMAGE_TIMEOUT = 15 * 60;
};

}

#endif /* __NGPOD_ABSTRACTTIMER_H__ */
