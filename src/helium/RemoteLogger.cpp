/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: RemoteLogger.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015-11-11 09:33:07
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

#include "TimeVal.h"
#include "RemoteLogger.h"

using namespace HELIUM;

const char *CRemoteLogger::LOG_PATH = "/tmp/log.sock";
struct sockaddr_un CRemoteLogger::m_LogAddr;

CRemoteLogger::~CRemoteLogger()
{
    close(m_SockFd);
}

int CRemoteLogger::logOpen(const char *ident, unsigned long groups)
{
    int ret = 0;
    m_Groups = groups;

    if (m_SockFd == -1)
    {
        m_LogAddr.sun_family = AF_UNIX;
        (void)strncpy(m_LogAddr.sun_path, LOG_PATH, sizeof(m_LogAddr.sun_path));

        m_SockFd = socket(AF_UNIX, SOCK_DGRAM, 0);

        /// 将套接字设置为非阻塞模式
        int val;
        if ((val = fcntl(m_SockFd, F_GETFL, 0)) < 0)
        {
            return -1;
        }

        val |= O_NONBLOCK;
        ret = fcntl(m_SockFd, F_SETFL, val);
    }

    return ret;
}

inline void CRemoteLogger::logResync(void)
{
    return;
}

inline int CRemoteLogger::logClose(void)
{
    close(m_SockFd);
    return 0;
}

int CRemoteLogger::logMsg(LogGroup group, size_t indent_level, const std::string& func_name, 
                    size_t expected_size, const char *fmt, va_list msg_list)
{
    bool release = false;
    char *msg_ptr = NULL;

    if (groupEnabled(group) == false)
        return 0;

    /// 封装时间戳和模块名称
    char log_buf[LOGGER_MAXLINE];
    CTimeVal now(CTimeVal::gettimeofday());
    snprintf(log_buf, sizeof(log_buf) - 1, "[%s.%03d] <%s> ",
             now.fmtString("%Y-%m-%d %H:%M:%S").c_str(), ((now.getMicroSec() % 1000000) / 1000),
             m_AppName.c_str());
    size_t prefix_len = strlen(log_buf);

    /// 格式化日志消息负载
    msg_ptr = formatMsg(expected_size, fmt, msg_list, release);
    if (msg_ptr == NULL)
        return -1;

    /// 将时间戳和日志负载连接起来
    strncat(log_buf, msg_ptr, LOGGER_MAXLINE - prefix_len - 1);

    if (m_SockFd > 0)
    {
        sendto(m_SockFd, log_buf, strlen(log_buf), 0, (struct sockaddr*)&m_LogAddr, sizeof(m_LogAddr));
    }

    if (release == true)
        delete [] msg_ptr;

    return 0;
}

int CRemoteLogger::logFunc(LogGroup group, size_t indent_level, const std::string& func_name,
                    MarkType type)
{
    (void)indent_level;
    (void)func_name;
    (void)type;

    if (groupEnabled(group) == false)
        return 0;

    return 0;
}