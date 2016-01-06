/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: CoreApp.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月24日 15时47分58秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _CORE_APP_H
#define _CORE_APP_H

#include "App.h"
#include "Singleton.h"
#include "Repository.h"
#include "CoreSub.h"
#include "Transmit.h"
#include "Connector.h"
#include "PubService.h"
#include "Publisher.h"
#include "UnixAddress.h"
#include "ParamsConfig.h"

namespace COREBUSINESS
{
    /**
     * 核心业务处理程序类
     *
     */
    class CCoreApp : public HELIUM::CApp, public HELIUM::CSingleton<CCoreApp>
    {
        public:
            typedef HELIUM::CConnector<CCoreSub, HELIUM::CTcpSocket> CONN_TYPE;
            typedef HELIUM::CConnector<CTransmit, HELIUM::CTcpSocket> TRANS_TYPE;
            typedef HELIUM::CRepository<CPubService> REPO_TYPE;

        public:
            CCoreApp();
            ~CCoreApp();

            void initService();
            void processEvents();

            const CParamsConfig *getConfig() const { return m_pConfig; }

            /// 获取发布服务资源
            REPO_TYPE* getServRepository() { return &m_ServRepo; }
            CPublisher* getPublisher() { return m_Publisher; }

        private:
            CCoreApp(const CCoreApp&);
            CCoreApp& operator=(const CCoreApp&);

        private:
            /// 应用程序配置
            CParamsConfig *m_pConfig;

            /// 自定义参数集
            CustomParams m_CustomParameters;

            /// 连接至数据交换模块
            CONN_TYPE m_SwitchConnector;

            /// 连接至传输控制模块
            TRANS_TYPE m_TransCtrlConn;

            /// 数据交换模块连接服务
            CCoreSub* m_CoreSub;

            /// 数据转发服务
            CTransmit* m_DataTransmit;

            CPublisher* m_Publisher;

            /// 数据交换模块的地址
            HELIUM::CUnixAddress m_SwitchAddr;

            /// 传输控制模块的地址
            HELIUM::CUnixAddress m_TransAddr;

            /// 核心数据发布服务容器
            REPO_TYPE m_ServRepo;
    };

    #define CORE_APP CCoreApp::getInstance()
    #define REACTOR   CORE_APP->getReactor()
    #define APPCONFIG CORE_APP->getConfig()
    #define SERV_REPO CORE_APP->getServRepository()
    #define PUBLISHER CORE_APP->getPublisher()
}

#endif //_CORE_APP_H
