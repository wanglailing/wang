/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PubService.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月11日 08时20分22秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _PUB_SERVICE_H
#define _PUB_SERVICE_H

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Reactor.h"
#include "Buffer.h"

namespace COREBUSINESS
{
    /**
     * 数据发布服务.侦听TCP端口，接受其它模块的连接。
     * 并广播核心业务数据。
     *
     */
    class CPubService : public HELIUM::CServiceHandler<HELIUM::CTcpSocket>
    {
        public:
            CPubService(HELIUM::CTcpSocket *stream);
            ~CPubService();

            virtual int open(HELIUM::CReactor* r);
            virtual int handleRead(int fd);
            virtual int handleClose(int fd);

        private:
            /// 读取套接字数据缓冲区
            HELIUM::CBuffer m_InputBuffer;
    };

    inline CPubService::CPubService(HELIUM::CTcpSocket *stream)
        : HELIUM::CServiceHandler<HELIUM::CTcpSocket>(stream)
    {
    }

    inline CPubService::~CPubService()
    {
    }
}

#endif //_PUB_SERVICE_H
