#ifndef __NGPOD_ABSTRACTWALLPAPER_H__
#define __NGPOD_ABSTRACTWALLPAPER_H__

#include <string>
#include <glibmm/datetime.h>

namespace ngpod
{

class AbstractWallpaper
{
public:
    typedef std::pair<bool, std::string> Result;

    AbstractWallpaper(const std::string& dir);
    Result SetFromData(const char* data, int length);

protected:
    virtual Result SetFromFile(const std::string& path) = 0;

    const std::string dir;
};

}

#endif /* __NGPOD_ABSTRACTWALLPAPER_H__ */