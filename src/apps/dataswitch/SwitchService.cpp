/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: SwitchService.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月20日 16时23分32秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <algorithm>

#include "SwitchService.h"
#include "DataSwitchApp.h"
#include "BoardBusOp.h"
#include "MemDump.h"
#include "Packet.h"

using namespace HELIUM;
using namespace DMS;

int CSwitchService::open(HELIUM::CReactor* r)
{
    r->registerIOHandler(this, getStream().getHandler(), READ_EVENT);
    SERV_REPO->push_back(this);
    return 0;
}

int CSwitchService::handleRead(int fd)
{
    if (getStream().getHandler() != fd)
        return -1;

    int save_error;
    int read_len = m_InputBuffer.readFd(fd, &save_error);
    if (read_len < 0)
    {
        ERR_LOG((SOCKET, "Switch Socket read error.[%d]\n", read_len));
        if (save_error != EWOULDBLOCK)
        {
            return -1;
        }
    }
    else if (read_len == 0)
    {
        DBG_LOG((SOCKET, "Switch subscriber has dropped connection!\n"));
        return -1;
    }
    else
    {
        uint16_t frame_length;
        const char* begin_pos = m_InputBuffer.peek();
        const char* end_pos = begin_pos + m_InputBuffer.readableBytes();
        while (begin_pos < end_pos)
        {
            const char* frame_pos;
            if ((frame_pos = CExchangePacket::findFrame(begin_pos, end_pos, &frame_length)) == NULL)
                break;

            processData(frame_pos, (int)frame_length);

            begin_pos = frame_pos + frame_length + sizeof(uint32_t);
            m_InputBuffer.retrieveUntil(begin_pos);
        }
    }

    return 0;
}

void CSwitchService::processData(const char* data, int length)
{
    if (data == NULL || length <= 0)
        return;

    /// 报文中可能包含多个子包
    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = data + offset + 4;
        int payload_len = subpack_hdr->len - 4;

        switch (subpack_hdr->dataid)
        {
            case DEV_UNICAST:
                //CMemDump::dumpToLog(DATASWITCH, "Client response: ", payload_ptr, payload_len);
                BOARD_BUS->flushData(payload_ptr, payload_len);
                break;
            case DEV_MULTICAST:
                BOARD_BUS->multicastData(payload_ptr, payload_len);
                break;
            case DEV_RAWDATA:
                BOARD_BUS->writeData(payload_ptr, payload_len);
                break;
            case REMOTE_CMD:
                {
                    int resp_len;
                    char* response = DATASWITCH_APP->runCmd(payload_ptr, payload_len, &resp_len);
                    if (response != NULL && resp_len > 0)
                    {
                        BOARD_BUS->flushData(response, resp_len);
                    }
                }
                break;
            case HEARTBEAT:
                setQueriedDataSet(payload_ptr, payload_len);
                DATASWITCH_APP->setModuleInfo(payload_ptr, payload_len);
                break;
            default:
                break;
        }

        offset += subpack_hdr->len;
    }
}

bool CSwitchService::isDataQueried(unsigned char dataid)
{
    /// 如果数据集非空，且第一个元素为0xFF，表示支持所有的数据查询
    if (!m_ServDataSets.empty() && m_ServDataSets[0] == 0xFF)
        return true;

    std::vector<unsigned char>::iterator iter;
    iter = std::find(m_ServDataSets.begin(), m_ServDataSets.end(), dataid);

    return (iter != m_ServDataSets.end());
}

void CSwitchService::setQueriedDataSet(const char* data, int length)
{
    char* delimiter = strchr(data, '|');
    if (delimiter == NULL)
        return;
    else if ((delimiter - data + 1) == length)
        return;

    m_ServDataSets.clear();

    /// '*'表示请求所有的数据类型
    if (*(delimiter + 1) == '*')
    {
        m_ServDataSets.push_back(0xFF);
        return;
    }

    /**
     * 确保token落在[data, data + length)之内
     *
     */
    char* token;
    while ((token = strchr(delimiter + 1, ',')) != NULL && ((token - data) < length))
    {
        m_ServDataSets.push_back((unsigned char)CAppConfig::convertHexString(delimiter + 1, 
                    token - delimiter - 1));
        delimiter = token;
    }

    if ((delimiter - data) < length)
        m_ServDataSets.push_back((unsigned char)CAppConfig::convertHexString(delimiter + 1, 
                    data + length - delimiter - 1));
}

int CSwitchService::handleClose(int /*fd*/)
{
    DBG_LOG((DATASWITCH, "The data subscriber was disconnected!\n"));
    SERV_REPO->erase(this);
    delete this;
    return 0;
}

