/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: StdoutLogger.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月17日 13时38分24秒
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
#include <stdarg.h>

#include "StdoutLogger.h"

using namespace HELIUM;

inline int CStdoutLogger::logOpen(unsigned long groups)
{
    m_Groups = groups;
    return 0;
}

void CStdoutLogger::logResync(void)
{
    std::cout << std::flush;
}

inline int CStdoutLogger::logClose(void)
{
    return 0;
}

int CStdoutLogger::logMsg(LogGroup group, size_t indent_level, const std::string& func_name, 
                    size_t expected_size, const char *fmt, va_list msg_list)
{
    bool release = false;
    char *msg_ptr = NULL;

    if (groupEnabled(group) == false)
        return 0;

    addTimeStamp(std::cout);
    indentFuncName(std::cout, indent_level, func_name, FUNC_MSG);

    msg_ptr = formatMsg(expected_size, fmt, msg_list, release);
    if (msg_ptr == NULL)
        return -1;

    std::cout << msg_ptr;

    if (release == true)
        delete [] msg_ptr;

    return 0;
}

int CStdoutLogger::logFunc(LogGroup group, size_t indent_level, const std::string& func_name,
                    MarkType type)
{
    if (groupEnabled(group) == false)
        return 0;

    addTimeStamp(std::cout);
    indentFuncName(std::cout, indent_level, func_name, type);

    if (type == FUNC_ENTRY)
    {
        std::cout << "-----vvvvvv-----" << std::endl;
    }
    else if (type == FUNC_EXIT)
    {
        std::cout << "-----^^^^^^-----" << std::endl;
    }

    return 0;
}
