#include "ngpod-settings.h"

#include <boost/assign.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <sstream>
#include <vector>

#include "Logger.h"
#include "utils.h"

using namespace boost;
using namespace boost::assign;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace std;

namespace ngpod
{

Settings::~Settings()
{
}

Settings::Settings()
    : log("Settings")
{
    using boost::filesystem::path;
    SETTINGS_DIR = (path(GetConfigDir()) / ".config" / "gumik").string();
    SETTINGS_PATH = (path(SETTINGS_DIR) / "ngpod.settings").string();
}

bool Settings::Initialize()
{
    if (!CreateConfigDir())
    {
        return false;
    }

    try
    {
        read_json(SETTINGS_PATH, tree);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

void Settings::SetLastDate (const gregorian::date& date)
{
    tree.put<int>("last_date.year", static_cast<unsigned short>(date.year()));
    tree.put<int>("last_date.month", date.month().as_number());
    tree.put<int>("last_date.day", date.day().as_number());
    SaveSettings ();
}

gregorian::date Settings::GetLastDate() const
{
    int year = tree.get<int>("last_date.year", 1400);
    int month = tree.get<int>("last_date.month", 1);
    int day = tree.get<int>("last_date.day", 1);

    return gregorian::date (year, month, day);
}

void Settings::SetDir (const string& dir)
{
    tree.put("dir", dir);
    SaveSettings();
}

string Settings::GetDir() const
{
    return tree.get<string>("dir", "");
}

string Settings::GetLogFile() const
{
    return tree.get<string>("log_file", "");
}

posix_time::time_duration Settings::GetTimeSpan() const
{
    int hour = tree.get<int>("time_span.hour", 0);
    int minute = tree.get<int>("time_span.minute", 0);

    return posix_time::time_duration(hour, minute, 0, 0);
}

bool Settings::CreateConfigDir()
{
    try
    {
        create_directories(SETTINGS_DIR);
        return true;
    }
    catch (filesystem_error& error)
    {
        log << "Cannot create settings directory: " << error.what();
        return false;
    }
}

void Settings::SaveSettings()
{
    write_json(SETTINGS_PATH, tree);
}

}