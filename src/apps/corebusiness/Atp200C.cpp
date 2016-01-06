/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Atp200C.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月01日 10时59分02秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Atp200C.h"
#include "Utils.h"
#include "AtpAPI.h"

using namespace COREBUSINESS;

static CAtp200C* atp_200C = 0;

int atpInitPlugin(void)
{
    atp_200C = new COREBUSINESS::CAtp200C(5000);
    return 0;
}

int atpFreePlugin(void)
{
    delete atp_200C; atp_200C = 0;
    return 0;
}

const char* atpGetVersion(void)
{
    return atp_200C->getVersion();
}

unsigned short atpGetRunSpeed(uint64_t now)
{
    return atp_200C->getRunSpeed(now);
}

unsigned int atpGetState(uint64_t now)
{
    return atp_200C->getState(now);
}

int atpParseData(const unsigned char* buf, int length, void* parsed_buf, uint64_t now)
{
    return atp_200C->parseData(buf, length, (HELIUM::CBuffer*)parsed_buf, now);
}

void atpUpdateRunDistance(unsigned int distance)
{
    atp_200C->updateRunDistance(distance);
}

void atpUpdateTrainNumber(const void* trainum, unsigned int timestamp)
{
    atp_200C->updateTrainum(trainum, timestamp);
}

void atpUpdateDataState(const void* pstate)
{
    atp_200C->updateDataState(pstate);
}

unsigned char* atpGetRealtimeInfo(int* length)
{
    return atp_200C->getRealtimeInfo(length);
}

//////////////////////////////////// CAtp200C的定义 ////////////////////////////////////

CAtp200C::HDR_LAYOUT CAtp200C::HdrFieldsLayout[] = {
    {HDRFIELD_NIDMSG,      8},
    {HDRFIELD_MSGLEN,     10},
    {HDRFIELD_YEAR,        7},
    {HDRFIELD_MONTH,       4},
    {HDRFIELD_DAY,         5},
    {HDRFIELD_HOUR,        5},
    {HDRFIELD_MINUTE,      6},
    {HDRFIELD_SECOND,      6},
    {HDRFIELD_TTS,         5},
    {HDRFIELD_STATUS,      4},
    {HDRFIELD_QSCALE,      2},
    {HDRFIELD_NIDLRBG,    24},
    {HDRFIELD_DLRBG,      15},
    {HDRFIELD_QDIRLRBG,    2},
    {HDRFIELD_QDLRBG,      2},
    {HDRFIELD_VTRAIN,      9},
    {HDRFIELD_VLIMITSPEED, 9},
    {HDRFIELD_VTARGET,     9},
    {HDRFIELD_LEVEL,       3},
    {HDRFIELD_MODE,        4},
    {HDRFIELD_CABSIGCODE,  5},
    {HDRFIELD_CABSIGSPEED, 3},
    {HDRFIELD_NIDJOINT,    8}
};

CAtp200C::MSG_PARSER CAtp200C::MessageParsers[] = {
    {{NID_ENGINE,          24},         parseNIDEngine},
    {{NID_DRIVERINFO,      92},        parseDriverInfo},
    {{NID_DRIVEROP,         8},   parseDriverOperation},
    {{NID_BALISEPASSING,   -1},     parseBalisePassing},    /// 废弃应答器的报文长度不固定
    {{NID_CTRLCMDOUTSTATE,  5},   parseCtrlCmdOutState},
    {{NID_ALARMINFO,        8},         parseAlarmInfo},
    {{NID_TARGETDISTANCE,  17},    parseTargetDistance},
    {{NID_TVMINFO,        792},           parseTVMInfo},
    {{NID_LINKSTATISTICS,  56},    parseLinkStatistics},
    {{NID_SLIPSLIDEINFO,    4},     parseSlipSlideInfo},
    {{NID_ATPDEFAULTINFO,   4},    parseATPDefaultInfo},
    {{NID_ABNORMALINFOGND,  4}, parseAbnormalInfoOnGnd},
    {{NID_BALISETELEGRAM, 896},    parseBaliseTelegram},
    {{NID_DEVICESTATUS,    18},      parseDeviceStatus},
    {{NID_DRIVERINTERACT,  17}, parseDriverInteraction}
};

