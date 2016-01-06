/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TimeVal.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月25日 11时27分30秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/rtc.h>

#include "TimeVal.h"

using namespace HELIUM;

CTimeVal CTimeVal::m_Zero;
static const long ONE_SECOND = 1000000;

void CTimeVal::init(long sec, long msec, int tz)
{
    tv_sec = sec;
    tv_usec = msec;
    m_TimeZone = tz;

    normalize();
}

CTimeVal::CTimeVal()
{
    init(0, 0, GMT);
}

CTimeVal::CTimeVal(long secs, long msecs)
{
    init(secs, msecs, GMT);
}

CTimeVal::CTimeVal(double secs) : m_TimeZone(GMT)
{
    tv_sec = (long)secs;
    tv_usec = (long)((secs - (double)tv_sec) * 1000000.0f);

    normalize();
}

CTimeVal::CTimeVal(const timeval& tv)
{
    init(tv.tv_sec, tv.tv_usec, GMT);
}

CTimeVal::CTimeVal(const CTimeVal& tv)
{
    init(tv.tv_sec, tv.tv_usec, tv.m_TimeZone);
}

CTimeVal::operator double() const
{
    return (tv_sec + tv_usec / 1000000.0f);
}

long CTimeVal::milliSec(void) const
{
    return ((getMicroSec() % 1000000) / 1000);
}

CTimeVal& CTimeVal::operator=(const CTimeVal& tv)
{
    init(tv.tv_sec, tv.tv_usec, tv.m_TimeZone);
    return (*this);
}

CTimeVal& CTimeVal::operator+=(const CTimeVal& rhs)
{
    tv_sec += rhs.tv_sec;
    tv_usec += rhs.tv_usec;

    if (tv_usec >= ONE_SECOND)
    {
        tv_sec++;
        tv_usec -= ONE_SECOND;
    }
    else if (tv_sec >= 1 && tv_usec < 0)
    {
        tv_sec--;
        tv_usec += ONE_SECOND;
    }

    normalize();
    return (*this);
}

CTimeVal& CTimeVal::operator-=(const CTimeVal& rhs)
{
    tv_sec -= rhs.tv_sec;
    tv_usec -= rhs.tv_usec;

    if (tv_usec < 0)
    {
        tv_sec--;
        tv_usec += ONE_SECOND;
    }
    else if (tv_usec >= ONE_SECOND)
    {
        tv_sec++;
        tv_usec -= ONE_SECOND;
    }

    normalize();
    return (*this);
}

bool CTimeVal::operator<(const CTimeVal& rhs) const
{
    return ( tv_sec < rhs.tv_sec || (tv_sec == rhs.tv_sec && tv_usec < rhs.tv_usec) );
}

bool CTimeVal::operator==(const CTimeVal& rhs) const
{
    return ( !(*this < rhs || rhs < *this) );
}

std::string CTimeVal::fmtString(const char *fmt) const
{
    struct tm ct;
    char buf[80];
    memset(buf, 0 , sizeof(buf));

    if (m_TimeZone == GMT)
    {
        ct = *(localtime((const time_t*)&tv_sec));
    }
    else
    {
        ct = *(gmtime((const time_t*)&tv_sec));
    }

    if (fmt == NULL)
    {
        strftime(buf, sizeof(buf), "%Y/%j %H:%M:%S", &ct);
        sprintf(buf + strlen(buf), ".%03ld", (tv_usec % 1000000) / 1000);
    }
    else
    {
        strftime(buf, sizeof(buf), fmt, &ct);
    }

    return (std::string(buf));
}

std::string CTimeVal::fmtHHMMSS(void) const
{
    return fmtString("%T");
}

std::string CTimeVal::fmtYYMMDD(void) const
{
    return fmtString("%Y_%m_%d");
}

std::string CTimeVal::fmtHHMMSSMLS(void) const
{
    struct tm ct;
    char buf[80];
    memset(buf, 0 , sizeof(buf));

    if (m_TimeZone == GMT)
    {
        ct = *(localtime((const time_t*)&tv_sec));
    }
    else
    {
        ct = *(gmtime((const time_t*)&tv_sec));
    }

    strftime(buf, sizeof(buf), "%H:%M:%S", &ct);
    sprintf(buf + strlen(buf), ".%03ld", (tv_usec % 1000000) / 1000);

    return (std::string(buf));
}

std::string CTimeVal::fmtMMSS(void) const
{
    return fmtString("%M:%S");
}

