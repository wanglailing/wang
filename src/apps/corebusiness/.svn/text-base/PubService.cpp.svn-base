/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PubService.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月11日 08时26分44秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "PubService.h"
#include "CoreApp.h"

using namespace HELIUM;
using namespace COREBUSINESS;

int CPubService::open(HELIUM::CReactor* r)
{
    r->registerIOHandler(this, getStream().getHandler(), READ_EVENT);
    SERV_REPO->push_back(this);
    return 0;
}

int CPubService::handleRead(int fd)
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
        /// 不处理订阅者发送的数据
        m_InputBuffer.retrieveAll();
    }

    return 0;
}

int CPubService::handleClose(int /*fd*/)
{
    DBG_LOG((COREPROC, "Subscriber disconnected!\n"));
    SERV_REPO->erase(this);
    delete this;
    return 0;
}

