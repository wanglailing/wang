/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GNetScanner.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月09日 13时10分38秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "GNetScanner.h"
#include "Logger.h"

using namespace HELIUM;
using namespace EXTSERVICE;

CGNetScan::CGNetScan() : m_RxQual(0xFF), m_ModuleAid(0), m_ModuleBid(0), m_ModuleAmode(0xFFFF), m_ModuleBmode(0xFFFF)
{
    memset(m_BcchArfcns, 0, sizeof(m_BcchArfcns));
    m_SmallestFrmLen = sizeof(FrameHeader) + sizeof(FrameTail);
}

CGNetScan::~CGNetScan()
{
}

void CGNetScan::parseFrame(uint16_t module_id, const char* data, int length)
{
    if (data == NULL || length < m_SmallestFrmLen)
        return;

    /**
     * 验证数据帧的格式
     */
    const FrameHeader* pFrmHdr = reinterpret_cast<const FrameHeader*>(data);
    uint8_t appid = pFrmHdr->modeid & 0x03;
    if (pFrmHdr->stx != 0x02 || appid != 0x02)
    {
        DBG_LOG((DATAWATCHER, "header error: stx=%02X, appid=%02X\n", pFrmHdr->stx, appid));
        return;
    }

    int msglen = ntohs(pFrmHdr->msglen);
    if ((msglen + 6) > length)
    {
        DBG_LOG((DATAWATCHER, "length error: msglen=%d\n", msglen));
        return;
    }

    const FrameTail* pFrmTail = reinterpret_cast<const FrameTail*>(data + msglen + 4);
    if (pFrmTail->etx != 0x03)
    {
        DBG_LOG((DATAWATCHER, "etx error: etx=%02X\n", pFrmTail->etx));
        return;
    }

    uint8_t mode = pFrmHdr->modeid >> 6;
    uint8_t submode = (pFrmHdr->modeid >> 2) & 0x0F;
    uint16_t workmode = mode + ((uint16_t)submode << 8);
    if (module_id == m_ModuleBid)
    {
        if (m_ModuleBmode != workmode)
        {
            m_ModuleBmode = workmode;
            DBG_LOG((DATAWATCHER, "Module ID=%04X, Work Mode=%04X\n", module_id, workmode));
        }
    }
    else if (module_id == m_ModuleAid)
    {
        if (m_ModuleAmode != workmode)
        {
            m_ModuleAmode = workmode;
            DBG_LOG((DATAWATCHER, "Module ID=%04X, Work Mode=%04X\n", module_id, workmode));
        }
    }
    else
    {
        DBG_LOG((DATAWATCHER, "Unknown Module ID = %04X\n", module_id));
        return;
    }


    /**
     * 解析数据帧的内容
     */
    if (pFrmHdr->msgid == 0x0073) /// 实时数据
    {
        DBG_LOG((DATAWATCHER, "Msg ID=%04X, ResvMode=%02X\n", pFrmHdr->msgid, pFrmHdr->resvmode));

        /// 待机数据
        if (pFrmHdr->resvmode == 0x00)
        {
            /// 第196个字节为信号质量(RxQual)
            m_RxQual = data[195];
            DBG_LOG((DATAWATCHER, "RxQual=%02X\n", m_RxQual));
        }
        /// 待机状态发往DMS的数据
        else if (pFrmHdr->resvmode == 0x03)
        {
            const IdleData* pRealtimeDat = reinterpret_cast<const IdleData*>(data + sizeof(FrameHeader));
            gatherArfcns<const IdleData*>(pRealtimeDat);
        }
        /// 通话状态发往DMS的数据
        else if (pFrmHdr->resvmode == 0x04)
        {
            const DedicatedData* pRealtimeDat = reinterpret_cast<const DedicatedData*>(data + sizeof(FrameHeader));
            gatherArfcns<const DedicatedData*>(pRealtimeDat);
        }
    }
    /// RF扫描的结果
    else if (pFrmHdr->msgid == 0x0153 && pFrmHdr->resvmode == 0x03)
    {
        uint16_t arfcn_num;
        ::memcpy(&arfcn_num, data + sizeof(FrameHeader), sizeof(uint16_t));
        arfcn_num = ntohs(arfcn_num);

        for (int i = 0; i < 7; ++i)
        {
            RFTrace rf_trace;
            ::memcpy(&rf_trace, data + sizeof(FrameHeader) + 2 + i * sizeof(rf_trace), sizeof(rf_trace));
            //DBG_LOG((DATAWATCHER, "<%d>Arfcn=%d, rx_lev=%d\n", i, ntohs(rf_trace.arfcn), rf_trace.rx_lev));
        }
    }
}

bool CGNetScan::isArfcnsChanged(const uint16_t* arfcns)
{
    if (arfcns == NULL)
        return false;

    /// 第1个总是为服务小区
    if (m_BcchArfcns[0] != arfcns[0])
        return true;

    for (int i = 1; i < 7; ++i)
    {
        if (findArfcn(m_BcchArfcns[i], &arfcns[1]) == false)
            return true;
    }

    return false;
}

bool CGNetScan::findArfcn(uint16_t bcch, const uint16_t* arfcns)
{
    for (int i = 0; i < 6; ++i)
    {
        if (bcch == *(arfcns + i))
            return true;
    }

    return false;
}

void CGNetScan::dumpArfcns(void) const
{
    DBG_LOG((DATAWATCHER, "[Serving Cell] %d:%d", m_BcchArfcns[0] & 0x7fff, m_BcchArfcns[0] >> 15));
    for (int i = 1; i < 7; ++i)
    {
        DBG_LOG((DATAWATCHER, "[Neighbour-%d] %d:%d", i, m_BcchArfcns[0] & 0x7fff, m_BcchArfcns[0] >> 15));
    }
}

