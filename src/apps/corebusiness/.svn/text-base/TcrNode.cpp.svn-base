/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TcrNode.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月26日 13时14分40秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <string.h>

#include "TcrNode.h"

using namespace HELIUM;
using namespace COREBUSINESS;

CTcrNode::CTcrNode(int interval) : CDataNode(interval), m_TrackVoltage(0xff)
{
    memset(&m_TcrInfo, 0, sizeof(m_TcrInfo));
}

CTcrNode::~CTcrNode()
{
}

void CTcrNode::encodeTcrInfo(float carrfreq, float lowfreq)
{
    uint8_t status = 0;

    status &= 0xF3;
    if ((carrfreq > 1650 && carrfreq < 1750) || (carrfreq > 2250 && carrfreq < 2350))
    {
        status |= 0x04;
    }
    else if ((carrfreq > 1950 && carrfreq < 2050) || (carrfreq > 2550 && carrfreq < 2650))
    {
        status |= 0x08;
    }
    else
    {
        status |= 0x0C;
    }

    status &= 0xFC;
    if ((lowfreq >= 16.9 - 0.2) && (lowfreq <= 16.9 + 0.2)) 
    {
        status |= 0x01;    /// 正线停车(U)
    }
    else if ( ((lowfreq >= 19.1 - 0.2) && (lowfreq <= 19.1 + 0.2)) || 
              ((lowfreq >= 18.0 - 0.2) && (lowfreq <= 18.0 + 0.2)) )
    {
        status |= 0x02;    /// 侧线停车(UU, UUS)
    }
    else
    {
        status |= 0x03;    /// 其它灯码
    }
}

int CTcrNode::parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    (void)nodes_buf;
    (void)checkTimeliness(now);

    if (len == sizeof(TCRPROTO_DATA))
    {
        TCRPROTO_DATA* tcr_data = (TCRPROTO_DATA*)buf;
        m_TcrInfo.carrfreq = tcr_data->carrfreq * 0.1f;
        m_TcrInfo.lowfreq  = tcr_data->lowfreq * 0.01f;
        m_TcrInfo.voltage  = tcr_data->sigampling1 * 0.05f;
        m_TrackVoltage = (unsigned char)m_TcrInfo.voltage;
        m_Status = (uint8_t)STATUS_OK;
    }
    else
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
    }

    return 0;
}

void CTcrNode::dump(void)
{
    static int seq;
    DBG_LOG((DATAWATCHER, "-------------------- TCR:%d --------------------\n", seq++));
    DBG_LOG((DATAWATCHER, "Carrier Freq: %f\n", 0.0f));
    DBG_LOG((DATAWATCHER, "Low     Freq: %f\n", 0.0f));
    DBG_LOG((DATAWATCHER, "Voltage     : %f\n", m_TcrInfo.voltage));
}

unsigned char* CTcrNode::getParsedInfo(int* length)
{
    (void)length;
    return NULL;
}

void CTcrNode::clearParsedInfo(void)
{
    return;
}

