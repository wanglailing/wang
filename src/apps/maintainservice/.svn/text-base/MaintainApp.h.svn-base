/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: MaintainApp.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月20日 17时15分46秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _MAINTAIN_APP_H
#define _MAINTAIN_APP_H

#include "App.h"
#include "Singleton.h"
#include "Repository.h"
#include "MaintainService.h"
#include "ConnService.h"
#include "AcceptServer.h"
#include "Connector.h"
#include "UnixAddress.h"
#include "SlaveDevManager.h"

namespace DMS
{
    /**
     * 维护服务程序类
     *
     */
    class CMaintainApp : public HELIUM::CApp, public HELIUM::CSingleton<CMaintainApp>
    {
        public:
            typedef HELIUM::CRepository<CMaintainService> REPO_TYPE;
            typedef HELIUM::CConnector<ConnService, HELIUM::CTcpSocket> CONN_TYPE;

        public:
            CMaintainApp();
            ~CMaintainApp();

            virtual void initService();
            virtual void processEvents();

            /// 获取维护客户端服务资源
            REPO_TYPE* getServRepository() { return &m_ServRepo; }

            CAcceptServer* getMaintainServ() { return m_AcceptServ; }
            ConnService*   getSwitchService() { return m_ConnServ; }

        private:
            /// 禁止赋值和拷贝
            CMaintainApp(const CMaintainApp&);
            CMaintainApp& operator=(const CMaintainApp&);

        private:
            /// 接收维护客户端的连接
            CAcceptServer *m_AcceptServ;

            /// 连接至数据交换模块
            CONN_TYPE m_SwitchConnector;

            /// 数据交换模块的地址
            HELIUM::CUnixAddress m_SwitchAddr;

            /// 数据交换模块连接服务
            ConnService* m_ConnServ;

            /// 维护客户端服务容器
            REPO_TYPE m_ServRepo;
    };

    #define MAINTAIN_APP CMaintainApp::getInstance()
    #define REACTOR MAINTAIN_APP->getReactor()
    #define SERV_REPO MAINTAIN_APP->getServRepository()
}

#endif //_MAINTAIN_APP_H
