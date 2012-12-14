#include "GtkTimer.h"

#include <glibmm.h>

using namespace Glib;

namespace ngpod
{

GtkTimer::GtkTimer(Watcher& watcher, Settings& settings, IPresenter& presenter,
        AbstractWallpaper& wallpaper)
    : AbstractTimer(watcher, settings, presenter, wallpaper)
{
}

void GtkTimer::AddTimeout(int seconds)
{
    signal_timeout().connect_seconds(sigc::mem_fun(*this, &GtkTimer::Tick), seconds);
}

}