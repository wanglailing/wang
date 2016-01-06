/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: AtpNode.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月03日 09时26分52秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "AtpNode.h"
#include "CirNode.h"
#include "Utils.h"
#include "BaliseParser.h"
#include "TrainEncoder.h"

using namespace HELIUM;
using namespace COREBUSINESS;

static const char* CarrierFreqSets[] = {"UNKNOWN", "0", "550", "650", "750", "850", "1700", "2000", "2300", "2600"};

static const char* LowFreqSets[] = {
    "UNKNOWN", "0", "H", "HU", "HB", "U", "U2S", "U2", "UU", "UUS",
    "LU", "LU2", "L", "L2", "L3", "L4", "L5", "L6", "25.7", "27.9", "U3"
};

/// 空字符串为占位符，实际上不存在这些制动信息
static const char* BrakeSets[] = {
    "NONE", "SB1", "SB2", "SB3", "SB4", "SB5", "SB6", "SB7", "SB8", "UNKNOWN", "", "", "", "EB", "", "PULLCUT"
};

CAtpNode::CAtpNode(int interval) : CDataNode(interval), m_VerInfo(""), m_SpeedInfo(0xffffffff)
{
    initOpBaseInfo(m_OperateBase);

    initOpBaseInfo(m_RealtimeInfo.opbase);
    m_RealtimeInfo.enctrain = 0xffffffff;

    ::memset(&m_RealtimeInfo.balise, 0x00, sizeof(m_RealtimeInfo.balise));
    ::memset(&m_RealtimeInfo.stamps, 0xFF, sizeof(m_RealtimeInfo.stamps));
    ::memset(&m_BaliseTelegram, 0x00, sizeof(m_BaliseTelegram));
    ::memset(&m_DataStatus, 0x00, sizeof(m_DataStatus));

    m_LastUpdateStamp = CTimeVal::gettimeofday();
}

void CAtpNode::initOpBaseInfo(OperBasic &opbase)
{
    ::memset(&opbase.basic, 0xff, sizeof(opbase.basic));
    opbase.basic.dateTime = 0;
    opbase.basic.runDistance = 0;
    ::memset(opbase.trainop.trainum.flag, 0x20, sizeof(opbase.trainop.trainum.flag));
    opbase.trainop.trainum.num = 0;
    opbase.trainop.driverid = 0;
}

void CAtpNode::updateRunDistance(unsigned int distance)
{
    (void)distance;
}

void CAtpNode::updateTrainum(const void* trainum, unsigned int now)
{
    (void)now;

    const CCirNode::CirTrainum* cir_trainum = (const CCirNode::CirTrainum*)trainum;
    if (cir_trainum->stamp == 0)
        return;

    if (::memcmp(&m_RealtimeInfo.opbase.trainop.trainum, &(cir_trainum->parsed), sizeof(TrainNumber)) == 0)
        return;

    ::memcpy(&m_RealtimeInfo.opbase.trainop.trainum, &(cir_trainum->parsed), sizeof(TrainNumber));
    m_RealtimeInfo.enctrain = TRAIN_ENCODER->encodeTrainNum(cir_trainum->parsed);
    m_RealtimeInfo.stamps.trainum = cir_trainum->stamp;
}

void CAtpNode::updateDataState(const void* pstate)
{
    if (pstate == NULL)
    {
        return;
    }

    ::memcpy(&m_DataStatus, pstate, sizeof(m_DataStatus));
}

unsigned short CAtpNode::getRunSpeed(uint64_t now)
{
    if (recvTimeout(now) == true)
        return 0;

    m_SpeedInfo = m_RealtimeInfo.opbase.basic.speedInfo;

    unsigned short run_speed = 0;
    run_speed = (unsigned short)m_SpeedInfo & 0x3ff;
    if (run_speed == 0x3ff)
    {
        run_speed = 0;
    }

    return run_speed;
}

unsigned char* CAtpNode::getRealtimeInfo(int* length)
{
    if (length)
    {
        *length = sizeof(m_RealtimeInfo);
    }

    m_RealtimeInfo.opbase.basic.reserved = m_TypeId;

    return (unsigned char*)&m_RealtimeInfo;
}

void CAtpNode::resetDataInfo(uint8_t reset_area)
{
    (void)reset_area;

    m_RealtimeInfo.opbase.basic.trackFreq  = 0xffff;
    m_RealtimeInfo.opbase.basic.modeLevel  = 0xff;
    m_RealtimeInfo.opbase.basic.speedInfo  = 0xffffffff;
    m_RealtimeInfo.opbase.basic.targetInfo = 0xffffffff;
    m_RealtimeInfo.opbase.basic.mileage    = 0xffffffff;
    m_RealtimeInfo.opbase.basic.brake      = 0xff;
}

