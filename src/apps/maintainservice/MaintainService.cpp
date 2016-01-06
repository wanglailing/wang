/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: MaintainService.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月24日 10时41分25秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "MaintainService.h"
#include "MaintainApp.h"
#include "MemDump.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace DMS;

const char* CMaintainService::m_GetStatusCmd = "$GETSTATUS$$$";
const char* CMaintainService::m_WriteDataCmd = "$WRITEDATA$$$";

/// 维护软件发送的心跳数据为"$", 为了防止缓冲区中囤积大量的心跳字符，
/// 需要定时将他们清理掉。
const char* CMaintainService::m_HeartbeatCmd = "$$$$$$$$$$$$$";

int CMaintainService::open(HELIUM::CReactor* r)
{
    r->registerIOHandler(this, getStream().getHandler(), READ_EVENT);
    SERV_REPO->push_back(this);
    return 0;
}

int CMaintainService::handleRead(int fd)
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
        processClientData();
    }

    return 0;
}

void CMaintainService::processClientData(void)
{
    if (m_InputBuffer.readableBytes() <= 0)
        return;

    const char *writedata_pos = m_InputBuffer.findString(m_WriteDataCmd, strlen(m_WriteDataCmd));
    const char *getstatus_pos = m_InputBuffer.findString(m_GetStatusCmd, strlen(m_GetStatusCmd));
    const char *heartbeat_pos = m_InputBuffer.findString(m_HeartbeatCmd, strlen(m_HeartbeatCmd));

    if (writedata_pos != NULL)
    {
        /// 删除写数据标识及其前面的字符
        m_InputBuffer.retrieveUntil(writedata_pos + strlen(m_WriteDataCmd));
        if (m_InputBuffer.readableBytes() <= 0)
        {
            DBG_LOG((MTSERVICE, "There is no data to write to the device!"));
            return;
        }

        CExchangePacket forward_container((uint8_t)PROG_MTSERVICE);

        SUBPACK_HEADER subpack_hdr;
        subpack_hdr.dataid = DEV_RAWDATA;
        subpack_hdr.len = sizeof(SUBPACK_HEADER) + m_InputBuffer.readableBytes();

        forward_container.append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));
        forward_container.append((uint8_t*)m_InputBuffer.peek(), m_InputBuffer.readableBytes());
        forward_container.finish();

        /// 向数据交换模块转发数据
        MAINTAIN_APP->getSwitchService()->pushData((const char*)(forward_container.begin()), 
                forward_container.readableBytes());
    }
    else if (getstatus_pos != NULL)
    {
        char buf[256];
        memset(buf, 0, sizeof(buf));

        int length = HELIUM::SLAVEDEV_MGR->getSlaveDevState(buf, sizeof(buf));
        if (length > 0)
        {
            getStream().write(buf, length);
            m_bQueriedDevState = true;
        }
    }

    if (writedata_pos || getstatus_pos || heartbeat_pos)
    {
        m_InputBuffer.retrieveAll();
    }
}

int CMaintainService::handleClose(int /*fd*/)
{
    DBG_LOG((MTSERVICE, "Maintainer client disconnected!\n"));
    SERV_REPO->erase(this);
    delete this;
    return 0;
}


