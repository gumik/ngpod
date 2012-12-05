#include "MateWallpaper.h"

#include <mateconf/mateconf.h>

using namespace std;

namespace ngpod
{

MateWallpaper::MateWallpaper(const string& dir)
    : AbstractWallpaper(dir)
{
}

AbstractWallpaper::Result MateWallpaper::SetFromFile(const std::string& path)
{
    // Change desktop background
    MateConfEngine *conf = mateconf_engine_get_default ();
    gchar *value = mateconf_engine_get_string (conf, "/desktop/mate/background/picture_filename", NULL);
    GError* error;
    if (!mateconf_engine_set_string (conf, "/desktop/mate/background/picture_filename", path.c_str(), &error))
    {
        mateconf_engine_unref (conf);
        string msg = string("Cannot set wallpaper: ") + error->message;
        g_error_free(error);
        return Result(false, msg);
    }

    mateconf_engine_unref (conf);

    return Result(true, "");
}

}