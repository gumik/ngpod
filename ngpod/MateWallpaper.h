#ifndef __NGPOD_MATEWALLPAPER_H__
#define __NGPOD_MATEWALLPAPER_H__

#include "AbstractWallpaper.h"

namespace ngpod
{

class MateWallpaper : public AbstractWallpaper
{
public:
    MateWallpaper(const std::string& dir);

protected:
    /*override*/ Result SetFromFile(const std::string& path);
};

}

#endif /* __NGPOD_MATEWALLPAPER_H__ */
