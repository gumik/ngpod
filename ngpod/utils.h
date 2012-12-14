#ifndef __NGPOD_UTILS_H
#define __NGPOD_UTILS_H

#include <boost/date_time.hpp>

std::string GetPathFromDate(const boost::posix_time::ptime& now, const std::string& dir);
std::string GetConfigDir();
std::string StrReplace (const std::string& input, const std::string& search, const std::string& replace);

#endif
