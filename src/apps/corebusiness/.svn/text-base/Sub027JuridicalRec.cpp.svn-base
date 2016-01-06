/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: Sub027JuridicalRec.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月09日 10时51分33秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Sub027JuridicalRec.h"
#include "AtpNode.h"
#include "Utils.h"
#include "PacketManager.h"
#include "TrainEncoder.h"
#include "RbcTriggerSets.h"

using namespace COREBUSINESS;

const char* CJuridicalRecParser::reference = "Subset027";

CJuridicalRecParser::MSG_PARSER CJuridicalRecParser::MessageParsers[] = {
    {NID_GENERALMSG,                    NULL},
    {NID_DATAENTRYOVER,   parseDataEntryComp},
    {NID_EBORDER,               parseEBOrder},
    {NID_SBORDER,               parseSBOrder},
    {NID_EVENTS,                 parseEvents},
    {NID_TELEFROMBALISE, parseTeleFromBalise},
    {NID_MSGFROMEUROLOOP,               NULL},
    {NID_MSGFROMRADIO,                  NULL},
    {NID_MSGFROMRBC,         parseMsgFromRBC},
    {NID_MSGTORBC,             parseMsgToRBC},
    {NID_DRIVERACT,        parseDriverAction},
    {NID_BALISEGRPERR,         parseBGPError},
    {NID_RADIOERROR,         parseRadioError},
    {NID_STMINFO,                       NULL},
    {NID_DATAFROMEXTSRC,                NULL},
    {NID_STARTFIXEDTXT,   parseStartFixedTxt},
    {NID_STOPFIXEDTXT,     parseStopFixedTxt},
    {NID_STARTPLAINTXT,   parseStartPlainTxt},
    {NID_STOPPLAINTXT,     parseStopPlainTxt},
    {NID_RSPVALUE,     parseRestrictiveSpeed},
    {NID_TARGETSPEED,       parseTargetSpeed},
    {NID_TARGETDISTANCE, parseTargetDistance},
    {NID_RELEASESPEED,                  NULL},
    {NID_WARNING,               parseWarning},
    {NID_SRSPEEDDIST,                   NULL},
    {NID_STMSELECTED,                   NULL},
    {NID_PERMITSPEED,       parsePermitSpeed}
};

///消息头部的字段布局是按照v2.3.0版定义的，其它版本略有不同。
CJuridicalRecParser::HDR_LAYOUT CJuridicalRecParser::HdrFieldsLayout[] = {
    {HDRFIELD_NIDMSG,       8},
    {HDRFIELD_MSGLEN,      11},
    {HDRFIELD_YEAR,         7},
    {HDRFIELD_MONTH,        4},
    {HDRFIELD_DAY,          5},
    {HDRFIELD_HOUR,         5},
    {HDRFIELD_MINUTE,       6},
    {HDRFIELD_SECOND,       6},
    {HDRFIELD_TTS,          5},
    {HDRFIELD_QSCALE,       2},
    {HDRFIELD_NIDLRBG,     24},
    {HDRFIELD_DLRBG,       15},
    {HDRFIELD_QDIRLRBG,     2},
    {HDRFIELD_QDLRBG,       2},
    {HDRFIELD_LDOUBTOVER,  15},
    {HDRFIELD_LDOUBTUNDER, 15},
    {HDRFIELD_VTRAIN,       7},
    {HDRFIELD_DRIVERID,   384},
    {HDRFIELD_NIDENGINE,   24},
    {HDRFIELD_LEVEL,        3},
    {HDRFIELD_MODE,         4}
};

