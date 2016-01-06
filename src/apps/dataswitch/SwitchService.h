/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: SwitchService.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月20日 16时12分10秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _SWITCH_SERVICE_H
#define _SWITCH_SERVICE_H

#include <vector>

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Reactor.h"
#include "Buffer.h"

namespace DMS
{
    /**
     * 数据交换服务。
     *
     */
    class CSwitchService : public HELIUM::CServiceHandler<HELIUM::CTcpSocket>
    {
        public:
            CSwitchService(HELIUM::CTcpSocket *stream);
            ~CSwitchService();

            virtual int open(HELIUM::CReactor* r);
            virtual int handleRead(int fd);
            virtual int handleClose(int fd);

            /// dataid是否被查询过
            bool isDataQueried(unsigned char dataid);

        private:
            /// 处理一帧完整的数据
            void processData(const char* data, int length);

            /// 设置查询的数据集
            void setQueriedDataSet(const char* data, int length);

        private:
            /// 读取套接字数据缓冲区
            HELIUM::CBuffer m_InputBuffer;

            /// 请求的数据集
            std::vector<unsigned char> m_ServDataSets;
    };

    inline CSwitchService::CSwitchService(HELIUM::CTcpSocket *stream)
        : HELIUM::CServiceHandler<HELIUM::CTcpSocket>(stream)
    {
        setId("Data Switch Service");
    }

    inline CSwitchService::~CSwitchService()
    {
    }
}

#endif //_SWITCH_SERVICE_H

