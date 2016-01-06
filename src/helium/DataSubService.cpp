/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataSubService.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月24日 16时28分19秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "DataSubService.h"
#include "Logger.h"

using namespace HELIUM;

CDataSubscriber::CDataSubscriber() : m_Reactor(NULL),
    m_HeartbeatTid(0), m_TimerName("heartbeat"), 
    m_HeartbeatInterval(1.0f), m_HeartbeatInfo(NULL)
{
    memset(m_SubSet, 0xFF, sizeof(m_SubSet));
}

CDataSubscriber::~CDataSubscriber()
{
    delete m_HeartbeatInfo; m_HeartbeatInfo = NULL;
}

void CDataSubscriber::creatHeartbeatPacket(uint8_t senderid, uint16_t dataid, 
        const char*content, size_t len)
{
    m_HeartbeatInfo = new CExchangePacket(senderid, dataid, content, len);
}

int CDataSubscriber::open(CReactor* r)
{
    m_Reactor = r;
    m_HeartbeatTid = m_Reactor->registerTimerHandler(this, m_HeartbeatInterval, m_TimerName);

    return 0;
}

int CDataSubscriber::handleRead(int fd)
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

            processData(frame_pos, (int)frame_length);

            begin_pos = frame_pos + frame_length + sizeof(uint32_t);
            m_InputBuffer.retrieveUntil(begin_pos);
        }
    }

    return 0;
}

int CDataSubscriber::handleClose(int fd)
{
    if (m_Reactor && m_HeartbeatTid > 0)
    {
        m_Reactor->removeTimerHandler(m_HeartbeatTid);
        m_HeartbeatTid = 0;
    }

    m_InputBuffer.retrieveAll();

    return 0;
}

int CDataSubscriber::handleTimeout(TimerId tid)
{
    if (tid == m_HeartbeatTid && m_HeartbeatInfo != NULL)
    {
        getStream().write((const char*)(m_HeartbeatInfo->begin()), m_HeartbeatInfo->readableBytes());
    }

    return 0;
}

int CDataSubscriber::getSubscribeSet(char* dst, int buflen)
{
    int result = 0;

    for (unsigned int i = 0; i < sizeof(m_SubSet); ++i)
    {
        if (m_SubSet[i] == 0xFF)
            break;

        char type[8];
        snprintf(type, sizeof(type) - 1, "%02X,", m_SubSet[i]);
        strncat(dst, type, 3);
        result += 3;

        if ((result + 3) >= buflen)
            break;
    }

    /// 去除末尾的","
    if (result > 0)
    {
        dst[result - 1] = '\0';
    }

    return (result - 1);
}

