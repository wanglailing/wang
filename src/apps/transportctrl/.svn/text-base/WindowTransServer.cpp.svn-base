/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: WindowTransServer.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月29日 16时42分31秒
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

#include "WindowTransServer.h"
#include "Logger.h"
#include "Upgrader.h"
#include "GprsPackage.h"
#include "TransportApp.h"

#include "../DmsBusinessDat.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

CWTransServer::CWTransServer() : CWindowTrans(), m_NeededID(0)
{
    m_DataProcs[GPRS_DMSDESC]  = procDmsUpgradeDesc;
    m_DataProcs[GPRS_DMSCONT]  = procDmsUpgradeContent;
    m_DataProcs[GPRS_EOASDESC] = forwardEoasUpgradeDesc;
    m_DataProcs[GPRS_EOASCONT] = forwardEoasUpgradeContent;
}

CWTransServer::~CWTransServer()
{
}

std::string CWTransServer::insertPacket(std::string* packet)
{
    if (packet == NULL)
        return std::string("");

    std::string result("");
    unsigned int cli_flag_pos = packet->find("WTC#", 0);
    if (cli_flag_pos != std::string::npos)
    {
        unsigned int dat_flag_pos;
        if ((dat_flag_pos = packet->find("DT#", cli_flag_pos)) != std::string::npos)
        {
            if (m_bConnected == true)
            {
                dat_flag_pos += 3;
                unsigned int upright_spliter = packet->find('|', dat_flag_pos);
                if (upright_spliter != std::string::npos)
                {
                    unsigned int data_id = atoi(packet->substr(dat_flag_pos, upright_spliter - dat_flag_pos).c_str());
                    if (data_id >= m_NeededID)
                    {
                        /// 只保留数据负载
                        packet->erase(0, upright_spliter + 1);
                        m_Buffer->addToBuf(data_id, CPacketBuff::ACK, packet);
                    }
                }

                result = std::string("RECV#") + m_Buffer->getAckIDs(m_NeededID);
            }
            else
            {
                result = std::string("RECONNECT#");
            }
        }
        else if ((dat_flag_pos = packet->find("CONNECT#", cli_flag_pos)) != std::string::npos)
        {
            if (m_bConnected == false)
            {
                m_bConnected = true;
                m_NeededID = 0;
                m_Buffer->clear();

                unsigned int id_flag_pos, id_end_pos;
                if ((id_flag_pos = packet->find("SENDID:", dat_flag_pos + 8)) != std::string::npos)
                {
                    if ((id_end_pos = packet->find('#', id_flag_pos + 7)) != std::string::npos)
                    {
                        m_NeededID = atoi(packet->substr(id_flag_pos + 7, id_end_pos - id_flag_pos - 7).c_str());
                    }
                }
            }

            result = std::string("CONNECTED#");
        }

        result = std::string(CDataCoder::DmsTagHead) + std::string("WTS#ACK#") + result;
    }

    CDataCoder::wrapData(result, ::strlen(CDataCoder::DmsTagHead), CDataCoder::DmsTagTail);
    return result;
}

void CWTransServer::processPackets(void)
{
    std::string *enc_data;
    while ((enc_data = m_Buffer->getItemFromBuf(m_NeededID)) != NULL)
    {
        CDataCoder::decodeData(&m_DecompressBuf, enc_data);
        processRawData(m_DecompressBuf.peek(), m_DecompressBuf.readableBytes());

        m_Buffer->removeItem(m_NeededID++);
    }
}

void CWTransServer::processRawData(const char *data, int length)
{
    if (data == NULL || length <= 0)
        return;

    const GPRS_HEADER* gprs_hdr = (const GPRS_HEADER*)data;
    std::map<unsigned short, OpFunc>::iterator iter;
    iter = m_DataProcs.find(gprs_hdr->dataid);
    if (iter != m_DataProcs.end())
    {
        iter->second(data + sizeof(GPRS_HEADER), length - sizeof(GPRS_HEADER));
    }
    else
    {
        DBG_LOG((TRANSCTRL, "Unknown data id[%04X]!\n", gprs_hdr->dataid));
    }
}

void CWTransServer::procDmsUpgradeDesc(const char *data, int length)
{
    if (length != sizeof(FILE_DESCRIPTION))
    {
        DBG_LOG((TRANSCTRL, "Description length error[%d]!\n", length));
        return;
    }

    const FILE_DESCRIPTION* file_desc = (const FILE_DESCRIPTION*)data;
    UPGRADER->createUpgradeFile(file_desc);
}

void CWTransServer::procDmsUpgradeContent(const char *data, int length)
{
    UPGRADER->writeToFile(data, length);
}

void CWTransServer::forwardEoasUpgradeDesc(const char *data, int length)
{
    DATA_SINK->forwardData((uint8_t)PROG_EOASERV, UPGRADE_DESC, data, length);
}

void CWTransServer::forwardEoasUpgradeContent(const char *data, int length)
{
    DATA_SINK->forwardData((uint8_t)PROG_EOASERV, UPGRADE_CONTENT, data, length);
}

