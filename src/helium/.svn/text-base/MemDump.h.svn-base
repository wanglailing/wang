/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: MemDump.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月27日 15时48分54秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _MEM_DUMP_H
#define _MEM_DUMP_H

#include <stdio.h>
#include <string.h>

#include "Logger.h"

namespace HELIUM
{
    /**
     * 内存数据转储。将二进制内存数据转换成用户可见的16进制/ASCII格式的字符串。
     *
     */
    class CMemDump
    {
        public:
            explicit CMemDump(const char *msg, int len);
            ~CMemDump();

            /// 获取要转储的内存的地址
            const char *getMemDump(void) const;

            /**
             * 将转储后的内存写入日志
             *
             * @param mask 日志掩码
             * @param info 内存数据说明信息
             * @param msg  内存地址
             * @param len  内存数据长度
             *
             */
            static void dumpToLog(unsigned long mask, const char *info, const char *msg, int len);

        private:
            /// 转储数据的缓冲区
            char *m_Dump;
    };

    inline CMemDump::~CMemDump()
    {
        if (m_Dump)
        {
            delete m_Dump;
            m_Dump = NULL;
        }
    }

    inline const char* CMemDump::getMemDump(void) const
    {
        return (const char *)m_Dump;
    }
}

#endif //_MEM_DUMP_H
