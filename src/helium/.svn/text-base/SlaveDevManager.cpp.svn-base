/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: SlaveDevManager.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月15日 10时17分15秒
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

#include "SlaveDevManager.h"
#include "Logger.h"

using namespace HELIUM;

HELIUM_DECL_SINGLETON(CSlaveDevManager);

CSlaveDevManager::CSlaveDevManager()
{
}

CSlaveDevManager::~CSlaveDevManager()
{
}

void CSlaveDevManager::update(const unsigned char* buf, int len)
{
    if (buf == NULL || len <= 0)
        return;

    unsigned char dev_count = *buf;
    if ((int)(dev_count * sizeof(DeviceItem) + 1) != len) 
    {
        return;
    }

    m_DevList.clear();
    const unsigned char* payload = buf + 1;
    for (unsigned char i = 0; i < dev_count; ++i)
    {
        DeviceItem dev_item;
        memcpy(&dev_item, payload, sizeof(DeviceItem));
        m_DevList.push_back(dev_item);

        payload += sizeof(DeviceItem);
    }
}

DeviceItem CSlaveDevManager::queryDeviceItem(uint16_t dat_id)
{
    DeviceItem result;
    memset(&result, 0, sizeof(DeviceItem));

    for (unsigned int i = 0; i < m_DevList.size(); ++i)
    {
        if ((uint16_t)(m_DevList[i].devid.id >> 16) == dat_id)
            return m_DevList[i];
    }

    return result;
}

void CSlaveDevManager::queryDeviceItems(uint8_t datype, ITEM_VEC& item_buf)
{
    for (unsigned int i = 0; i < m_DevList.size(); ++i)
    {
        if ((uint8_t)(m_DevList[i].devid.id >> 16) == datype)
            item_buf.push_back(m_DevList[i]);
    }
}

int CSlaveDevManager::getSlaveDevState(char *buf, int len) const
{
    if (m_DevList.empty() == true)
        return -1;

    /// 第一个字节为从板个数, 将其扩展为四个字节
    int slave_count = m_DevList.size();
    memcpy(buf, &slave_count, sizeof(int));
    int ret = sizeof(int);

    for (int i = 0; ret < len && i < (int)m_DevList.size(); ++i)
    {
        memcpy(buf + ret, &(m_DevList[i]), sizeof(DeviceItem));
        ret += sizeof(DeviceItem);
    }

    return ret;
}

