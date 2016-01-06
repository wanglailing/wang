/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Cpu2ToJru.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月06日 08时39分04秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <arpa/inet.h>

#include "Cpu2ToJru.h"
#include "AtpNode.h"
#include "Utils.h"
#include "TrainEncoder.h"

using namespace COREBUSINESS;

CCpu2ToJru::CCpu2ToJru(AtpRTInfo* rtinfo) : CDataNode(5000), m_pAtpRealtimeInfo(rtinfo), m_RunSpeed(0x3ff)
{
    ::memset(&m_Cpu2Info, 0, sizeof(m_Cpu2Info));
}

CCpu2ToJru::~CCpu2ToJru()
{
}

int CCpu2ToJru::parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    if (!buf || len != sizeof(CPU2_FRAME))
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
    }

    CPU2_FRAME* cpu2_frame = (CPU2_FRAME*)buf;
    if (cpu2_frame->head != 0xFF || cpu2_frame->tail != 0xFD)
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
    }

    m_RecvTimestamp = now;

    /// ATP时间(年份的高8位为实际年数的十位和个位，低8位预留)
    cpu2_frame->data.datetime.year >>= 8;
    m_pAtpRealtimeInfo->opbase.basic.dateTime = CAtpNode::convertDateTime(cpu2_frame->data.datetime);

    /// 走行距离
    m_pAtpRealtimeInfo->opbase.basic.runDistance = ntohl(cpu2_frame->data.distance);

    int data_filled = 0;

    FRAME_FLAG frame_flag = (FRAME_FLAG)(cpu2_frame->flag);
    switch (frame_flag)
    {
        case FRAME_CPU2:
            parseBaseFromCpu2(cpu2_frame->data.payload.cpu2_board);
            data_filled = checkCpu2Triggers(cpu2_frame->data.payload.cpu2_board, nodes_buf, 
                    m_pAtpRealtimeInfo->opbase.basic.modeLevel >> 4);
            break;
        case FRAME_BALISE:
            data_filled = parseBalise(cpu2_frame->data.payload.balise, nodes_buf);
            break;
        case FRAME_ERROR:
            break;
        default:
            break;
    }

    m_Status = (uint8_t)STATUS_OK;
    return data_filled;
}

void CCpu2ToJru::parseBaseFromCpu2(const CPU2_BOARD& cpu2)
{
    /// 等级信息
    unsigned char level = changeLevel(cpu2.recdata.initparams.ctcslevel);
    m_pAtpRealtimeInfo->opbase.basic.modeLevel &= 0x0f;
    m_pAtpRealtimeInfo->opbase.basic.modeLevel |= (level << 4);

    /// 速度信息
    m_RunSpeed = (uint16_t)(ntohs(cpu2.recdata.ctrl_speed) * 0.0125);
    uint16_t eb_speed  = (uint16_t)(ntohs(cpu2.recdata.speed_info.ebp) * 0.0125);
    uint16_t sb_speed  = (uint16_t)(ntohs(cpu2.recdata.speed_info.nbp) * 0.0125);
    m_pAtpRealtimeInfo->opbase.basic.speedInfo = (eb_speed << 20) + \
                                   (sb_speed << 10) + m_RunSpeed;
    m_pAtpRealtimeInfo->stamps.atpspeed = m_RecvTimestamp;

    /// 目标信息
    m_pAtpRealtimeInfo->opbase.basic.targetInfo = ntohs(cpu2.recdata.dminfo.target_speed) + 
                                 ((uint32_t)ntohs(cpu2.recdata.dminfo.target_distance) << 10);

    /// 里程标
    m_pAtpRealtimeInfo->opbase.basic.mileage = ntohl(cpu2.recdata.mileage);

    /// 低频
    m_pAtpRealtimeInfo->opbase.basic.trackFreq = (uint16_t)decodeLowFreq(cpu2.recdata.tcrinfo.signal) << 4;
    /// 载频
    m_pAtpRealtimeInfo->opbase.basic.trackFreq |= (uint16_t)decodeCarrierFreq(cpu2.recdata.tcrinfo.carrierfreq);

    /// 车次
    uint32_t traino;
    ::memcpy(&traino, &cpu2.trainum[2], sizeof(traino));
    TRAIN_ENCODER->decodeTrainNum(m_pAtpRealtimeInfo->opbase.trainop.trainum, ntohl(traino));

    /// 司机号
    uint32_t driver_id = ntohl(cpu2.driverid);
    if (driver_id > 999999 && driver_id < 9999999)
        m_pAtpRealtimeInfo->opbase.trainop.driverid = driver_id;

    /// 制动命令
    m_pAtpRealtimeInfo->opbase.basic.brake = parseBrakeOrder(cpu2.recdata.brake_cmd);
}