CJuridicalRecParser::DATAENTRY_LAYOUT CJuridicalRecParser::DatEntryCompLayout[] = {
    {7,  0}, /// V_MAXTRAIN
    {15, 0}, /// NC_TRAIN
    {12, 0}, /// L_TRAIN
    {3, 18}, /// N_SERVICE_SECTIONS(定义了V_SERVICEDECEL_CAP和A_SERVICEDECEL_CAP的个数)
    {3, 18}, /// N_EMERGENCY_SECTIONS(定义了V_EMERGENCYDECEL_CAP和A_EMERGENCYDECEL_CAP的个数)
    {12, 0}, /// T_CUT_OFF
    {12, 0}, /// T_SERVICE_DELAY
    {12, 0}, /// T_EMERGENCY_DELAY
    {8,  0}, /// M_LOADINGGAUGE
    {7,  0}, /// M_AXLELOAD
    {5,  8}, /// N_ITER(定义了M_TRACTION的个数)
    {2,  0}, /// M_AIRTIGHT
    {1,  0}, /// M_ADHESION
    {24, 0}, /// NID_MN
    {10, 0}, /// NID_C
    {14, 0}, /// NID_RBC
    {64, 0}, /// NID_RADIO
    {32, 0}  /// NID_OPERATIONAL(车次信息)
};

CJuridicalRecParser::CJuridicalRecParser(int interval, JRU_VERSION jru_ver, bool use_permited,
                                         AtpRTInfo* rtinfo) :
    CDataNode(interval), m_ProtoVer(jru_ver), m_C3Timestamp(0), m_EBOrder(0), m_SBOrder(0), m_TargetSpeed(0),
    m_RestrictiveSpeed(0), m_PermitSpeed(0), m_bGeneralInfoRecvd(false), m_bUsePermited(use_permited), m_Events(0),
    m_DrvActions(0), m_Warning(0), m_BaliseGrpErr(0), m_RadioError(0), m_pAtpRealtimeInfo(rtinfo)
{
    if (m_ProtoVer == JRU_200)
    {
        HdrFieldsLayout[HDRFIELD_MSGLEN].bitlen    = 10;
        HdrFieldsLayout[HDRFIELD_DRIVERID].bitlen  = 80;
        HdrFieldsLayout[HDRFIELD_NIDENGINE].bitlen = 32;
    }
    else if (m_ProtoVer == JRU_224 || m_ProtoVer == JRU_229)
    {
        HdrFieldsLayout[HDRFIELD_DRIVERID].bitlen  = 64;
        HdrFieldsLayout[HDRFIELD_NIDENGINE].bitlen = 32;
    }

    m_MsgHdrLength = calcMsgHeaderLen();

    m_FromRBCMsg = new uint8_t[sizeof(JRUMSG_HEADER) + RBC_MSG_LEN];
    m_ToRBCMsg   = new uint8_t[sizeof(JRUMSG_HEADER) + RBC_MSG_LEN];
    m_StartDisplayFixedTxt = new uint8_t[sizeof(JRUMSG_HEADER) + FIXED_TEXT_LEN];
    m_StopDisplayFixedTxt  = new uint8_t[sizeof(JRUMSG_HEADER) + FIXED_TEXT_LEN];
    m_StartDisplayPlainTxt = new uint8_t[sizeof(JRUMSG_HEADER) + PLAIN_TEXT_LEN];
    m_StopDisplayPlainTxt  = new uint8_t[sizeof(JRUMSG_HEADER) + PLAIN_TEXT_LEN];

    JRUMSG_HEADER* msg_hdr;
    msg_hdr = (JRUMSG_HEADER*)m_FromRBCMsg;
    msg_hdr->id = NID_MSGFROMRBC;
    msg_hdr->payload_len = 0;

    msg_hdr = (JRUMSG_HEADER*)m_ToRBCMsg;
    msg_hdr->id = NID_MSGTORBC;
    msg_hdr->payload_len = 0;

    msg_hdr = (JRUMSG_HEADER*)m_StartDisplayFixedTxt;
    msg_hdr->id = NID_STARTFIXEDTXT;
    msg_hdr->payload_len = 0;

    msg_hdr = (JRUMSG_HEADER*)m_StopDisplayFixedTxt;
    msg_hdr->id = NID_STOPFIXEDTXT;
    msg_hdr->payload_len = 0;

    msg_hdr = (JRUMSG_HEADER*)m_StartDisplayPlainTxt;
    msg_hdr->id = NID_STARTPLAINTXT;
    msg_hdr->payload_len = 0;

    msg_hdr = (JRUMSG_HEADER*)m_StopDisplayPlainTxt;
    msg_hdr->id = NID_STOPPLAINTXT;
    msg_hdr->payload_len = 0;

    m_JruPackets = new HELIUM::CBuffer();
}