CAtp200C::DRIVERINFO_LAYOUT CAtp200C::DriverInfoLayout[] = {
    {DRIVERINFO_LANGUAGE,         6},
    {DRIVERINFO_COMPANY,         10},
    {DRIVERINFO_DRIVERID,        32},
    {DRIVERINFO_MISSION_LETTERS, 24},
    {DRIVERINFO_MISSION_DIGITS,  20}
};

CAtp200C::BALISEPASS_LAYOUT CAtp200C::BalisePassLayout[] = {
    {BALISEPASS_QSCALE,   2},
    {BALISEPASS_NIDLRBG, 24},
    {BALISEPASS_DLRBG,   15},
    {BALISEPASS_QDIRLRBG, 2},
    {BALISEPASS_QDLRBG,   2},
    {BALISEPASS_MERROR,   8}
};

CAtp200C::TRACKCIRCUIT_LAYOUT CAtp200C::TrackCircuitLayout[] = {
    {TRACKCIRCUIT_NIDXUSR,  9},
    {TRACKCIRCUIT_QDIR,     2},
    {TRACKCIRCUIT_LPACKET, 13},
    {TRACKCIRCUIT_QSCALE,   2},
    {TRACKCIRCUIT_DSIGNAL, 15}
};

CAtp200C::CAtp200C(int interval) : CAtpNode(interval), m_CtrlCmdOutState(0), m_AlarmInfo(0),
                                   m_DeviceStatus(0), m_DriverInteract(0), m_ZeroFreqStamp(0),
                                   m_C2Changed(false), m_BaliseChanged(false)
{
    m_TypeId = 2;
    m_VerInfo = std::string("libAtp200C:2.0.3.1013");

    resetCTCS2Data();
}

void CAtp200C::resetCTCS2Data(void)
{
    ::memset(&m_C2Data, 0xFF, sizeof(CTCS2_DATA));
    m_C2Data.dmitxtcode = 0x01;
}

LOWFREQ_CODER CAtp200C::decodeLowFreqCode(uint32_t lowfreq)
{
    switch (lowfreq)
    {
        case 0x01: return LOWFREQ_L3;
        case 0x02: return LOWFREQ_L2;
        case 0x03: return LOWFREQ_L;
        case 0x04: return LOWFREQ_LU;
        case 0x05: return LOWFREQ_LU2;
        case 0x06: return LOWFREQ_U;
        case 0x07: return LOWFREQ_U2S;
        case 0x08: return LOWFREQ_U2;
        case 0x09: return LOWFREQ_U3;
        case 0x0A: return LOWFREQ_UUS;
        case 0x0B: return LOWFREQ_UU;
        case 0x0C: return LOWFREQ_HB;
        case 0x0D: return LOWFREQ_HU;
        case 0x0E: return LOWFREQ_H;
        case 0x0F: return LOWFREQ_L4;
        case 0x10: return LOWFREQ_L5;
        case 0x11: return LOWFREQ_257;
        case 0x12: return LOWFREQ_279;
        default:   return LOWFREQ_UNKNOWN;
    }
}

CARRIER_FREQ CAtp200C::decodeCarrFreqCode(uint8_t carrfreq)
{
    switch (carrfreq)
    {
        case  0: return FREQ_0;
        case  1:
        case  5:
        case  6: return FREQ_1700;
        case  2:
        case  7:
        case  8: return FREQ_2000;
        case  3:
        case  9:
        case 10: return FREQ_2300;
        case  4:
        case 11:
        case 12: return FREQ_2600;
        default: return FREQ_UNKNOWN;
    }
}

