/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: AcceptServer.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月21日 09时35分44秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ACCEPT_SERVER_H
#define _ACCEPT_SERVER_H

#include "Reactor.h"
#include "TcpSocket.h"
#include "UdpSocket.h"
#include "Acceptor.h"
#include "MaintainService.h"

namespace DMS
{
    /**
     * TCP服务器，接受维护客户端的连接。
     *
     */
    class CAcceptServer
    {
        public:
            typedef HELIUM::CAcceptor<CMaintainService, HELIUM::CTcpSocket> ACCEPTOR_TYPE;

        public:
            explicit CAcceptServer(HELIUM::CReactor *r);
            ~CAcceptServer();

            /**
             * 初始化DMS业务数据管理.
             *
             * @return 0 -- 成功; -1 -- 失败
             */
            int init(void);

            /**
             * 向维护软件反馈从板数据
             *
             * @param buf 数据地址
             * @param len 数据长度
             *
             * @return 发送的字节数; -1 -- 出错
             */
            int responseData(const char *buf, int len);

        private:
            CAcceptServer(const CAcceptServer&);
            CAcceptServer& operator=(const CAcceptServer&);

        private:
            /// 反应堆对象
            HELIUM::CReactor *m_Reactor;

            /// DMS维护服务
            ACCEPTOR_TYPE *m_MaintainAcceptor;

            /// UDP套接字，向维护软件反馈数据
            HELIUM::CUdpSocket m_Responser;
    };
}

#endif //_ACCEPT_SERVER_H
