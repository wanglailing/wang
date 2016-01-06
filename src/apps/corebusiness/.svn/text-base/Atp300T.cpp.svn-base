/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: Atp300T.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月17日 15时31分36秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>

#include "Atp300T.h"
#include "AtpAPI.h"
#include "../JruProto.h"

using namespace COREBUSINESS;

static CAtp300T* atp_300T = 0;

int atpInitPlugin(void)
{
    atp_300T = new COREBUSINESS::CAtp300T(30000);
    return 0;
}

int atpFreePlugin(void)
{
    delete atp_300T; atp_300T = 0;
    return 0;
}

const char* atpGetVersion(void)
{
    return atp_300T->getVersion();
}

unsigned short atpGetRunSpeed(uint64_t now)
{
    return atp_300T->getRunSpeed(now);
}

unsigned int atpGetState(uint64_t now)
{
    return atp_300T->getState(now);
}

int atpParseData(const unsigned char* buf, int length, void* parsed_buf, uint64_t now)
{
    return atp_300T->parseData(buf, length, (HELIUM::CBuffer*)parsed_buf, now);
}

void atpUpdateRunDistance(unsigned int distance)
{
    atp_300T->updateRunDistance(distance);
}

void atpUpdateTrainNumber(const void* trainum, unsigned int timestamp)
{
    atp_300T->updateTrainum(trainum, timestamp);
}

void atpUpdateDataState(const void* pstate)
{
    atp_300T->updateDataState(pstate);
}

unsigned char* atpGetRealtimeInfo(int* length)
{
    return atp_300T->getRealtimeInfo(length);
}

//////////////////////////////////// CAtp300T的定义 ////////////////////////////////////
CAtp300T::CAtp300T(int interval) : CAtpNode(interval)
{
    m_TypeId = 4;
    m_VerInfo = std::string("libAtp300T:2.0.3.1030");

    m_JRUParser = new CJuridicalRecParser(interval, CJuridicalRecParser::JRU_230, true,
                                          &m_RealtimeInfo);
    m_STMParser = new CSTMToJRU(m_JRUParser->getHeaderLen(), &m_RealtimeInfo);
}

CAtp300T::~CAtp300T()
{
    delete m_STMParser; m_STMParser = NULL;
    delete m_JRUParser; m_JRUParser = NULL;
}

int CAtp300T::parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    int result = 0;
    int jruhdr_offset = sizeof(HELIUM::SUBPACK_HEADER);
    int payload_offset = jruhdr_offset + sizeof(JRUHeader);
    if (buf == NULL || len <= payload_offset)
        return -1;

    JRUHeader* jru_hdr = (JRUHeader*)(buf + jruhdr_offset);
    if (jru_hdr->startChar != JRU_BOUNDCHAR || jru_hdr->tag != JRU_HEADFLAG)
        return -1;

    m_RecvTimestamp = now;

    /**
     * DMS作为服务器来接收JRU或者MVB数据，根据侦听端口号区分不同的数据。
     *
     */
    if (jru_hdr->listenPort == JRU_LISTENPORT)
    {
        unsigned char pinfo_flag = buf[payload_offset];
        if (pinfo_flag == 0xFF)
        {
            /// 接下来的8个字节为JRU时间戳，不做处理
            payload_offset += 9;
        }

        /// 最后一个字节为0x7E边界，从len中减去
        int payload_length = len - payload_offset - 1;
        const unsigned char* payload_buf = buf + payload_offset;
        bool has_stminfo;
        bool has_jruinfo;
        CJuridicalRecParser::MSG_INFO msg_info;
        memset(&msg_info, 0, sizeof(msg_info));

        do {
            has_stminfo = false;
            has_jruinfo = false;
            result = m_JRUParser->parseFrame(payload_buf, payload_length, &msg_info, now);
            if (msg_info.nid == CJuridicalRecParser::NID_STMINFO)
            {
                m_STMParser->setC3Timestamp(m_JRUParser->getC3Timestamp());
                m_STMParser->parseData(payload_buf, msg_info.len, NULL, now);
                if (m_STMParser->getSendFlag() == true)
                {
                    has_stminfo = true;
                    HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_CTCS2,
                            (unsigned char*)payload_buf, msg_info.len, 2);
                    m_STMParser->resetSendFlag();
                }
            }

            HELIUM::CBuffer* jru_packets = m_JRUParser->getPacketsBuf();
            if (jru_packets->readableBytes() > 0)
            {
                has_jruinfo = true;
                nodes_buf->append(jru_packets->peek(), jru_packets->readableBytes());
                jru_packets->retrieveAll();
            }

            bool base_changed = makeBasicInfo((has_stminfo || has_jruinfo), now);
            if (has_stminfo == true || has_jruinfo == true || base_changed == true)
            {
                pushBaseWithDataState(nodes_buf);
            }

            if (msg_info.len <= 0)
            {
                break;
            }

            payload_buf += msg_info.len;
            payload_length -= msg_info.len;
        } while(payload_length > 0);
    }
    else if (jru_hdr->listenPort == MVB_LISTENPORT)
    {
        /// MVB数据只记录，不解析
    }

    return result;
}