void CAtp200C::getHeaderInfo(void)
{
    /// ATP时间
    m_RealtimeInfo.opbase.basic.dateTime = convertDateTime(&m_HdrFieldValues[HDRFIELD_YEAR]);

    /// 速度信息
    uint32_t sb_speed = m_HdrFieldValues[HDRFIELD_VLIMITSPEED] + 5;
    uint32_t eb_speed = m_HdrFieldValues[HDRFIELD_VLIMITSPEED] + 10;
    m_RealtimeInfo.opbase.basic.speedInfo = (eb_speed << 20) + (sb_speed << 10) + (m_HdrFieldValues[HDRFIELD_VTRAIN]);
    m_RealtimeInfo.stamps.atpspeed = m_RecvTimestamp;

    /// 低频灯码
    LOWFREQ_CODER lowfreq = decodeLowFreqCode(m_HdrFieldValues[HDRFIELD_CABSIGCODE]);
    m_RealtimeInfo.opbase.basic.trackFreq &= 0x0F;            /// 清除旧的低频灯码
    m_RealtimeInfo.opbase.basic.trackFreq |= (lowfreq << 4);

    /// 目标速度
    m_RealtimeInfo.opbase.basic.targetInfo &= 0xFFFFFC00;
    m_RealtimeInfo.opbase.basic.targetInfo |= m_HdrFieldValues[HDRFIELD_VTARGET];

    /// 模式等级
    uint8_t mode_level = (m_HdrFieldValues[HDRFIELD_LEVEL] << 4) +
                         (m_HdrFieldValues[HDRFIELD_MODE]);
    m_RealtimeInfo.opbase.basic.modeLevel = mode_level;
}

int CAtp200C::parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    int result = 0;
    int offset = sizeof(HELIUM::SUBPACK_HEADER);
    if (buf == NULL || len <= offset)
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
    }

    (void)checkTimeliness(now);

    m_BaliseChanged = false;
    m_C2Changed = false;

    /// C2数据不需要继承，解析之前将其复位
    resetCTCS2Data();

    /// 跳过子报文头部
    const unsigned char* payload = buf + offset;
    int total_bitlen = (len - offset) << 3;

    /// 解析头部
    int bitoffset = 0;
    for (unsigned int i = 0; i < COUNTOF(HdrFieldsLayout); ++i)
    {
        m_HdrFieldValues[i] = ConvertBitsToUInt32(payload, total_bitlen, bitoffset, HdrFieldsLayout[i].bitlen);
        bitoffset += HdrFieldsLayout[i].bitlen;
    }

    getHeaderInfo();

    /// 解析消息域
    int left_bitlen = (m_HdrFieldValues[HDRFIELD_MSGLEN] << 3) - bitoffset;
    while (left_bitlen > 8)
    {
        uint32_t nid_message = ConvertBitsToUInt32(payload, total_bitlen, bitoffset, 8); /// 消息标识的长度固定为8比特
        bitoffset += 8;
        left_bitlen -= 8;

        unsigned int parser_index = nid_message - 3;
        if (parser_index < COUNTOF(MessageParsers))
        {
            int msg_bitlen = MessageParsers[parser_index].callback(this, payload, total_bitlen, bitoffset, 
                    MessageParsers[parser_index].layout.bitlen);
            bitoffset += msg_bitlen;
            left_bitlen -= msg_bitlen;
        }
        else
        {
            break;
        }
    }

    /// 应答器报文
    if (m_BaliseChanged == true)
    {
        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_BALISE,
                m_BaliseTelegram, BALISE_LENGTH, 1);

        result = updateBaliseInfo(m_BaliseTelegram, BALISE_LENGTH, now, &m_RealtimeInfo);
    }

    /// C2数据包
    if (m_C2Changed == true)
    {
        HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_CTCS2,
                (unsigned char*)&m_C2Data, sizeof(m_C2Data), 2);
    }

    /// 基础信息包
    if (opbaseInfoChanged(m_BaliseChanged || m_C2Changed, now) == true)
    {
        pushBaseWithDataState(nodes_buf);
    }

    m_Status = (uint8_t)STATUS_OK;
    return result;
}

