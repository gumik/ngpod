#include "AbstractWallpaper.h"

#include <giomm.h>
#include "utils.h"

using namespace Glib;
using namespace Gio;

namespace ngpod
{

AbstractWallpaper::AbstractWallpaper(const ustring& dir)
    : dir(dir)
{
}

AbstractWallpaper::Result AbstractWallpaper::SetFromData(const char* data, uint length)
{
    // Prepare path
    DateTime now = DateTime::create_now_local();
    ustring path = GetPathFromDate (now, dir);

    // Write data to file
    RefPtr<File> file = File::create_for_path(path);

    RefPtr<FileOutputStream> stream = file->create_file();

    if (!stream)
    {
        return Result(false, "Cannot create file: " + path);
    }

    gsize bytes_written = 0;
    if (!stream->write_all(data, length, bytes_written))
    {
        return Result(false, "Cannot write to file: " + path);
    }

    if (!stream->close())
    {
        return Result(false, "Cannot close file: " + path);
    }

    return SetFromFile(path);
}

}