/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Atp200H.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月15日 11时43分13秒
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

#include "Atp200H.h"
#include "Utils.h"
#include "AtpAPI.h"
#include "TrainEncoder.h"

using namespace COREBUSINESS;

static CAtp200H* atp_200H = 0;

int atpInitPlugin(void)
{
    atp_200H = new COREBUSINESS::CAtp200H(5000);
    return 0;
}

int atpFreePlugin(void)
{
    delete atp_200H; atp_200H = 0;
    return 0;
}

const char* atpGetVersion(void)
{
    return atp_200H->getVersion();
}

unsigned short atpGetRunSpeed(uint64_t now)
{
    return atp_200H->getRunSpeed(now);
}

unsigned int atpGetState(uint64_t now)
{
    return atp_200H->getState(now);
}

int atpParseData(const unsigned char* buf, int length, void* parsed_buf, uint64_t now)
{
    return atp_200H->parseData(buf, length, (HELIUM::CBuffer*)parsed_buf, now);
}

void atpUpdateRunDistance(unsigned int distance)
{
    atp_200H->updateRunDistance(distance);
}

void atpUpdateTrainNumber(const void* trainum, unsigned int timestamp)
{
    /// 200H不使用CIR车次
    (void)trainum;
    (void)timestamp;
}

void atpUpdateDataState(const void* pstate)
{
    atp_200H->updateDataState(pstate);
}

unsigned char* atpGetRealtimeInfo(int* length)
{
    return atp_200H->getRealtimeInfo(length);
}

//////////////////////////////////// CAtp200H的定义 ////////////////////////////////////
LOWFREQ_CODER CAtp200H::LowFreqDictionary[] = {
    LOWFREQ_L5,  LOWFREQ_L4,  LOWFREQ_L3, LOWFREQ_L2,  LOWFREQ_L,  LOWFREQ_LU, LOWFREQ_LU2, LOWFREQ_U,
    LOWFREQ_U2S, LOWFREQ_U2,  LOWFREQ_U3, LOWFREQ_UUS, LOWFREQ_UU, LOWFREQ_HB, LOWFREQ_HU,  LOWFREQ_H,
    LOWFREQ_257, LOWFREQ_279, LOWFREQ_L3, LOWFREQ_L2,  LOWFREQ_L,  LOWFREQ_LU, LOWFREQ_LU2, LOWFREQ_U,
    LOWFREQ_U2S, LOWFREQ_U2,  LOWFREQ_U3, LOWFREQ_UUS, LOWFREQ_UU, LOWFREQ_HB, LOWFREQ_HU,  LOWFREQ_H
};

CAtp200H::CAtp200H(int interval) : CAtpNode(interval)
{
    m_TypeId = 1;
    m_VerInfo = std::string("libAtp200H:2.0.3.1013");
    ::memset(&m_MainData, 0, sizeof(m_MainData));

    ::memset(&m_AssistData, 0, sizeof(m_AssistData));
    m_AssistData.type = 0x0B;

    ::memset(&m_VCOther, 0, sizeof(m_VCOther));
    m_VCOther.type = 0x08;

    ::memset(&m_FailData, 0, sizeof(m_FailData));
    m_FailData.type = 0x09;
}

CAtp200H::~CAtp200H()
{
}

