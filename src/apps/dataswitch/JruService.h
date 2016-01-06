/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: JruService.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月12日 14时52分56秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _JRU_SERVICE_H
#define _JRU_SERVICE_H

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Reactor.h"
#include "Buffer.h"

namespace DMS
{
    /**
     * JRU数据接收服务
     *
     */
    class CJRUService : public HELIUM::CServiceHandler<HELIUM::CTcpSocket>
    {
        public:
            CJRUService(HELIUM::CTcpSocket *stream);
            ~CJRUService();

            int open(HELIUM::CReactor* r);
            int handleRead(int fd);
            int handleClose(int fd);

        private:
            /// 处理原始的JRU数据
            void processRawData(void);

        private:
            /// 读取套接字数据缓冲区
            HELIUM::CBuffer m_InputBuffer;

            /// 数据封装缓冲区
            HELIUM::CBuffer m_WrapBuffer;

            /// ATP帧缓冲区
            char m_FrameBuf[1024];

            /// 用侦听端口标识数据类型
            unsigned short m_ListenPort;
    };
}

#endif //_JRU_SERVICE_H
