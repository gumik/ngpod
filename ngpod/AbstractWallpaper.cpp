#include "AbstractWallpaper.h"

#include <boost/date_time.hpp>
#include <fstream>

#include "utils.h"

using namespace boost::posix_time;
using namespace std;

namespace ngpod
{

AbstractWallpaper::AbstractWallpaper(const string& dir)
    : dir(dir)
{
}

AbstractWallpaper::Result AbstractWallpaper::SetFromData(const char* data, int length)
{
    // Prepare path
    ptime now = second_clock::local_time();
    string path = GetPathFromDate (now, dir);

    // Write data to file
    ofstream stream;
    stream.open(path.c_str());

    if (!stream.good())
    {
        return Result(false, "Cannot create file: " + path);
    }

    stream.write(data, length);
    if (!stream.good())
    {
        return Result(false, "Cannot write to file: " + path);
    }

    stream.close();
    if (!stream.good())
    {
        return Result(false, "Cannot close file: " + path);
    }

    return SetFromFile(path);
}

}