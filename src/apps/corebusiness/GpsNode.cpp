/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GpsNode.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月26日 10时21分13秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <string.h>

#include "GpsNode.h"
#include "PacketManager.h"
#include "TimeVal.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace COREBUSINESS;

CGpsNode::CGpsNode(int interval) : CDataNode(interval), m_AdjustCount(0)
{
    memset(&m_GpsRealtimeInfo, 0, sizeof(m_GpsRealtimeInfo));
    memset(&m_GpsRealtimeInfo.stamp, 0xFF, sizeof(m_GpsRealtimeInfo.stamp));
}

CGpsNode::~CGpsNode()
{
}

int CGpsNode::parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    (void)nodes_buf;

    int result = 0;
    (void)checkTimeliness(now);

    if (len == sizeof(GPSPROTO_DATA))
    {
        GPSPROTO_DATA* gps_data = (GPSPROTO_DATA*)buf;
        uint16_t gps_speed = (uint16_t)(gps_data->speed / 1000);
        if (m_GpsRealtimeInfo.params.longitude != gps_data->longitude ||
            m_GpsRealtimeInfo.params.latitude != gps_data->latitude ||
            m_GpsRealtimeInfo.params.speed != gps_speed)
        {
            result = 1;
        }

        m_GpsRealtimeInfo.params.longitude = gps_data->longitude;
        m_GpsRealtimeInfo.params.latitude = gps_data->latitude;
        m_GpsRealtimeInfo.params.speed = gps_speed;
        m_GpsRealtimeInfo.params.status = (gps_data->status << 6) | gps_data->starnum;

        if (gps_data->status & 0x01)
        {
            m_GpsRealtimeInfo.stamp = now;
            m_Status = (uint8_t)STATUS_OK;
        }
        else
        {
            m_Status = (uint8_t)STATUS_ERROR;
        }

        /// GPS校时
        if ((m_AdjustCount++ % 20 == 0) && (gps_data->status & 0x01))
        {
            struct tm gps_time;
            memset(&gps_time, 0, sizeof(gps_time));
            gps_time.tm_year = gps_data->year + 2000;
            gps_time.tm_mon  = gps_data->month;
            gps_time.tm_mday = gps_data->day;
            gps_time.tm_hour = gps_data->hour;
            gps_time.tm_min  = gps_data->minute;
            gps_time.tm_sec  = gps_data->second;

            CTimeVal::adjustTime(&gps_time, 10, 0, 1);
        }
    }
    else
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
    }

    return result;
}

void CGpsNode::dump(void)
{
    static int seq;
    DBG_LOG((DATAWATCHER, "-------------------- GPS:%d --------------------\n", seq++));
    DBG_LOG((DATAWATCHER, "Longitude   : %d\n", m_GpsRealtimeInfo.params.longitude));
    DBG_LOG((DATAWATCHER, "Latitude    : %d\n", m_GpsRealtimeInfo.params.latitude));
    DBG_LOG((DATAWATCHER, "Run Speed   : %3u km/h\n", m_GpsRealtimeInfo.params.speed));
}

unsigned char* CGpsNode::getParsedInfo(int* length)
{
    if (length != NULL)
        *length = sizeof(GpsRTInfo);

    return (unsigned char*)&m_GpsRealtimeInfo;
}

