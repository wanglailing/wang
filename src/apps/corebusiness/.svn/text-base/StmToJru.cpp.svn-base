/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: StmToJru.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月17日 08时42分17秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "StmToJru.h"
#include "AtpNode.h"
#include "Utils.h"
#include "TrainEncoder.h"
#include "Sub027JuridicalRec.h"

using namespace COREBUSINESS;

CSTMToJRU::HDR_LAYOUT CSTMToJRU::HdrFieldsLayout[] = {
    {HDRFIELD_NIDPACKET,     8},
    {HDRFIELD_PACKETLEN,    13},
    {HDRFIELD_JRUTIME,      32},
    {HDRFIELD_NLITER,        8},
    {HDRFIELD_FSTORAGETIME,  1},
    {HDRFIELD_POSITION,     27},
    {HDRFIELD_PEROID,       16}
};

CSTMToJRU::GENERALINFO_LAYOUT CSTMToJRU::GeneralInfoLayout[] = {
    {GENERALINFO_RUNSPEED,   11},
    {GENERALINFO_TRAINPOS,   27},
    {GENERALINFO_CARRFREQ,    5},
    {GENERALINFO_LOWFREQ,     8},
    {GENERALINFO_PERSPEED,    9},
    {GENERALINFO_TARGETSPEED, 9},
    {GENERALINFO_EBSPEED,     9},
    {GENERALINFO_SBSPEED,     9},
    {GENERALINFO_TARGETDIS,  21},
    {GENERALINFO_MILEAGE,    20},
    {GENERALINFO_BRAKECMD,    4}
};

CSTMToJRU::DATETIME_LAYOUT CSTMToJRU::DateTimeLayout[] = {
    {DATETIME_YEAR,         7},
    {DATETIME_MONTH,        4},
    {DATETIME_DAY,          5},
    {DATETIME_HOUR,         5},
    {DATETIME_MINUTE,       6},
    {DATETIME_SECOND,       6},
    {DATETIME_TTS,          5}
};

CSTMToJRU::TRAINDATA_LAYOUT CSTMToJRU::TrainDataLayout[] = {
    {TRAINDATA_RUNUM,      32},
    {TRAINDATA_TYPE,        4},
    {TRAINDATA_LENGTH,     12},
    {TRAINDATA_MAXSPEED,    9},
    {TRAINDATA_LOADGAUGE,   3},
    {TRAINDATA_BRAKPERC,    8},
    {TRAINDATA_ADHERECOFF,  7},
    {TRAINDATA_AXLELOAD,    9},
    {TRAINDATA_ARITIGHT,    2},
    {TRAINDATA_TRACTION,    4},
    {TRAINDATA_MAXACC,      8}
};

CSTMToJRU::CSTMToJRU(int jru_hdrlen, AtpRTInfo* rtinfo) :
    CDataNode(8000), m_JruHdrBitLen(jru_hdrlen), m_pAtpRealtimeInfo(rtinfo),
    m_C2RunDistance(0), m_C3Timestamp(0), m_RunSpeed(0x3ff),
    m_Mode(0x0f), m_bSendTriggered(false)
{
    /// 初始化消息解析函数集
    m_MessageParsers[NID_GENERAL]    = parseGeneralInfo;
    m_MessageParsers[NID_DMI]        = triggerSending;
    m_MessageParsers[NID_APPRUN]     = parseAppRunInfo;
    m_MessageParsers[NID_TIME]       = parseTimeInfo;
    m_MessageParsers[NID_TRAINDATA]  = parseTrainData;
    m_MessageParsers[NID_EXCEPTION]  = triggerSending;

    m_HeaderBitLen = getHeaderLength();
}

int CSTMToJRU::getHeaderLength(void)
{
    int result = 0;
    for (unsigned int i = 0; i < COUNTOF(HdrFieldsLayout); ++i)
    {
        result += HdrFieldsLayout[i].bitlen;
    }

    return result;
}

int CSTMToJRU::parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    (void)nodes_buf;

    if (buf == NULL || len <= 0)
        return -1;

    int total_bitlen = len << 3;

    /// 跳过JRU头部
    int bitoffset = m_JruHdrBitLen;
    for (unsigned int i = 0; i < COUNTOF(HdrFieldsLayout); ++i)
    {
        m_HdrFieldValues[i] = ConvertBitsToUInt32(buf, total_bitlen, bitoffset, HdrFieldsLayout[i].bitlen);
        bitoffset += HdrFieldsLayout[i].bitlen;
    }

    /// STM发往JRU的报文标识为161
    if (m_HdrFieldValues[HDRFIELD_NIDPACKET] != 161)
    {
        return -1;
    }

    /// 检查数据时效性
    (void)checkTimeliness(now);

    /// 解析消息域
    int left_bitlen = m_HdrFieldValues[HDRFIELD_PACKETLEN] - m_HeaderBitLen;
    while (left_bitlen > 18)
    {
        uint32_t nid_subpacket = ConvertBitsToUInt32(buf, total_bitlen, bitoffset, 8); /// 子包标识的长度固定为8比特
        bitoffset += 8;

        uint32_t len_subpacket = ConvertBitsToUInt32(buf, total_bitlen, bitoffset, 10); /// 子包的长度固定为10比特
        bitoffset += 10;

        /// 子包的数据负载长度
        int subpayload_len = len_subpacket - 18;

        PARSER_ITER iter = m_MessageParsers.find(static_cast<NID_MSG>(nid_subpacket));
        if (iter != m_MessageParsers.end())
        {
            iter->second(this, buf, total_bitlen, bitoffset, subpayload_len);
        }

        bitoffset += subpayload_len;
        left_bitlen -= len_subpacket;
    }

    m_Status = (uint8_t)STATUS_OK;
    return 0;
}

