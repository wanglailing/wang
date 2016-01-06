/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PacketBuffer.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月28日 13时35分50秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>

#include "PacketBuffer.h"

using namespace TRANSPORTCTRL;

CPacketBuff::CPacketBuff() : m_MaxID(0)
{
    m_Container = new ITEM_MAPPER;
}

CPacketBuff::~CPacketBuff()
{
    if (m_Container)
    {
        ITEM_MAPPER::iterator iter;
        for (iter = m_Container->begin(); iter != m_Container->end(); ++iter)
        {
            delete iter->second.data;
        }

        m_Container->clear();
        delete m_Container; 
        m_Container = NULL;
    }
}

unsigned int CPacketBuff::addToBuf(unsigned int id, ACK_TYPE ack_type, std::string* data)
{
    if (data != NULL)
    {
        m_Container->insert(std::make_pair(id, PACK_ITEM(ack_type, data)));
        if (id > m_MaxID)
            m_MaxID = id;
    }

    return 0;
}

std::string* CPacketBuff::getItemFromBuf(unsigned int id)
{
    ITEM_MAPPER::const_iterator iter = m_Container->find(id);
    if (iter != m_Container->end())
        return iter->second.data;
    else
        return NULL;
}

void CPacketBuff::removeContinuedAckItems(unsigned int last_id)
{
    bool erase_flag = false;
    ITEM_MAPPER::iterator begin_iter = m_Container->begin();
    ITEM_MAPPER::iterator end_iter = begin_iter;
    while (end_iter != m_Container->end())
    {
        if (end_iter->second.acked == NAK || end_iter->first > last_id)
            break;
        else
        {
            erase_flag = true;
            delete end_iter->second.data;
            end_iter++;
        }
    }

    if (erase_flag == true)
    {
        m_Container->erase(begin_iter, end_iter);
    }
}

int CPacketBuff::removeItem(unsigned int id)
{
    ITEM_MAPPER::iterator iter = m_Container->find(id);
    if (iter != m_Container->end())
    {
        iter->second.acked = ACK;
    }

    removeContinuedAckItems(id);

    return 0;
}

int CPacketBuff::removeItems(unsigned int begin_id, unsigned int end_id)
{
    /// 确保begin_id小于end_id
    if (begin_id > end_id)
    {
        unsigned int temp;
        temp = begin_id;
        begin_id = end_id;
        end_id = temp;
    }

    ITEM_MAPPER::iterator iter;
    for (iter = m_Container->begin(); iter != m_Container->end(); ++iter)
    {
        if (iter->first > end_id)
            break;

        if (iter->first >= begin_id && iter->first <= end_id)
            iter->second.acked = ACK;
    }

    removeContinuedAckItems(end_id);

    return 0;
}

void CPacketBuff::reset(void)
{
    ITEM_MAPPER::iterator iter;
    for (iter = m_Container->begin(); iter != m_Container->end(); ++iter)
    {
        iter->second.acked = NAK;
    }
}

void CPacketBuff::clear(void)
{
    ITEM_MAPPER::iterator iter;
    for (iter = m_Container->begin(); iter != m_Container->end(); ++iter)
    {
        delete iter->second.data;
    }

    m_Container->clear();
    m_MaxID = 0;
}

unsigned int CPacketBuff::getFirstNakID(void)
{
    ITEM_MAPPER::const_iterator iter;
    for (iter = m_Container->begin(); iter != m_Container->end(); ++iter)
    {
        if (iter->second.acked == NAK)
            return iter->first;
    }

    return m_MaxID;
}

unsigned int CPacketBuff::getNextNakID(unsigned int current_id)
{
    ITEM_MAPPER::const_iterator iter = m_Container->find(current_id);
    for (; iter != m_Container->end(); ++iter)
    {
        if (iter->second.acked == NAK)
            return iter->first;
    }

    return getFirstNakID();
}

unsigned int CPacketBuff::getLastID(void)
{
    if (m_Container->empty())
        return m_MaxID;
    else
        return m_Container->rbegin()->first;
}

std::string CPacketBuff::getAckIDs(unsigned int need_id)
{
    std::string result("");
    char ids_buf[64] = {0};

    if (m_Container->empty())
    {
        if (need_id > 1)
            snprintf(ids_buf, sizeof(ids_buf) - 1, "0-%d|", need_id - 1);
        else if (need_id > 0)
            strcpy(ids_buf, "0|");
        else
            strcpy(ids_buf, "");

        result = ids_buf;
    }
    else
    {
        unsigned int begin_id;
        unsigned int end_id;

        ITEM_MAPPER::const_iterator iter = m_Container->begin();

        if (need_id > 0)
        {
            begin_id = 0;
            end_id = need_id - 1;
        }
        else
        {
            begin_id = iter->first;
            end_id = begin_id;
            iter++;
        }
        
        for (; iter != m_Container->end(); ++iter)
        {
            if (iter->first == end_id + 1)
            {
                ++end_id;
            }
            else
            {
                if (begin_id == end_id)
                    snprintf(ids_buf, sizeof(ids_buf) - 1, "%d|", end_id);
                else
                    snprintf(ids_buf, sizeof(ids_buf) - 1, "%d-%d|", begin_id, end_id);

                begin_id = iter->first;
                end_id = begin_id;
                result += ids_buf;
            }
        }

        if (begin_id == end_id)
            snprintf(ids_buf, sizeof(ids_buf) - 1, "%d|", end_id);
        else
            snprintf(ids_buf, sizeof(ids_buf) - 1, "%d-%d|", begin_id, end_id);

        result += ids_buf;
    }

    return result;
}

void CPacketBuff::setToAcked(unsigned int packet_id)
{
    ITEM_MAPPER::iterator iter = m_Container->find(packet_id);
    if (iter != m_Container->end())
    {
        iter->second.acked = ACK;
    }
}

