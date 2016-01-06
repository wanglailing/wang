/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: BaliseParser.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月28日 15时27分42秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>

#include "BaliseParser.h"
#include "Utils.h"

using namespace COREBUSINESS;

CBaliseParser::HDR_LAYOUT CBaliseParser::HdrFieldsLayout[] = {
    {HDRFIELD_QUPDOWN,  1, 0},
    {HDRFIELD_MVERSION, 7, 0}, 
    {HDRFIELD_QMEDIA,   1, 0},
    {HDRFIELD_NPIG,     3, 0},
    {HDRFIELD_NTOTAL,   3, 0},
    {HDRFIELD_MDUP,     2, 0},
    {HDRFIELD_MMCOUNT,  8, 0},
    {HDRFIELD_NIDC,    10, 0},
    {HDRFIELD_NIDBG,   14, 0},
    {HDRFIELD_QLINK,    1, 0}
};

unsigned int CBaliseParser::getBaliseNo(const unsigned char* balise, int length)
{
    int total_bitlen = length << 3;
    int bitoffset = 0;

    for (unsigned int i = 0; i < COUNTOF(HdrFieldsLayout); ++i)
    {
        HdrFieldsLayout[i].value = ConvertBitsToUInt32(balise, total_bitlen, bitoffset, HdrFieldsLayout[i].bitlen);
        bitoffset += HdrFieldsLayout[i].bitlen;
    }

    unsigned int result = (unsigned int)HdrFieldsLayout[HDRFIELD_NIDBG].value | 
                          (unsigned int)HdrFieldsLayout[HDRFIELD_NIDC].value   << 14 |
                          (unsigned int)HdrFieldsLayout[HDRFIELD_NTOTAL].value << 24 |
                          (unsigned int)HdrFieldsLayout[HDRFIELD_NPIG].value   << 27; 
    return result;
}

void CBaliseParser::dumpBaliseNo(unsigned int baliseno, char* dump_buf, int buf_len)
{
    if (dump_buf == NULL)
        return;

    ::snprintf(dump_buf, buf_len, "%03d-%d-%02d-%03d-%d [%d]", 
            (baliseno >>  17) & 0x7F,        /// 大区编号
            (baliseno >>  14) & 0x07,        /// 分区编号
            (baliseno >>   8) & 0x3F,        /// 车站编号
            (baliseno       ) & 0xFF,        /// 应答器编号
            ((baliseno >> 27) & 0x07) + 1,   /// 在应答器组中的位置
            ((baliseno >> 24) & 0x07) + 1    /// 应答器数
            );
}

