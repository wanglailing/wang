/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CoredataSub.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月20日 15时33分15秒
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
#include <time.h>

#include "Logger.h"
#include "MemDump.h"

#include "CoredataSub.h"
#include "TcaservApp.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace EXTSERVICE;

CCoredataSub::CCoredataSub()
{
    initTcasFrame();
    
    memset(&m_RealtimeAtp, 0, sizeof(m_RealtimeAtp));
    memset(&m_RealtimeGps, 0, sizeof(m_RealtimeGps));
}

CCoredataSub::~CCoredataSub()
{
}

int CCoredataSub::open(HELIUM::CReactor* r)
{
    (void)r;
    return 0;
}

void CCoredataSub::close(void)
{
}

int CCoredataSub::handleRead(int fd)
{
    int save_error;
    int read_len = m_InputBuffer.readFd(fd, &save_error);
    if (read_len < 0)
    {
        DBG_LOG((SOCKET, "Core pub read error!\n"));
        return -1;
    }
    else if (read_len == 0)
    {
        DBG_LOG((SOCKET, "Peer has dropped connection!\n"));
        return -1;
    }
    else
    {
        uint16_t frame_length;
        const char* begin_pos = m_InputBuffer.peek();
        const char* end_pos = begin_pos + m_InputBuffer.readableBytes();
        while (begin_pos < end_pos)
        {
            const char* frame_pos;
            if ((frame_pos = CExchangePacket::findFrame(begin_pos, end_pos, &frame_length)) == NULL)
                break;

            parseFrame(frame_pos, (int)frame_length);

            begin_pos = frame_pos + frame_length + sizeof(uint32_t);
            m_InputBuffer.retrieveUntil(begin_pos);
        }
    }

    return 0;
}

void CCoredataSub::parseFrame(const char* data, int length)
{
    (void)length;

    int offset = sizeof(CAPSUL_HEADER);
    memcpy(&m_RealtimeAtp, data + offset, sizeof(m_RealtimeAtp));
    offset += sizeof(m_RealtimeAtp);
    memcpy(&m_RealtimeGps, data + offset, sizeof(m_RealtimeGps));
}

TcasFrame* CCoredataSub::getTcasFrame(TCAS_DIRECT direction) 
{
    static uint64_t timenow;

    if (direction == TO_SERVER)
    {
        m_TcasFrame.header.seq++;

        uint8_t time_elapsed;
        timenow = CTimeVal::getMonotonicMs();

        time_elapsed = (timenow - m_RealtimeAtp.stamps.trainum) / 10000;
        m_TcasFrame.operate.aging   = (time_elapsed <= 240) ? time_elapsed : 255;
        m_TcasFrame.operate.trainum = m_RealtimeAtp.enctrain;

        time_elapsed = (timenow - m_RealtimeAtp.stamps.balise) / 10000;
        m_TcasFrame.balise.aging       = (time_elapsed <= 240) ? time_elapsed : 255;
        m_TcasFrame.balise.passbalino  = m_RealtimeAtp.balise.passbalino;
        m_TcasFrame.balise.passdist    = m_RealtimeAtp.balise.passdist;
        m_TcasFrame.balise.priorbalino = m_RealtimeAtp.balise.priorbalino;

        time_elapsed = (timenow - m_RealtimeGps.stamp) / 10000;
        m_TcasFrame.gpspos.aging     = (time_elapsed <= 240) ? time_elapsed : 255;
        m_TcasFrame.gpspos.longitude = m_RealtimeGps.params.longitude;
        m_TcasFrame.gpspos.latitude  = m_RealtimeGps.params.latitude;
        m_TcasFrame.gpspos.speed     = m_RealtimeGps.params.speed;

        time_elapsed = (timenow - m_RealtimeAtp.stamps.atpspeed) / 10000;
        m_TcasFrame.atp.aging = (time_elapsed <= 240) ? time_elapsed : 255;
        m_TcasFrame.atp.speed = (uint16_t)(m_RealtimeAtp.opbase.basic.speedInfo & 0x3ff);
    }

    m_TcasFrame.base.aging     = 0;
    m_TcasFrame.base.timestamp = timenow;
    m_TcasFrame.base.rundist   = m_RealtimeAtp.opbase.basic.runDistance;

    return &m_TcasFrame;
}

void CCoredataSub::initTcasFrame(void)
{
    memset(&m_TcasFrame, 0, sizeof(m_TcasFrame));
    m_TcasFrame.header.flag     = 0x5A5A;
    m_TcasFrame.header.len      = sizeof(TcasFrame);
    m_TcasFrame.header.protover = 0x0020;
    m_TcasFrame.header.seq      = 0xFFFF;

    m_TcasFrame.base.type    = 0x21;
    m_TcasFrame.base.len     = sizeof(BaseData);
    m_TcasFrame.base.aging   = 255;
    m_TcasFrame.base.trainid = APPCONFIG->getTrainIDNum();

    m_TcasFrame.operate.type  = 0x22;
    m_TcasFrame.operate.len   = sizeof(OpData);
    m_TcasFrame.operate.aging = 255;

    m_TcasFrame.balise.type  = 0x23;
    m_TcasFrame.balise.len   = sizeof(BaliseData);
    m_TcasFrame.balise.aging = 255;

    m_TcasFrame.gpspos.type  = 0x24;
    m_TcasFrame.gpspos.len   = sizeof(GpsPosData);
    m_TcasFrame.gpspos.aging = 255;

    m_TcasFrame.atp.type  = 0x25;
    m_TcasFrame.atp.len   = sizeof(AtpData);
    m_TcasFrame.atp.aging = 255;

    m_TcasFrame.datetime.type = 0x2E;
    m_TcasFrame.datetime.len  = sizeof(DateTimeData);
}

