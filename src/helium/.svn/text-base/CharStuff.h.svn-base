/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CharStuff.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月16日 10时33分43秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _CHAR_STUFF_H
#define _CHAR_STUFF_H

#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "Singleton.h"

namespace HELIUM
{
    /**
     * 字符填充(封装数据包)
     *
     */
    class CCharStuffer : public CSingleton<CCharStuffer>
    {
        public:
            CCharStuffer() {}
            ~CCharStuffer() {}

            /**
             * 插入边界字符和转义字符
             *
             * @param dst        目标缓冲区
             * @param dst_count  目标缓冲区的容量
             * @param src        原始数据
             * @param srclen     原始数据的长度
             *
             * @return 字符填充后的长度
             */
            int enstuff(char *dst, int dst_count, const char *src, int srclen);

            /**
             * 去除填充的字符(还原一帧数据)
             *
             * @param dst        目标缓冲区
             * @param dst_count  目标缓冲区的容量
             * @param src        原始数据
             * @param srcpos     查找的起始位置
             * @param srclen     原始数据的长度
             *
             * @return 原始数据帧的长度
             */
            int destuff(char *dst, int dst_count, const char *src, int *srcpos, int srclen);

        public:
            /// 边界字符
            static const char Boundary;

            /// 转义字符
            static const char Escape;
    };

    #define CHAR_STUFFER CCharStuffer::getInstance()
}

#endif //_CHAR_STUFF_H
