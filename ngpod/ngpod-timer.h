#ifndef __NGPOD_TIMER_H__
#define __NGPOD_TIMER_H__

#include <glibmm.h>
#include "ngpod-watcher.h"
#include "ngpod-settings.h"
#include "ngpod-presenter.h"
#include "AbstractWallpaper.h"
#include "Logger.h"

namespace ngpod
{

class Timer
{
public:
    Timer(Watcher& watcher, Settings& settings, Presenter& presenter,
        AbstractWallpaper& wallpaper);
    void Start();

private:
    bool Tick ();
    void WatcherFinishedCallback ();
    void AddTimeout (int seconds);
    void UpdateLastDateInSettings ();
    void PresenterMadeChoiceCallback ();

    Watcher& watcher;
    Settings& settings;
    Presenter& presenter;
    AbstractWallpaper& wallpaper;
    Logger log;

    static const int NOT_NEEDED_TIMEOUT = 60;
    static const int FAILED_TIMEOUT = 60;
    static const int SUCCESS_TIMEOUT = 5 * 60;
    static const int SUCCESS_NO_IMAGE_TIMEOUT = 15 * 60;
};

}

#endif /* __NGPOD_TIMER_H__ */
