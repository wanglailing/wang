/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: UdpSocket.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月30日 08时48分51秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _UDP_SOCKET_H
#define _UDP_SOCKET_H

#include "Socket.h"

namespace HELIUM
{
    /**
     * UDP套接字
     *
     */
    class CUdpSocket : public CSocket
    {
        public:
            CUdpSocket() {}
            CUdpSocket(const handler_t fd) { m_Fd = fd; }

            virtual ~CUdpSocket() {}

            bool open(int domain);
            bool close(void);
            bool bind(const CAddress& my_addr);

            handler_t getHandler(void) const { return m_Fd; }

            const int getDomain(void) const { return m_Type; }

            /**
             * 接收数据
             *
             * @param buf          缓冲区地址
             * @param size         数据长度
             * @param peer_addr    对端地址
             *
             * @return 接收的字节数；-1--出错
             *
             */
            int recvfrom(char *buf, int size, CAddress *peer_addr);

            /**
             * 发送数据
             *
             * @param buf          缓冲区地址
             * @param size         数据长度
             * @param dest_addr    目的地址
             *
             * @return 发送的字节数；-1--出错
             *
             */
            int sendto(const char *buf, int size, const CAddress *dest_addr);

        protected:
            void setHandler(int fd) { m_Fd = fd; }
            void setDomain(int type) { m_Type = type; }
    };
}

#endif //_UDP_SOCKET_H
