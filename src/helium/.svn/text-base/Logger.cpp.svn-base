/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Logger.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月17日 11时01分21秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdarg.h>

#include "Logger.h"
#include "StdoutLogger.h"
#include "RemoteLogger.h"

using namespace HELIUM;

HELIUM_DECL_SINGLETON(CLogger);

int CLogger::logOpen(unsigned long groups)
{
    if (m_Impl != NULL)
    {
        std::cerr << "Log implementation already exists!" << std::endl;
        return -1;
    }

    m_Impl = new CStdoutLogger;
    return m_Impl->logOpen(groups);
}

int CLogger::logOpen(const char *ident, unsigned long groups)
{
    if (m_Impl != NULL)
    {
        std::cerr << "Log implementation already exists!" << std::endl;
        return -1;
    }

    m_Impl = new CRemoteLogger;
    return m_Impl->logOpen(ident, groups);
}

int CLogger::logOpen(const char *logfname, unsigned long groups, unsigned long maxfsize)
{
    (void)logfname;
    (void)groups;
    (void)maxfsize;

    return -1;
}

int CLogger::logClose(void)
{
    int ret = 0;
    if (m_Impl)
    {
        ret = m_Impl->logClose();
        delete m_Impl;
        m_Impl = NULL;
    }
    return ret;
}

int CLogger::logMsg(unsigned long groups, const char *fmt, ...)
{
    va_list vp;
    size_t expected_size = 0;
    std::string empty_str;
    if (m_Impl == NULL)
        return -1;

    int ret;
    char c;

    /// 估计消息的长度
    va_start(vp, fmt);
    ret = vsnprintf(&c, 1, fmt, vp);
    va_end(vp);

    expected_size = ret + 1;
    va_start(vp, fmt);
    ret = m_Impl->logMsg(static_cast<LogGroup>(groups), m_FuncContext.size(),
            m_FuncContext.size() ? m_FuncContext.top() : empty_str, expected_size, fmt, vp);
    va_end(vp);

    return ret;
}

int CLogger::logFunc(unsigned long groups, MarkType type)
{
    std::string empty_str;
    if (m_Impl == NULL)
        return -1;

    return m_Impl->logFunc(static_cast<LogGroup>(groups), m_FuncContext.size(),
            m_FuncContext.size() ? m_FuncContext.top() : empty_str, type);
}

