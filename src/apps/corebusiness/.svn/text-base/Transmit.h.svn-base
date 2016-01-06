/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Transmit.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月03日 10时21分21秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _TRANSMIT_H
#define _TRANSMIT_H

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Reactor.h"

namespace COREBUSINESS
{
    /**
     * 将DMS业务数据转发给传输控制模块
     *
     */
    class CTransmit : public HELIUM::CServiceHandler<HELIUM::CTcpSocket>
    {
        public:
            CTransmit();
            ~CTransmit();

            int open(HELIUM::CReactor* r);
            void close(void);
            int handleRead(int fd);

            int writeData(const char* buf, int length);

        private:
            CTransmit(const CTransmit&);
            CTransmit& operator=(const CTransmit&);
    };
}

#endif //_TRANSMIT_H
