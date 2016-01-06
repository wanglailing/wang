/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Connector.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月19日 16时14分44秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _CONNECTOR_H
#define _CONNECTOR_H

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>

#include "EventHandler.h"
#include "Logger.h"
#include "Address.h"
#include "Reactor.h"
#include "TimeVal.h"

namespace HELIUM
{
    /**
     * 模板参数PEER_CONNECTOR暂未使用。
     *
     */
    template<class SERVICE_HANDLER, class PEER_CONNECTOR>
    class CConnector : public virtual CEventHandler
    {
        public:
            CConnector();
            virtual ~CConnector();

            /**
             * 初始化套接字, 并连接到服务器
             *
             * @param r      反应堆
             * @param sh     套接字服务
             * @param addr   服务器地址
             * @param proto  协议类型
             * @param tv     超时重连间隔,默认2秒
             *
             * @return 0 -- 成功； -1 -- 错误
             *
             */
            virtual int open(CReactor* r, SERVICE_HANDLER* sh, CAddress* addr, int proto, const CTimeVal& tv = CTimeVal(2.0));

            /**
             * 关闭套接字流
             *
             * @return 0 -- 成功； -1 -- 错误
             *
             */
            virtual int close(void);

            /**
             * 连接到服务器
             *
             *
             * @return 0 -- 成功； -1 -- 错误
             *
             */
            int connect(void);

            virtual int handleRead(int fd);

            /**
             * 当套接字可写时，说明已连接上服务器,将套接字状态改为已连接，并删除写事件
             * 的响应器。
             *
             * @param fd 套接字描述符
             *
             * @return 0 -- 成功； -1 -- 错误
             *
             */
            virtual int handleWrite(int fd);

            virtual int handleClose(int fd);

            /**
             * 重连定时器响应函数
             *
             * @param tid 定时器ID
             *
             * @return 0 -- 成功； -1 -- 错误
             *
             */
            virtual int handleTimeout(TimerId tid);

        protected:
            enum ProgressState
            {
                IDLE,              /**< 套接字初始化 */
                WAITING,           /**< 等待连接完成 */
                CONNECTED,         /**< 已连接 */
                FAILED             /**< 连接失败 */
            };

            /**
             * 创建与套接字对应的服务
             *
             * @param sh 如果sh为空，则会创建一个服务响应对象。
             *
             * @return 服务指针
             *
             */
            virtual SERVICE_HANDLER* makeServiceHandler(SERVICE_HANDLER* sh);

            /**
             * 连接至服务器.
             *
             * @return 0 -- 成功； -1 -- 错误。
             *
             */
            virtual int connectServiceHandler(void);

            /**
             * 激活套接字服务.
             *
             * @return 0 -- 成功； -1 -- 错误。
             *
             */
            virtual int activateServiceHandler(void);

        private:
            /// 执行异步连接
            void doAsync(void);

            /// 设置重连定时器(连接中途断开时由SERVICE_HANDLER调用)
            void setReconnectTimer(void);

        protected:
            /// 重连超时间隔
            CTimeVal m_ReconnTimeout;

            /// 异步连接定时器ID
            TimerId m_AsyncTid;

            /// 重连定时器ID
            TimerId m_ReconnTid;

            /// 反应堆的引用
            CReactor* m_Reactor;

            /// 套接字连接状态
            ProgressState m_State;

            /// 服务响应器的引用
            SERVICE_HANDLER* m_ServHandler;

            /// 套接字描述符
            int m_Fd;

            /// 通信地址
            CAddress* m_Address;

            /// 协议类型
            int m_ProtoType;
    };

#define CSH SERVICE_HANDLER
#define CPC PEER_CONNECTOR

    template<class CSH, class CPC>
    inline CConnector<CSH, CPC>::CConnector() : m_AsyncTid(0), m_ReconnTid(0), m_Reactor(NULL), 
        m_State(IDLE), m_ServHandler(NULL), m_Fd(-1)
    {
    }

    template<class CSH, class CPC>
    inline CConnector<CSH, CPC>::~CConnector()
    {
        if (m_ServHandler)
        {
            delete m_ServHandler;
            m_ServHandler = NULL;
        }
    }

    template<class CSH, class CPC>
    int CConnector<CSH, CPC>::open(CReactor* r, SERVICE_HANDLER* sh, CAddress* addr, int proto, const CTimeVal& tv)
    {
        m_Reactor = r;
        m_Address = addr;
        m_ProtoType = proto;
        m_ReconnTimeout = tv;

        if (m_Address->bad())
        {
            DBG_LOG((SOCKET, "The socket address is error!\n"));
            return -1;
        }

        m_ServHandler = makeServiceHandler(sh);

        return 0;
    }

