/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: LoggerImplement.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月17日 13时05分38秒
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
#include <errno.h>

#include <algorithm>
#include <iomanip>

#include "LoggerImplement.h"
#include "TimeVal.h"

using namespace HELIUM;

char CLoggerImpl::m_MsgBuf[LOGGER_MAXLINE];

CLoggerImpl::CLoggerImpl() : m_Groups(0), m_LogFName(""), m_AppName(""),
                             m_TimestampFlag(false), m_Timezone(1)
{
}

inline int CLoggerImpl::logOpen(unsigned long /*groups*/)
{
    errno = ENOSYS;
    return -1;
}

inline int CLoggerImpl::logOpen(const char * /*ident*/,
                                unsigned long /*groups*/)
{
    errno = ENOSYS;
    return -1;
}

inline int CLoggerImpl::logOpen(const char * /*logfname*/, 
                                unsigned long /*groups*/, 
                                unsigned long /*maxfsize*/)
{
    errno = ENOSYS;
    return -1;
}

unsigned short CLoggerImpl::addTimeStamp(std::ostream& sink)
{
    unsigned short bytecount = 0;
    if (timestampEnabled())
    {
        CTimeVal now(CTimeVal::gettimeofday());
        sink << '[' << now.fmtString("%Y-%m-%d %H:%M:%S") << '.';
        char oldfill = sink.fill('0');
        sink << std::setw(3) << ((now.getMicroSec() % 1000000) / 1000) << "] ";
        sink.fill(oldfill);

        bytecount = 26;
    }
    return bytecount;
}

unsigned short CLoggerImpl::indentFuncName(std::ostream& sink, size_t indent_level, const std::string& func_name,
                    MarkType type)
{
    unsigned short bytecount = 0;

    if (func_name.size())
    {
    }

    return bytecount;
}

char* CLoggerImpl::formatMsg(size_t expected_size, const char *fmt, va_list msg_list, bool& release)
{
    int ret = 0;
    char *msg = m_MsgBuf;
    release = false;
    expected_size++;    // 包含字符串结束标志'\0'
    if (expected_size >= LOGGER_MAXLINE)
    {
        msg = new char[expected_size];
        release = true;
    }

    ret = vsnprintf(msg, expected_size, fmt, msg_list);
    return (ret < 0 ? NULL : msg);
}
