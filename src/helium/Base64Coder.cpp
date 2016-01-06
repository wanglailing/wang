/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Base64Coder.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月18日 11时18分09秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Base64Coder.h"

using namespace HELIUM;

const std::string CBase64Coder::m_Base64Map = std::string("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/");

std::string* CBase64Coder::encode(const unsigned char* buf, unsigned int len)
{
    unsigned int i,a,x,b;
    std::string* result = new std::string("");

    a = 0;
    b = 0;

    for (i = 0; i < len; i++)
    {
        x = buf[i];
        b = (b << 8) + x;
        a += 8;
        while (a >=6)
        {
            a -= 6;
            x = b / (1 << a);
            b %= (1 << a);
            *result += m_Base64Map[x];
        }
    }

    if (a > 0)
    {
        x = b << (6 - a);
        *result += m_Base64Map[x];
    }

    return result;
}

int CBase64Coder::decode(std::string* msg, CBuffer* output_buf)
{
    unsigned int i, a, x, b;

    a = 0;
    b = 0;

    for (i = 0; i < msg->length(); i++)
    {
        x = m_Base64Map.find(msg->at(i), 0);
        if(x != std::string::npos)
        {
            b = b * 64 + x;
            a += 6;
            if (a >= 8)
            {
                a -= 8;
                x = b >> a;
                b %= (1 << a);
                x %= 256;
                output_buf->append((char *)&x, 1);
            }
        }
        else
            break;
    }

    return 0;
}