    template<class CSH, class CPC>
    inline int CConnector<CSH, CPC>::close(void)
    {
        return 0;
    }

    template<class CSH, class CPC>
    void CConnector<CSH, CPC>::doAsync(void)
    {
        m_Reactor->registerIOHandler(this, m_Fd, WRITE_EVENT);
        m_AsyncTid = m_Reactor->registerTimerHandler(this, m_ReconnTimeout, "Async connect Timer");
        m_State = WAITING;
    }

    template<class CSH, class CPC>
    int CConnector<CSH, CPC>::connect(void)
    {
        if (connectServiceHandler() == -1)
        {
            if (errno == EWOULDBLOCK || errno == EINPROGRESS)
            {
                doAsync();
            }
            else
            {
                if (m_Fd > 0)
                {
                    ::close(m_Fd);
                    // DBG_LOG((SOCKET, "CONNECT ERROR: Recycle the socket fd[%d]!\n", m_Fd));
                    m_Fd = -1;
                }

                /// 注册重连定时器
                setReconnectTimer();
            }

            return 0;
        }

        return activateServiceHandler();
    }

    template<class CSH, class CPC>
    int CConnector<CSH, CPC>::handleRead(int fd)
    {
        if (m_ServHandler)
        {
            return m_ServHandler->handleRead(fd);
        }
        else
        {
            return -1;
        }
    }

    template<class CSH, class CPC>
    int CConnector<CSH, CPC>::handleWrite(int fd)
    {
        if (m_Fd != fd)
            return -1;

        /// 删除定时器
        m_Reactor->removeTimerHandler(m_AsyncTid);
        m_AsyncTid = 0;

        m_Reactor->removeHandler(this, WRITE_EVENT);

        int error;
        int ret;
        socklen_t n = sizeof(error);
        ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)&error, &n);
        if (ret == 0 && error == 0)
        {
            if (activateServiceHandler() == 0)
            {
                DBG_LOG((SOCKET, "socket nonblocking connect completed!\n"));
                m_State = CONNECTED;
            }
            else
            {
                DBG_LOG((SOCKET, "socket nonblocking connect failed!\n"));
                m_State = FAILED;
            }

            return 0;
        }

        m_State = FAILED;
        if (error == ECONNREFUSED)
        {
            DBG_LOG((SOCKET, "Connection was refused!\n"));
        }

        m_ServHandler->close();

        return 0;
    }

    template<class CSH, class CPC>
    int CConnector<CSH, CPC>::handleClose(int fd)
    {
        if (m_Fd != fd)
            return -1;

        if (m_ServHandler)
        {
            m_ServHandler->handleClose(fd);
        }

        ::close(m_Fd);
        setReconnectTimer();

        return 0;
    }

    template<class CSH, class CPC>
    int CConnector<CSH, CPC>::handleTimeout(TimerId tid)
    {
        m_State = FAILED;
        if (tid == m_AsyncTid)
            m_Reactor->removeHandler(this, WRITE_EVENT);

        connect();

        return 0;
    }

    template<class CSH, class CPC>
    void CConnector<CSH, CPC>::setReconnectTimer(void)
    {
        m_ReconnTid = m_Reactor->registerTimerHandler(this, m_ReconnTimeout, "Reconnect Timer");
        // DBG_LOG((SOCKET, "Register reconnect timer[%d]!\n", m_ReconnTid));
        m_State = WAITING;
    }

    template<class CSH, class CPC>
    SERVICE_HANDLER* CConnector<CSH, CPC>::makeServiceHandler(SERVICE_HANDLER* sh)
    {
        SERVICE_HANDLER* new_sh = sh;

        if (sh == NULL)
            new_sh = new SERVICE_HANDLER;

        return new_sh;
    }

    template<class CSH, class CPC>
    int CConnector<CSH, CPC>::connectServiceHandler(void)
    {
        /// SERVICE_HANDLER是PEER_CONNECTOR的实例
        PEER_CONNECTOR& s = *m_ServHandler; 
        if (!s.open(m_ProtoType))
        {
            ERR_LOG((FATALERR, "Socket::open [%d] failed!\n", m_ProtoType));
            return -1;
        }

        m_Fd = s.getHandler();
        s.setOption(HELIUM::CSocket::NONBLOCKING, 1);

        CAddress& addr = *m_Address;
        return (s.connect(addr) ? 0 : -1);
    }

    template<class CSH, class CPC>
    int CConnector<CSH, CPC>::activateServiceHandler(void)
    {
        m_Reactor->registerIOHandler(this, m_Fd, READ_EVENT);
        return m_ServHandler->open(m_Reactor);
    }
}

#endif //_CONNECTOR_H
