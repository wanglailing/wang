/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GNetMonApp.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月10日 14时11分44秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _GNETMON_APP_H
#define _GNETMON_APP_H

#include "App.h"
#include "AppConfig.h"
#include "Singleton.h"
#include "UnixAddress.h"
#include "TcpSocket.h"
#include "Connector.h"

#include "RawdataSub.h"

namespace EXTSERVICE
{
    /**
     * G网检测扩展服务程序
     *
     */
    class CGNetMonApp : public HELIUM::CApp, public HELIUM::CSingleton<CGNetMonApp>
    {
        public:
            typedef HELIUM::CConnector<CRawdataSub, HELIUM::CTcpSocket> HUB_CONN;

        public:
            CGNetMonApp();
            ~CGNetMonApp();

            void initService();
            void processEvents();

            HELIUM::CAppConfig* getConfig(void) { return &m_Config; }

        private:
            CGNetMonApp(const CGNetMonApp&);
            CGNetMonApp& operator=(const CGNetMonApp&);

        private:
            /// 应用程序配置
            HELIUM::CAppConfig m_Config;

            /// 连接数据交换模块
            HUB_CONN   m_HubConn;

            CRawdataSub*  m_RawdataSub;
            HELIUM::CUnixAddress m_HubAddr;
    };

    #define GNETMON_APP CGNetMonApp::getInstance()
    #define APPCONFIG   GNETMON_APP->getConfig()
    #define REACTOR     GNETMON_APP->getReactor()
}

#endif //_GNETMON_APP_H
