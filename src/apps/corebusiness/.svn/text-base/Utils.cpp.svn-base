/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Utils.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月17日 13时28分49秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Utils.h"
#include "BaliseParser.h"

uint32_t ConvertBitsToUInt32(const unsigned char* buf, int total_bitlen, int bitpos, int bitlen)
{
    if (buf == NULL || bitpos < 0 || bitpos >= total_bitlen || bitlen <= 0 || bitlen > 32)
        return 0;

    if ((bitpos + bitlen) > total_bitlen)
        bitlen = total_bitlen - bitpos;

    register uint32_t result = 0;

    int bytepos = bitpos / 8;
    int bitoffset = bitpos % 8;

    while (bitlen > 0)
    {
        int processed = 8 - bitoffset;
        if (processed >= bitlen)
        {
            result = (result << bitlen) | ( (unsigned char)(buf[bytepos] << bitoffset) >> (8 - bitlen) );
            break;
        }
        else
        {
            result = (result << processed) | ( (unsigned char)(buf[bytepos++] << bitoffset) >> bitoffset );
            bitlen -= processed;
            bitoffset = 0;
        }
    }

    return result;
}

uint8_t ConvertBCDToDecimal(uint8_t bcd)
{
    return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

uint16_t ConvertBCDToDecimal(uint16_t bcd)
{
    return ( bcd             >> 12) * 1000  +
           ((bcd & 0x0f00)   >>  8) * 100   +
           ((bcd & 0x0f0)    >>  4) * 10    +
           ((bcd & 0x0f));
}

uint32_t ConvertBCDToDecimal(uint32_t bcd)
{
    return ((bcd & 0x0f0000) >> 16) * 10000 +
           ((bcd & 0x0f000)  >> 12) * 1000  +
           ((bcd & 0x0f00)   >>  8) * 100   +
           ((bcd & 0x0f0)    >>  4) * 10    +
           ((bcd & 0x0f));
}

int updateBaliseInfo(const unsigned char* balise, int length, uint64_t now, AtpRTInfo* atpinfo)
{
    int changed = 1;
    unsigned int balise_no = COREBUSINESS::CBaliseParser::getBaliseNo(balise, length);

    /// 当应答器组变化时，再更新上一个过应答器编号
    if ((balise_no & 0x0FFFFFF) != (atpinfo->balise.passbalino & 0x0FFFFFF))
    {
        if ((now - atpinfo->stamps.balise) >= 240000)
        {
            atpinfo->balise.priorbalino = 0;
        }
        else
        {
            atpinfo->balise.priorbalino = atpinfo->balise.passbalino;
        }

        changed = 2;
    }

    atpinfo->balise.passbalino = balise_no;
    atpinfo->stamps.balise = now;

    return changed;
}

