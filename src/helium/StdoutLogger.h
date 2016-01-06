/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: StdoutLogger.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月17日 13时25分49秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _STDOUT_LOGGER_H
#define _STDOUT_LOGGER_H

#include <fstream>
#include <iostream>
#include <string>

#include "LoggerImplement.h"

namespace HELIUM
{
    /**
     * 定位到标准输出的日志类
     *
     */
    class CStdoutLogger : public CLoggerImpl
    {
        public:
            CStdoutLogger() {}

            virtual int logOpen(unsigned long groups);
            virtual void logResync(void); 
            virtual int logClose(void);

            virtual int logMsg(LogGroup group, size_t indent_level, const std::string& func_name, 
                    size_t expected_size, const char *fmt, va_list msg_list);

            virtual int logFunc(LogGroup group, size_t indent_level, const std::string& func_name,
                    MarkType type);

        private:
            /// 禁止拷贝
            CStdoutLogger(const CStdoutLogger&);
            CStdoutLogger& operator=(const CStdoutLogger&);
    };
}

#endif //_STDOUT_LOGGER_H
