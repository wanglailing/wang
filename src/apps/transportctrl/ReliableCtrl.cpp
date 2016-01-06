/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ReliableCtrl.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月07日 16时51分33秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "ReliableCtrl.h"
#include "MemDump.h"
#include "DataCoder.h"
#include "Logger.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

CReliableCtrl::CReliableCtrl()
{
    m_transClient = new CWTransClient();
    m_transServer = new CWTransServer();
}

CReliableCtrl::~CReliableCtrl()
{
    delete m_transClient; m_transClient = NULL;
    delete m_transServer; m_transServer = NULL;
}

int CReliableCtrl::procBusinessPacket(CBuffer* packet)
{
    std::string* encoded = CDataCoder::encodeData(packet);
    m_transClient->insertPacket(encoded);

    return 0;
}

int CReliableCtrl::procReceivedPacket(std::string& packet, char *response, int buflen)
{
    /// 报文末尾需有8个字节的校验和
    if (packet.length() <= 8)
        return 0;

    int payload_length = packet.length() - 8;
    std::string orignal_crc = packet.substr(payload_length, 8);
    std::string new_crc = CDataCoder::calcCRC32(packet.c_str(), payload_length);
    if (orignal_crc != new_crc)
    {
        DBG_LOG((TRANSCTRL, "orig_crc=%s, new_crc=%s\n", orignal_crc.c_str(), new_crc.c_str()));
        return 0;
    }

    /// 删除校验和
    packet.erase(payload_length, 8);

    std::string data_flag = packet.substr(0, 4);
    if (data_flag == "WTC#")
    {
        /// 复制一份数据，由缓冲区管理类负责回收内存
        std::string* cli_data = new std::string(packet);
        std::string ack = m_transServer->insertPacket(cli_data);
        if (ack != "")
        {
            snprintf(response, buflen - 1, "%s", ack.c_str());
        }

        return ack.length();
    }
    else if (data_flag == "WTS#")
    {
        m_transClient->parseACK(packet);
    }
    else
    {
        /// todo: 处理经过编码的命令
        CBuffer output;
        CDataCoder::decodeData(&output, &packet);
        CMemDump::dumpToLog(TRANSCTRL, "remote cmd:", output.peek(), output.readableBytes());
    }

    return 0;
}

