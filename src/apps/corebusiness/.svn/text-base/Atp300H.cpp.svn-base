/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: Atp300H.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月15日 17时11分52秒
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
#include <assert.h>

#include "Atp300H.h"
#include "Utils.h"
#include "AtpAPI.h"
#include "TrainEncoder.h"
#include "RbcTriggerSets.h"

using namespace COREBUSINESS;

static CAtp300H* atp_300H = 0;

int atpInitPlugin(void)
{
    atp_300H = new COREBUSINESS::CAtp300H(5000);
    return 0;
}

int atpFreePlugin(void)
{
    delete atp_300H; atp_300H = 0;
    return 0;
}

const char* atpGetVersion(void)
{
    return atp_300H->getVersion();
}

unsigned short atpGetRunSpeed(uint64_t now)
{
    return atp_300H->getRunSpeed(now);
}

unsigned int atpGetState(uint64_t now)
{
    return atp_300H->getState(now);
}

int atpParseData(const unsigned char* buf, int length, void* parsed_buf, uint64_t now)
{
    return atp_300H->parseData(buf, length, (HELIUM::CBuffer*)parsed_buf, now);
}

void atpUpdateRunDistance(unsigned int distance)
{
    atp_300H->updateRunDistance(distance);
}

void atpUpdateTrainNumber(const void* trainum, unsigned int timestamp)
{
    /// 300H不使用CIR车次
    (void)trainum;
    (void)timestamp;
}

void atpUpdateDataState(const void* pstate)
{
    atp_300H->updateDataState(pstate);
}

unsigned char* atpGetRealtimeInfo(int* length)
{
    return atp_300H->getRealtimeInfo(length);
}

//////////////////////////////////// CAtp300H的定义 ////////////////////////////////////
LOWFREQ_CODER CAtp300H::LowFreqDictionary[] = {
    LOWFREQ_L5,  LOWFREQ_L4,  LOWFREQ_L3, LOWFREQ_L2,  LOWFREQ_L,  LOWFREQ_LU, LOWFREQ_LU2, LOWFREQ_U,
    LOWFREQ_U2S, LOWFREQ_U2,  LOWFREQ_U3, LOWFREQ_UUS, LOWFREQ_UU, LOWFREQ_HB, LOWFREQ_HU,  LOWFREQ_H,
    LOWFREQ_257, LOWFREQ_279, LOWFREQ_L3, LOWFREQ_L2,  LOWFREQ_L,  LOWFREQ_LU, LOWFREQ_LU2, LOWFREQ_U,
    LOWFREQ_U2S, LOWFREQ_U2,  LOWFREQ_UNKNOWN, LOWFREQ_UUS, LOWFREQ_UU, LOWFREQ_HB, LOWFREQ_HU,  LOWFREQ_H
};

CARRIER_FREQ CAtp300H::CarrFreqDictionary[] = {
    FREQ_1700, FREQ_2300, FREQ_1700, FREQ_1700, FREQ_1700, FREQ_2300, FREQ_2300, FREQ_2300,
    FREQ_2000, FREQ_2600, FREQ_2000, FREQ_2000, FREQ_2000, FREQ_2600, FREQ_2600, FREQ_2600
};

CAtp300H::CAtp300H(int interval) : CAtpNode(interval)
{
    m_TypeId = 6;
    m_VerInfo = std::string("libAtp300H:2.0.3.1013");
    ::memset(&m_MainData, 0xFF, sizeof(m_MainData));

    ::memset(&m_AssistData, 0xFF, sizeof(m_AssistData));
    m_AssistData.type = 0x0A;

    ::memset(m_WirelessBuf, 0, sizeof(m_WirelessBuf));
}

CAtp300H::~CAtp300H()
{
}

