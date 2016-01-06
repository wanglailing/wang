/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Acceptor.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月24日 08时24分17秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "EventHandler.h"
#include "Logger.h"
#include "Address.h"
#include "Reactor.h"

namespace HELIUM
{
    /**
     * CAcceptor封装了TCP/IP的连接请求。该类实现了被动式初始化通信服务的
     * 基本策略。
     * SERVICE_HANDLER是服务处理类，它需要从接口类CServiceHandler派生。
     * PEER_ACCEPTOR是具体的套接字类(由CAcceptor在建立被动式连接后所使用
     * 的通信机制)。它要从CSocket类中派生。
     *
     * @see CReactor
     * @see CServiceHandler
     *
     */
    template<class SERVICE_HANDLER, class PEER_ACCEPTOR>
    class CAcceptor : public virtual CEventHandler
    {
        public:
            CAcceptor(CReactor *r);
            virtual ~CAcceptor();

            /**
             * 初始化侦听套接字
             *
             * @param local_addr 本地地址
             *
             * @return 0 -- 成功； -1 -- 错误
             *
             */
            virtual int open(CAddress& local_addr);

            /**
             * 关闭对端套接字流
             *
             * @return 0 -- 成功； -1 -- 错误
             *
             */
            virtual int close(void);

            /**
             * 当有新的连接到来时，由CReactor调用.
             *
             * @param fd 侦听套接字
             *
             * @return 0 -- 成功； -1 -- 错误。如果返回-1，CReactor将会把它删除。
             *
             */
            virtual int handleRead(int fd);

            /**
             * 当PEER_ACCEPTOR流出错，或者CReactor接到了关闭事件处理的命令时，由CReactor调用.
             *
             * @param fd 侦听套接字
             *
             * @return -1 
             *
             */
            virtual int handleClose(int fd);

        protected:
            /**
             * 创建与套接字对应的服务
             *
             * @param sock 对端套接字
             *
             * @return 服务指针。
             *
             */
            virtual SERVICE_HANDLER* makeServiceHandler(PEER_ACCEPTOR *sock);

            /**
             * 接受新的连接.
             *
             * @param new_socket 连接套接字
             *
             * @return 0 -- 成功； -1 -- 错误。
             *
             */
            virtual int acceptServiceHandler(PEER_ACCEPTOR *&new_socket);

            /**
             * 激活套接字服务.
             *
             * @param new_socket 连接套接字
             *
             * @return 0 -- 成功； -1 -- 错误。
             *
             */
            virtual int activateServiceHandler(PEER_ACCEPTOR *new_socket);

        protected:
            /// 侦听套接字
            PEER_ACCEPTOR m_ListenSocket;

            /// 反应器
            CReactor *m_Reactor;
    };

#define SH SERVICE_HANDLER
#define PA PEER_ACCEPTOR

    template<class SH, class PA>
    inline CAcceptor<SH, PA>::CAcceptor(CReactor *r) : m_Reactor(r)
    {
    }

    template<class SH, class PA>
    inline CAcceptor<SH, PA>::~CAcceptor()
    {
    }

    template<class SH, class PA>
    int CAcceptor<SH, PA>::open(CAddress& local_addr)
    {
        if (!m_ListenSocket.open(local_addr.getAddress()->sa_family))
            return -1;

        if (!m_ListenSocket.bind(local_addr))
            return -1;

        m_Reactor->registerIOHandler(this, m_ListenSocket.getHandler(), READ_EVENT);

        return 0;
    }

    template<class SH, class PA>
    inline int CAcceptor<SH, PA>::close(void)
    {
        m_ListenSocket.close();
        return 0;
    }

    template<class SH, class PA>
    inline int CAcceptor<SH, PA>::handleClose(int /* fd */)
    {
        delete this;
        return -1;
    }

    template<class SH, class PA>
    int CAcceptor<SH, PA>::handleRead(int fd)
    {
        CFdSet mask;
        timeval poll = {0, 0};
        PA *new_socket = NULL;

        if (fd != m_ListenSocket.getHandler())
            return -1;

        do
        {
            if (acceptServiceHandler(new_socket) == -1)
                return -1;

            if (activateServiceHandler(new_socket) == -1)
                return -1;

            mask.reset();
            mask.setFd(fd);
        } while (::select(fd + 1, &mask, NULL, NULL, &poll) == 1);

        return 0;
    }

    template<class SH, class PA>
    inline SERVICE_HANDLER* CAcceptor<SH, PA>::makeServiceHandler(PEER_ACCEPTOR *sock)
    {
        return new SERVICE_HANDLER(sock);
    }

    template<class SH, class PA>
    inline int CAcceptor<SH, PA>::acceptServiceHandler(PEER_ACCEPTOR *&new_socket)
    {
        new_socket = m_ListenSocket.accept();
        return (new_socket ? 0 : -1);
    }

    template<class SH, class PA>
    int CAcceptor<SH, PA>::activateServiceHandler(PEER_ACCEPTOR *new_socket)
    {
        if (!new_socket)
            return -1;

        SH* service_handler = makeServiceHandler(new_socket);
        if (service_handler->open(m_Reactor) < 0)
            service_handler->close();

        return 0;
    }
}

#endif //_ACCEPTOR_H
