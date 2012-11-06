#ifndef __NGPOD_WALLPAPER_H__
#define __NGPOD_WALLPAPER_H__

#include <glib.h>
#include <glibmm.h>
#include <glibmm/datetime.h>

namespace ngpod
{

class Wallpaper
{
public:
    Wallpaper(const Glib::ustring& dir);
    bool SetFromData(const char* data, gsize data_length, GError** error);

private:
    Glib::ustring GetPathFromDate(const Glib::DateTime& now, const Glib::ustring& dir);

    Glib::ustring dir;
};

}

#endif /* __NGPOD_WALLPAPER_H__ */