int CAtp300H::parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    int result = 0;
    int offset = sizeof(HELIUM::SUBPACK_HEADER);
    if (buf == NULL || len != (int)(sizeof(MSU_DATA) + offset))
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
    }

    (void)checkTimeliness(now);

    bool balise_changed = false;
    bool main_changed   = false;
    bool assist_changed = false;
    bool rbc_changed    = false;

    MSU_DATA* msu_dat = (MSU_DATA*)(buf + offset);

    /// ATP时间
    msu_dat->datetime.year = ntohs(msu_dat->datetime.year);
    m_RealtimeInfo.opbase.basic.dateTime = convertDateTime(msu_dat->datetime);

    uint16_t msgtype = ntohs(msu_dat->msgtype);
    uint16_t msgid = msgtype & 0x0FFF;
    uint8_t express_type = (uint8_t)(msgtype >> 15);
    if (express_type == 0)        /// 主表示
    {
        if (msgid == (uint16_t)INFO_CTRLMSG)
        {
            parseBaseFromVC(&(msu_dat->payload.vc));
            main_changed = checkCtrlMsgTriggers(&m_MainData, &(msu_dat->payload.vc));
        }
        else if (msgid == (uint16_t)INFO_BALISE)
        {
            balise_changed = parseBaliseRec(&(msu_dat->payload.balise));
        }
        else if (msgid == (uint16_t)INFO_WIRELESS)
        {
            rbc_changed = parseWirelessRec(&(msu_dat->payload.wireless));
        }
    }
    else if (express_type == 1)   /// 从表示
    {
        if (msgid == (uint16_t)INFO_CTRLMSG)
        {
            assist_changed = checkCtrlMsgTriggers(&m_AssistData.misc, &(msu_dat->payload.vc));
            uint8_t brake = (uint8_t)parseBrakeOrder(msu_dat->payload.vc.common_data1.brake_cmd);
            if (m_AssistData.brake != brake)
            {
                m_AssistData.brake = brake;
                assist_changed = true;
            }
            m_AssistData.systype = (uint8_t)((msgtype >> 12) & 0x01);
        }
    }

    /**
     * 封装数据包
     */
    if (balise_changed == true)   /// 应答器数据
    {
        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_BALISE,
                m_BaliseTelegram, BALISE_LENGTH, 1);

        result = updateBaliseInfo(m_BaliseTelegram, BALISE_LENGTH, now, &m_RealtimeInfo);
    }

    if (main_changed == true)   /// 主系(C2)数据
    {
        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_CTCS2,
                (unsigned char*)&m_MainData, sizeof(m_MainData), 2);
    }

    if (rbc_changed == true)   /// RBC数据
    {
        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_JRU,
                m_WirelessBuf, m_WirelessLen + 3);
    }

    if (assist_changed == true)   /// 备系数据
    {
        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_CUSTOM,
                (unsigned char*)&m_AssistData, sizeof(m_AssistData), 2);
    }

    if (opbaseInfoChanged(balise_changed || main_changed || rbc_changed || assist_changed, now) == true)
    {
        pushBaseWithDataState(nodes_buf);
    }

    m_Status = (uint8_t)STATUS_OK;
    return result;
}

void CAtp300H::parseBaseFromVC(const VC_RECORD* vc_rec)
{
    /// 司机号
    m_RealtimeInfo.opbase.trainop.driverid = ntohl(vc_rec->common_data1.driverid);

    /// 车次编码
    m_RealtimeInfo.enctrain = ntohl(vc_rec->common_data1.trainum);
    TRAIN_ENCODER->decodeTrainNum(m_RealtimeInfo.opbase.trainop.trainum, m_RealtimeInfo.enctrain);
    m_RealtimeInfo.stamps.trainum = m_RecvTimestamp;

    /// 制动指令
    m_RealtimeInfo.opbase.basic.brake = (uint8_t)parseBrakeOrder(vc_rec->common_data1.brake_cmd);

    /// 列车运行速度
    uint16_t run_speed = (uint16_t)(ntohs(vc_rec->common_data1.ctrl_speed) * 0.0125);
    m_RealtimeInfo.opbase.basic.speedInfo &= 0xFFFFFC00;
    m_RealtimeInfo.opbase.basic.speedInfo |= run_speed;
    m_RealtimeInfo.stamps.atpspeed = m_RecvTimestamp;

    /// 等级模式
    uint8_t level, mode;
    level = vc_rec->common_data1.priority & 0x07;
    if (level == 1)    /// C2等级
    {
        parseSpeedInfo(&(vc_rec->ctcs2_data.speed_info));
        mode = vc_rec->ctcs2_data.ctrlmode;
    }
    else if (level == 3)    /// C3等级
    {
        parseSpeedInfo(&(vc_rec->ctcs3_data.speed_info));
        mode = vc_rec->ctcs3_data.ctrlmode;
    }
    else    /// 未知等级
    {
        mode = vc_rec->common_data1.ctrlmode;
    }
    convertMode(mode);
    m_RealtimeInfo.opbase.basic.modeLevel = (level << 4) + mode;

    /// 载频
    CARRIER_FREQ carrier_freq = decodeCarrFreq(vc_rec->ctcs2_data.tcr_signal);

    /// 低频
    LOWFREQ_CODER lowfreq = decodeLowFreq(vc_rec->ctcs2_data.tcr_signal);

    m_RealtimeInfo.opbase.basic.trackFreq = (uint16_t)((lowfreq << 4) + carrier_freq);

    /// 里程
    m_RealtimeInfo.opbase.basic.mileage = ntohl(vc_rec->ctcs3_data.mileage);
}