int CAtp200C::parseNIDEngine(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    (void)obj;
    (void)data;
    (void)total_bitlen;
    (void)bitpos;

    return bitlen;
}

int CAtp200C::parseDriverInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    int bitoffset = bitpos;
    for (unsigned int i = 0; i < COUNTOF(DriverInfoLayout); ++i)
    {
        obj->m_DriverInfoValues[i] = ConvertBitsToUInt32(data, total_bitlen, bitoffset, DriverInfoLayout[i].bitlen);
        bitoffset += DriverInfoLayout[i].bitlen;
    }

    /// 解析车次
    memcpy(obj->m_RealtimeInfo.opbase.trainop.trainum.flag, &(obj->m_DriverInfoValues[DRIVERINFO_MISSION_LETTERS]), 4);
    for (int i = 0; i < 3; ++i)
    {
        char temp = obj->m_RealtimeInfo.opbase.trainop.trainum.flag[i];
        if ((temp >= '0' && temp <= '9') || (temp >= 'A' && temp <= 'Z'))
            continue;
        else
            obj->m_RealtimeInfo.opbase.trainop.trainum.flag[i] = 0;
    }
    obj->m_RealtimeInfo.opbase.trainop.trainum.num = ConvertBCDToDecimal(obj->m_DriverInfoValues[DRIVERINFO_MISSION_DIGITS]);
    obj->m_RealtimeInfo.stamps.trainum = obj->m_RecvTimestamp;

    /// 解析司机号
    if (obj->m_DriverInfoValues[DRIVERINFO_COMPANY] == 0 &&
       (obj->m_DriverInfoValues[DRIVERINFO_LANGUAGE] == 0 || obj->m_DriverInfoValues[DRIVERINFO_LANGUAGE] == 5) &&
       (obj->m_DriverInfoValues[DRIVERINFO_DRIVERID] >= 1000000) &&
       (obj->m_DriverInfoValues[DRIVERINFO_DRIVERID] <= 9999999))
    {
        obj->m_RealtimeInfo.opbase.trainop.driverid = obj->m_DriverInfoValues[DRIVERINFO_DRIVERID];
    }

    return bitlen;
}

int CAtp200C::parseDriverOperation(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    uint8_t driverop = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitpos, bitlen);
    if (obj->m_C2Data.driverop != driverop)
    {
        obj->m_C2Data.driverop = driverop;
        obj->m_C2Changed = true;
    }

    return bitlen;
}

int CAtp200C::setTrackSection(TRACK_SECTION* track_section, const unsigned char* data, int total_bitlen, int bitpos)
{
    track_section->nidsignal = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitpos, 4);
    track_section->nidfreq   = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitpos + 4, 5);
    track_section->length    = (uint16_t)ConvertBitsToUInt32(data, total_bitlen, bitpos + 4 + 5, 15);

    return (4 + 5 + 15);
}

int CAtp200C::parseBalisePassing(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    (void)bitlen;

    int bitoffset = bitpos;
    for (unsigned int i = 0; i < COUNTOF(BalisePassLayout); ++i)
    {
        obj->m_BalisePassValues[i] = ConvertBitsToUInt32(data, total_bitlen, bitoffset, BalisePassLayout[i].bitlen);
        bitoffset += BalisePassLayout[i].bitlen;
    }

    for (unsigned int i = 0; i < COUNTOF(TrackCircuitLayout); ++i)
    {
        obj->m_TrackCircuitValues[i] = (uint16_t)ConvertBitsToUInt32(data, total_bitlen, bitoffset, TrackCircuitLayout[i].bitlen);
        bitoffset += TrackCircuitLayout[i].bitlen;
    }

    /// 获取第一个轨道区段
    TRACK_SECTION* pTrackSection = obj->m_TrackSections;
    bitoffset += setTrackSection(pTrackSection, data, total_bitlen, bitoffset);

    /// 获取轨道区段数
    uint8_t section_num = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitoffset, 5);
    bitoffset += 5;

    /// 获取剩余的轨道区段
    for (unsigned int i = 1; i <= section_num && i < COUNTOF(obj->m_TrackSections); ++i)
    {
        bitoffset += setTrackSection(pTrackSection + i, data, total_bitlen, bitoffset);
    }

    return (bitoffset - bitpos);
}

