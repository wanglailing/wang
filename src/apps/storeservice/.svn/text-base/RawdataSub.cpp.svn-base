/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: RawdataSub.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月31日 17时17分17秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "RawdataSub.h"
#include "DataRecorder.h"
#include "SharedBuffer.h"
#include "StoreApp.h"
#include "PowerdropDetect.h"

#include "CharStuff.h"
#include "MemDump.h"
#include "Logger.h"
#include "Packet.h"
#include "TimeVal.h"
#include "../JruProto.h"

using namespace HELIUM;
using namespace STORESERVICE;

CRawdataSub::CRawdataSub() : m_CirId(DMS_CIR), m_GpsId(DMS_GPS), m_AtpId(ATP_300T), 
    m_AtpC2Id(ATP_300S_C2), m_GNetId(DMS_GNET), m_BatteryId(DMS_BATT), m_RecvGpsTimestamp(0)
{
    DataId data_id;
    if ((data_id = APPCONFIG->getSlaveDevType("CIR")) != 0)
        m_CirId = data_id;

    if ((data_id = APPCONFIG->getSlaveDevType("GPS")) != 0)
        m_GpsId = data_id;

    if ((data_id = APPCONFIG->getSlaveDevType("ATP300T")) != 0)
        m_AtpId = data_id;

    if ((data_id = APPCONFIG->getSlaveDevType("ATP300S_C2")) != 0)
        m_AtpC2Id = data_id;

    if ((data_id = APPCONFIG->getSlaveDevType("GNETMON")) != 0)
        m_GNetId = data_id;

    if ((data_id = APPCONFIG->getSlaveDevType("BATTERY")) != 0)
        m_BatteryId = data_id;

    m_AtpHeader.timeflag  = 0xFF;
    m_AtpHeader.timestamp = 0.0f;
    m_AtpHeader.gpsflag   = 0xEE;
    m_AtpHeader.gpstatus  = STATUS_TIMEOUT;
    m_AtpHeader.gpspeed   = 0;
}

CRawdataSub::~CRawdataSub()
{
}

void CRawdataSub::processData(const char* data, int length)
{
    time_t clock_now;
    ::time(&clock_now);

    uint64_t mono_timenow = CTimeVal::getMonotonicMs();

    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = (data + offset + 4);
        const char* packet_ptr  = (data + offset);
        int payload_len = subpack_hdr->len - 4;

        if (subpack_hdr->dataid == m_CirId)
        {
            storeCirData(payload_ptr, payload_len, clock_now);
        }
        else if ((uint8_t)(subpack_hdr->dataid) == (uint8_t)m_AtpId)
        {
            storeAtpData(packet_ptr, subpack_hdr->len, clock_now, mono_timenow);
        }
        else if ((uint8_t)(subpack_hdr->dataid) == (uint8_t)m_GNetId)
        {
            storeGNetData(payload_ptr, payload_len, clock_now);
        }
        else if (subpack_hdr->dataid == m_GpsId)
        {
            parseGpsData(payload_ptr, payload_len, mono_timenow);
        }
        else if (subpack_hdr->dataid == m_BatteryId)
        {
            POWERDROP_DETECTER->decode((unsigned char*)payload_ptr, payload_len);
        }

        offset += subpack_hdr->len;
    }

    return;
}

int CRawdataSub::storeCirData(const char *buf, int len, time_t now)
{
    HELIUM::CBuffer cir_buf;

    char hdr_flag[3] = {'#', '1', '#'};
    char tail_flag[3] = {'@', '@', '@'};
    cir_buf.append(buf, len);

    std::string time_stamp = CDataRecorder::timeStamp(now);
    cir_buf.append(time_stamp.c_str(), time_stamp.length());

    cir_buf.append(tail_flag, 3);
    cir_buf.prepend(hdr_flag, 3);

    SHARED_BUFFER->pushData((unsigned char)DATAREC_CIR, cir_buf.peek(), cir_buf.readableBytes(), now, 1);
    return 0;
}

int CRawdataSub::storeGNetData(const char *buf, int len, time_t now)
{
    SHARED_BUFFER->pushData((unsigned char)DATAREC_GNET, buf, len, now, 1);
    return 0;
}

int CRawdataSub::storeAtpData(const char *buf, int len, time_t clock_now, uint64_t mono_now)
{
    int offset = sizeof(HELIUM::SUBPACK_HEADER);
    HELIUM::SUBPACK_HEADER* subpack_hdr = (HELIUM::SUBPACK_HEADER*)buf;

    /// 更新ATP数据头部
    m_AtpHeader.timestamp = CTimeVal::localTDateTime();
    m_AtpHeader.gpstatus = getGpsState(mono_now);

    unsigned char data_type;
    if (subpack_hdr->dataid == m_AtpC2Id)         /// CPU2数据
    {
        data_type = (unsigned char)DATAREC_ATPC2;
    }
    else
    {
        data_type = (unsigned char)DATAREC_ATP;
    }

    HELIUM::CBuffer frm_buf;

    ///将数据放入共享缓冲区中。300T数据需做字符填充转换
    if (subpack_hdr->dataid == m_AtpId)
    {
        int stuff_len;
        char stuff_buf[2048];

        JRUHeader* jru_hdr = (JRUHeader*)(buf + offset);
        if ( jru_hdr->startChar == JRU_BOUNDCHAR && jru_hdr->tag == JRU_HEADFLAG &&
             jru_hdr->listenPort == JRU_LISTENPORT )
        {
            offset += sizeof(JRUHeader);

            unsigned char pinfo_flag = buf[offset];
            if (pinfo_flag == 0xFF)
            {
                /// 接下来的8个字节为JRU时间戳，不做处理
                offset += 9;
            }

            /// 最后一个字节为0x7E边界，从len中减去
            int jru_length = len - offset - 1;

            /**
             * 将头部和负载封装成单独的帧
             */
            stuff_len = CHAR_STUFFER->enstuff(stuff_buf, sizeof(stuff_buf),
                    (const char*)&m_AtpHeader, sizeof(m_AtpHeader)); 
            frm_buf.append(stuff_buf, stuff_len);

            stuff_len = CHAR_STUFFER->enstuff(stuff_buf, sizeof(stuff_buf),
                    buf + offset, jru_length); 
            frm_buf.append(stuff_buf, stuff_len);
        }
    }
    else
    {
        frm_buf.append(&m_AtpHeader, sizeof(m_AtpHeader));
        frm_buf.append(buf + offset, len - offset);
    }
    SHARED_BUFFER->pushData(data_type, frm_buf.peek(), frm_buf.readableBytes(), clock_now, 1);

    return 0;
}

int CRawdataSub::parseGpsData(const char *buf, int len, uint64_t now)
{
    m_RecvGpsTimestamp = now;

    if (len == sizeof(GPSPROTO_DATA))
    {
        GPSPROTO_DATA* gps_data = (GPSPROTO_DATA*)buf;
        m_AtpHeader.gpspeed  = gps_data->speed / 1000;
        m_AtpHeader.gpstatus = STATUS_OK;
    }
    else
    {
        m_AtpHeader.gpstatus = STATUS_ERROR;
        return -1;
    }

    return 0;
}

uint32_t CRawdataSub::getGpsState(uint64_t now)
{
    uint32_t state = m_AtpHeader.gpstatus;
    if (m_RecvGpsTimestamp > 0 && (now - m_RecvGpsTimestamp) >= 2000)
        state = STATUS_TIMEOUT;

    return state;
}

