/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataService.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月05日 11时06分36秒
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

#include "DataService.h"
#include "TransportApp.h"
#include "Logger.h"
#include "MemDump.h"
#include "../DmsDatatype.h"
#include "../DmsBusinessDat.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

int CDataService::open(HELIUM::CReactor* r)
{
    r->registerIOHandler(this, getStream().getHandler(), READ_EVENT);
    SERV_REPO->push_back(this);
    return 0;
}

int CDataService::handleRead(int fd)
{
    if (getStream().getHandler() != fd)
        return -1;

    int save_error;
    int read_len = m_InputBuffer.readFd(fd, &save_error);
    if (read_len < 0)
    {
        ERR_LOG((SOCKET, "Socket read error.[%d]\n", read_len));
        if (save_error != EWOULDBLOCK)
        {
            return -1;
        }
    }
    else if (read_len == 0)
    {
        DBG_LOG((SOCKET, "Peer has dropped connection!\n"));
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

            parseFrame(frame_pos, (int)frame_length);

            begin_pos = frame_pos + frame_length + sizeof(uint32_t);
            m_InputBuffer.retrieveUntil(begin_pos);
        }
    }

    return 0;
}

void CDataService::parseFrame(const char* data, int length)
{
    if (data == NULL || length <= 0)
        return;

    CAPSUL_HEADER* capsulate_hdr = (CAPSUL_HEADER*)data;
    m_ServiceId = capsulate_hdr->senderid;
    PROG_ID data_source = (PROG_ID)(capsulate_hdr->senderid);

    bool recv_base = false;
    int push_interval = 0;

    /// 报文中可能包含多个子包
    unsigned subhdr_len = sizeof(SUBPACK_HEADER);
    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = data + offset + subhdr_len;
        int payload_len = subpack_hdr->len - subhdr_len;

        if (data_source == PROG_COREPROC && subpack_hdr->dataid == CORE_DATASETS)
        {
            DATA_SINK->getTcaser()->parseCoreData(payload_ptr, payload_len);
            offset += subpack_hdr->len;
            continue;
        }
        else if (subpack_hdr->dataid == REG_CMD)
        {
            m_RegCmds.clear();
            m_RegCmds.append(payload_ptr, payload_len);
            offset += subpack_hdr->len;
            continue;
        }

        if (DATA_SINK->m_BusinessHeader.packcount == 255)
        {
            DATA_SINK->combinePackets();
            DATA_SINK->pushPacket();
        }

        DATA_SINK->m_BusinessHeader.packcount++;

        /// DMS核心业务数据的基础信息包中包含车次和司机号
        if ((data_source == PROG_COREPROC) && (*payload_ptr == (const char)DMSMSG_BASE))
        {
            recv_base = true;
            bool op_changed = false;
            payload_len -= sizeof(TrainOper);
            BasicData* basic_ptr = (BasicData*)(payload_ptr + 1);
            DATA_SINK->setAtpId(basic_ptr->reserved);

            TrainOper* trainop_ptr = (TrainOper*)(payload_ptr + 1 + sizeof(BasicData));
            if (::memcmp(&(DATA_SINK->m_BusinessHeader.operate), trainop_ptr, sizeof(TrainOper)) != 0)
            {
                ::memcpy(&(DATA_SINK->m_BusinessHeader.operate), trainop_ptr, sizeof(TrainOper));
                op_changed = true;
            }

            /// 合并基础信息包和其它业务数据包
            DATA_SINK->m_FinalBuf.append(payload_ptr, payload_len);

            DATA_SINK->combinePackets();

            if (op_changed == true)
            {
                DATA_SINK->pushPacket();
            }
        }
        else
        {
            DATA_SINK->m_BusinessDataBuf.append(payload_ptr, payload_len);
        }

        offset += subpack_hdr->len;
    }

    if (recv_base == true)
    {
        push_interval = CTimeVal::getMonotonicMs() - DATA_SINK->getPushStamp();
    }

    unsigned int finalbuf_len = DATA_SINK->m_FinalBuf.readableBytes();
    if (finalbuf_len >= CDataSink::BufUpperbound || (finalbuf_len > 0 && push_interval >= 60000))
    {
        DATA_SINK->pushPacket();
    }
}

int CDataService::handleClose(int /*fd*/)
{
    DBG_LOG((TRANSCTRL, "Data source disconnected!\n"));
    SERV_REPO->erase(this);
    delete this;
    return 0;
}


