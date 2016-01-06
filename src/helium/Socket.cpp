/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Socket.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月23日 14时21分25秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Socket.h"

using namespace HELIUM;

bool CSocket::turnOptionOn(opt_t opt)
{
    if (opt == NONBLOCKING)
        return setFdOptions(O_NONBLOCK);

    int optname;
    if (opt == REUSEADDR)
    {
        optname = SO_REUSEADDR;
    }
    else
    {
        ERR_LOG((FATALERR, "Invalid socket option[%d]!\n", opt));
        return false;
    }

    return setSockOption(SOL_SOCKET, optname, 1) == 0;
}

bool CSocket::turnOptionOff(opt_t opt)
{
    if (opt == NONBLOCKING)
        return clearFdOptions(O_NONBLOCK);

    int optname;
    if (opt == REUSEADDR)
    {
        optname = SO_REUSEADDR;
    }
    else
    {
        ERR_LOG((FATALERR, "Invalid socket option[%d]!\n", opt));
        return false;
    }

    return setSockOption(SOL_SOCKET, optname, 0) == 0;
}

bool CSocket::setOption(opt_t opt, int arg)
{
    int optname;

    if (opt == NONBLOCKING)
    {
        return ( (arg == 1) ? setFdOptions(O_NONBLOCK) : clearFdOptions(O_NONBLOCK) );
    }

    if (opt == RCVLOWAT)
    {
        optname = SO_RCVLOWAT;
    }
    else if (opt == SNDLOWAT)
    {
        optname = SO_SNDLOWAT;
    }
    else
    {
        ERR_LOG((FATALERR, "Invalid socket option[%d]!\n", opt));
        return false;
    }

    return setSockOption(SOL_SOCKET, optname, arg) == 0;
}

int CSocket::getOption(opt_t opt) const
{
    return -1;
}

int CSocket::setSockOption(int level, int optname, int value)
{
    int ret = setsockopt(m_Fd, level, optname, (const char*)&value, sizeof(value));
    if (ret < 0)
    {
        //setState();
    }

    return ret;
}

int CSocket::setFdOptions(long flags)
{
    int val;
    int ret;

    if ((val = fcntl(m_Fd, F_GETFL, 0)) < 0)
    {
        return -1;
    }

    val |= flags;
    ret = fcntl(m_Fd, F_SETFL, val);

    return ret;
}

int CSocket::clearFdOptions(long flags)
{
    long oldflags;
    long newflags;
    int ret;

    if ((oldflags = fcntl(m_Fd, F_GETFL, 0)) < 0)
    {
        return -1;
    }

    newflags = oldflags & ~flags;
    ret = fcntl(m_Fd, F_SETFL, newflags);

    return ret;
}


