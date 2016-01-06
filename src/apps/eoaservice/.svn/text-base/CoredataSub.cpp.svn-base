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
#include "EoaservApp.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace EXTSERVICE;

CCoredataSub::CCoredataSub()
{
    atpTypeToNum();

    memset(&m_ParsedAtp, 0, sizeof(m_ParsedAtp));
    m_ParsedAtp.trainid = APPCONFIG->getTrainIDNum();
    m_ParsedAtp.atptype = (uint8_t)m_AtpNum;
    
    memset(&m_RealtimeAtp, 0, sizeof(m_RealtimeAtp));
}

CCoredataSub::~CCoredataSub()
{
}

void CCoredataSub::atpTypeToNum(void)
{
    std::string atp_type = APPCONFIG->getAtpType();
    if (atp_type == "300S")
        m_AtpNum = ATP300S;
    else if (atp_type == "200C")
        m_AtpNum = ATP200C;
    else if (atp_type == "200H")
        m_AtpNum = ATP200H;
    else if (atp_type == "300D")
        m_AtpNum = ATP300D;
    else if (atp_type == "300H")
        m_AtpNum = ATP300H;
    else if (atp_type == "200T")
        m_AtpNum = ATP200T;
    else
        m_AtpNum = ATP300T;
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

    int offset = sizeof(CAPSUL_HEADER) + sizeof(SUBPACK_HEADER);
    memcpy(&m_RealtimeAtp, data + offset, sizeof(m_RealtimeAtp));
}

PARSED_ATP* CCoredataSub::getParsedAtp(void)
{
    struct timespec tp = {0, 0};
    ::clock_gettime(CLOCK_REALTIME, &tp);
    m_ParsedAtp.dmsecs = tp.tv_sec;
    m_ParsedAtp.dmsnaosecs = tp.tv_nsec;

    m_ParsedAtp.mileage  = m_RealtimeAtp.opbase.basic.mileage;
    m_ParsedAtp.rundist  = m_RealtimeAtp.opbase.basic.runDistance;
    m_ParsedAtp.ebspeed  = (uint16_t)(m_RealtimeAtp.opbase.basic.speedInfo >> 20);
    m_ParsedAtp.runspeed = (uint16_t)(m_RealtimeAtp.opbase.basic.speedInfo & 0x3ff);

    encodeAtpTime(m_ParsedAtp.atptime, m_RealtimeAtp.opbase.basic.dateTime);

    m_ParsedAtp.brake    = m_RealtimeAtp.opbase.basic.brake;

    m_ParsedAtp.driverno = m_RealtimeAtp.opbase.trainop.driverid;
    memcpy(m_ParsedAtp.trainstr, &(m_RealtimeAtp.opbase.trainop.trainum), 7);
    m_ParsedAtp.trainum = m_RealtimeAtp.enctrain;

    m_ParsedAtp.baliseno = m_RealtimeAtp.balise.passbalino;
    m_ParsedAtp.passdist = m_RealtimeAtp.balise.passdist;

    return &m_ParsedAtp; 
}

void CCoredataSub::encodeAtpTime(uint8_t* buf, uint32_t atp_time)
{
    uint8_t date_time[6];
    date_time[0] = (uint8_t)(atp_time >> 26);            /// 年(自2000年以来经历的年数)
    date_time[1] = (uint8_t)(atp_time >> 22 & 0xf);      /// 月
    date_time[2] = (uint8_t)(atp_time >> 17 & 0x1f);     /// 日
    date_time[3] = (uint8_t)(atp_time >> 12 & 0x1f);     /// 时
    date_time[4] = (uint8_t)(atp_time >> 6  & 0x3f);     /// 分
    date_time[5] = (uint8_t)(atp_time & 0x3f);           /// 秒

    buf[0] = 0x20;
    for (int i = 1; i < 7; ++i)
    {
        buf[i] = convertDecToBCD(date_time[i - 1]);
    }
}

uint8_t CCoredataSub::convertDecToBCD(uint8_t decimal)
{
    if (decimal > 99)
        return 0;

    return (((decimal / 10) << 4) + (decimal % 10));
}

