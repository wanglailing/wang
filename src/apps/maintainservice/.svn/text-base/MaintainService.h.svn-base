/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: MaintainService.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月24日 10时25分38秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _MAINTAIN_SERVICE_H
#define _MAINTAIN_SERVICE_H

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Reactor.h"
#include "Buffer.h"

namespace DMS
{
    /**
     * DMS车载设备维护服务.侦听TCP端口，接受维护软件的连接。
     * 向维护软件发送设备状态，并接收设备固件程序数据，完成设备的升级。
     *
     */
    class CMaintainService : public HELIUM::CServiceHandler<HELIUM::CTcpSocket>
    {
        public:
            CMaintainService(HELIUM::CTcpSocket *stream);
            ~CMaintainService();

            virtual int open(HELIUM::CReactor* r);
            virtual int handleRead(int fd);
            virtual int handleClose(int fd);

            const bool hasQueriedDevState(void) const { return m_bQueriedDevState; }

        private:
            /// 来自维护软件的数据都放到CBuffer对象中，该函数处理这些数据。
            void processClientData(void);

        private:
            /// 读取套接字数据缓冲区
            HELIUM::CBuffer m_InputBuffer;

            /**
             * 只有当客户端请求过设备状态时，才会向其发送各设备的数据。
             */
            bool m_bQueriedDevState;

            static const char *m_GetStatusCmd;
            static const char *m_WriteDataCmd;
            static const char *m_HeartbeatCmd;
    };

    inline CMaintainService::CMaintainService(HELIUM::CTcpSocket *stream)
        : HELIUM::CServiceHandler<HELIUM::CTcpSocket>(stream), 
        m_bQueriedDevState(false)
    {
    }

    inline CMaintainService::~CMaintainService()
    {
    }
}

#endif //_MAINTAIN_SERVICE_H
