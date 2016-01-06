/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CharStuff.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月16日 13时54分39秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "CharStuff.h"

using namespace HELIUM;

HELIUM_DECL_SINGLETON(CCharStuffer);

const char CCharStuffer::Boundary = 0x7E;
const char CCharStuffer::Escape   = 0x7D;

int CCharStuffer::enstuff(char *dst, int dst_count, const char *src, int srclen)
{
    int i = 0;
    dst[i++] = Boundary;

    for (int j = 0; j < srclen; ++j)
    {
        if ((i + 2) >= dst_count)
            break;

        if (src[j] == Boundary)
        {
            dst[i++] = Escape;
            dst[i++] = 0x5E;
        }
        else if (src[j] == Escape)
        {
            dst[i++] = Escape;
            dst[i++] = 0x5D;
        }
        else
        {
            dst[i++] = src[j];
        }
    }
    dst[i++] = Boundary;

    return i;
}

int CCharStuffer::destuff(char *dst, int dst_count, const char *src, int *srcpos, int srclen)
{
    bool had_head = false;
    int dst_len = 0;
    for (int i = *srcpos; i < srclen; ++i)
    {
        if (src[i] == Boundary)
        {
            if (had_head == true)
            {
                *srcpos = i + 1;
                return dst_len;
            }
            else
            {
                had_head = true;
            }
        }
        else if (src[i] == Escape)
        {
            if (had_head == true && dst_len < dst_count && ((i + 1) < srclen))
            {
                if (src[i + 1] == 0x5E)
                    dst[dst_len++] = Boundary;
                else
                    dst[dst_len++] = Escape;

                i++;
            }
        }
        else
        {
            if (had_head == true && dst_len < dst_count)
            {
                dst[dst_len++] = src[i];
            }
        }
    }

    return -1;
}

