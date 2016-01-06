/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Atp300S.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月17日 15时25分44秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Atp300S.h"
#include "AtpAPI.h"

using namespace COREBUSINESS;

static CAtp300S* atp_300S = 0;

int atpInitPlugin(void)
{
    atp_300S = new COREBUSINESS::CAtp300S(20000);
    return 0;
}

int atpFreePlugin(void)
{
    delete atp_300S; atp_300S = 0;
    return 0;
}

const char* atpGetVersion(void)
{
    return atp_300S->getVersion();
}

unsigned short atpGetRunSpeed(uint64_t now)
{
    return atp_300S->getRunSpeed(now);
}

unsigned int atpGetState(uint64_t now)
{
    return atp_300S->getState(now);
}

int atpParseData(const unsigned char* buf, int length, void* parsed_buf, uint64_t now)
{
    return atp_300S->parseData(buf, length, (HELIUM::CBuffer*)parsed_buf, now);
}

void atpUpdateRunDistance(unsigned int distance)
{
    atp_300S->updateRunDistance(distance);
}

void atpUpdateTrainNumber(const void* trainum, unsigned int timestamp)
{
    /// 300S不使用CIR车次
    (void)trainum;
    (void)timestamp;
}

void atpUpdateDataState(const void* pstate)
{
    atp_300S->updateDataState(pstate);
}

unsigned char* atpGetRealtimeInfo(int* length)
{
    return atp_300S->getRealtimeInfo(length);
}

//////////////////////////////////// CAtp300S的定义 ////////////////////////////////////
CAtp300S::CAtp300S(int interval) : CAtpNode(interval)
{
    m_TypeId = 3;
    m_VerInfo = std::string("libAtp300S:2.0.2.820");

    m_JRUParser = new CJuridicalRecParser(interval, CJuridicalRecParser::JRU_200, false,
                                          &m_RealtimeInfo);
    m_Cpu2Parser = new CCpu2ToJru(&m_RealtimeInfo);
}

CAtp300S::~CAtp300S()
{
    delete m_Cpu2Parser; m_Cpu2Parser = NULL;
    delete m_JRUParser; m_JRUParser = NULL;
}

int CAtp300S::parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    int result = 0;
    int offset = sizeof(HELIUM::SUBPACK_HEADER);
    HELIUM::SUBPACK_HEADER* subpack_hdr = (HELIUM::SUBPACK_HEADER*)buf;
    if (subpack_hdr->dataid == 0x0606)         /// CPU2数据
    {
        bool more_data = false;
        if ((result = m_Cpu2Parser->parseData(buf + offset, len - offset, nodes_buf, now)) > 0)
        {
            more_data = true;
        }

        bool base_changed = makeBasicInfo(more_data, now);
        if (more_data == true || base_changed == true)
        {
            m_OperateBase.basic.reserved = m_TypeId;
            HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_BASE, 
                    (unsigned char*)&m_OperateBase, sizeof(m_OperateBase));
        }
    }
    else if (subpack_hdr->dataid == 0x0106)    /// JRU(MVB)数据
    {
        result = parseMVBData(buf + offset, len - offset, nodes_buf, now);
    }

    return result;
}

int CAtp300S::parseMVBData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    int result = 0;
    int payload_length = len;
    const unsigned char* payload_buf = buf;
    bool has_jruinfo;
    CJuridicalRecParser::MSG_INFO msg_info;
    do {
        has_jruinfo = false;
        result = m_JRUParser->parseFrame(payload_buf, payload_length, &msg_info, now);

        HELIUM::CBuffer* jru_packets = m_JRUParser->getPacketsBuf();
        if (jru_packets->readableBytes() > 0)
        {
            has_jruinfo = true;
            nodes_buf->append(jru_packets->peek(), jru_packets->readableBytes());
            jru_packets->retrieveAll();
        }

        bool base_changed = makeBasicInfo(has_jruinfo, now);
        if (has_jruinfo == true || base_changed == true)
        {
            m_OperateBase.basic.reserved = 3;
            HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_BASE, 
                    (unsigned char*)&m_OperateBase, sizeof(m_OperateBase));
        }

        payload_buf += msg_info.len;
        payload_length -= msg_info.len;
    } while(payload_length > 0);

    return result;
}

unsigned int CAtp300S::getState(uint64_t now)
{
    unsigned int c2_state = m_Cpu2Parser->getState(now);
    unsigned int c3_state = m_JRUParser->getState(now);

    return ((c2_state << 16) | c3_state);
}

bool CAtp300S::makeBasicInfo(bool more_infos, uint64_t now)
{
    bool info_changed = false;
    bool cpu2_timeout = m_Cpu2Parser->recvTimeout(now);

    /// 优先使用C2等级下的列车运行速度
    uint16_t c2_speed;
    if (cpu2_timeout == false && (c2_speed = m_Cpu2Parser->getRunSpeed()) != 0x3ff)
    {
        m_RealtimeInfo.opbase.basic.speedInfo = (m_RealtimeInfo.opbase.basic.speedInfo >> 10) << 10;
        m_RealtimeInfo.opbase.basic.speedInfo |= c2_speed;
    }

    CJuridicalRecParser::ATP_LEVEL atp_level;
    atp_level = (CJuridicalRecParser::ATP_LEVEL)(m_RealtimeInfo.opbase.basic.modeLevel >> 4);
    if (atp_level == CJuridicalRecParser::LEVEL_STM)  /// C2等级
    {
        /// 如果C2等级下发生了紧急制动，且等级切换的时长小于2秒，仍沿用上一个制动命令
        if ((BRAKE_ORDER)m_RealtimeInfo.opbase.basic.brake == BRAKE_EB)
        {
            m_RealtimeInfo.opbase.basic.brake = m_OperateBase.basic.brake;
        }
    }
    else  /// C3等级
    {
        /// 如果C3等级下的模式发生了变化，将制动命令设为没有制动
        if (m_RealtimeInfo.opbase.basic.modeLevel != m_OperateBase.basic.modeLevel)
        {
            m_RealtimeInfo.opbase.basic.brake = (uint8_t)BRAKE_NONE;
        }
    }

    info_changed = opbaseInfoChanged(more_infos, now);

    /// 走行距离不是触发项，但要实时更新它
    if (cpu2_timeout == false)
    {
        m_OperateBase.basic.runDistance = m_RealtimeInfo.opbase.basic.runDistance;
    }
    else
    {
        /// 使用DMS自身计算的走行距离
    }

    return info_changed;
}

