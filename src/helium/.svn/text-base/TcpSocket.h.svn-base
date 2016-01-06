/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TcpSocket.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月23日 15时18分19秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _TCP_SOCKET_H
#define _TCP_SOCKET_H

#include "Socket.h"

namespace HELIUM
{
    /**
     * TCP套接字
     *
     */
    class CTcpSocket : public CSocket
    {
        public:
            CTcpSocket() : m_Path(NULL), m_PeerAddress(NULL) {}
            CTcpSocket(const handler_t fd, const SA* peer_addr) : m_Path(NULL) 
            { 
                m_Fd = fd; 
                m_PeerAddress = (SA*)peer_addr;
            }

            ~CTcpSocket() 
            { 
                this->close(); 

                delete m_Path;
                m_Path = NULL;

                delete m_PeerAddress;
                m_PeerAddress = NULL;
            }

            bool open(int domain);
            bool close(void);
            bool connect(const CAddress& addr);
            virtual bool bind(const CAddress& my_addr);

            int write(const char *buf, int len);
            int read(char *buf, int len);

            CTcpSocket* accept(void);

            handler_t getHandler(void) const { return m_Fd; }

            const int getDomain(void) const { return m_Type; }

            SA* getRemoteAddress(void) { return m_PeerAddress; }

        private:
            CTcpSocket(const CTcpSocket&);
            CTcpSocket& operator=(const CTcpSocket&);

        private:
            /// UNIX域地址
            char *m_Path;

            /// 对端地址
            SA *m_PeerAddress;
    };
}

#endif //_TCP_SOCKET_H
