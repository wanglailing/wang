/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: LoggerImplement.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月17日 10时17分27秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _LOGGER_IMPLEMENT_H
#define _LOGGER_IMPLEMENT_H

#include <ostream>
#include <cstdarg>

#include "LogMask.h"

#define BAD_HANDLER -1
typedef int handler_t;

namespace HELIUM
{
    inline bool isValidHandler(handler_t fd)
    {
        return (fd != BAD_HANDLER);
    }

    inline void disableHandler(handler_t& fd)
    {
        fd = BAD_HANDLER;
    }

    /**
     * 日志类的具体实现
     *
     */
    class CLoggerImpl
    {
        public:
            CLoggerImpl();
            virtual ~CLoggerImpl() {}

            void setAppName(const std::string &appname) { m_AppName = appname; }
            void enableGroup(LogGroup g) { m_Groups |= g; }
            void disableGroup(LogGroup g) { m_Groups &= ~g; }
            void enableGroups(unsigned long groups) { m_Groups |= groups; }
            void disableGroups(unsigned long groups) { m_Groups &= ~groups; }
            void enableAllGroups(void) { m_Groups = HELIUM::ALL; }
            void disableAllGroups(void) { m_Groups = 0; }
            bool groupEnabled(LogGroup g) const { return (m_Groups & g); }

            void enableTimeStamp(void) { m_TimestampFlag = true; }
            void disableTimeStamp(void) { m_TimestampFlag = false; }

            bool timestampEnabled(void) const { return m_TimestampFlag; }

            void setTimezone(int zone) { m_Timezone = zone; }

            /// 打开定位到标准出错的日志
            virtual int logOpen(unsigned long groups);

            /// 打开定位到套接字管道的日志
            virtual int logOpen(const char *ident, unsigned long groups);

            /// 打开定位到文件的日志
            virtual int logOpen(const char *logfname, unsigned long groups, unsigned long maxfsize);

            /// 同步日志信息到设备
            virtual void logResync(void) {}

            /// 关闭日志操作
            virtual int logClose(void) = 0;

            virtual int logMsg(LogGroup group, size_t indent_level, const std::string& func_name, 
                    size_t expected_size, const char *fmt, va_list msg_list) = 0;

            virtual int logFunc(LogGroup group, size_t indent_level, const std::string& func_name,
                    MarkType type) = 0;

        protected:
            /**
             * 添加时间戳
             *
             * @param sink 日志流汇聚
             *
             */ 
            virtual unsigned short addTimeStamp(std::ostream& sink);

            /**
             * 缩进函数名
             *
             * @param sink 日志流汇聚
             * @param indent_level 缩进量
             * @param func_name 函数名
             * @param type 标记类型
             *
             */ 
            virtual unsigned short indentFuncName(std::ostream& sink, size_t indent_level, const std::string& func_name,
                    MarkType type);

            char *formatMsg(size_t expected_size, const char *fmt, va_list msg_list, bool& release);

        public:
            static const unsigned int LOGGER_MAXLINE = 1024;

        protected:
            /// 启用的日志分组
            unsigned long m_Groups;

            /// 日志文件名
            std::string m_LogFName;

            /// 程序名称
            std::string m_AppName;

            /// 时间戳启用标志
            bool m_TimestampFlag;

            /// 时区
            int m_Timezone;

            /// 格式化日志信息的静态缓冲区
            static char m_MsgBuf[LOGGER_MAXLINE];
    };
}

#endif //_LOGGER_IMPLEMENT_H