bool CAtp300H::checkCtrlMsgTriggers(MISC_INFO* misc, const VC_RECORD* vc_rec)
{
    bool triggered = false;
    if (misc->fail_info1 != vc_rec->common_data1.fail_info1)
    {
        misc->fail_info1 = vc_rec->common_data1.fail_info1;
        triggered = true;
    }

    if (misc->eb_reason != vc_rec->common_data1.eb_reason)
    {
        misc->eb_reason = vc_rec->common_data1.eb_reason;
        triggered = true;
    }

    if (misc->nb_reason != vc_rec->common_data1.nb_reason)
    {
        misc->nb_reason = vc_rec->common_data1.nb_reason;
        triggered = true;
    }

    if (::memcmp(misc->di_info, vc_rec->common_data1.di_info, 6))
    {
        ::memcpy(misc->di_info, vc_rec->common_data1.di_info, 6);
        triggered = true;
    }

    if (misc->lma != vc_rec->ctcs2_data.tracuit.lma)
    {
        misc->lma = vc_rec->ctcs2_data.tracuit.lma;
        triggered = true;
    }

    if (misc->curr_loadfreqswitch != vc_rec->ctcs2_data.tracuit.curr_loadfreqswitch)
    {
        misc->curr_loadfreqswitch = vc_rec->ctcs2_data.tracuit.curr_loadfreqswitch;
        triggered = true;
    }

    if (misc->fail_info2 != vc_rec->common_data2.fail_info2)
    {
        misc->fail_info2 = vc_rec->common_data2.fail_info2;
        triggered = true;
    }

    return triggered;
}

bool CAtp300H::parseWirelessRec(const WIRELESS_RECORD* wireless_rec)
{
    /// 第一包无线数据
    if (wireless_rec->dataseq == 1)
    {
        /// 解析数据主体的长度
        m_WirelessLen = 0;
        for (int i = 0; i < 4; ++i)
        {
            m_WirelessLen |= wireless_rec->payload[i];
            if (i != 3)
                m_WirelessLen <<= 8;
        }

        /**
         * 缓冲区的前三个字节依次为消息标识和数据长度。
         * direction取值为0: OBU->RBC; 1: RBC->OBU。将0转换为10(TO RBC)，
         * 1转换为9(FROM RBC)
         */
        m_RecvLen = 0;

        m_WirelessBuf[m_RecvLen++] = 10 - wireless_rec->direction;
        m_WirelessBuf[m_RecvLen++] = (unsigned char)m_WirelessLen;
        m_WirelessBuf[m_RecvLen++] = (unsigned char)(m_WirelessLen >> 8);

        /// 无线数据没有分包
        if (wireless_rec->datacount == 1)
        {
            //::assert(m_WirelessLen <= (sizeof(wireless_rec->payload) - 4));
            ::memcpy(m_WirelessBuf + m_RecvLen, wireless_rec->payload + 4, m_WirelessLen);
            return procWirelessData();
        }
        else
        {
            int first_len = sizeof(wireless_rec->payload) - 4;
            ::memcpy(m_WirelessBuf + m_RecvLen, wireless_rec->payload + 4, first_len);
            m_RecvLen += first_len;
        }
    }
    /// 最后一包无线数据
    else if (wireless_rec->dataseq == wireless_rec->datacount)
    {
        int left_len = m_WirelessLen - m_RecvLen;
        //::assert(left_len >= 0 && left_len <= sizeof(wireless_rec->payload));
        ::memcpy(m_WirelessBuf + m_RecvLen, wireless_rec->payload, left_len);
        return procWirelessData();
    }
    else
    {
        //::assert((m_RecvLen + sizeof(wireless_rec->payload)) <= sizeof(m_WirelessBuf));
        ::memcpy(m_WirelessBuf + m_RecvLen, wireless_rec->payload, sizeof(wireless_rec->payload));
        m_RecvLen += sizeof(wireless_rec->payload);
    }

    return false;
}

