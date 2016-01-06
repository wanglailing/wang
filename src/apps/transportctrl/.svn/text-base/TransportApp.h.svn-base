/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: TransportApp.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月31日 16时15分38秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _TRANSPORT_APP_H
#define _TRANSPORT_APP_H

#include "App.h"
#include "AppConfig.h"
#include "Singleton.h"
#include "Repository.h"
#include "TransSubscribe.h"
#include "Connector.h"
#include "DataService.h"
#include "DataSink.h"
#include "ReliableCtrl.h"
#include "UnixAddress.h"

namespace TRANSPORTCTRL
{
    class CTransportApp : public HELIUM::CApp, public HELIUM::CSingleton<CTransportApp>
    {
        public:
            typedef HELIUM::CConnector<CTransSubscribe, HELIUM::CTcpSocket> CONN_TYPE;
            typedef HELIUM::CRepository<CDataService> REPO_TYPE;

        public:
            CTransportApp();
            ~CTransportApp();

            void initService();
            void processEvents();

            HELIUM::CAppConfig* getConfig(void) { return &m_Config; }
            REPO_TYPE* getServRepository() { return &m_DataServiceRepo; }
            CDataSink* getDataSink() { return m_DataSink; }
            CReliableCtrl* getReliableCtrl() { return m_ReliableCtrl; }

            CDataService* getServiceById(uint8_t serv_id);
            CDataService* getServiceByCmd(const char* cmd, int cmdlen);

        private:
            CTransportApp(const CTransportApp&);
            CTransportApp& operator=(const CTransportApp&);

        private:
            /// 应用程序配置
            HELIUM::CAppConfig m_Config;

            /// 连接至数据交换模块
            CONN_TYPE m_SwitchConnector;

            /// 数据交换模块连接服务
            CTransSubscribe* m_TransSub;

            /// 业务数据汇聚
            CDataSink* m_DataSink;

            /// 可靠传输控制
            CReliableCtrl* m_ReliableCtrl;

            /// 数据交换模块的地址
            HELIUM::CUnixAddress m_SwitchAddr;

            /// 业务数据接收服务容器
            REPO_TYPE m_DataServiceRepo;
    };

    #define TRANS_APP     CTransportApp::getInstance()
    #define APPCONFIG     TRANS_APP->getConfig()
    #define REACTOR       TRANS_APP->getReactor()
    #define SERV_REPO     TRANS_APP->getServRepository()
    #define DATA_SINK     TRANS_APP->getDataSink()
    #define RELIABLE_CTRL TRANS_APP->getReliableCtrl()
}

#endif //_TRANSPORT_APP_H
