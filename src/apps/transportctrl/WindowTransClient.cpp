/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: WindowTransClient.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月29日 10时54分47秒
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
#include <stdlib.h>
#include <string.h>

#include "WindowTransClient.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

CWTransClient::CWTransClient() : CWindowTrans(), m_SendID(0), m_CurrentID(0)
{
}

CWTransClient::~CWTransClient()
{
}

void CWTransClient::reset(void)
{
    m_bConnected = false;
    m_SendID = 0;
    m_CurrentID = 0;

    m_Buffer->clear();
}

void CWTransClient::insertPacket(std::string* packet)
{
    if (packet == NULL)
        return;

    m_Buffer->addToBuf(m_CurrentID++, CPacketBuff::NAK, packet);
}

int CWTransClient::getSendingPacket(char* dst, int dst_len)
{
    if (dst == NULL || dst_len <= 0)
        return 0;

    if (m_bConnected == true)
    {
        std::string* item = m_Buffer->getItemFromBuf(m_SendID);
        if (item == NULL || ((int)(m_SendID - m_Buffer->getFirstNakID()) > TransWindowSize))
        {
            m_SendID = m_Buffer->getFirstNakID();
            return 0;
        }
        else
        {
            ::snprintf(dst, dst_len, "%sWTC#DT#%u|%s", CDataCoder::DmsTagHead, m_SendID, item->c_str());
            m_SendID = m_Buffer->getNextNakID(m_SendID);
        }
    }
    else  /// 发送连接报文
    {
        m_Buffer->reset();
        m_SendID = m_Buffer->getFirstNakID();
        ::snprintf(dst, dst_len, "%sWTC#CONNECT#SENDID:%u#", CDataCoder::DmsTagHead, m_SendID);
    }

    CDataCoder::wrapData(dst, dst_len, ::strlen(CDataCoder::DmsTagHead), CDataCoder::DmsTagTail);

    return ::strlen(dst);
}

void CWTransClient::parseACK(std::string& ack)
{
    unsigned int ack_pos = ack.find("ACK#", 0);
    if (ack_pos > 0)
    {
        unsigned int echo_type_pos;
        if ((echo_type_pos = ack.find("RECV#", ack_pos)) != std::string::npos)
        {
            echo_type_pos += 5;

            unsigned int upright_spliter;
            while ((upright_spliter = ack.find('|', echo_type_pos)) != std::string::npos)
            {
                unsigned int begin_id;
                unsigned int end_id;
                std::string id_str = ack.substr(echo_type_pos, upright_spliter - echo_type_pos);
                unsigned int across_spliter;
                if ((across_spliter = id_str.find('-', 0)) != std::string::npos)
                {
                    sscanf(id_str.c_str(), "%u-%u", &begin_id, &end_id);
                    m_Buffer->removeItems(begin_id, end_id);
                }
                else
                {
                    begin_id = atoi(id_str.c_str());
                    m_Buffer->removeItem(begin_id);
                }

                echo_type_pos = upright_spliter + 1;
            }
        }
        else if ((echo_type_pos = ack.find("CONNECTED#", ack_pos)) != std::string::npos)
        {
            m_bConnected = true;
        }
        else if ((echo_type_pos = ack.find("RECONNECT#", ack_pos)) != std::string::npos)
        {
            m_bConnected = false;
            m_Buffer->reset();
            m_SendID = m_Buffer->getFirstNakID();
            m_CurrentID = m_Buffer->getLastID() + 1;
        }
    }
}