CJuridicalRecParser::~CJuridicalRecParser()
{
    delete [] m_FromRBCMsg; m_FromRBCMsg = NULL;
    delete [] m_ToRBCMsg;   m_ToRBCMsg   = NULL;
    delete [] m_StartDisplayFixedTxt;   m_StartDisplayFixedTxt = NULL;
    delete [] m_StopDisplayFixedTxt;    m_StopDisplayFixedTxt  = NULL;
    delete [] m_StartDisplayPlainTxt;   m_StartDisplayPlainTxt = NULL;
    delete [] m_StopDisplayPlainTxt;    m_StopDisplayPlainTxt  = NULL;

    delete m_JruPackets; m_JruPackets = NULL;
}

void CJuridicalRecParser::resetDataInfo(uint8_t reset_area)
{
    if (m_ProtoVer == JRU_200)
    {
        m_pAtpRealtimeInfo->opbase.basic.modeLevel  = 0xff;
        m_pAtpRealtimeInfo->opbase.basic.speedInfo  = 0xffffffff;
        m_pAtpRealtimeInfo->opbase.basic.targetInfo = 0xffffffff;
    }

    /// C3和C2都超时后才复位制动命令
    if (reset_area)
    {
        m_pAtpRealtimeInfo->opbase.basic.brake = 0xff;
    }
}

int CJuridicalRecParser::calcMsgHeaderLen(void)
{
    int result = 0;
    for (unsigned int i = 0; i < COUNTOF(HdrFieldsLayout); ++i)
    {
        result += HdrFieldsLayout[i].bitlen;
    }

    return result;
}

void CJuridicalRecParser::parseDriverId(const unsigned char* buf, int total_bitlen, int bitpos, int bitlen)
{
    bool effective = false;
    uint32_t driver_id = 0;

    if (m_ProtoVer == JRU_230)
    {
        bitpos += 320;
        bitlen -= 320;
    }

    for (int i = 0; i < bitlen; i += 8)
    {
        uint8_t asciinum = (uint8_t)ConvertBitsToUInt32(buf, total_bitlen, bitpos + i, 8);
        if (asciinum < '0' || asciinum > '9')
            continue;

        driver_id = driver_id * 10 + (asciinum - 0x30);
        effective = true;
    }

    if (effective == true)
        m_pAtpRealtimeInfo->opbase.trainop.driverid = driver_id;
}

int CJuridicalRecParser::parseFrame(const unsigned char* buf, int len, MSG_INFO* msg_info, uint64_t now)
{
    if (buf == NULL || len <= 0)
    {
        return 0;
    }

    int result = 0;

    int total_bitlen = len << 3;
    int bitoffset = 0;

    m_bGeneralInfoRecvd = false;

    /// 解析头部
    for (unsigned int i = 0; i < COUNTOF(HdrFieldsLayout); ++i)
    {
        if (i == HDRFIELD_DRIVERID)
        {
            parseDriverId(buf, total_bitlen, bitoffset, HdrFieldsLayout[i].bitlen);
        }
        else
        {
            m_HdrFieldValues[i] = ConvertBitsToUInt32(buf, total_bitlen, bitoffset, HdrFieldsLayout[i].bitlen);
        }
        bitoffset += HdrFieldsLayout[i].bitlen;
    }

    m_pAtpRealtimeInfo->opbase.basic.dateTime = CAtpNode::convertDateTime(&m_HdrFieldValues[HDRFIELD_YEAR]);

    msg_info->nid = m_HdrFieldValues[HDRFIELD_NIDMSG];
    msg_info->len = m_HdrFieldValues[HDRFIELD_MSGLEN];

    if (m_ProtoVer == JRU_200)
    {
        m_pAtpRealtimeInfo->enctrain = m_HdrFieldValues[HDRFIELD_NIDENGINE];
        TRAIN_ENCODER->decodeTrainNum(m_pAtpRealtimeInfo->opbase.trainop.trainum, m_HdrFieldValues[HDRFIELD_NIDENGINE]);
        m_pAtpRealtimeInfo->stamps.trainum = now;
    }

    /// 解析消息内容
    unsigned int parser_index = m_HdrFieldValues[HDRFIELD_NIDMSG] - 1;
    int payload_bitlen = (m_HdrFieldValues[HDRFIELD_MSGLEN] << 3) - m_MsgHdrLength;
    if ((parser_index < COUNTOF(MessageParsers)) && MessageParsers[parser_index].callback)
    {
        result = MessageParsers[parser_index].callback(this, buf, total_bitlen, bitoffset, payload_bitlen);
    }

    if (m_HdrFieldValues[HDRFIELD_NIDMSG] != NID_STMINFO)
    {
        (void)checkTimeliness(now);

        /// 记录C3时间
        if (m_HdrFieldValues[HDRFIELD_LEVEL] != (uint8_t)LEVEL_STM)
        {
            m_C3Timestamp = m_pAtpRealtimeInfo->opbase.basic.dateTime;
        }

        m_pAtpRealtimeInfo->opbase.basic.modeLevel = (m_HdrFieldValues[HDRFIELD_LEVEL] << 4) +
            (m_HdrFieldValues[HDRFIELD_MODE]);

        m_pAtpRealtimeInfo->opbase.basic.speedInfo >>= 10;
        m_pAtpRealtimeInfo->opbase.basic.speedInfo <<= 10;
        m_pAtpRealtimeInfo->opbase.basic.speedInfo |= m_HdrFieldValues[HDRFIELD_VTRAIN];

        m_pAtpRealtimeInfo->stamps.atpspeed = now;
    }

    m_Status = (uint8_t)STATUS_OK;
    return result;
}

