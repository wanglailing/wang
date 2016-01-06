/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Transmit.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月03日 10时31分15秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Transmit.h"

using namespace HELIUM;
using namespace COREBUSINESS;

CTransmit::CTransmit()
{
}

CTransmit::~CTransmit()
{
}

int CTransmit::open(HELIUM::CReactor* r)
{
    (void)r;
    return 0;
}

void CTransmit::close(void)
{
}

int CTransmit::handleRead(int fd)
{
    char buf[32];
    ssize_t read_len = read(fd, buf, sizeof(buf));
    if (read_len < 0)
    {
        DBG_LOG((SOCKET, "Transmit read error!\n"));
        return -1;
    }
    else if (read_len == 0)
    {
        DBG_LOG((SOCKET, "Peer has dropped connection!\n"));
        return -1;
    }

    return 0;
}

int CTransmit::writeData(const char* buf, int length)
{
    return getStream().write(buf, length);
}

