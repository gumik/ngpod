#include "utils.h"

#include <boost/date_time/date_facet.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <sstream>

using namespace std;
using namespace boost;

string GetPathFromDate(const posix_time::ptime& now, const string& dir)
{
    stringstream ss;
    gregorian::date_facet* facet = new gregorian::date_facet();
    facet->set_iso_extended_format();
    ss.imbue(locale(ss.getloc(), facet));
    ss << "pod-" << now.date() << ".png";
    filesystem::path path(dir);
    path /= ss.str();
    return path.string();
}

string GetConfigDir()
{
#ifdef __linux
    return getenv("HOME");
#else
    throw "Not implemented";
#endif
}

std::string StrReplace (const std::string& input, const std::string& search, const std::string& replace)
{
    string result;

    int prev = 0;
    int i = 0;
    while ((i = input.find(search, prev)) != string::npos)
    {
        result += input.substr(prev, i - prev);
        result += replace;
        prev = i + search.size();
    }

    result += input.substr(prev, i);

    return result;
}
