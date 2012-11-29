#ifndef __NGPOD_LOGGER_H__
#define __NGPOD_LOGGER_H__

#include <sstream>
#include <iostream>

namespace ngpod
{

class LoggerList
{
public:
    template <typename M>
    LoggerList& operator<< (const M& msg)
    {
        stream << msg;
        return *this;
    }


    std::stringstream stream;
};

class Logger
{
public:
    void operator+=(const ngpod::LoggerList& list)
    {
        std::cout << "Date here: " << list.stream.str() << std::endl;
    }

private:
};

#define LOG ngpod::Logger() += ngpod::LoggerList()
}

#endif /* __NGPOD_LOGGER_H__ */