int CAtp200H::parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    int result = 0;
    int offset = sizeof(HELIUM::SUBPACK_HEADER);
    if (buf == NULL || len != (int)(sizeof(DRU_RECORD) + offset))
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
    }

    (void)checkTimeliness(now);

    bool balise_changed = false;
    bool main_changed   = false;
    bool assist_changed = false;
    bool vc_changed     = false;
    bool fail_changed   = false;

    DRU_RECORD* dru_rec = (DRU_RECORD*)(buf + offset);

    /// ATP时间
    dru_rec->datetime.year = ntohs(dru_rec->datetime.year);
    m_RealtimeInfo.opbase.basic.dateTime = convertDateTime(dru_rec->datetime);

    uint16_t msgtype = ntohs(dru_rec->msgtype);
    if (msgtype == (uint16_t)INFO_CTRLMSG)
    {
        /// 司机号(转换成小端模式)
        m_RealtimeInfo.opbase.trainop.driverid = ntohl(dru_rec->payload.vc_record.driverid);

        /// 车次
        parseTrainNum(dru_rec->payload.vc_record.trainum);

        BOARD_DATA* main_data   = NULL;
        BOARD_DATA* assist_data = NULL;
        if (checkVCInUse(dru_rec->payload.vc_record.junction) == (uint8_t)VC1_INUSE)
        {
            main_data   = &(dru_rec->payload.vc_record.board_dat[0]);
            assist_data = &(dru_rec->payload.vc_record.board_dat[1]);
        }
        else
        {
            main_data   = &(dru_rec->payload.vc_record.board_dat[1]);
            assist_data = &(dru_rec->payload.vc_record.board_dat[0]);
        }

        parseBaseFromBoard(main_data);

        /// 检查主系基板信息触发点
        main_changed = checkBoardTriggers(&m_MainData, main_data, false);

        /// 检查备系基板信息触发点
        assist_changed = checkBoardTriggers(&m_AssistData.assist, assist_data, true);

        /// 检查VC信息触发点
        vc_changed = checkVCTriggers(&(dru_rec->payload.vc_record));
    }
    else if (msgtype == (uint16_t)INFO_BALISE)
    {
        for (int i = 0; i < BALISE_LENGTH; ++i)
        {
            m_BaliseTelegram[i] = dru_rec->payload.balise_rec.balise[i];
            balise_changed = true;
        }
    }
    else if (msgtype == (uint16_t)INFO_FAILURE)
    {
        ::memcpy(&m_FailData.info, &(dru_rec->payload.fail_info), sizeof(FAIL_INFO));
        fail_changed = true;
    }
    else
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
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

    if (fail_changed == true)   /// 失败信息数据
    {
        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_CUSTOM,
                (unsigned char*)&m_FailData, sizeof(m_FailData), 2);
    }

    if (vc_changed == true)   /// VC数据
    {
        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_CUSTOM,
                (unsigned char*)&m_VCOther, sizeof(m_VCOther), 2);
    }

    if (assist_changed == true)   /// 备系数据
    {
        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_CUSTOM,
                (unsigned char*)&m_AssistData, sizeof(m_AssistData), 2);
    }

    if (opbaseInfoChanged(balise_changed || main_changed || fail_changed ||
                vc_changed || assist_changed, now) == true)
    {
        pushBaseWithDataState(nodes_buf);
    }

    m_Status = (uint8_t)STATUS_OK;
    return result;
}

uint8_t CAtp200H::checkVCInUse(uint64_t junction)
{
    ///多路DI2(2) BUF4 DI端口的输入信息
    uint8_t di2_2 = (uint8_t)(junction >> 32);
    VC_INUSE vc_inuse = VC1_INUSE;

    /**
     * di2_2的第3位表示2系故障检测状态(1 -- 正常；0 -- 故障)；第4位表示1系故障检测状态
     * 当1系故障并且2系正常时，用2系；其它情况均用1系
     */
    if ( (di2_2 & 0x08) == 0 && (di2_2 & 0x04) != 0 )
    {
        vc_inuse = VC2_INUSE;
    }

    return (uint8_t)vc_inuse;
}

void CAtp200H::parseBaseFromBoard(const BOARD_DATA* board_data)
{
    /// 制动指令
    m_RealtimeInfo.opbase.basic.brake = (uint8_t)parseBrakeOrder(board_data->ope.brake_cmd);

    /// 速度信息
    uint16_t run_speed = (uint16_t)(ntohs(board_data->ope.run_speed) * 0.0125);
    uint16_t ebp_speed = (uint16_t)(ntohs(board_data->ope.speed_info.ebp) * 0.0125);
    uint16_t nbp_speed = (uint16_t)(ntohs(board_data->ope.speed_info.nbp) * 0.0125);
    m_RealtimeInfo.opbase.basic.speedInfo = (ebp_speed << 20) + (nbp_speed << 10) + run_speed;
    m_RealtimeInfo.stamps.atpspeed = m_RecvTimestamp;

    /// 目标数据
    uint16_t target_speed = (uint16_t)(ntohs(board_data->fsc.dminfo.target_speed) * 0.0125);
    uint32_t target_dist  = (uint32_t)(ntohs(board_data->fsc.dminfo.target_distance) * 0.2);
    m_RealtimeInfo.opbase.basic.targetInfo = (target_dist << 10) + target_speed;

    /// 等级模式
    uint8_t level, mode;
    mode = board_data->fsc.actionmode & 0x0F;
    if ((board_data->fsc.actionmode & 0xF0) == 0x20)
        level = 2;
    else
        level = 0;
    m_RealtimeInfo.opbase.basic.modeLevel = (level << 4) + mode;

    /// 载频
    CARRIER_FREQ carrier_freq = decodeSTMCarrFreq(board_data->ope.stm_signal);

    /// 低频灯码
    LOWFREQ_CODER lowfreq = decodeSTMLowFreq(board_data->ope.stm_signal);

    m_RealtimeInfo.opbase.basic.trackFreq = (uint16_t)((lowfreq << 4) + carrier_freq);
}