BRAKE_ORDER CAtp200C::decodeBrakeCommand(uint8_t brake_cmd)
{
    switch (brake_cmd)
    {
        case 1:
            return BRAKE_SB1;
        case 4:
            return BRAKE_SB4;
        case 6:
            return BRAKE_SB7;
        default:
            return BRAKE_NONE;
    }
}

int CAtp200C::parseCtrlCmdOutState(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    uint8_t ctrlcmd_outstate = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitpos, bitlen);
    if (obj->m_CtrlCmdOutState != ctrlcmd_outstate)
    {
        obj->m_CtrlCmdOutState = ctrlcmd_outstate;

        obj->m_C2Data.power_cutoff = (ctrlcmd_outstate >> 4) & 0x01;
        obj->m_C2Data.eb_cmd = (ctrlcmd_outstate >> 3) & 0x01;
        obj->m_C2Data.sb_cmd = ctrlcmd_outstate & 0x07;

        obj->m_C2Changed = true;

        /// 解析制动命令
        BRAKE_ORDER brake_order = decodeBrakeCommand(obj->m_C2Data.sb_cmd);
        if (brake_order == BRAKE_NONE && obj->m_C2Data.eb_cmd == 1)
        {
            brake_order = BRAKE_EB;
        }

        obj->m_RealtimeInfo.opbase.basic.brake = (uint8_t)brake_order;
    }

    return bitlen;
}

int CAtp200C::parseAlarmInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    uint8_t alarm = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitpos, bitlen);
    if (obj->m_AlarmInfo != alarm)
    {
        obj->m_AlarmInfo = alarm;

        obj->m_C2Data.alarm_type  = (alarm >> 1) & 0x7f;
        obj->m_C2Data.alarm_event = alarm & 0x01;

        obj->m_C2Changed = true;
    }
    return bitlen;
}

int CAtp200C::parseTargetDistance(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    uint32_t target_distance = ConvertBitsToUInt32(data, total_bitlen, bitpos, bitlen);
    target_distance &= 0x7FFF;

    obj->m_RealtimeInfo.opbase.basic.targetInfo &= 0x3FF;
    obj->m_RealtimeInfo.opbase.basic.targetInfo |= (target_distance << 10);
    return bitlen;
}

int CAtp200C::parseTVMInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    (void)obj;
    (void)data;
    (void)total_bitlen;
    (void)bitpos;

    return bitlen;
}

int CAtp200C::parseLinkStatistics(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    (void)obj;
    (void)data;
    (void)total_bitlen;
    (void)bitpos;

    return bitlen;
}

int CAtp200C::parseSlipSlideInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    uint8_t slip_slide = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitpos, bitlen);
    if (obj->m_C2Data.slipslideinfo != slip_slide)
    {
        obj->m_C2Data.slipslideinfo = slip_slide;
        obj->m_C2Changed = true;
    }
    return bitlen;
}

int CAtp200C::parseATPDefaultInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    uint8_t atp_default = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitpos, bitlen);
    if (obj->m_C2Data.atpdefinfo != atp_default)
    {
        obj->m_C2Data.atpdefinfo = atp_default;
        obj->m_C2Changed = true;
    }
    return bitlen;
}

