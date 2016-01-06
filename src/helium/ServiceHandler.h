/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ServiceHandler.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月24日 09时45分40秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _SERVICE_HANDLER_H
#define _SERVICE_HANDLER_H

#include "EventHandler.h"
#include "Reactor.h"

namespace HELIUM
{
    /**
     * 网络服务的抽象接口
     *
     */
    template<class PEER_STREAM>
    class CServiceHandler : public CEventHandler
    {
        public:
            CServiceHandler() : m_PeerStream(new PEER_STREAM)
            {
            }

            CServiceHandler(PEER_STREAM *ps) : m_PeerStream(ps)
            {
            }

            virtual ~CServiceHandler()
            {
                if (m_PeerStream)
                {
                    delete m_PeerStream;
                    m_PeerStream = (PEER_STREAM *)NULL;
                }
            }

            /**
             * 当连接被建立时调用此函数，进行服务相关的初始化工作.
             *
             * @param r 反应堆
             *
             * @return 0 -- 成功； -1 -- 错误
             *
             */
            virtual int open(CReactor* r) = 0;

            virtual void close(void)
            {
                if (m_PeerStream)
                    m_PeerStream->close();
            }

            operator PEER_STREAM&()
            {
                return *m_PeerStream;
            }

            PEER_STREAM& getStream(void)
            {
                return *m_PeerStream;
            }

        protected:
            /// 对端套接字流
            PEER_STREAM *m_PeerStream;
    };
}

#endif //_SERVICE_HANDLER_H
