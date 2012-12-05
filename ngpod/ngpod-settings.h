#ifndef __NGPOD_SETTINGS_H__
#define __NGPOD_SETTINGS_H__

#include <glib-object.h>
#include <glibmm/date.h>
#include "Logger.h"

namespace ngpod
{

class Settings
{
public:
    Settings();
    virtual ~Settings();

    bool Initialize();

    void SetLastDate(const Glib::Date& date);
    Glib::Date GetLastDate() const;

    void SetDir(const std::string& dir);
    std::string GetDir() const;

    std::string GetLogFile() const;

    GTimeSpan GetTimeSpan() const;

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
