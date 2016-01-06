/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataService.h
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

#ifndef _DATA_SERVICE_H
#define _DATA_SERVICE_H

#include <string>

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Reactor.h"
#include "Buffer.h"
#include "Packet.h"

namespace TRANSPORTCTRL
{
    class CDataService : public HELIUM::CServiceHandler<HELIUM::CTcpSocket>
    {
        public:
            CDataService(HELIUM::CTcpSocket *stream);
            ~CDataService();

            int open(HELIUM::CReactor* r);
            int handleRead(int fd);
            int handleClose(int fd);

            uint8_t getServId(void) { return m_ServiceId; }
            const std::string& getCmdSets(void) const { return m_RegCmds; }

        private:
            void parseFrame(const char* data, int length);

        private:
            /// 读取套接字数据缓冲区
            HELIUM::CBuffer m_InputBuffer;

            /// 服务标识
            uint8_t m_ServiceId;

            /// 数据服务模块能够处理的命令集
            std::string m_RegCmds;
    };

    inline CDataService::CDataService(HELIUM::CTcpSocket *stream)
        : HELIUM::CServiceHandler<HELIUM::CTcpSocket>(stream), m_ServiceId(0xFF)
    {
        setId("Business Data Receiver");
    }

    inline CDataService::~CDataService()
    {
    }
}

#endif //_DATA_SERVICE_H
