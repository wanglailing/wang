/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: SharedBuffer.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年03月25日 13时47分02秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>

#include "Logger.h"
#include "SharedBuffer.h"
#include "PowerdropDetect.h"

using namespace HELIUM;
using namespace STORESERVICE;

HELIUM_DECL_SINGLETON(CSharedBuffer);

CSharedBuffer::CSharedBuffer() : m_ReadPos(0), m_WritePos(0)
{
    /// 初始化缓冲区
    for (int i = 0; i < BUFFER_COUNT; ++i)
    {
        m_DataSlots[i] = new DataItem;
        ::memset(m_DataSlots[i], 0, sizeof(DataItem));
    }

    /// 初始化信号量
    if (::sem_init(&m_IdleSlot, 0, BUFFER_COUNT) < 0)
    {
        printf("Initialize Idle semaphore error!\n");
    }

    if (::sem_init(&m_StoredSlot, 0, 0) < 0)
    {
        printf("Initialize Stored semaphore error!\n");
    }
}

CSharedBuffer::~CSharedBuffer()
{
    for (int i = 0; i < BUFFER_COUNT; ++i)
    {
        delete m_DataSlots[i];
        m_DataSlots[i] = NULL;
    }

    ::sem_destroy(&m_IdleSlot);
    ::sem_destroy(&m_StoredSlot);
}

void CSharedBuffer::pushData(unsigned char type, const char* buf, int len, unsigned int now, char compress)
{
    static bool poweroff_flag = false;

    /// 系统掉电后，不再存储数据
    if (POWERDROP_DETECTER->getPowerState() == CPowerdropDetecter::POWER_OFF)
    {
        if (poweroff_flag == false)
        {
            poweroff_flag = true;
            sync();
            DBG_LOG((DATAWATCHER, "Flush the data buffer over!\n"));
        }

        return;
    }

    if (poweroff_flag == true)
    {
        poweroff_flag = false;
    }

    if (len > ITEM_CAPTION)
    {
        DBG_LOG((DATAWATCHER, "<panic>The data being stored is too long[%d]\n", len));
        return;
    }

    if (::sem_trywait(&m_IdleSlot) < 0)
    {
        return;
    }

    if (m_WritePos >= BUFFER_COUNT)
    {
        m_WritePos = 0;
    }

    /// 复制数据
    m_DataSlots[m_WritePos]->type = type;
    m_DataSlots[m_WritePos]->len  = len;
    m_DataSlots[m_WritePos]->timestamp = now;
    m_DataSlots[m_WritePos]->compress = compress;
    ::memcpy(m_DataSlots[m_WritePos]->data, buf, len);

    m_WritePos++;
    ::sem_post(&m_StoredSlot);
}

FileInfo* CSharedBuffer::getData(CDataRecorder* data_rec, char* compress_flag)
{
    FileInfo* file_info = NULL;

    ::sem_wait(&m_StoredSlot);
    {
        if (m_ReadPos >= BUFFER_COUNT)
        {
            m_ReadPos = 0;
        }

        /// 将数据从共享缓冲区放入文件描述符自身对应的缓冲区
        file_info = data_rec->queryFdBuffer(m_DataSlots[m_ReadPos]->type, m_DataSlots[m_ReadPos]->timestamp,
                m_DataSlots[m_ReadPos]->compress);
        if (file_info)
        {
            file_info->buf->append(m_DataSlots[m_ReadPos]->data, m_DataSlots[m_ReadPos]->len);
            *compress_flag = m_DataSlots[m_ReadPos]->compress;
        }
        else
        {
            DBG_LOG((DATAWATCHER, "Can not find the FD[%d]'s buffer!\n", m_DataSlots[m_ReadPos]->type));
        }

        m_DataSlots[m_ReadPos]->len = 0;

        m_ReadPos++;
    }
    ::sem_post(&m_IdleSlot);

    return file_info;
}

