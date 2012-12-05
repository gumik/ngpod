#ifndef __NGPOD_XFCEWALLPAPER_H__
#define __NGPOD_XFCEWALLPAPER_H__

#include "AbstractWallpaper.h"

namespace ngpod
{

class XfceWallpaper : public AbstractWallpaper
{
public:
    XfceWallpaper(const std::string& dir);

protected:
    /*override*/ Result SetFromFile(const std::string& path);
};

}

#endif /* __NGPOD_XFCEWALLPAPER_H__ */
