#ifndef __NGPOD_MATEWALLPAPER_H__
#define __NGPOD_MATEWALLPAPER_H__

#include <glib.h>
#include <glibmm.h>
#include <glibmm/datetime.h>
#include "AbstractWallpaper.h"

namespace ngpod
{

class MateWallpaper : public AbstractWallpaper
{
public:
    MateWallpaper(const Glib::ustring& dir);

protected:
    /*override*/ Result SetFromFile(const Glib::ustring& path);
};

}

#endif /* __NGPOD_MATEWALLPAPER_H__ */
