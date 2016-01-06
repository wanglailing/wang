/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TcasProcessor.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年06月19日 14时33分15秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <string>
#include <string.h>
#include <time.h>

#include "Buffer.h"
#include "DataCoder.h"
#include "TimeVal.h"
#include "Packet.h"

#include "TcasProcessor.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

CTcasProc::CTcasProc(uint32_t train_id)
{
    initTcasFrame(train_id);
    
    memset(&m_RealtimeAtp, 0, sizeof(m_RealtimeAtp));
    memset(&m_RealtimeAtp.stamps, 0xFF, sizeof(m_RealtimeAtp.stamps));
    memset(&m_RealtimeGps, 0, sizeof(m_RealtimeGps));
    memset(&m_RealtimeGps.stamp, 0xFF, sizeof(m_RealtimeGps.stamp));
}

CTcasProc::~CTcasProc()
{
}

void CTcasProc::parseDataStatus(const DataStatus& data_status)
{
    m_TcasFrame.base.devstate &= 0x3F;
    switch (data_status.gps)
    {
        case 2:
            break;
        case 1:
            m_TcasFrame.base.devstate |= 0x80;
            break;
        default:
            m_TcasFrame.base.devstate |= 0x40;
            break;
    }

    /**
     * ATP的C2或C3正常，则认为ATP正常；其它情况取C3下的状态
     *
     */
    uint8_t atp_status;
    if (data_status.atp == 2 || data_status.atpc2 == 2)
    {
        atp_status = 2;
    }
    else
    {
        atp_status = data_status.atp;
    }

    m_TcasFrame.base.devstate &= 0xCF;
    switch (atp_status)
    {
        case 2:
            break;
        case 1:
            m_TcasFrame.base.devstate |= 0x10;
            break;
        default:
            m_TcasFrame.base.devstate |= 0x20;
            break;
    }
}

void CTcasProc::parseCoreData(const char* data, int length)
{
    if (length == (int)(sizeof(m_RealtimeAtp) + sizeof(m_RealtimeGps) + sizeof(DataStatus)))
    {
        int offset = 0;
        memcpy(&m_RealtimeAtp, data + offset, sizeof(m_RealtimeAtp));
        offset += sizeof(m_RealtimeAtp);
        memcpy(&m_RealtimeGps, data + offset, sizeof(m_RealtimeGps));

        offset += sizeof(m_RealtimeGps);

        DataStatus data_st;
        memcpy(&data_st, data + offset, sizeof(data_st));
        parseDataStatus(data_st);
    }
}

uint8_t CTcasProc::timeElapsed(uint64_t now, uint64_t stamp)
{
    uint8_t time_elapsed;
    if (now < stamp)
        return 255;

    time_elapsed = (now - stamp) / 10000;
    return ((time_elapsed <= 240) ? time_elapsed : 255);
}

void CTcasProc::getLocalTime(unsigned char* buf, int buflen)
{
    (void)buflen;

    struct tm local_tm;
    time_t now = time(NULL);
    localtime_r(&now, &local_tm);
    local_tm.tm_year += 1900;

    int i = 0;
    buf[i++] = local_tm.tm_year / 100;
    buf[i++] = local_tm.tm_year % 100;
    buf[i++] = local_tm.tm_mon + 1;
    buf[i++] = local_tm.tm_mday;
    buf[i++] = local_tm.tm_hour;
    buf[i++] = local_tm.tm_min;
    buf[i++] = local_tm.tm_sec;
}

TcasFrame* CTcasProc::getTcasFrame(TCAS_DIRECT direction) 
{
    static uint64_t timenow;

    if (direction == TO_SERVER)
    {
        m_TcasFrame.header.seq++;

        timenow = CTimeVal::getMonotonicMs();

        m_TcasFrame.operate.aging   = timeElapsed(timenow, m_RealtimeAtp.stamps.trainum);
        m_TcasFrame.operate.trainum = m_RealtimeAtp.enctrain;

        m_TcasFrame.balise.aging       = timeElapsed(timenow, m_RealtimeAtp.stamps.balise);
        m_TcasFrame.balise.passbalino  = m_RealtimeAtp.balise.passbalino;
        m_TcasFrame.balise.passdist    = m_RealtimeAtp.balise.passdist;
        m_TcasFrame.balise.priorbalino = m_RealtimeAtp.balise.priorbalino;

        m_TcasFrame.gpspos.aging     = timeElapsed(timenow, m_RealtimeGps.stamp);
        m_TcasFrame.gpspos.longitude = m_RealtimeGps.params.longitude;
        m_TcasFrame.gpspos.latitude  = m_RealtimeGps.params.latitude;
        m_TcasFrame.gpspos.speed     = m_RealtimeGps.params.speed;

        m_TcasFrame.atp.aging = timeElapsed(timenow, m_RealtimeAtp.stamps.atpspeed);
        m_TcasFrame.atp.speed = (uint16_t)(m_RealtimeAtp.opbase.basic.speedInfo & 0x3ff);
    }

    m_TcasFrame.base.aging     = 0;
    m_TcasFrame.base.timestamp = timenow;
    m_TcasFrame.base.rundist   = m_RealtimeAtp.opbase.basic.runDistance;

    getLocalTime((unsigned char*)&m_TcasFrame.datetime.year, sizeof(m_TcasFrame.datetime) - 2);

    return &m_TcasFrame;
}

int CTcasProc::encodePacket(char* buf, int buflen)
{
    CBuffer packet;
    TcasFrame* tcas_frame = getTcasFrame(TO_SERVER);
    packet.append(tcas_frame, sizeof(TcasFrame));

    std::string* enc_str = CDataCoder::encodeData(&packet);
    enc_str->insert(0, CDataCoder::TcasTagHead, 3);
    CDataCoder::wrapData(*enc_str, 3, CDataCoder::TcasTagTail, CDataCoder::TcasTagCrc);

    int encoded_len = enc_str->length();
    if (encoded_len > buflen)
    {
        delete enc_str;
        return 0;
    }

    memcpy(buf, enc_str->c_str(), encoded_len);

    delete enc_str;
    return encoded_len;
}

void CTcasProc::initTcasFrame(uint32_t train_id)
{
    memset(&m_TcasFrame, 0, sizeof(m_TcasFrame));
    m_TcasFrame.header.flag     = 0x5A5A;
    m_TcasFrame.header.len      = sizeof(TcasFrame);
    m_TcasFrame.header.protover = 0x0020;
    m_TcasFrame.header.seq      = 0xFFFF;

    m_TcasFrame.base.type    = 0x21;
    m_TcasFrame.base.len     = sizeof(BaseData);
    m_TcasFrame.base.aging   = 255;

    /// 车号的顺序为：车端、数字部分和车型(DMS是车型在高位，车端在低位)
    m_TcasFrame.base.trainid  = (train_id << 31) | (train_id >> 24) | ((train_id & 0xFFFFFE) << 7);
    m_TcasFrame.base.devstate = 0x50;

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

