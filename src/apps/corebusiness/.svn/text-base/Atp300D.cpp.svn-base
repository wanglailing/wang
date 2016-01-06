/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Atp300D.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月09日 09时58分40秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Atp300D.h"
#include "Utils.h"
#include "AtpAPI.h"

using namespace COREBUSINESS;

static CAtp300D* atp_300D = 0;

int atpInitPlugin(void)
{
    atp_300D = new COREBUSINESS::CAtp300D(5000);
    return 0;
}

int atpFreePlugin(void)
{
    delete atp_300D; atp_300D = 0;
    return 0;
}

const char* atpGetVersion(void)
{
    return atp_300D->getVersion();
}

unsigned short atpGetRunSpeed(uint64_t now)
{
    return atp_300D->getRunSpeed(now);
}

unsigned int atpGetState(uint64_t now)
{
    return atp_300D->getState(now);
}

int atpParseData(const unsigned char* buf, int length, void* parsed_buf, uint64_t now)
{
    return atp_300D->parseData(buf, length, (HELIUM::CBuffer*)parsed_buf, now);
}

void atpUpdateRunDistance(unsigned int distance)
{
    atp_300D->updateRunDistance(distance);
}

void atpUpdateTrainNumber(const void* trainum, unsigned int timestamp)
{
    atp_300D->updateTrainum(trainum, timestamp);
}

unsigned char* atpGetSharedInfo(int* length)
{
    return atp_300D->getSharedInfo(length);
}

//////////////////////////////////// CAtp300D的定义 //////////////////////////////////// 
CAtp300D::PACKET_PARSER PacketParsers[] = {
    {NID_ETCSMODE,            parseETCSMode},///
    {NID_CURRSPEED,          parseCurrSpeed},///
    {NID_TARGETVALUES,    parseTargetValues},///
    {NID_BRAKEAPP,            parseBrakeApp},///
    {NID_ATPSYSWITCH,     parseATPSysSwitch},///
    {NID_ETCSOBUDIAG, parseETCSObuDiagnosis},
    {NID_SHORTBALISE,  parseShortBaliseTele},
    {NID_LONGBALISE,    parseLongBaliseTele},
    {NID_DRIVERTEXT,        parseDriverText},///
    {NID_TCRSYSWITCH,     parseTCRSysSwitch},///
    {NID_PROCIMAGE,       parseProcessImage}
};

int CAtp300D::parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    int offset = sizeof(HELIUM::SUBPACK_HEADER);
    if (buf == NULL || len <= offset)
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
    }

    /// 验证校验和

    /// 解析报文
    uint8_t packet_id = *(buf + offset);
}

int CAtp300D::parseETCSMode(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    /// 模式
    uint8_t mode = data[0];
    if (mode >= 0x0F)
        mode = 0x0F;

    return 0;
}

int CAtp300D::parseCurrSpeed(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    /// 运行速度
    uint16_t run_speed = (uint16_t)data[0] << 8;
    run_speed |= data[1];

    /// 将速度单位由0.01m/s转换为1km/h
    run_speed *= 0.036;

    return 0;
}

int CAtp300D::parseTargetValues(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    TargetValues* target_values = (TargetValues*)data;

    /// 允许速度
    target_values->permit_speed = ::ntohs(target_values->permit_speed);
    target_values->permit_speed *= 0.036;

    /// 目标速度
    target_values->target_speed = ::ntohs(target_values->target_speed);
    target_values->target_speed *= 0.036;

    return 0;
}

int CAtp300D::parseBrakeApp(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    /// 制动命令
    uint8_t eb_cmd = data[0];
    uint8_t sb_cmd = data[1];

    return 0;
}

BRAKE_ORDER CAtp300D::parseEBOrder(uint8_t brake_info) const
{
    if ((brake_info & 0x07) == 0x02)
        return BRAKE_EB;
    else
        return BRAKE_NONE;
}

BRAKE_ORDER CAtp300D::parseSBOrder(uint8_t brake_info) const
{
    if ((brake_info & 0x07) == 0x02)
        return BRAKE_SB7;
    else
        return BRAKE_NONE;
}

int CAtp300D::parseATPSysSwitch(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    uint8_t bypass = data[0];
    return 0;
}

int CAtp300D::parseETCSObuDiagnosis(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    return 0;
}

int CAtp300D::parseShortBaliseTele(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    return 0;
}

int CAtp300D::parseLongBaliseTele(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    return 0;
}

int CAtp300D::parseProcessImage(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    return 0;
}

int CAtp300D::parseDriverText(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    uint16_t telegram_data = (uint16_t)data[0] << 8;
    telegram_data |= data[1];

    return 0;
}

int CAtp300D::parseTCRSysSwitch(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen)
{
    uint8_t bypass = data[0];

    return 0;
}