void CSTMToJRU::resetDataInfo(uint8_t reset_area)
{
    m_pAtpRealtimeInfo->opbase.basic.trackFreq  = 0xffff;
    m_pAtpRealtimeInfo->opbase.basic.targetInfo = 0xffffffff;
    m_pAtpRealtimeInfo->opbase.basic.mileage    = 0xffffffff;

    /// C3和C2都超时后才复位速度信息和制动命令
    if (reset_area)
    {
        m_pAtpRealtimeInfo->opbase.basic.speedInfo = 0xffffffff;
        m_pAtpRealtimeInfo->opbase.basic.brake = 0xff;
    }
    else  /// C2超时且C3正常时，仅复位制动速度
    {
        m_pAtpRealtimeInfo->opbase.basic.speedInfo |= 0xfffffc00;
    }
}

int CSTMToJRU::parseGeneralInfo(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    int bitoffset = bitpos;
    for (unsigned int i = 0; i < COUNTOF(GeneralInfoLayout); ++i)
    {
        obj->m_GeneralInfoValues[i] = ConvertBitsToUInt32(data, total_bitlen, bitoffset, GeneralInfoLayout[i].bitlen);
        bitoffset += GeneralInfoLayout[i].bitlen;
    }

    /// 速度信息
    obj->m_pAtpRealtimeInfo->stamps.atpspeed = obj->m_RecvTimestamp;
    obj->m_RunSpeed = obj->m_GeneralInfoValues[GENERALINFO_RUNSPEED];
    if (obj->m_RunSpeed > 1023)
    {
        obj->m_RunSpeed = 2048 - obj->m_RunSpeed;
    }

    obj->m_pAtpRealtimeInfo->opbase.basic.speedInfo &= 0xFFFFFC00;
    obj->m_pAtpRealtimeInfo->opbase.basic.speedInfo |= obj->m_RunSpeed;

    /// 走行距离取低26位(第27位是符号位)
    obj->m_C2RunDistance = obj->m_GeneralInfoValues[GENERALINFO_TRAINPOS] & 0x3ffffff;

    /// 载频和低频灯码
    CARRIER_FREQ carrier_freq = decodeCarrFreqCode(obj->m_GeneralInfoValues[GENERALINFO_CARRFREQ]);
    LOWFREQ_CODER lowfreq = decodeLowFreqCode(obj->m_GeneralInfoValues[GENERALINFO_LOWFREQ]);
    obj->m_pAtpRealtimeInfo->opbase.basic.trackFreq = (uint16_t)((lowfreq << 4) + carrier_freq);

    /// 里程标(单位：米)
    obj->m_pAtpRealtimeInfo->opbase.basic.mileage = obj->m_GeneralInfoValues[GENERALINFO_MILEAGE] * 10;

    /// 在C2等级下才使用STM中的制动速度、目标信息和制动命令
    CJuridicalRecParser::ATP_LEVEL atp_level;
    atp_level = (CJuridicalRecParser::ATP_LEVEL)(obj->m_pAtpRealtimeInfo->opbase.basic.modeLevel >> 4);
    if (atp_level == CJuridicalRecParser::LEVEL_STM)  /// C2等级
    {
        /// 制动速度
        obj->m_pAtpRealtimeInfo->opbase.basic.speedInfo &= 0x3FF;
        obj->m_pAtpRealtimeInfo->opbase.basic.speedInfo |= (obj->m_GeneralInfoValues[GENERALINFO_EBSPEED] << 20);
        obj->m_pAtpRealtimeInfo->opbase.basic.speedInfo |= (obj->m_GeneralInfoValues[GENERALINFO_SBSPEED] << 10);

        /// 目标信息
        obj->m_pAtpRealtimeInfo->opbase.basic.targetInfo = (obj->m_GeneralInfoValues[GENERALINFO_TARGETDIS] << 10) + \
            obj->m_GeneralInfoValues[GENERALINFO_TARGETSPEED];

        /// 如果C2等级下发生了紧急制动，且等级切换的时长小于2秒，仍沿用上一个制动命令
        BRAKE_ORDER brakecmd = decodeBrakeCommand((uint8_t)(obj->m_GeneralInfoValues[GENERALINFO_BRAKECMD]));
        if ( brakecmd == BRAKE_EB && obj->m_C3Timestamp != 0 &&
             (decodeTimeToSecond(obj->m_pAtpRealtimeInfo->opbase.basic.dateTime) - \
              decodeTimeToSecond(obj->m_C3Timestamp)) <= 2 )
        {
            return 0;
        }

        obj->m_pAtpRealtimeInfo->opbase.basic.brake = (uint8_t)brakecmd;
    }

    return 0;
}

