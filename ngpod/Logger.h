#ifndef __NGPOD_LOGGER_H__
#define __NGPOD_LOGGER_H__

#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <iostream>

namespace ngpod
{

class Log
{
public:
    Log(const std::string& line = "", bool valid = true)
        : valid(valid)
    {
        stream = new std::stringstream();

        using namespace boost::posix_time;
        ptime now = second_clock::local_time();
        *stream << "[" << to_simple_string(now) << "]";

        if (!line.empty())
        {
            *stream << "[" << line << "]";
        }

        *stream << ": ";
    }

    Log(const Log& other)
        : valid(true)
    {
        stream = other.stream;
    }

    ~Log()
    {
        if (valid)
        {
            std::cout << stream->str() << std::endl;
            delete stream;
        }
    }

    template <typename T>
    Log& operator<<(const T& msg)
    {
        *stream << msg;
        return *this;
    }

private:
    Log& operator=(const Log&);
    std::stringstream* stream;
    bool valid;
};

class Logger
{
public:
    Logger(const std::string& domain)
        : domain(domain)
    {

    }

    template <typename T>
    Log operator<<(const T& msg)
    {
        return Log(domain, false) << msg;
    }

private:
    std::string domain;
};

}

#endif /* __NGPOD_LOGGER_H__ */