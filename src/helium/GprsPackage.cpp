/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GprsPackage.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月31日 09时26分43秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "GprsPackage.h"
#include "CharStuff.h"

using namespace HELIUM;

const int CGprsPackage::BuffLength = 2048;

CGprsPackage::CGprsPackage(bool stuff_data) : m_bStuffGprs(stuff_data)
{
    if (stuff_data == true)
    {
        m_StuffedBuf   = new char[BuffLength];
        m_DestuffedBuf = new char[BuffLength];
    }
    else
    {
        m_StuffedBuf = NULL;
        m_DestuffedBuf = NULL;
    }
}

CGprsPackage::~CGprsPackage()
{
    if (m_StuffedBuf)
    {
        delete [] m_StuffedBuf; m_StuffedBuf = NULL;
    }

    if (m_DestuffedBuf)
    {
        delete [] m_DestuffedBuf; m_DestuffedBuf = NULL;
    }
}

void CGprsPackage::parseData(unsigned short source, const char *buf, int length, void* response)
{
    if (buf == NULL || length <= 0)
        return;

    if (m_bStuffGprs == true)
    {
        int dat_len;
        int searchpos = 0;
        do {
            dat_len = CHAR_STUFFER->destuff(m_DestuffedBuf, BuffLength, buf, &searchpos, length);
            if (dat_len > 1)
            {
                if (m_DestuffedBuf[0] == CHANNEL_TOCPU_SERVDAT)
                {
                    procBusinessData(source, &m_DestuffedBuf[1], dat_len - 1, response);
                }
            }
        } while (dat_len >= 0);
    }
    else
    {
        procBusinessData(source, buf, length, response);
    }
}

char* CGprsPackage::stuffData(char *src, int srclen, int* stuffed_len, char channel_type)
{
    if (m_bStuffGprs == true)
    {
        /// 第一个字节为通道类型
        src[0] = channel_type;
        *stuffed_len = CHAR_STUFFER->enstuff(m_StuffedBuf, BuffLength, src, srclen);
        return m_StuffedBuf;
    }
    else
    {
        *stuffed_len = srclen - 1;
        return (src + 1);
    }
}

