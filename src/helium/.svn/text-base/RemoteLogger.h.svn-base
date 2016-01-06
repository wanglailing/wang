/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: RemoteLogger.h
 *
 *       版本号: 1.0
 *     创建日期: 2015-11-10 16:51:44
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _REMOTE_LOGGER_H
#define _REMOTE_LOGGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>

#include "LoggerImplement.h"

namespace HELIUM
{
    /**
     * 定位到套接字管道的日志类
     *
     */
    class CRemoteLogger : public CLoggerImpl
    {
        public:
            CRemoteLogger() : m_SockFd(-1) {}
            ~CRemoteLogger();

            virtual int logOpen(const char *ident, unsigned long groups);
            virtual void logResync(void);
            virtual int logClose(void);

            virtual int logMsg(LogGroup group, size_t indent_level, const std::string& func_name,
                    size_t expected_size, const char *fmt, va_list msg_list);

            virtual int logFunc(LogGroup group, size_t indent_level, const std::string& func_name,
                    MarkType type);

            static const char *LOG_PATH;

        private:
            /// 禁止拷贝
            CRemoteLogger(const CRemoteLogger&);
            CRemoteLogger& operator=(const CRemoteLogger&);

        private:
            int m_SockFd;                           /// 套接字描述符
            static struct sockaddr_un m_LogAddr;    /// 日志服务器的地址
    };
}

#endif //_REMOTE_LOGGER_H