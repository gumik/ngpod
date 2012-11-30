#ifndef __NGPOD_LOGGER_H__
#define __NGPOD_LOGGER_H__

#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

namespace ngpod
{

class Log
{
public:
    Log(bool valid) : valid(valid)
    {
        sstream = new std::stringstream;
    }

    Log(const Log& other)
        : valid(true)
    {
        sstream = other.sstream;
    }

    ~Log()
    {
        if (valid)
        {
            std::cout << sstream->str() << std::endl;

            if (file_stream.good())
            {
                file_stream << sstream->str() << std::endl;
                file_stream.flush();
            }

            delete sstream;
        }
    }

    template <typename T>
    Log& operator<<(const T& msg)
    {
        *sstream << msg;
        return *this;
    }

    static bool SetFile(const std::string& path)
    {
        file_stream.open(path.c_str(), std::ios_base::app);
        return file_stream.good();
    }

private:
    Log& operator=(const Log&);
    std::stringstream* sstream;
    bool valid;

    static std::ofstream file_stream;
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
        using namespace boost::posix_time;
        ptime now = second_clock::local_time();

        return Log(false) << "[" << to_simple_string(now) << "]"
                          << "[" << domain << "]"
                          << ": " << msg;
    }

private:
    std::string domain;
};

}

#endif /* __NGPOD_LOGGER_H__ */