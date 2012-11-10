#include "XfceWallpaper.h"

#include <xfconf/xfconf.h>
#include "utils.h"

using namespace Glib;

namespace ngpod
{

XfceWallpaper::XfceWallpaper(const ustring& dir)
    : AbstractWallpaper(dir)
{
}

AbstractWallpaper::Result XfceWallpaper::SetFromFile(const Glib::ustring& path)
{
    GError* error = NULL;
    if (!xfconf_init(&error))
    {
        ustring msg = ustring("Cannot initialize xfconf: ") + error->message;
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