bool CAtp200H::checkBoardTriggers(BOARD_DATA* dst, const BOARD_DATA* src, bool assist)
{
    bool triggered = false;

    if ( dst->fsc.fsc_state          != src->fsc.fsc_state ||
         dst->fsc.fsc_error          != src->fsc.fsc_error ||
         dst->fsc.dminfo.text        != src->fsc.dminfo.text ||
         dst->fsc.dminfo.switch_type != src->fsc.dminfo.switch_type ||
         dst->ope.ope_state          != src->ope.ope_state ||
         dst->ope.fail_info          != src->ope.fail_info ||
         dst->ope.eb_reason          != src->ope.eb_reason ||
         dst->ope.nb_reason          != src->ope.nb_reason ||
         dst->ope.vc_di              != src->ope.vc_di  ||
         /// 主系的制动命令已包含在基础信息中，故只需检查备系的制动命令
         (assist == true && dst->ope.brake_cmd != src->ope.brake_cmd) )
    {
        ::memcpy(dst, src, sizeof(*dst));
        triggered = true;
    }

    return triggered;
}

bool CAtp200H::checkVCTriggers(const VC_RECORD* vc_rec)
{
    bool result = false;

    if (m_VCOther.druinfo != vc_rec->druinfo)
    {
        m_VCOther.druinfo = vc_rec->druinfo;
        result = true;
    }

    if (m_VCOther.junction != vc_rec->junction)
    {
        m_VCOther.junction = vc_rec->junction;
        result = true;
    }

    if (m_VCOther.multidi != vc_rec->multidi)
    {
        m_VCOther.multidi = vc_rec->multidi;
        result = true;
    }

    if (m_VCOther.trndrveffective != vc_rec->trndrveffective)
    {
        m_VCOther.trndrveffective = vc_rec->trndrveffective;
        result = true;
    }

    if (m_VCOther.reserved1 != vc_rec->reserved1)
    {
        m_VCOther.reserved1 = vc_rec->reserved1;
        result = true;
    }

    if (m_VCOther.lkjinfo != vc_rec->lkjinfo)
    {
        m_VCOther.lkjinfo = vc_rec->lkjinfo;
        result = true;
    }

    if (m_VCOther.failtrigger != vc_rec->failtrigger)
    {
        m_VCOther.failtrigger = vc_rec->failtrigger;
        result = true;
    }

    return result;
}

void CAtp200H::parseTrainNum(const uint8_t* num_info)
{
    memset(&(m_RealtimeInfo.opbase.trainop.trainum), 0, sizeof(TrainNumber));
    for (unsigned int i = 0, j = 0; i < 3 && j < sizeof(m_RealtimeInfo.opbase.trainop.trainum.flag); ++i)
    {
        char n = (char)num_info[i];
        if ((n >= 'A' && n <= 'Z') || (n >= '0' && n <= '9'))
            m_RealtimeInfo.opbase.trainop.trainum.flag[j++] = n;
    }

    uint32_t decimal_part;
    decimal_part  = (num_info[3] >> 4) * 10000;
    decimal_part += (num_info[3] & 0x0F) * 1000;
    decimal_part += (num_info[4] >> 4) * 100;
    decimal_part += (num_info[4] & 0x0F) * 10;
    decimal_part += (num_info[5] >> 4);
    m_RealtimeInfo.opbase.trainop.trainum.num = decimal_part;

    m_RealtimeInfo.enctrain = TRAIN_ENCODER->encodeTrainNum(m_RealtimeInfo.opbase.trainop.trainum);
    m_RealtimeInfo.stamps.trainum = m_RecvTimestamp;
}

BRAKE_ORDER CAtp200H::parseBrakeOrder(uint8_t brake_info) const
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

LOWFREQ_CODER CAtp200H::decodeSTMLowFreq(const uint8_t* stm_signal) const
{
    uint8_t temp;
    uint32_t lowfreq_map;
    lowfreq_map  = (uint32_t)stm_signal[0] << 24;
    lowfreq_map |= (uint32_t)stm_signal[1] << 16;
    temp = (stm_signal[2] & 0xC0) | (stm_signal[5] & 0x3F);
    lowfreq_map |= (uint32_t)temp << 8;
    lowfreq_map |= stm_signal[6];

    for (unsigned int i = 0; i < 32; ++i)
    {
        if ( (lowfreq_map >> (31 - i)) & 0x01 )
            return LowFreqDictionary[i];
    }

    return LOWFREQ_UNKNOWN;
}

CARRIER_FREQ CAtp200H::decodeSTMCarrFreq(const uint8_t* stm_signal) const
{
    if (stm_signal[3] & 0x40)
        return FREQ_2300;
    else if (stm_signal[3] & 0x80)
        return FREQ_1700;
    else if (stm_signal[4] & 0x40)
        return FREQ_2600;
    else if (stm_signal[4] & 0x80)
        return FREQ_2000;
    else if (stm_signal[7] & 0x01)
        return FREQ_850;
    else if (stm_signal[7] & 0x02)
        return FREQ_750;
    else if (stm_signal[7] & 0x04)
        return FREQ_650;
    else if (stm_signal[7] & 0x08)
        return FREQ_550;
    else
        return FREQ_0;
}

