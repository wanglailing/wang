/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Logger.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月16日 17时43分40秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _LOGGER_H
#define _LOGGER_H

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <string>
#include <stack>

#include "Singleton.h"
#include "LoggerImplement.h"

namespace HELIUM
{
    /**
     * 日志类的抽象接口
     *
     * @see CLoggerImpl
     */
    class CLogger : public CSingleton<CLogger>
    {
        public:
            CLogger() : m_Impl(NULL) {}
            ~CLogger() { this->logClose(); }

        public:
            /**
             * 设置程序名称
             *
             * @param appname 将要设置的程序名
             *
             */
            void setAppName(const std::string &appname);

            /**
             * 开启日志分组
             *
             * @param g 日志分组
             *
             */
            void enableGroup(LogGroup g);

            /**
             * 关闭日志分组
             *
             * @param g 日志分组
             *
             */
            void disableGroup(LogGroup g);

            /**
             * 开启多个日志分组
             *
             * @param groups 日志分组的组合
             *
             */
            void enableGroups(unsigned long groups);

            /**
             * 关闭多个日志分组
             *
             * @param groups 日志分组的组合
             *
             */
            void disableGroups(unsigned long groups);

            /**
             * 开启所有的日志分组
             *
             *
             */ 
            void enableAllGroups(void);

            /**
             * 关闭所有的日志分组
             *
             *
             */ 
            void disableAllGroups(void);

            /**
             * 日志分组是否开启
             *
             *
             */ 
            bool groupEnabled(LogGroup g) const;

            /**
             * 打开时间戳
             *
             *
             */ 
            void enableTimeStamp(void);

            /**
             * 关闭时间戳
             *
             *
             */ 
            void disableTimeStamp(void);

            /**
             * 是否开启时间戳
             *
             *
             */ 
            bool timestampEnabled(void) const;

            /**
             * 设置时区
             *
             * @param zone  0 -- GMT; 1 -- Local
             *
             */ 
            void setTimezone(int zone);

            /**
             * 将日志信息打印到标准输出
             *
             * @param groups 日志分组的组合
             *
             */
            int logOpen(unsigned long groups);

            /**
             * 将日志信息打印到套接字管道
             *
             * @param ident  日志前缀
             * @param groups 日志分组的组合
             *
             */
            int logOpen(const char *ident, unsigned long groups);

            /**
             * 将日志信息写入文件
             *
             * @param logfname 日志文件的名称
             * @param groups   日志分组的组合
             * @param maxfsize 日志文件的最大长度
             *
             */
            int logOpen(const char *logfname, unsigned long groups, unsigned long maxfsize);

            /**
             * 同步日志文件到磁盘设备
             *
             *
             */ 
            void logResync(void);

            /**
             * 关闭日志
             *
             *
             */ 
            int logClose(void);

            int logMsg(unsigned long groups, const char *fmt, ...);
            int logFunc(unsigned long groups, MarkType type);

        private:
            /// 日志类的具体实现
            CLoggerImpl *m_Impl;

            /// 函数调用上下文
            std::stack<std::string> m_FuncContext;
    };

    inline void CLogger::setAppName(const std::string &appname)
    {
        if (m_Impl)
            m_Impl->setAppName(appname);
    }

    inline void CLogger::enableTimeStamp(void) 
    { 
        if (m_Impl)
            m_Impl->enableTimeStamp();
    }

    inline void CLogger::enableGroup(LogGroup g) 
    { 
        if (m_Impl)
            m_Impl->enableGroup(g);
    }

    inline void CLogger::disableGroup(LogGroup g) 
    { 
        if (m_Impl)
            m_Impl->disableGroup(g);
    }

    inline void CLogger::enableGroups(unsigned long groups) 
    { 
        if (m_Impl)
            m_Impl->enableGroups(groups);
    }

    inline void CLogger::disableGroups(unsigned long groups) 
    { 
        if (m_Impl)
            m_Impl->disableGroups(groups);
    }

    inline void CLogger::enableAllGroups(void) 
    { 
        if (m_Impl)
            m_Impl->enableAllGroups();
    }

    inline void CLogger::disableAllGroups(void) 
    { 
        if (m_Impl)
            m_Impl->disableAllGroups();
    }

    inline bool CLogger::groupEnabled(LogGroup g) const
    {
        return m_Impl ? m_Impl->groupEnabled(g) : false;
    }


    inline void CLogger::disableTimeStamp(void) 
    { 
        if (m_Impl)
            m_Impl->disableTimeStamp();
    }

    inline bool CLogger::timestampEnabled(void) const 
    { 
        return m_Impl ? m_Impl->timestampEnabled() : false;
    }

    inline void CLogger::setTimezone(int zone) 
    { 
        if (m_Impl)
            m_Impl->setTimezone(zone);
    }

    inline void CLogger::logResync(void)
    {
        if (m_Impl)
            m_Impl->logResync();
    }

    /**
     * @def LOGGER
     * 日志类单件对象的快捷方式
     *
     */
    #define LOGGER HELIUM::CLogger::getInstance()

    /**
     * @def DBG_LOG(X)
     * 将调试信息写到日志
     *
     */
    #if defined(HELIUM_NLOGGING)
    #define DBG_LOG(X) do {} while(0)
    #else
    #define DBG_LOG(X) \
        do { LOGGER->logMsg X; } while (0)
    #endif

    /**
     * @def ERR_LOG(X)
     * 将错误信息写到日志
     *
     */
    #if defined(HELIUM_NLOGGING)
    #define ERR_LOG(X) do {} while(0)
    #else
    #define ERR_LOG(X) \
        do { \
            LOGGER->logMsg X; \
            LOGGER->logMsg(HELIUM::FATALERR, "errno: %d \"%s\"\n", errno, strerror(errno)); \
        } while (0)
    #endif

    /// 该命名空间包含了使用频率最高的日志相关的操作
    namespace Log
    {
        /**
         * 在做其它日志操作之前，应该先调用这个函数，以便正确设置应用程序名称。
         *
         */
        inline void setAppName(const std::string &name) { LOGGER->setAppName(name); }

        /**
         * 打开日志文件
         *
         * @param logfname   日志文件名
         * @param groups     日志组
         * @param maxfsize   日志文件的最大长度
         */
        inline int openLogFile(const char *logfname, unsigned long groups = ALL, unsigned long maxfsize = 1024 * 1024 * 10) 
        {
            return LOGGER->logOpen(logfname, groups, maxfsize);
        }

        /**
         * 打开日志到标准输出
         *
         * @param groups     日志组
         */
        inline int openLogStdout(unsigned long groups = ALL) { return LOGGER->logOpen(groups); }

        /**
         * 打开日志到套接字管道
         *
         * @param ident  日志前缀
         * @param groups 日志组
         */
        inline int openLogRemote(const char *ident, unsigned long groups = ALL) { return LOGGER->logOpen(ident, groups); }

        /**
         * 刷新日志输出
         */
        inline void logResync(void) { LOGGER->logResync(); }

        /**
         * 关闭日志
         */
        inline int logClose(void) { return LOGGER->logClose(); }

        /**
         * 打开时间戳
         */
        inline void enableTimeStamp(void) { LOGGER->enableTimeStamp(); }

        /**
         * 关闭时间戳
         */
        inline void disableTimeStamp(void) { LOGGER->disableTimeStamp(); }
    }
}

#endif //_LOGGER_H
