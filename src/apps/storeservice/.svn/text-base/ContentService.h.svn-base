/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ContentService.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月05日 11时01分11秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef __CONTENT_SERVICE_H
#define __CONTENT_SERVICE_H

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Reactor.h"
#include "Buffer.h"
#include "Packet.h"

namespace STORESERVICE
{
    class CContServ : public HELIUM::CServiceHandler<HELIUM::CTcpSocket>
    {
        public:
            CContServ(HELIUM::CTcpSocket *stream);
            ~CContServ();

            int open(HELIUM::CReactor* r);
            int handleRead(int fd);
            int handleClose(int fd);

        private:
            void parseFrame(const char* data, int length);

        private:
            /// 读取套接字数据缓冲区
            HELIUM::CBuffer m_InputBuffer;
    };

    inline CContServ::CContServ(HELIUM::CTcpSocket *stream)
        : HELIUM::CServiceHandler<HELIUM::CTcpSocket>(stream)
    {
        setId("File Content Receiver");
    }

    inline CContServ::~CContServ()
    {
    }
}

#endif //__CONTENT_SERVICE_H