int CJuridicalRecParser::parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    (void)buf;
    (void)len;
    (void)nodes_buf;
    (void)now;

    return 0;
}

void CJuridicalRecParser::setBrakeSpeed(CJuridicalRecParser* obj, uint32_t sb_speed)
{
    uint32_t eb_speed;

    if (sb_speed >= 255)
        eb_speed = sb_speed + 10;
    else
        eb_speed = sb_speed + 5;

    obj->m_pAtpRealtimeInfo->opbase.basic.speedInfo &= 0x3FF;
    obj->m_pAtpRealtimeInfo->opbase.basic.speedInfo |= (eb_speed << 20);
    obj->m_pAtpRealtimeInfo->opbase.basic.speedInfo |= (sb_speed << 10);
}

int CJuridicalRecParser::parseDataEntryComp(CJuridicalRecParser* obj, const unsigned char* raw_data,
        int total_bitlen, int bitpos, int bitlen)
{
    if (obj->m_ProtoVer == JRU_230)
    {
        int field_pos = bitpos;
        for (unsigned int i = 0; i < COUNTOF(DatEntryCompLayout); ++i)
        {
            int field_len = DatEntryCompLayout[i].bitlen;
            if (DatEntryCompLayout[i].increment > 0)
            {
                if ((field_pos + field_len) > total_bitlen)
                    return bitlen;

                uint32_t times = ConvertBitsToUInt32(raw_data, total_bitlen, field_pos, field_len);
                field_pos += (times * DatEntryCompLayout[i].increment);
            }
            field_pos += field_len;
        }

        /// 消息长度符合字段布局
        if ((field_pos - bitpos) <= bitlen)
        {
            obj->m_pAtpRealtimeInfo->enctrain = ConvertBitsToUInt32(raw_data, total_bitlen, field_pos - 32, 32);
            TRAIN_ENCODER->decodeTrainNum(obj->m_pAtpRealtimeInfo->opbase.trainop.trainum, 
                    obj->m_pAtpRealtimeInfo->enctrain);
            obj->m_pAtpRealtimeInfo->stamps.trainum = obj->m_RecvTimestamp;
        }
    }

    return bitlen;
}

int CJuridicalRecParser::parseEBOrder(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    obj->m_bGeneralInfoRecvd = true;

    obj->m_EBOrder = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 1);
    if (obj->m_EBOrder == 1)
        obj->m_pAtpRealtimeInfo->opbase.basic.brake = (uint8_t)BRAKE_EB;
    else
        obj->m_pAtpRealtimeInfo->opbase.basic.brake = (uint8_t)BRAKE_NONE;

    return 1;
}