time_t CSTMToJRU::decodeTimeToSecond(uint32_t encode_date)
{
    struct tm calendar_time;
    memset(&calendar_time, 0, sizeof(calendar_time));
    calendar_time.tm_year  = (encode_date >> 26) + 2000 - 1900;
    calendar_time.tm_mon   = (encode_date >> 22) & 0x0F - 1;
    calendar_time.tm_mday  = (encode_date >> 17) & 0x1F;
    calendar_time.tm_hour  = (encode_date >> 12) & 0x1F;
    calendar_time.tm_min   = (encode_date >>  6) & 0x3F;
    calendar_time.tm_sec   = encode_date & 0x3F;
    calendar_time.tm_isdst = 0;

    return mktime(&calendar_time);
}

BRAKE_ORDER CSTMToJRU::decodeBrakeCommand(uint8_t brake_cmd)
{
    switch (brake_cmd)
    {
        case 1:
            return BRAKE_SB1;
        case 4:
            return BRAKE_SB4;
        case 7:
            return BRAKE_SB7;
        case 0xF:
            return BRAKE_EB;
        default:
            return BRAKE_NONE;
    }
}

LOWFREQ_CODER CSTMToJRU::decodeLowFreqCode(uint32_t lowfreq)
{
    switch (lowfreq)
    {
        case 0x03: return LOWFREQ_H;
        case 0x05: return LOWFREQ_279;
        case 0x09: return LOWFREQ_HU;
        case 0x11: return LOWFREQ_257;
        case 0x21: return LOWFREQ_HB;
        case 0x06: return LOWFREQ_L4;
        case 0x0A: return LOWFREQ_U3;
        case 0x12: return LOWFREQ_L5;
        case 0x22: return LOWFREQ_U2S;
        case 0x0C: return LOWFREQ_UUS;
        case 0x14: return LOWFREQ_UU;
        case 0x24: return LOWFREQ_U;
        case 0x18: return LOWFREQ_LU2;
        case 0x28: return LOWFREQ_U2;
        case 0x30: return LOWFREQ_LU;
        case 0x3C: return LOWFREQ_L2;
        case 0x3A: return LOWFREQ_L;
        case 0x36: return LOWFREQ_L3;
        default:   return LOWFREQ_UNKNOWN;
    }
}

CARRIER_FREQ CSTMToJRU::decodeCarrFreqCode(uint32_t carrfreq)
{
    switch (carrfreq)
    {
        case  0: return FREQ_0;
        case  1:
        case  5: return FREQ_1700;
        case  2:
        case  6: return FREQ_2000;
        case  3:
        case  7: return FREQ_2300;
        case  4:
        case  8: return FREQ_2600;
        default: return FREQ_UNKNOWN;
    }
}

int CSTMToJRU::parseAppRunInfo(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    obj->m_Mode = (uint8_t)ConvertBitsToUInt32(data, total_bitlen, bitpos, 5);
    obj->m_bSendTriggered = true;
    return 0;
}

int CSTMToJRU::parseTimeInfo(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    int bitoffset = bitpos;
    for (unsigned int i = 0; i < COUNTOF(DateTimeLayout); ++i)
    {
        obj->m_DateTimeValues[i] = ConvertBitsToUInt32(data, total_bitlen, bitoffset, DateTimeLayout[i].bitlen);
        bitoffset += DateTimeLayout[i].bitlen;
    }

    return 0;
}

int CSTMToJRU::parseTrainData(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    int bitoffset = bitpos;
    for (unsigned int i = 0; i < COUNTOF(TrainDataLayout); ++i)
    {
        obj->m_TrainDataValues[i] = ConvertBitsToUInt32(data, total_bitlen, bitoffset, TrainDataLayout[i].bitlen);
        bitoffset += TrainDataLayout[i].bitlen;
    }

    obj->m_pAtpRealtimeInfo->enctrain = obj->m_TrainDataValues[TRAINDATA_RUNUM];
    TRAIN_ENCODER->decodeTrainNum(obj->m_pAtpRealtimeInfo->opbase.trainop.trainum, obj->m_TrainDataValues[TRAINDATA_RUNUM]);
    obj->m_pAtpRealtimeInfo->stamps.trainum = obj->m_RecvTimestamp;

    return 0;
}

int CSTMToJRU::triggerSending(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen)
{
    (void)data;
    (void)bitpos;
    (void)bitlen;

    obj->m_bSendTriggered = true;
    return 0;
}

