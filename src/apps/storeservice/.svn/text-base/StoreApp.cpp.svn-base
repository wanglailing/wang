/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: StoreApp.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月31日 16时25分52秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "StoreApp.h"
#include "Logger.h"
#include "../CommAddress.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace STORESERVICE;

HELIUM_DECL_SINGLETON(CStoreApp);

CStoreApp::CStoreApp() : m_RawdatSub(NULL), m_DataSink(NULL), m_StoreManager(NULL), m_LogService(NULL),
                         m_DataRecorder(NULL), m_SwitchAddr(DATASWITCH_ADDRESS)
{
    m_ProcName = "storeserv";
    m_Version = "2.0.2.565";
}

CStoreApp::~CStoreApp()
{
    delete m_RawdatSub; m_RawdatSub = NULL;
    delete m_DataSink; m_DataSink = NULL;

    m_StoreManager->stop();
    delete m_StoreManager; m_StoreManager = NULL;

    m_LogService->stop();
    delete m_LogService; m_LogService = NULL;

    m_DataRecorder->stop();
    delete m_DataRecorder; m_DataRecorder = NULL;
}

void CStoreApp::initService()
{
    m_StoreManager = new CStoreDevMgr();
    m_LogService = new CLogService(m_StoreManager->isCFCardMounted());
    m_DataRecorder = new CDataRecorder();

    sleep(1);
    DBG_LOG((DATAWATCHER, "[%s v%s] START RUNNING...\n", m_ProcName.c_str(), m_Version.c_str()));

    /// 等待数据交换进程启动
    sleep(2);
    m_RawdatSub = new CRawdataSub();
    m_RawdatSub->setId("Raw Data Subscriber");
    std::string app_fullname = m_ProcName +  ":" + m_Version;
    app_fullname.append("\n|*");
    const char* name_ptr = app_fullname.c_str();
    m_RawdatSub->creatHeartbeatPacket((uint8_t)PROG_STORESERV, HEARTBEAT, name_ptr, strlen(name_ptr));

    if (m_SwitchConnector.open(REACTOR, m_RawdatSub, &m_SwitchAddr, AF_UNIX) < 0)
    {
        /// open error!
    }
    else
    {
        m_SwitchConnector.connect();
    }

    m_DataSink = new CContSink(REACTOR);
    m_DataSink->init();
}

void CStoreApp::processEvents()
{
    while (serviceActive())
    {
        m_Reactor.waitForEvents();
    }

    m_Reactor.stopReactor();
}

