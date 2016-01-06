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

#include <map>

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Reactor.h"
#include "Packet.h"
#include "Buffer.h"

namespace EXTSERVICE
{
    class CRawdataSub;

    /**
     * 将EOAS业务数据转发给传输控制模块
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
            int handleTimeout(HELIUM::TimerId tid);

            /// 向传输控制模块发送报文
            int sendPacket(void);

            /**
             * 将数据放入缓冲区
             *
             * @param data    数据地址
             * @param length  数据长度
             *
             */
            void pushData(const char* data, int length);

            /// 复位业务数据缓冲区
            void resetBuffer(void) { m_EoasData.reset(); }

            /// 业务数据缓冲区封装结束
            void finishBuffer(void) { m_EoasData.finish(); }

            void setDataCommiter(CRawdataSub* sub) { m_DataCommiter = sub; }

        private:
            CTransmit(const CTransmit&);
            CTransmit& operator=(const CTransmit&);

            void parseFrame(const char* data, int length);

            void procRemoteCmd(const char* cmd, int length);

        private:
            /**
             * key为与EOAS主机通信时用的数据类型，
             * value为与地面服务器通信时用的数据类型
             *
             */
            std::map<unsigned char, unsigned char> m_EoasDatMap;

            /// 封装好的EOAS业务数据
            HELIUM::CExchangePacket m_EoasData;

            /// 读取数据缓冲区
            HELIUM::CBuffer m_InputBuffer;

            HELIUM::TimerId m_RegCmdTid;

            CRawdataSub* m_DataCommiter;
    };
}

#endif //_TRANSMIT_H
