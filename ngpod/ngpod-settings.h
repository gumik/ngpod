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

    void SetDir(const Glib::ustring& dir);
    Glib::ustring GetDir() const;

    Glib::ustring GetLogFile() const;

    GTimeSpan GetTimeSpan() const;

private:
    bool CreateConfigDir();
    void SaveSettings();

    GKeyFile *key_file;
    Glib::ustring SETTINGS_PATH;
    Glib::ustring SETTINGS_DIR;
    Logger log;
};

}

#endif /* __NGPOD_SETTINGS_H__ */
