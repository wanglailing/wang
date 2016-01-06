/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TcpSocket.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月23日 15时39分42秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "TcpSocket.h"

using namespace HELIUM;

bool CTcpSocket::open(int domain)
{
    m_Type = domain;
    m_Fd = socket(domain, SOCK_STREAM, 0);
    if (m_Fd < 0)
    {
        ERR_LOG((FATALERR, "OS::socket() error:m_fd = %d\n", m_Fd));
        return false;
    }

    turnOptionOn(CSocket::NONBLOCKING);
    return true;
}

bool CTcpSocket::close(void)
{
    if (isValidHandler(m_Fd))
    {
        ::close(m_Fd);
        DBG_LOG((SOCKET, "<CTcpSocket> close the socket fd[%d].\n", m_Fd)); 
    }

    return true;
}

bool CTcpSocket::connect(const CAddress& addr)
{
    if ( !isValidHandler(m_Fd) && open(getDomain()) == false )
        return false;

    int ret = ::connect(m_Fd, (SA*)addr.getAddress(), addr.getLength());
    if (ret < 0)
    {
        if (errno == EINPROGRESS || errno == EWOULDBLOCK)
        {
            DBG_LOG((SOCKET, "Fd {%d} OS::connect() error[%s].\n", m_Fd, strerror(errno))); 
        }
        else
        {
            ERR_LOG((SOCKET, "Fd {%d} OS::connect() error[%s]!!!\n", m_Fd, strerror(errno))); 
        }

        return false;
    }

    DBG_LOG((SOCKET, "Connection opend Fd{%d}.\n", m_Fd)); 
    return true;
}

bool CTcpSocket::bind(const CAddress& my_addr)
{
    if (getDomain() == AF_UNIX)
    {
        char *p = ((SA_UN *)my_addr.getAddress())->sun_path;
        m_Path = new char[strlen(p) + 1];
        strcpy(m_Path, p);

        struct stat sb;
        if (stat(m_Path, &sb) == 0)
        {
            if (S_ISSOCK(sb.st_mode) || S_ISFIFO(sb.st_mode))
            {
                unlink(m_Path);
            }
        }
    }

    turnOptionOn(CSocket::REUSEADDR);
    int ret = ::bind(m_Fd, my_addr.getAddress(), my_addr.getLength());
    if (ret < 0)
    {
        return false;
    }

    ::listen(m_Fd, 5);
    return true;
}

int CTcpSocket::write(const char *buf, int len)
{
    return ::write(m_Fd, buf, len);
}

int CTcpSocket::read(char *buf, int len)
{
    return ::read(m_Fd, buf, len);
}

CTcpSocket* CTcpSocket::accept(void)
{
    socklen_t length = 0;
    SA* remote_address = NULL;
    handler_t new_fd;

    disableHandler(new_fd);

    if (getDomain() == AF_UNIX)
    {
        remote_address = (SA*)new SA_IN;
        length = sizeof(struct sockaddr_in);
    }
    else
    {
        remote_address = (SA*)new SA_UN;
        length = sizeof(struct sockaddr_un);
    }

    memset(remote_address, 0, length);

    new_fd = ::accept(m_Fd, remote_address, &length);
    if (!isValidHandler(new_fd))
    {
        ERR_LOG((FATALERR, "accept() error:new_fd = %d\n", new_fd));
        close();
        delete remote_address;
        return NULL;
    }

    if (length == sizeof(SA_IN))
    {
        SA_IN *sa_in = (SA_IN*)remote_address;
        DBG_LOG((SOCKET, "Accept new TCP connection from %s:%d.\n", 
                    inet_ntoa(sa_in->sin_addr), ntohs(sa_in->sin_port)));
    }
    else
    {
        SA_UN *sa_un = (SA_UN*)remote_address;
        DBG_LOG((SOCKET, "Accept new UNIX connection from %s.\n", 
                    sa_un->sun_path));
    }

    /// 新创建的套接字负责销毁远端地址
    CTcpSocket *s = new CTcpSocket(new_fd, remote_address);
    s->turnOptionOn(CSocket::NONBLOCKING);
    return s;
}


