#ifndef __NGPOD_ABSTRACTWALLPAPER_H__
#define __NGPOD_ABSTRACTWALLPAPER_H__

#include <glibmm.h>
#include <glibmm/datetime.h>

namespace ngpod
{

class AbstractWallpaper
{
public:
    typedef std::pair<bool, Glib::ustring> Result;

    AbstractWallpaper(const Glib::ustring& dir);
    Result SetFromData(const char* data, uint length);

protected:
    virtual Result SetFromFile(const Glib::ustring& path) = 0;

    const Glib::ustring dir;
};

}

#endif /* __NGPOD_ABSTRACTWALLPAPER_H__ */