int CCpu2ToJru::checkCpu2Triggers(const CPU2_BOARD& cpu2, HELIUM::CBuffer* nodes_buf, uint8_t level)
{
    bool triggered;

    /**
     * DMI中的目标速度和目标距离不作为触发项。
     * 另外，这里的DMI结构使用200H(300H)所定义的，与实际的DMI结构定义略有不同。
     * 因为不解析DMI信息的内容，故没有再重新定义300S自身的DMI结构体。
     */
    m_Cpu2Info.dminfo.target_speed = cpu2.recdata.dminfo.target_speed;
    m_Cpu2Info.dminfo.target_distance = cpu2.recdata.dminfo.target_distance;

    if (level == 3)
    {
        triggered = (m_Cpu2Info.ctrlmode != cpu2.recdata.ctrlmode || 
                     m_Cpu2Info.tmminfo  != cpu2.recdata.tmminfo);
    }
    else
    {
        triggered = (m_Cpu2Info.cpu2status        != cpu2.recdata.cpu2status || 
                     m_Cpu2Info.cpu2failinfo      != cpu2.recdata.cpu2failinfo || 
                     m_Cpu2Info.ctrlmode          != cpu2.recdata.ctrlmode || 
                     m_Cpu2Info.lma               != cpu2.recdata.lma || 
                     m_Cpu2Info.eb_reason         != cpu2.recdata.eb_reason  || 
                     m_Cpu2Info.nb_reason         != cpu2.recdata.nb_reason   || 
                     m_Cpu2Info.trainpos_property != cpu2.recdata.trainpos_property || 
                     m_Cpu2Info.tmminfo           != cpu2.recdata.tmminfo ||
                     m_Cpu2Info.tcrinfo.state     != cpu2.recdata.tcrinfo.state ||
                     ::memcmp(&m_Cpu2Info.dminfo, &cpu2.recdata.dminfo, sizeof(DMI_INFO))
                    );
    }

    if (triggered == true)
    {
        m_Cpu2Info.cpu2status        = cpu2.recdata.cpu2status;
        m_Cpu2Info.cpu2failinfo      = cpu2.recdata.cpu2failinfo;
        m_Cpu2Info.ctrlmode          = cpu2.recdata.ctrlmode;
        m_Cpu2Info.lma               = cpu2.recdata.lma;
        m_Cpu2Info.eb_reason         = cpu2.recdata.eb_reason;
        m_Cpu2Info.nb_reason         = cpu2.recdata.nb_reason;
        m_Cpu2Info.trainpos_property = cpu2.recdata.trainpos_property;
        m_Cpu2Info.tmminfo           = cpu2.recdata.tmminfo;

        /// 里程标不作为触发项
        m_Cpu2Info.mileage           = cpu2.recdata.mileage;

        ::memcpy(&m_Cpu2Info.tcrinfo, &cpu2.recdata.tcrinfo, sizeof(TCR_INFO));
        ::memcpy(&m_Cpu2Info.dminfo, &cpu2.recdata.dminfo, sizeof(DMI_INFO));

        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_CTCS2, 
                (unsigned char*)&m_Cpu2Info, sizeof(m_Cpu2Info), 2);

        return 1;
    }

    return 0;
}

unsigned char CCpu2ToJru::changeLevel(unsigned char level)
{
    switch (level)
    {
        case 0x00: return 0;
        case 0x01: return 5;
        case 0x02: return 1;
        case 0x03: return 3;
        default:   return 0x0f;
    }
}

int CCpu2ToJru::parseBalise(const BALISE_PACKET& balise_packet, HELIUM::CBuffer* nodes_buf)
{
    int result = 0;
    if (::memcmp(m_BalisePacket, balise_packet.balise, BALISE_LENGTH))
    {
        ::memcpy(m_BalisePacket, balise_packet.balise, BALISE_LENGTH);

        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_BALISE, m_BalisePacket, BALISE_LENGTH, 1);
        result = updateBaliseInfo(m_BalisePacket, BALISE_LENGTH, m_RecvTimestamp, m_pAtpRealtimeInfo);
    }

    return result;
}

LOWFREQ_CODER CCpu2ToJru::decodeLowFreq(uint32_t signal) const
{
    switch (signal)
    {
        case 0x00000080: return LOWFREQ_L6;
        case 0x00000040: return LOWFREQ_L5;
        case 0x00000020: return LOWFREQ_L4;
        case 0x00000010: return LOWFREQ_L3;
        case 0x00000008: return LOWFREQ_L2;
        case 0x00000004: return LOWFREQ_L;
        case 0x00000002: return LOWFREQ_LU;
        case 0x00000001: return LOWFREQ_LU2;
        case 0x00008000: return LOWFREQ_U;
        case 0x00004000: return LOWFREQ_U2S;
        case 0x00002000: return LOWFREQ_U2;
        case 0x00001000: return LOWFREQ_UUS;
        case 0x00000800: return LOWFREQ_UU;
        case 0x00000400: return LOWFREQ_HB;
        case 0x00000200: return LOWFREQ_HU;
        case 0x00000100: return LOWFREQ_H;
        case 0x00800000: return LOWFREQ_257;
        case 0x00400000: return LOWFREQ_279;
        case 0x00200000: return LOWFREQ_U3;
        case 0x00010000: return LOWFREQ_0;
        default:         return LOWFREQ_UNKNOWN;
    }
}

CARRIER_FREQ CCpu2ToJru::decodeCarrierFreq(uint16_t freq_code) const
{
    switch (freq_code)
    {
        case 0x8000: return FREQ_550;
        case 0x4000: return FREQ_650;
        case 0x2000: return FREQ_750;
        case 0x1000: return FREQ_850;
        case 0x0080: return FREQ_1700;
        case 0x0040: return FREQ_2300;
        case 0x0020: return FREQ_2000;
        case 0x0010: return FREQ_2600;
        default:     return FREQ_0;
    }
}

BRAKE_ORDER CCpu2ToJru::parseBrakeOrder(uint8_t brake_info) const
{
    BRAKE_ORDER brake_order = BRAKE_NONE;

    if (brake_info & 0x80)
        brake_order = BRAKE_EB;
    else if (brake_info & 0x40)
        brake_order = BRAKE_SB7;
    else if (brake_info & 0x20)
        brake_order = BRAKE_SB4;
    else if (brake_info & 0x10)
        brake_order = BRAKE_SB1;
    else if (brake_info & 0x08)
        brake_order = BRAKE_PULLCUT;

    return brake_order;
}