uint32_t CAtpNode::convertDateTime(const uint32_t* date_time)
{
    if (date_time == NULL)
        return 0;

    /// date_time的元素依次为：年、月、日、时、分、秒
    return ((date_time[0] << 26) + \
            (date_time[1] << 22) + \
            (date_time[2] << 17) + \
            (date_time[3] << 12) + \
            (date_time[4] <<  6) + \
            (date_time[5]));
}

uint32_t CAtpNode::convertDateTime(const DATE_TIME& datetime)
{
    uint32_t date_time[6];
    date_time[0] = ConvertBCDToDecimal(datetime.year);
    if (date_time[0] >= 2000)
        date_time[0] -= 2000;
    date_time[1] = ConvertBCDToDecimal(datetime.month);
    date_time[2] = ConvertBCDToDecimal(datetime.day);
    date_time[3] = ConvertBCDToDecimal(datetime.hour);
    date_time[4] = ConvertBCDToDecimal(datetime.minute);
    date_time[5] = ConvertBCDToDecimal(datetime.second);

    return convertDateTime(date_time);
}

bool CAtpNode::opbaseInfoChanged(bool more_changed, uint64_t now)
{
    bool info_changed = false;

    /// 更新ATP数据状态
    unsigned int atp_state = getState(now);
    m_DataStatus.atp   = (uint8_t)atp_state;
    m_DataStatus.atpc2 = (uint8_t)(atp_state >> 16);

    /// 判断基础信息的触发项点(除ATP时间、里程标、走行距离和目标里程外，其余均为触发项)
    if ( more_changed == true ||
         (m_RealtimeInfo.opbase.basic.modeLevel != m_OperateBase.basic.modeLevel) ||
         (m_RealtimeInfo.opbase.basic.trackFreq != m_OperateBase.basic.trackFreq) ||
         (m_RealtimeInfo.opbase.basic.brake != m_OperateBase.basic.brake) ||
         (speedInfoChanged(m_RealtimeInfo.opbase.basic.speedInfo, m_OperateBase.basic.speedInfo) == true) )
    {
        info_changed = true;
    }

    /// 检查车次是否发生变化
    if (::memcmp(&m_OperateBase.trainop.trainum, &(m_RealtimeInfo.opbase.trainop.trainum), sizeof(TrainNumber)) &&
        m_RealtimeInfo.opbase.trainop.trainum.num != 0xffffffff)
    {
        ::memcpy(&m_OperateBase.trainop.trainum, &(m_RealtimeInfo.opbase.trainop.trainum), sizeof(TrainNumber));
        info_changed = true;
    }

    /// 检查司机号是否发生变化
    if (m_RealtimeInfo.opbase.trainop.driverid != 0xffffffff &&
        m_RealtimeInfo.opbase.trainop.driverid != m_OperateBase.trainop.driverid)
    {
        m_OperateBase.trainop.driverid = m_RealtimeInfo.opbase.trainop.driverid;
        info_changed = true;
    }

    /// 当触发项变化时，更新基础信息
    if (info_changed == true)
    {
        ::memcpy(&m_OperateBase.basic, &m_RealtimeInfo.opbase.basic, sizeof(BasicData));
    }

    return info_changed;
}

void CAtpNode::pushBaseWithDataState(HELIUM::CBuffer* nodes_buf)
{
    /// 使用预留字段传输ATP类型标识
    m_OperateBase.basic.reserved = m_TypeId;

    /// 每个基础包都要带一个状态包
    HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_STATUS,
                                     (unsigned char*)&m_DataStatus, sizeof(m_DataStatus));
    HELIUM::CPacketManager::packData(nodes_buf, (uint8_t)DMSMSG_BASE,
                                     (unsigned char*)&m_OperateBase, sizeof(m_OperateBase));
}

bool CAtpNode::speedInfoChanged(uint32_t infoa, uint32_t infob)
{
    uint16_t run_speeda = (uint16_t)(infoa & 0x03FF);
    uint16_t sb_speeda = (uint16_t)((infoa >> 10) & 0x03FF);
    uint16_t eb_speeda = (uint16_t)((infoa >> 20) & 0x03FF);

    uint16_t run_speedb = (uint16_t)(infob & 0x03FF);
    uint16_t sb_speedb = (uint16_t)((infob >> 10) & 0x03FF);
    uint16_t eb_speedb = (uint16_t)((infob >> 20) & 0x03FF);

    return ( isSpeedChanged(run_speeda, run_speedb) ||
             isSpeedChanged(sb_speeda,   sb_speedb) ||
             isSpeedChanged(eb_speeda,   eb_speedb) );
}

bool CAtpNode::isSpeedChanged(uint16_t speeda, uint16_t speedb)
{
    return ((speeda == 0 && speedb != 0) || (speeda != 0 && speedb == 0) || (abs(speeda - speedb) >= 10));
}

