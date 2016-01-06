/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: EoaservApp.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月13日 10时16分46秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _EOASERV_APP_H
#define _EOASERV_APP_H

#include "App.h"
#include "AppConfig.h"
#include "Singleton.h"
#include "UnixAddress.h"
#include "TcpSocket.h"
#include "Connector.h"

#include "RawdataSub.h"
#include "CoredataSub.h"
#include "Transmit.h"

namespace EXTSERVICE
{
    /**
     * EOAS扩展服务程序
     *
     */
    class CEoaservApp : public HELIUM::CApp, public HELIUM::CSingleton<CEoaservApp>
    {
        public:
            typedef HELIUM::CConnector<CRawdataSub, HELIUM::CTcpSocket> HUB_CONN;
            typedef HELIUM::CConnector<CCoredataSub, HELIUM::CTcpSocket> CORE_CONN;
            typedef HELIUM::CConnector<CTransmit, HELIUM::CTcpSocket>   TRANS_CONN;

        public:
            CEoaservApp();
            ~CEoaservApp();

            void initService();
            void processEvents();

            HELIUM::CAppConfig* getConfig(void) { return &m_Config; }

        private:
            CEoaservApp(const CEoaservApp&);
            CEoaservApp& operator=(const CEoaservApp&);

        private:
            /// 应用程序配置
            HELIUM::CAppConfig m_Config;

            /// 连接数据交换模块
            HUB_CONN   m_HubConn;

            /// 连接核心业务数据处理模块
            CORE_CONN  m_CoreConn;

            /// 连接传输控制模块
            TRANS_CONN m_TransCtrlConn;

            CRawdataSub*  m_RawdataSub;
            CCoredataSub* m_CoredataSub;
            CTransmit*    m_DataTransmit;

            HELIUM::CUnixAddress m_HubAddr;
            HELIUM::CUnixAddress m_TransAddr;
            HELIUM::CUnixAddress m_CoreAddr;
    };

    #define EOASERV_APP CEoaservApp::getInstance()
    #define APPCONFIG   EOASERV_APP->getConfig()
    #define REACTOR     EOASERV_APP->getReactor()
}

#endif //_EOASERV_APP_H
