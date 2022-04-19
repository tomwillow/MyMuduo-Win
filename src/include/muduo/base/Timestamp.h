#pragma once

#include "Types.h"

#ifdef WIN32

//#ifndef _WINSOCK2API_
//#define WIN32_MEAN_AND_LEAN
//#include <Windows.h>
//#endif
#define LOCALTIME_R(pTM, pTimeT)  localtime_s(pTM, pTimeT)
#else
#define LOCALTIME_R(tm, ti)  localtime_r(ti, tm)
#endif

#include <chrono>

namespace muduo
{

    class Timestamp
    {
    public:
        using selected_clock = std::chrono::system_clock;

        Timestamp();
        Timestamp(std::chrono::time_point<selected_clock> tp);
        Timestamp(time_t t);
        Timestamp(FILETIME fileTime);
        bool operator<(const Timestamp &other) const;
        bool operator<=(const Timestamp &other) const;
        bool operator>(const Timestamp &other) const;
        bool operator>=(const Timestamp &other) const;
        Timestamp operator+(const std::chrono::milliseconds &dura) const;
        Timestamp operator+(double sec) const;

        selected_clock::time_point Raw() const;

        muduo::string ToString() const;

        static Timestamp now();
    private:
        selected_clock::time_point tp;

    };

    double TimeDifference(Timestamp t1, Timestamp t2);
}