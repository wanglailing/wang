/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: UdpSocket.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月30日 08时59分16秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "UdpSocket.h"

using namespace HELIUM;

bool CUdpSocket::open(int domain)
{
    m_Type = domain;
    m_Fd = ::socket(m_Type, SOCK_DGRAM, 0);
    if (m_Fd < 0)
        return false;

    turnOptionOn(CSocket::NONBLOCKING);
    return true;
}

bool CUdpSocket::close(void)
{
    if (m_Fd >= 0)
    {
        ::close(m_Fd);
        m_Fd = -1;
    }

    return true;
}

bool CUdpSocket::bind(const CAddress& my_addr)
{
    int ret = ::bind(m_Fd, (SA*)my_addr.getAddress(),
            my_addr.getLength());
    if (ret < 0)
        return false;

    return true;
}

int CUdpSocket::recvfrom(char *buf, int size, CAddress *peer_addr)
{
    int len;
    socklen_t pa_len = peer_addr->getLength();
    SA *pa = peer_addr->getAddress();
    len = ::recvfrom(getHandler(), buf, size, 0, pa, &pa_len);

    return len;
}

int CUdpSocket::sendto(const char *buf, int size, const CAddress *dest_addr)
{
    return ::sendto(getHandler(), buf, size, 0, 
            dest_addr->getAddress(), dest_addr->getLength());
}