int CJuridicalRecParser::parseSBOrder(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    obj->m_bGeneralInfoRecvd = true;

    obj->m_SBOrder = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 1);
    if (obj->m_SBOrder == 1)
        obj->m_pAtpRealtimeInfo->opbase.basic.brake = (uint8_t)BRAKE_SB7;
    else
        obj->m_pAtpRealtimeInfo->opbase.basic.brake = (uint8_t)BRAKE_NONE;

    return 1;
}

int CJuridicalRecParser::parseTargetSpeed(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    obj->m_bGeneralInfoRecvd = true;

    obj->m_TargetSpeed = (uint16_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 7);
    obj->m_TargetSpeed *= 5;

    obj->m_pAtpRealtimeInfo->opbase.basic.targetInfo >>= 10;
    obj->m_pAtpRealtimeInfo->opbase.basic.targetInfo <<= 10;
    obj->m_pAtpRealtimeInfo->opbase.basic.targetInfo |= obj->m_TargetSpeed;
    return 7;
}

int CJuridicalRecParser::parseTargetDistance(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    float scale;
    uint8_t scale_coff;
    uint32_t target_distance;

    uint32_t dist = ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 17);
    if (obj->m_ProtoVer == JRU_230)
    {
        scale_coff = (uint8_t)(dist >> 15);
        target_distance = dist & 0x7fff;
    }
    else
    {
        scale_coff = (uint8_t)dist & 0x03;
        target_distance = dist >> 2;
    }

    switch (scale_coff)
    {
        case 0:
            scale = 0.1f;
            break;
        case 2:
            scale = 10.0f;
            break;
        default:
            scale = 1.0f;
            break;
    }

    target_distance *= scale;

    obj->m_pAtpRealtimeInfo->opbase.basic.targetInfo &= 0x3ff;
    obj->m_pAtpRealtimeInfo->opbase.basic.targetInfo |= (target_distance << 10);

    return 17;
}

int CJuridicalRecParser::parseTeleFromBalise(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    int result = 0;
    bool changed = false;
    for (int i = 0; i < BALISE_LENGTH; ++i)
    {
        uint8_t balise = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, (bitpos + i * 8), 8);
        if (obj->m_BalisePacket[i] != balise)
        {
            obj->m_BalisePacket[i] = balise;
            changed = true;
        }
    }

    if (changed == true)
    {
        HELIUM::CPacketManager::packData(obj->m_JruPackets, (uint8_t)DMSMSG_BALISE, 
                (unsigned char*)obj->m_BalisePacket, BALISE_LENGTH, 1);

        result = updateBaliseInfo((unsigned char*)obj->m_BalisePacket, BALISE_LENGTH, 
                obj->m_RecvTimestamp, obj->m_pAtpRealtimeInfo);
    }

    return result;
}

int CJuridicalRecParser::parseMsgFromRBC(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    if (obj->m_ProtoVer == JRU_230)
    {
        /// 跳过NID_C和NID_RBC
        bitpos += 24;
        bitlen -= 24;
    }

    uint8_t rbc_id = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 8);
    if (RBC_SETS->isRbcTriggered((uint8_t)NID_MSGFROMRBC, rbc_id) == true)
    {
        msgGather(obj->m_JruPackets, raw_data, obj->m_FromRBCMsg,
                total_bitlen, bitpos, bitlen, RBC_MSG_LEN);
    }

    return 0;
}

int CJuridicalRecParser::parseMsgToRBC(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    if (obj->m_ProtoVer == JRU_230)
    {
        /// 跳过NID_C和NID_RBC
        bitpos += 24;
        bitlen -= 24;
    }

    uint8_t rbc_id = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 8);
    if (obj->m_ProtoVer == JRU_230 && rbc_id == 129)
    {
        getTrainumInRbcMsg(obj, raw_data, total_bitlen, bitpos, bitlen);
    }

    if (RBC_SETS->isRbcTriggered((uint8_t)NID_MSGTORBC, rbc_id) == true)
    {
        msgGather(obj->m_JruPackets, raw_data, obj->m_ToRBCMsg,
                total_bitlen, bitpos, bitlen, RBC_MSG_LEN);
    }

    return 0;
}