void CAtpNode::trimString(char* str, int strlen)
{
    int i = 0;
    while (i < strlen)
    {
        if (str[i] == 0x20)
        {
            for (int j = i + 1; j < strlen; ++j)
            {
                if (str[j] != 0x20)
                {
                    str[i++] = str[j];
                    str[j] = '\0';
                }
            }
        }
        else
        {
            i++;
        }
    }
}

void CAtpNode::dump(void)
{
    static int seq;
    DBG_LOG((DATAWATCHER, "-------------------- ATP:%d --------------------\n", seq++));
    DBG_LOG((DATAWATCHER, "Train     ID: 10086\n"));
    DBG_LOG((DATAWATCHER, "Driver    ID: %08u\n", m_OperateBase.trainop.driverid));

    char train_letters[16] = {'\0'};
    ::memcpy(train_letters, m_OperateBase.trainop.trainum.flag, sizeof(m_OperateBase.trainop.trainum.flag));
    trimString(train_letters, sizeof(train_letters));
    DBG_LOG((DATAWATCHER, "Train Number: %s%d\n", train_letters, m_OperateBase.trainop.trainum.num));

    uint8_t year, month, day, hour, minute, second;
    year   = (uint8_t)(m_OperateBase.basic.dateTime >> 26);
    month  = (uint8_t)(m_OperateBase.basic.dateTime >> 22 & 0xf);
    day    = (uint8_t)(m_OperateBase.basic.dateTime >> 17 & 0x1f);
    hour   = (uint8_t)(m_OperateBase.basic.dateTime >> 12 & 0x1f);
    minute = (uint8_t)(m_OperateBase.basic.dateTime >> 6  & 0x3f);
    second = (uint8_t)(m_OperateBase.basic.dateTime & 0x3f);

    DBG_LOG((DATAWATCHER, "Date    Time: %u-%02u-%02u %02u:%02u:%02u\n",
                (uint32_t)year + 2000, month, day, hour, minute, second));

    DBG_LOG((DATAWATCHER, "Mileage     : K%u+%u\n", m_OperateBase.basic.mileage / 1000,
                m_OperateBase.basic.mileage % 1000));
    DBG_LOG((DATAWATCHER, "Run Distance: %u m\n", m_OperateBase.basic.runDistance));
    DBG_LOG((DATAWATCHER, "EB Speed    : %3u km/h\n", (m_OperateBase.basic.speedInfo >> 20) & 0x03ff));
    DBG_LOG((DATAWATCHER, "SB Speed    : %3u km/h\n", (m_OperateBase.basic.speedInfo >> 10) & 0x03ff));
    DBG_LOG((DATAWATCHER, "Run Speed   : %3u km/h\n", m_OperateBase.basic.speedInfo & 0x03ff));
    DBG_LOG((DATAWATCHER, "Level: %u; Mode: %u\n",
                m_OperateBase.basic.modeLevel >> 4, m_OperateBase.basic.modeLevel & 0x0f));

    DBG_LOG((DATAWATCHER, "Target Dis. : %u m; Target Speed: %3u km/h\n", m_OperateBase.basic.targetInfo >> 10,
                m_OperateBase.basic.targetInfo & 0x03ff));

    uint8_t carrier_freq = m_OperateBase.basic.trackFreq & 0x0f;
    const char* carrfreq_depict = "";
    if (carrier_freq < FREQ_TOTAL)
        carrfreq_depict = CarrierFreqSets[carrier_freq];

    uint8_t low_freq = m_OperateBase.basic.trackFreq >> 4;
    const char* lowfreq_depict = "";
    if (low_freq < LOWFREQ_TOTAL)
        lowfreq_depict = LowFreqSets[low_freq];

    DBG_LOG((DATAWATCHER, "Carrier Freq: %u(%s); Low Freq: %u(%s)\n", carrier_freq, carrfreq_depict,
                low_freq, lowfreq_depict));

    const char* brake_depict = "";
    if (m_OperateBase.basic.brake < BRAKE_TOTAL)
        brake_depict = BrakeSets[m_OperateBase.basic.brake];
    DBG_LOG((DATAWATCHER, "Brake  Order: %u(%s)\n", m_OperateBase.basic.brake, brake_depict));

    static unsigned int balise_no;
    if (balise_no != m_RealtimeInfo.balise.passbalino)
    {
        balise_no = m_RealtimeInfo.balise.passbalino;

        char dump_baliseno[32];
        CBaliseParser::dumpBaliseNo(balise_no, dump_baliseno, sizeof(dump_baliseno));
        DBG_LOG((DATAWATCHER, "Balise No.  : %s\n", dump_baliseno));
    }
}

