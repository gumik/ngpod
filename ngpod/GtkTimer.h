#ifndef __NGPOD_GTKTIMER__
#define __NGPOD_GTKTIMER__

#include "AbstractTimer.h"

namespace ngpod
{

class GtkTimer : public AbstractTimer
{
public:
    GtkTimer(Watcher& watcher, Settings& settings, IPresenter& presenter,
        AbstractWallpaper& wallpaper);

protected:
    /*override*/ void AddTimeout(int seconds);
};

}

#endif /*__NGPOD_GTKTIMER__*/