unsigned int CAtp300T::getState(uint64_t now)
{
    unsigned int c2_state = m_STMParser->getState(now);
    unsigned int c3_state = m_JRUParser->getState(now);

    uint8_t reset_scope = 0;
    if (c2_state == STATUS_TIMEOUT)
    {
        reset_scope |= 0x10;
    }

    if (c3_state == STATUS_TIMEOUT)
    {
        reset_scope |= 0x01;
    }
    resetDataInfo(reset_scope);

    return ((c2_state << 16) | c3_state);
}

void CAtp300T::resetDataInfo(uint8_t reset_area)
{
    uint8_t reset_c2 = reset_area >> 4;
    uint8_t reset_c3 = reset_area & 0x0f;

    if (reset_c2)
    {
        m_STMParser->resetDataInfo(reset_c3);
    }

    if (reset_c3)
    {
        m_JRUParser->resetDataInfo(reset_c2);
    }
}

bool CAtp300T::makeBasicInfo(bool more_infos, uint64_t now)
{
    bool info_changed = false;
    bool stm_timeout = m_STMParser->isRecvTimeout();

    /// 优先使用C2等级下的列车运行速度
    uint16_t c2_speed;
    if (stm_timeout == false && (c2_speed = m_STMParser->getRunSpeed()) != 0x3ff)
    {
        m_RealtimeInfo.opbase.basic.speedInfo = (m_RealtimeInfo.opbase.basic.speedInfo >> 10) << 10;
        m_RealtimeInfo.opbase.basic.speedInfo |= c2_speed;
    }

    CJuridicalRecParser::ATP_LEVEL atp_level;
    atp_level = (CJuridicalRecParser::ATP_LEVEL)(m_RealtimeInfo.opbase.basic.modeLevel >> 4);
    if (atp_level == CJuridicalRecParser::LEVEL_STM)  /// C2等级
    {
        m_RealtimeInfo.opbase.basic.modeLevel &= 0xf0;
        m_RealtimeInfo.opbase.basic.modeLevel |= m_STMParser->getMode();
    }

    info_changed = opbaseInfoChanged((more_infos || m_JRUParser->recvdGeneralInfo()), now);
    m_JRUParser->resetGeneralInfoRecvFlag();

    /// 走行距离不是触发项，但要实时更新它
    if (stm_timeout == false)
    {
        m_OperateBase.basic.runDistance = m_STMParser->getC2RunDistance();
    }
    else
    {
        /// 使用DMS自身计算的走行距离
        m_OperateBase.basic.runDistance = m_RealtimeInfo.opbase.basic.runDistance;
    }

    return info_changed;
}