bool CAtp300H::procWirelessData(void)
{
    unsigned char rbc_type = m_WirelessBuf[0];
    unsigned char rbc_id   = m_WirelessBuf[3];

    return RBC_SETS->isRbcTriggered(rbc_type, rbc_id);
}

bool CAtp300H::parseBaliseRec(const BALISE_RECORD* balise_rec)
{
    bool changed = false;
    for (int i = 0; i < BALISE_LENGTH; ++i)
    {
        if (m_BaliseTelegram[i] != balise_rec->balise[i])
        {
            m_BaliseTelegram[i] = balise_rec->balise[i];
            changed = true;
        }
    }

    return changed;
}

void CAtp300H::parseSpeedInfo(const SPEED_INFO* speed_info)
{
    uint16_t eb_speed = (uint16_t)(ntohs(speed_info->ebp) * 0.0125);
    uint16_t sb_speed = (uint16_t)(ntohs(speed_info->nbp) * 0.0125);
    uint16_t target_speed = (uint16_t)(ntohs(speed_info->permit) * 0.0125);

    m_RealtimeInfo.opbase.basic.speedInfo &= 0x03FF;
    m_RealtimeInfo.opbase.basic.speedInfo |= (uint32_t)sb_speed << 10;
    m_RealtimeInfo.opbase.basic.speedInfo |= (uint32_t)eb_speed << 20;

    m_RealtimeInfo.opbase.basic.targetInfo &= 0xFFFFFC00;
    m_RealtimeInfo.opbase.basic.targetInfo |= target_speed;
}

void CAtp300H::convertMode(uint8_t& mode)
{
    if (mode == 0x04 || mode == 0x0B || mode == 0x0C || mode == 0x0E || mode == 0x0F)
    {
        mode = 0x0F;
        return;
    }

    if (mode == 0x1D)
    {
        mode = 0x04;
    }
    else if (mode <= 0x13 || (mode >= 0x1B && mode <= 0x1E))
    {
        mode &= 0x0F;
    }
    else
    {
        mode = 0x0F;
    }
}

BRAKE_ORDER CAtp300H::parseBrakeOrder(uint8_t brake_info) const
{
    BRAKE_ORDER brake_order = BRAKE_NONE;

    if (brake_info & 0x10)
        brake_order = BRAKE_EB;
    else if (brake_info & 0x08)
        brake_order = BRAKE_SB7;
    else if (brake_info & 0x04)
        brake_order = BRAKE_SB4;
    else if (brake_info & 0x02)
        brake_order = BRAKE_SB1;
    else if (brake_info & 0x01)
        brake_order = BRAKE_PULLCUT;

    return brake_order;
}

LOWFREQ_CODER CAtp300H::decodeLowFreq(const uint8_t* tcr_signal) const
{
    uint8_t temp;
    uint32_t lowfreq_map;
    lowfreq_map  = (uint32_t)tcr_signal[0] << 24;
    lowfreq_map |= (uint32_t)tcr_signal[1] << 16;
    temp = (tcr_signal[2] & 0xC0) | (tcr_signal[5] & 0x3F);
    lowfreq_map |= (uint32_t)temp << 8;
    lowfreq_map |= tcr_signal[6];

    for (unsigned int i = 0; i < 32; ++i)
    {
        if ( (lowfreq_map >> (31 - i)) & 0x01 )
            return LowFreqDictionary[i];
    }

    return LOWFREQ_UNKNOWN;
}

CARRIER_FREQ CAtp300H::decodeCarrFreq(const uint8_t* tcr_signal) const
{
    uint16_t carrfreq_map = (uint16_t)tcr_signal[3] << 8;
    carrfreq_map |= tcr_signal[4];

    for (unsigned int i = 0; i < 16; ++i)
    {
        if ( (carrfreq_map >> (15 - i)) & 0x01 )
            return CarrFreqDictionary[i];
    }

    return FREQ_UNKNOWN;
}