int CAtp200C::parseAbnormalInfoOnGnd(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    uint8_t abnormity_gnd = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitpos, bitlen);
    if (obj->m_C2Data.abnormitygndinfo != abnormity_gnd)
    {
        obj->m_C2Data.abnormitygndinfo = abnormity_gnd;
        obj->m_C2Changed = true;
    }
    return bitlen;
}

int CAtp200C::parseBaliseTelegram(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    bitpos += 64;    /// 跳过过应答器的位置和时间信息

    uint8_t balise[BALISE_LENGTH];
    for (int i = 0; i < BALISE_LENGTH; ++i)
    {
        balise[i] = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, (bitpos + i * 8), 8);
        if (obj->m_BaliseTelegram[i] != balise[i])
        {
            obj->m_BaliseTelegram[i] = balise[i];
            obj->m_BaliseChanged = true;
        }
    }

    return bitlen;
}

int CAtp200C::parseDeviceStatus(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    uint32_t devstatus = ConvertBitsToUInt32(data, total_bitlen, bitpos, bitlen);
    obj->m_C2Data.atpstatus  = (uint8_t)(devstatus >> 16) & 0x03;
    obj->m_C2Data.tiustatus  = (uint8_t)(devstatus >> 14) & 0x03;
    obj->m_C2Data.btmstatus  = (uint8_t)(devstatus >> 12) & 0x03;
    obj->m_C2Data.odostatus  = (uint8_t)(devstatus >> 10) & 0x03;
    obj->m_C2Data.dmistatus  = (uint8_t)(devstatus >>  8) & 0x03;
    obj->m_C2Data.cfskstatus = (uint8_t)(devstatus >>  6) & 0x03;
    obj->m_C2Data.carrfreq   = (uint8_t)(devstatus) & 0x3f;

    /**
     * 解析载频:
     * 载频取0时，可能为虚假的值。只有在持续2500毫秒以上都收到0时，
     * 才认为0有效。
     *
     */
    CARRIER_FREQ carrier_freq = decodeCarrFreqCode(obj->m_C2Data.carrfreq);
    if (carrier_freq == FREQ_0)
    {
        if (obj->m_ZeroFreqStamp == 0)
        {
            obj->m_ZeroFreqStamp = obj->m_RecvTimestamp;
        }
        else if ((obj->m_RecvTimestamp - obj->m_ZeroFreqStamp) > 2500)
        {
            obj->m_RealtimeInfo.opbase.basic.trackFreq &= 0x03F0;
            obj->m_RealtimeInfo.opbase.basic.trackFreq |= (uint8_t)carrier_freq;
        }
    }
    else
    {
        obj->m_ZeroFreqStamp = 0;

        obj->m_RealtimeInfo.opbase.basic.trackFreq &= 0x03F0;
        obj->m_RealtimeInfo.opbase.basic.trackFreq |= (uint8_t)carrier_freq;
    }

    /// 不监测TIU状态和载频的变化
    if ((obj->m_DeviceStatus & 0x33FC0) != (devstatus & 0x33FC0))
    {
        obj->m_C2Changed = true;
    }

    obj->m_DeviceStatus = devstatus;

    return bitlen;
}

int CAtp200C::parseDriverInteraction(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    uint32_t drv_interact = ConvertBitsToUInt32(data, total_bitlen, bitpos, bitlen);
    if (obj->m_DriverInteract != drv_interact)
    {
        obj->m_DriverInteract = drv_interact;

        obj->m_C2Data.drvdmiop       = (uint8_t)(drv_interact >> 12) & 0x1f;
        obj->m_C2Data.trackhandlepos = (uint8_t)(drv_interact >> 10) & 0x03;
        obj->m_C2Data.direhandlepos  = (uint8_t)(drv_interact >>  9) & 0x01;
        obj->m_C2Data.atpisolate     = (uint8_t)(drv_interact >>  8) & 0x01;
        obj->m_C2Data.dmitxtcode     = (uint8_t)(drv_interact) & 0xff;

        obj->m_C2Changed = true;
    }
    return bitlen;
}