std::string CTimeVal::fmtMMSSMLS(void) const
{
    struct tm ct;
    char buf[80];
    memset(buf, 0 , sizeof(buf));

    if (m_TimeZone == GMT)
    {
        ct = *(localtime((const time_t*)&tv_sec));
    }
    else
    {
        ct = *(gmtime((const time_t*)&tv_sec));
    }

    strftime(buf, sizeof(buf), "%M:%S", &ct);
    sprintf(buf + strlen(buf), ".%03ld", (tv_usec % 1000000) / 1000);

    return (std::string(buf));
}

std::string CTimeVal::fmtSSMLS(void) const
{
    struct tm ct;
    char buf[80];
    memset(buf, 0 , sizeof(buf));

    if (m_TimeZone == GMT)
    {
        ct = *(localtime((const time_t*)&tv_sec));
    }
    else
    {
        ct = *(gmtime((const time_t*)&tv_sec));
    }

    strftime(buf, sizeof(buf), "%S", &ct);
    sprintf(buf + strlen(buf), ".%03ld", (tv_usec % 1000000) / 1000);

    return (std::string(buf));
}

CTimeVal CTimeVal::gettimeofday(void)
{
    timeval tv;
    ::gettimeofday(&tv, 0);

    return (tv);
}

CTimeVal CTimeVal::getMonotonicTime(void)
{
    timeval tv;
    struct timespec ts = {0, 0};
    ::clock_gettime(CLOCK_MONOTONIC, &ts);
    tv.tv_sec = ts.tv_sec;
    tv.tv_usec = ts.tv_nsec / 1000;

    return tv;
}

uint64_t CTimeVal::getMonotonicMs(void)
{
    struct timespec ts = {0, 0};
    ::clock_gettime(CLOCK_MONOTONIC, &ts);
    unsigned int millisec = (unsigned int)(ts.tv_nsec / 1000000);

    return (uint64_t)(ts.tv_sec * 1000) + 
           (uint64_t)millisec;
}

double CTimeVal::localTDateTime(void)
{
    time_t now = time(NULL);

    /// 进行时区转换(默认为东8区)
    long local_now = now + 28800;

    double unix_startdate = 25569.0f; /// 从1899-12-30到1970-01-01的天数
    double secs_inday = (double)(local_now % 86400) / 86400.0f; /// 当天的秒数在一天中的比例
    double tdate_time = (local_now / 86400) + unix_startdate + secs_inday;

    return tdate_time;
}

int CTimeVal::adjustTime(struct tm* calendar_time, int threshold, int rtt, int utc)
{
    /// 年、月、日为0，或者时刻为08:00时，不进行校时
    if (calendar_time->tm_year == 0 || calendar_time->tm_mon == 0 || calendar_time->tm_mday == 0 || 
       (calendar_time->tm_hour == 8 && calendar_time->tm_min == 0))
    {
        return -1;
    }

    calendar_time->tm_year -= 1900;
    calendar_time->tm_mon  -= 1;
    calendar_time->tm_isdst = 0;
    time_t base_time = mktime(calendar_time);
    if (base_time == (time_t)-1L)
    {
        return -1;
    }

    /// 将UTC时间转为东8区时间
    if (utc)
    {
        base_time += 28800;
    }

    /// 远程时间需要做传输时延补偿
    base_time += rtt;

    /// 获取本地系统时钟
    time_t now = time(NULL);

    if (abs((int)(now - base_time)) > threshold)
    {
        /// 调整系统时钟
        if (stime(&base_time) < 0)
        {
            return -1;
        }

        struct tm base_tm;
        memset(&base_tm, 0, sizeof(base_tm));
        localtime_r(&base_time, &base_tm);

        /// 修改硬件时钟
        int rtc;
        if ((rtc = open("/dev/rtc0", O_WRONLY)) < 0)
        {
            return -1;
        }

        ioctl(rtc, RTC_SET_TIME, &base_tm);
        close(rtc);
    }

    return 0;
}

void CTimeVal::normalize(void)
{
    if (tv_usec >= ONE_SECOND)
    {
        do
        {
            tv_sec++;
            tv_usec -= ONE_SECOND;
        } while (tv_usec >= ONE_SECOND);
    }
    else if (tv_usec <= -ONE_SECOND)
    {
        do
        {
            tv_sec--;
            tv_usec += ONE_SECOND;
        } while (tv_usec <= -ONE_SECOND);
    }

    if (tv_sec >= 1 && tv_usec < 0)
    {
        tv_sec--;
        tv_usec += ONE_SECOND;
    }
    else if (tv_sec < 0 && tv_usec > 0)
    {
        tv_sec++;
        tv_usec -= ONE_SECOND;
    }
}
