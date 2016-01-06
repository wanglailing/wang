/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: BoardBusOp.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月13日 16时36分09秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "BoardBusOp.h"
#include "Logger.h"
#include "SlaveDevManager.h"

using namespace HELIUM;
using namespace DMS;

HELIUM_DECL_SINGLETON(CBoardBusOp);

const int CBoardBusOp::FlushBufCount = 4096;
const char* CBoardBusOp::m_BusDevName = "/dev/EMCstatic3";

CBoardBusOp::CBoardBusOp() : m_SendSeq(0), m_BusFd(-1)
{
    m_FlushBuf = new char[FlushBufCount];
}

CBoardBusOp::~CBoardBusOp()
{
    delete [] m_FlushBuf;
    closeBus();
}

int CBoardBusOp::openBus(void)
{
    if ( (m_BusFd = open(m_BusDevName, O_RDWR | O_SYNC)) < 0 )
    {
        ERR_LOG((DATASWITCH, "Failed to open the BOARD BUS device{%s}!\n", strerror(errno)));
        return -1;
    }

    return 0;
}

void CBoardBusOp::closeBus(void)
{
    if (m_BusFd > 0)
    {
        close(m_BusFd);
        m_BusFd = -1;
    }
}

int CBoardBusOp::getBusFd(void) const
{
    return m_BusFd;
}

int CBoardBusOp::queryDeviceState(void) 
{
    char buf[64];
    memset(buf, 0x0, sizeof(buf));
    buf[sizeof(BusDataHeader)] = (uint8_t)INTER_GET_DEVSTATUS; 

    sendToBus(INTERNAL_SERVICE, buf, 1);

    return 0;
}

void CBoardBusOp::flushData(const char *buf, int len)
{
    if (buf == NULL || len <= 0)
        return;

    if ((int)(len + sizeof(BusDataHeader) + 2) > FlushBufCount)
    {
        ERR_LOG((DATASWITCH, "The data len is too long[%d]!!!\n", len));
        return;
    }

    memcpy(m_FlushBuf + sizeof(BusDataHeader), buf, len);
    sendToBus(NORMAL_SERVICE, m_FlushBuf, len);
}

void CBoardBusOp::multicastData(const char *buf, int len)
{
    /**
     * 前4个字节为设备标识符(第3个字节为设备类型)
     */
    if (buf == NULL || len <= 4)
        return;

    if ((int)(len + sizeof(BusDataHeader) + 2) > FlushBufCount)
    {
        ERR_LOG((DATASWITCH, "The data len is too long[%d]!!!\n", len));
        return;
    }

    uint8_t data_type = *(buf + 2);
    CSlaveDevManager::ITEM_VEC item_buf;
    SLAVEDEV_MGR->queryDeviceItems(data_type, item_buf);
    if (item_buf.empty())
        return;

    memcpy(m_FlushBuf + sizeof(BusDataHeader), buf, len);

    /// 向同一类型的从板发送数据
    for (unsigned int i = 0; i < item_buf.size(); ++i)
    {
        memcpy(m_FlushBuf + sizeof(BusDataHeader), &(item_buf[i].devid.id), sizeof(unsigned int));
        sendToBus(NORMAL_SERVICE, m_FlushBuf, len);
    }
}

int CBoardBusOp::sendToBus(BUS_SERVTYPE bus_servtype, char *buf, int len) 
{
    /// 填充帧头部
    BusDataHeader *pBusDatHead = (BusDataHeader *)buf;
    pBusDatHead->headFlag = 0xF0F0;
    pBusDatHead->payloadLen = len;
    pBusDatHead->seq = m_SendSeq++;
    pBusDatHead->ToS = (uint8_t)bus_servtype;

    int sendLen = sizeof(BusDataHeader) + len;

    /// 填充帧尾
    buf[sendLen++] = 0x0F;
    buf[sendLen++] = 0x0F;

    return writeData(buf, sendLen);
}

int CBoardBusOp::writeData(const char *buf, int len)
{
    if (m_BusFd < 0)
    {
        ERR_LOG((DATASWITCH, "The bus device was not opened!!!\n"));
        return -1;
    }

    ssize_t writeLen = write(m_BusFd, buf, len);
    if (writeLen < 0)
    {
        /// ERR_LOG((DATASWITCH, "Write data to bus error!!!\n"));
    }

    return writeLen;
}

int CBoardBusOp::recvFromBus(char *buf, int len)
{
    assert(buf != NULL && len > 0);

    ssize_t read_length = read(m_BusFd, buf, len);
    return read_length;
}

