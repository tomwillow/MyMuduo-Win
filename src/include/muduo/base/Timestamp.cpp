#define _CRT_SECURE_NO_WARNINGS
#include "Timestamp.h"

#include <cassert>

using namespace std;
using namespace muduo;

#define TICKS_PER_SECOND 10000000
#define EPOCH_DIFFERENCE 11644473600LL

// Windows时间从1601-01-01T00：00：00Z开始。离UNIX/Linux时间(1970-01-01T00：00：00Z)还有11644473600秒。Windows时间为100纳秒。
// 因此，从UNIX时间获得秒的函数如下所示
time_t ConvertWindowsTimeToUnixTime(long long input) {
    long long temp;
    temp = input / TICKS_PER_SECOND; //convert from 100ns intervals to seconds;
    temp = temp - EPOCH_DIFFERENCE;  //subtract number of seconds between epochs
    return (time_t)temp;
}


Timestamp::Timestamp() : tp(selected_clock::now())
{

}

Timestamp::Timestamp(std::chrono::time_point<selected_clock> tp):
tp(tp)
{

}

Timestamp::Timestamp(time_t t)
{
    tp = selected_clock::from_time_t(t);
    //assert(selected_clock::to_time_t(tp) == t);
}

Timestamp::Timestamp(FILETIME fileTime)
{
    time_t t = ConvertWindowsTimeToUnixTime(*((long long *)&fileTime));
    tp = selected_clock::from_time_t(t);
}

bool Timestamp::operator<(const Timestamp &other) const
{
    return tp.time_since_epoch().count() < other.tp.time_since_epoch().count();
}

bool Timestamp::operator<=(const Timestamp &other) const
{
    return tp.time_since_epoch().count() <= other.tp.time_since_epoch().count();
}

bool Timestamp::operator>(const Timestamp &other) const
{
    return tp.time_since_epoch().count() > other.tp.time_since_epoch().count();
}

bool Timestamp::operator>=(const Timestamp &other) const
{
    return tp.time_since_epoch().count() >= other.tp.time_since_epoch().count();
}

Timestamp Timestamp::operator+(const std::chrono::milliseconds &dura) const
{
    return tp+dura;
}

Timestamp muduo::Timestamp::operator+(double sec) const
{
    auto newTp = tp + chrono::duration_cast<selected_clock::duration>(chrono::duration<double, std::ratio<1>>(sec));
    return Timestamp(newTp);
}

Timestamp::selected_clock::time_point Timestamp::Raw() const
{
    return tp;
}

muduo::string muduo::Timestamp::ToString() const
{
    const int bufSize = 64;
    char buf[bufSize];

    auto t = selected_clock::to_time_t(tp);
    tm tempTM;
    errno_t err = LOCALTIME_R(&tempTM, &t);
    strftime(buf, bufSize, "%Y-%m-%d %H:%M:%S", &tempTM);
    return string(buf);
}

Timestamp Timestamp::now()
{
    return Timestamp();
}

double muduo::TimeDifference(Timestamp t1, Timestamp t2)
{
    auto dt = t1.Raw() - t2.Raw();
    return chrono::duration<double, std::ratio<1>>(dt).count();
}
