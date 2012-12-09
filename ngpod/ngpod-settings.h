#ifndef __NGPOD_SETTINGS_H__
#define __NGPOD_SETTINGS_H__

#include <glib-object.h>
#include <boost/date_time/date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Logger.h"

namespace ngpod
{

class Settings
{
public:
    Settings();
    virtual ~Settings();

    bool Initialize();

    void SetLastDate(const boost::gregorian::date& date);
    boost::gregorian::date GetLastDate() const;

    void SetDir(const std::string& dir);
    std::string GetDir() const;

    std::string GetLogFile() const;

    boost::posix_time::time_duration GetTimeSpan() const;

private:
    bool CreateConfigDir();
    void SaveSettings();

    GKeyFile *key_file;
    std::string SETTINGS_PATH;
    std::string SETTINGS_DIR;
    Logger log;
};

}

#endif /* __NGPOD_SETTINGS_H__ */