void CJuridicalRecParser::getTrainumInRbcMsg(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    uint8_t packet_id;
    uint16_t packet_len;

    /// 跳过NID_MESSAGE、L_MESSAGE、T_TRAIN和NID_ENGINE
    bitpos += 74;
    while (bitpos < bitlen - 21)
    {
        packet_id = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 8);
        packet_len = (uint16_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos + 8, 13);
        if (packet_id == 11 && (bitpos + 21 + 32 < bitlen))
        {
            obj->m_pAtpRealtimeInfo->enctrain = ConvertBitsToUInt32(raw_data, total_bitlen, bitpos + 21, 32);
            TRAIN_ENCODER->decodeTrainNum(obj->m_pAtpRealtimeInfo->opbase.trainop.trainum, 
                    obj->m_pAtpRealtimeInfo->enctrain);
            obj->m_pAtpRealtimeInfo->stamps.trainum = obj->m_RecvTimestamp;
            break;
        }

        bitpos += packet_len;
    }
}

int CJuridicalRecParser::parseEvents(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    uint8_t events[sizeof(JRUMSG_HEADER) + 1];
    events[sizeof(JRUMSG_HEADER)] = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 8);
    if (obj->m_Events != events[sizeof(JRUMSG_HEADER)])
    {
        JRUMSG_HEADER* jru_hdr = (JRUMSG_HEADER*)events;
        jru_hdr->id = (uint8_t)NID_EVENTS;
        jru_hdr->payload_len = 1;
        obj->m_Events = events[sizeof(JRUMSG_HEADER)];
        HELIUM::CPacketManager::packData(obj->m_JruPackets, (uint8_t)DMSMSG_JRU, events, sizeof(events));
    }

    return 8;
}

int CJuridicalRecParser::parseDriverAction(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    uint8_t drvacts[sizeof(JRUMSG_HEADER) + 1];
    drvacts[sizeof(JRUMSG_HEADER)] = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 8);
    if (obj->m_DrvActions != drvacts[sizeof(JRUMSG_HEADER)])
    {
        JRUMSG_HEADER* jru_hdr = (JRUMSG_HEADER*)drvacts;
        jru_hdr->id = (uint8_t)NID_DRIVERACT;
        jru_hdr->payload_len = 1;
        obj->m_DrvActions = drvacts[sizeof(JRUMSG_HEADER)];
        HELIUM::CPacketManager::packData(obj->m_JruPackets, (uint8_t)DMSMSG_JRU, drvacts, sizeof(drvacts));
    }

    return 8;
}

int CJuridicalRecParser::parseBGPError(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    uint8_t msg[sizeof(JRUMSG_HEADER) + sizeof(uint32_t)];
    uint32_t* bgp_err = (uint32_t*)&msg[sizeof(JRUMSG_HEADER)];
    *bgp_err = ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 32);
    if (obj->m_BaliseGrpErr != *bgp_err)
    {
        JRUMSG_HEADER* jru_hdr = (JRUMSG_HEADER*)msg;
        jru_hdr->id = (uint8_t)NID_BALISEGRPERR;
        jru_hdr->payload_len = sizeof(uint32_t);
        obj->m_BaliseGrpErr = *bgp_err;
        HELIUM::CPacketManager::packData(obj->m_JruPackets, (uint8_t)DMSMSG_JRU, msg, sizeof(msg));
    }

    return 32;
}

int CJuridicalRecParser::parseRadioError(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    uint8_t msg[sizeof(JRUMSG_HEADER) + sizeof(uint32_t)];
    uint32_t* radio_err = (uint32_t*)&msg[sizeof(JRUMSG_HEADER)];
    *radio_err = ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 32);
    if (obj->m_RadioError != *radio_err)
    {
        JRUMSG_HEADER* jru_hdr = (JRUMSG_HEADER*)msg;
        jru_hdr->id = (uint8_t)NID_RADIOERROR;
        jru_hdr->payload_len = sizeof(uint32_t);
        obj->m_RadioError = *radio_err;
        HELIUM::CPacketManager::packData(obj->m_JruPackets, (uint8_t)DMSMSG_JRU, msg, sizeof(msg));
    }

    return 32;
}

