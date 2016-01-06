/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: JruService.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月12日 14时58分42秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "JruService.h"
#include "DataSwitchApp.h"
#include "BoardBusOp.h"
#include "CharStuff.h"
#include "MemDump.h"
#include "Packet.h"
#include "../DmsBusinessDat.h"
#include "../JruProto.h"

using namespace HELIUM;
using namespace DMS;

CJRUService::CJRUService(HELIUM::CTcpSocket *stream)
    : HELIUM::CServiceHandler<HELIUM::CTcpSocket>(stream), m_ListenPort(0)
{
    memset(m_FrameBuf, 0, sizeof(m_FrameBuf));
    setId("JRU Data Service");
}

CJRUService::~CJRUService()
{
}

int CJRUService::open(HELIUM::CReactor* r)
{
    r->registerIOHandler(this, getStream().getHandler(), READ_EVENT);
    JRU_REPO->push_back(this);

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    socklen_t len = sizeof(local_addr);
    if (getsockname(getStream().getHandler(), (struct sockaddr*)&local_addr, &len) == 0)
    {
        m_ListenPort = ntohs(local_addr.sin_port);
    }

    JRUHeader *jru_hdr = (JRUHeader *)m_FrameBuf;
    jru_hdr->startChar = JRU_BOUNDCHAR;
    jru_hdr->tag = JRU_HEADFLAG;
    jru_hdr->listenPort = m_ListenPort;

    return 0;
}

int CJRUService::handleRead(int fd)
{
    if (getStream().getHandler() != fd)
        return -1;

    int save_error;
    int read_len = m_InputBuffer.readFd(fd, &save_error);
    if (read_len < 0)
    {
        ERR_LOG((SOCKET, "JRU Socket read error.[%d]\n", read_len));
        if (save_error != EWOULDBLOCK)
        {
            return -1;
        }
    }
    else if (read_len == 0)
    {
        DBG_LOG((SOCKET, "JRU has dropped connection!\n"));
        return -1;
    }
    else
    {
        //CMemDump::dumpToLog(DATASWITCH, "Wrap JRU data++++++++++++++++++: ", 
        //       m_InputBuffer.peek(), m_InputBuffer.readableBytes());
        processRawData();
    }

    return 0;
}

void CJRUService::processRawData(void)
{
    if (m_InputBuffer.readableBytes() <= 2)
        return;

    m_WrapBuffer.retrieveAll();

    int raw_len = m_InputBuffer.readableBytes();
    const char* raw_data = m_InputBuffer.peek();
    char* payload_buff = &m_FrameBuf[sizeof(JRUHeader)];
    int buff_count = (int)(sizeof(m_FrameBuf) - sizeof(JRUHeader));
    int payload_len;
    int searchpos = 0;
    do {
        payload_len = CHAR_STUFFER->destuff(payload_buff, buff_count, raw_data, &searchpos, raw_len);
        if (payload_len > 0)
        {
            int dst_len = payload_len + sizeof(JRUHeader);
            m_FrameBuf[dst_len++] = JRU_BOUNDCHAR;  /// 加上结束标记

            SUBPACK_HEADER subpack_hdr;
            subpack_hdr.dataid = DATASWITCH_APP->get300tId();
            subpack_hdr.len = sizeof(subpack_hdr) + dst_len;
            m_WrapBuffer.append((char*)&subpack_hdr, sizeof(subpack_hdr));
            m_WrapBuffer.append(m_FrameBuf, dst_len);
        }

        if (payload_len >= 0)
        {
            m_InputBuffer.retrieveUntil(raw_data + searchpos);
        }
    } while (payload_len >= 0);

    if (m_WrapBuffer.readableBytes() > 0)
    {
        /// 广播数据
        // CMemDump::dumpToLog(DATASWITCH, "Wrap JRU data++++++++++++++++++: ", 
        //        m_WrapBuffer.peek(), m_WrapBuffer.readableBytes());
        DATA_HUB->broadcastData(NULL, m_WrapBuffer.peek(), m_WrapBuffer.readableBytes());
    }
}

int CJRUService::handleClose(int /*fd*/)
{
    DBG_LOG((DATASWITCH, "JRU data sender has disconnected!\n"));
    JRU_REPO->erase(this);
    delete this;
    return 0;
}

