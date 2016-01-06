/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: DataSwitchApp.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月27日 17时00分46秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DATASWITCH_APP_H
#define _DATASWITCH_APP_H

#include <map>

#include "App.h"
#include "AppConfig.h"
#include "Singleton.h"
#include "DataHub.h"
#include "JruService.h"
#include "SwitchService.h"
#include "Repository.h"

namespace DMS
{
    /**
     * 数据交互程序类
     *
     */
    class CDataSwitchApp : public HELIUM::CApp, public HELIUM::CSingleton<CDataSwitchApp>
    {
        public:
            typedef HELIUM::CRepository<CSwitchService> REPO_TYPE;
            typedef HELIUM::CRepository<CJRUService> JRUREPO_TYPE;

        public:
            CDataSwitchApp();
            ~CDataSwitchApp();

            void initService();
            void processEvents();

            /// 获取程序配置对象
            HELIUM::CAppConfig* getConfig(void) { return &m_Config; }

            /// 获取维护客户端服务资源
            REPO_TYPE* getServRepository() { return &m_ServRepo; }

            /// 获取JRU客户端服务资源(仅限于ATP-T)
            JRUREPO_TYPE* getJruRepository() { return &m_JruRepo; }

            CDataHub* getDataHubber() { return m_DataHubber; }

            /// 设置模块信息
            void setModuleInfo(const char* info, int length);

            /// 执行远程命令
            char* runCmd(const char* cmd, int len, int* rc_len);

            DataId get300tId(void) const { return m_300tId; }

        private:
            /// 禁止赋值和拷贝
            CDataSwitchApp(const CDataSwitchApp&);
            CDataSwitchApp& operator=(const CDataSwitchApp&);

            /// 填充命令执行结果
            int stuffCmdResult(unsigned short sender, char *src, int srclen, char channel_type);

            /// 配置GSMR插板
            char* configGsmr(unsigned short sender, char config_cmd, int* rc_len);

            /// 获取所有模块的信息
            std::string getAllModules(void);

            /// 使用远程时间校正本地时间
            void adjustByRemoteTime(const char* time_buf, int len);

        private:
            /// 应用程序配置
            HELIUM::CAppConfig m_Config;

            /// 设备数据中继器
            CDataHub *m_DataHubber;

            /// 远程命令执行结果
            char m_ExecResult[256];

            /// 所有模块的名称及版本号(key为模块名，value为版本号)
            std::map<std::string, std::string> ModulesCaption;

            /// 维护客户端服务容器
            REPO_TYPE m_ServRepo;

            /// JRU客户端服务容器
            JRUREPO_TYPE m_JruRepo;

            /// 300T数据标识
            DataId m_300tId;
    };

    #define DATASWITCH_APP CDataSwitchApp::getInstance()
    #define APP_CONFIG DATASWITCH_APP->getConfig()
    #define REACTOR    DATASWITCH_APP->getReactor()
    #define SERV_REPO  DATASWITCH_APP->getServRepository()
    #define JRU_REPO   DATASWITCH_APP->getJruRepository()
    #define DATA_HUB   DATASWITCH_APP->getDataHubber()
}

#endif //_DATASWITCH_APP_H