void CJuridicalRecParser::msgGather(HELIUM::CBuffer* jru_container, const unsigned char* raw_data, 
        uint8_t* msg_buf, int total_bitlen, int bitpos, int bitlen, int max_length)
{
    int msglen = 0;
    bool changed = false;
    while (bitlen > 0)
    {
        if (msglen > max_length)
            break;

        int unitlen;
        if (bitlen > 8)
            unitlen = 8;
        else
            unitlen = bitlen;

        int msgoffset = sizeof(JRUMSG_HEADER) + msglen;
        uint8_t temp = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, unitlen);
        if (msg_buf[msgoffset] != temp)
        {
            msg_buf[msgoffset] = temp;
            changed = true;
        }

        msglen++;
        bitpos += unitlen;
        bitlen -= unitlen;
    }

    JRUMSG_HEADER* msg_hdr = (JRUMSG_HEADER*)msg_buf;
    if ( changed == true || (msglen > 0 && msg_hdr->payload_len != msglen) )
    {
        msg_hdr->payload_len = msglen;
        HELIUM::CPacketManager::packData(jru_container, (uint8_t)DMSMSG_JRU, msg_buf, msglen + sizeof(JRUMSG_HEADER));
    }
}

int CJuridicalRecParser::parseStartFixedTxt(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    msgGather(obj->m_JruPackets, raw_data, obj->m_StartDisplayFixedTxt, 
            total_bitlen, bitpos, bitlen, FIXED_TEXT_LEN);

    return bitlen;
}

int CJuridicalRecParser::parseStopFixedTxt(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    msgGather(obj->m_JruPackets, raw_data, obj->m_StopDisplayFixedTxt, 
            total_bitlen, bitpos, bitlen, FIXED_TEXT_LEN);

    return bitlen;
}

int CJuridicalRecParser::parseStartPlainTxt(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    msgGather(obj->m_JruPackets, raw_data, obj->m_StartDisplayPlainTxt, 
            total_bitlen, bitpos, bitlen, PLAIN_TEXT_LEN);

    return bitlen;
}

int CJuridicalRecParser::parseStopPlainTxt(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    msgGather(obj->m_JruPackets, raw_data, obj->m_StopDisplayPlainTxt, 
            total_bitlen, bitpos, bitlen, PLAIN_TEXT_LEN);

    return bitlen;
}

int CJuridicalRecParser::parseRestrictiveSpeed(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    if (obj->m_bUsePermited == false)
    {
        uint8_t speed;
        speed = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 7);

        /// speed的单位是5km/h，将其换算成1km/h
        obj->m_RestrictiveSpeed = (uint16_t)speed * 5;

        setBrakeSpeed(obj, obj->m_RestrictiveSpeed + 5);
    }

    return 7;
}

int CJuridicalRecParser::parseWarning(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    uint8_t warning[sizeof(JRUMSG_HEADER) + 1];
    warning[sizeof(JRUMSG_HEADER)] = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 8);
    if (obj->m_Warning != warning[sizeof(JRUMSG_HEADER)])
    {
        JRUMSG_HEADER* jru_hdr = (JRUMSG_HEADER*)warning;
        jru_hdr->id = (uint8_t)NID_WARNING;
        jru_hdr->payload_len = 1;
        obj->m_Warning = warning[sizeof(JRUMSG_HEADER)];
        HELIUM::CPacketManager::packData(obj->m_JruPackets, (uint8_t)DMSMSG_JRU, warning, sizeof(warning));
    }

    return 8;
}

int CJuridicalRecParser::parsePermitSpeed(CJuridicalRecParser* obj, const unsigned char* raw_data, 
        int total_bitlen, int bitpos, int bitlen)
{
    if (obj->m_bUsePermited == true)
    {
        obj->m_bGeneralInfoRecvd = true;

        uint8_t speed;
        speed = (uint8_t)ConvertBitsToUInt32(raw_data, total_bitlen, bitpos, 7);

        /// speed的单位是5km/h，将其换算成1km/h
        obj->m_PermitSpeed = (uint16_t)speed * 5;

        setBrakeSpeed(obj, obj->m_PermitSpeed + 5);
    }

    return 7;
}

