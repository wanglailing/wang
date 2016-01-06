/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: StoreApp.h
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

#ifndef _STORE_APP_H
#define _STORE_APP_H

#include "ContentService.h"
#include "ContentSink.h"
#include "RawdataSub.h"
#include "StoreDevMgr.h"
#include "LogService.h"
#include "DataRecorder.h"

#include "App.h"
#include "AppConfig.h"
#include "Singleton.h"
#include "Repository.h"
#include "Connector.h"
#include "UnixAddress.h"

namespace STORESERVICE
{
    class CStoreApp : public HELIUM::CApp, public HELIUM::CSingleton<CStoreApp>
    {
        public:
            typedef HELIUM::CConnector<CRawdataSub, HELIUM::CTcpSocket> CONN_TYPE;
            typedef HELIUM::CRepository<CContServ> REPO_TYPE;

        public:
            CStoreApp();
            ~CStoreApp();

            void initService();
            void processEvents();

            HELIUM::CAppConfig* getConfig(void) { return &m_Config; }
            REPO_TYPE* getServRepository() { return &m_DataServiceRepo; }
            CContSink* getContentSink() { return m_DataSink; }
            CStoreDevMgr* getStoreMgr() { return m_StoreManager; }
            CDataRecorder* getDataRecorder() { return m_DataRecorder; }

        private:
            CStoreApp(const CStoreApp&);
            CStoreApp& operator=(const CStoreApp&);

        private:
            /// 应用程序配置
            HELIUM::CAppConfig m_Config;

            /// 连接至数据交换模块
            CONN_TYPE m_SwitchConnector;

            /// 数据交换模块连接服务
            CRawdataSub* m_RawdatSub;

            /// 业务数据汇聚
            CContSink* m_DataSink;

            /// 存储设备管理
            CStoreDevMgr* m_StoreManager;

            /// 日志服务
            CLogService* m_LogService;

            /// 数据记录器
            CDataRecorder* m_DataRecorder;

            /// 数据交换模块的地址
            HELIUM::CUnixAddress m_SwitchAddr;

            /// 业务数据接收服务容器
            REPO_TYPE m_DataServiceRepo;
    };

    #define STORE_APP     CStoreApp::getInstance()
    #define APPCONFIG     STORE_APP->getConfig()
    #define REACTOR       STORE_APP->getReactor()
    #define SERV_REPO     STORE_APP->getServRepository()
    #define CONT_SINK     STORE_APP->getContentSink()
    #define STORE_MGR     STORE_APP->getStoreMgr()
    #define DATA_REC      STORE_APP->getDataRecorder()
}

#endif //_STORE_APP_H
