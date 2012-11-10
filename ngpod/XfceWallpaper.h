#ifndef __NGPOD_XFCEWALLPAPER_H__
#define __NGPOD_XFCEWALLPAPER_H__

#include "AbstractWallpaper.h"

namespace ngpod
{

class XfceWallpaper : public AbstractWallpaper
{
public:
    XfceWallpaper(const Glib::ustring& dir);

protected:
    /*override*/ Result SetFromFile(const Glib::ustring& path);
};

}

#endif /* __NGPOD_XFCEWALLPAPER_H__ */
