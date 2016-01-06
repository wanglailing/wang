/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TcaservApp.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月26日 13时12分44秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef __TCASERV_APP_H
#define __TCASERV_APP_H

#include "App.h"
#include "Singleton.h"
#include "UnixAddress.h"
#include "TcpSocket.h"
#include "Connector.h"

#include "RawdataSub.h"
#include "CoredataSub.h"

namespace EXTSERVICE
{
    /**
     * TCAS扩展服务程序
     *
     */
    class CTcaservApp : public HELIUM::CApp, public HELIUM::CSingleton<CTcaservApp>
    {
        public:
            typedef HELIUM::CConnector<CRawdataSub, HELIUM::CTcpSocket> HUB_CONN;
            typedef HELIUM::CConnector<CCoredataSub, HELIUM::CTcpSocket> CORE_CONN;

        public:
            CTcaservApp();
            ~CTcaservApp();

            void initService();
            void processEvents();

        private:
            CTcaservApp(const CTcaservApp&);
            CTcaservApp& operator=(const CTcaservApp&);

        private:
            /// 连接数据交换模块
            HUB_CONN   m_HubConn;

            /// 连接核心业务数据处理模块
            CORE_CONN  m_CoreConn;

            CRawdataSub*  m_RawdataSub;
            CCoredataSub* m_CoredataSub;

            HELIUM::CUnixAddress m_HubAddr;
            HELIUM::CUnixAddress m_CoreAddr;
    };

    #define TCASERV_APP CTcaservApp::getInstance()
    #define APPCONFIG   TCASERV_APP->getConfig()
    #define REACTOR     TCASERV_APP->getReactor()
}

#endif //__TCASERV_APP_H
