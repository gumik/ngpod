#include "XfceWallpaper.h"

#include <xfconf/xfconf.h>

using namespace std;

namespace ngpod
{

XfceWallpaper::XfceWallpaper(const string& dir)
    : AbstractWallpaper(dir)
{
}

AbstractWallpaper::Result XfceWallpaper::SetFromFile(const std::string& path)
{
    GError* error = NULL;
    if (!xfconf_init(&error))
    {
        string msg = string("Cannot initialize xfconf: ") + error->message;
        g_error_free(error);
        return Result(false, msg);
    }

    XfconfChannel* channel = xfconf_channel_get("xfce4-desktop");

    if (!xfconf_channel_set_string(channel, "/backdrop/screen0/monitor0/image-path", path.c_str()))
    {
        xfconf_shutdown();
        return Result(false, "Cannot set xfconf property");
    }

    xfconf_shutdown();

    return Result(true, "");
}

}