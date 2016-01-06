/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: CoreApp.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月24日 15时56分20秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "CoreApp.h"
#include "Logger.h"
#include "AtpPlugin.h"
#include "../CommAddress.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace COREBUSINESS;

HELIUM_DECL_SINGLETON(CCoreApp);

CCoreApp::CCoreApp() : m_CoreSub(NULL), m_DataTransmit(NULL), m_Publisher(NULL),
    m_SwitchAddr(DATASWITCH_ADDRESS), m_TransAddr(BUSINESS_SINK_ADDRESS)
{
    m_ProcName = "coreproc";
    m_Version = "2.0.3.1030";

    m_pConfig = new CParamsConfig(&m_CustomParameters);
}

CCoreApp::~CCoreApp()
{
    unloadPlugin();
    delete m_CoreSub; m_CoreSub = NULL;
    delete m_DataTransmit; m_DataTransmit = NULL;
    delete m_Publisher; m_Publisher = NULL;
    delete m_pConfig; m_pConfig = NULL;
}

void CCoreApp::initService()
{
    DBG_LOG((DATAWATCHER, "[%s v%s] START RUNNING...\n", m_ProcName.c_str(), m_Version.c_str()));
    std::string libatp_name("libAtp");
    libatp_name += APPCONFIG->getAtpType();
    loadPlugin(libatp_name);

    m_CoreSub = new CCoreSub();
    m_CoreSub->setId("Raw Data Subscriber");
    std::string app_fullname = m_ProcName + ":" + m_Version;
    if (gAtpHandlers.getVersion)
    {
        app_fullname += "\n";
        app_fullname += gAtpHandlers.getVersion();
    }

    app_fullname.append("\n|");
    char dataset[256] = {'\0'};
    m_CoreSub->getSubscribeSet(dataset, sizeof(dataset));
    app_fullname.append(dataset);

    const char* name_ptr = app_fullname.c_str();
    m_CoreSub->creatHeartbeatPacket((uint8_t)PROG_COREPROC, HEARTBEAT, name_ptr, strlen(name_ptr));

    if (m_SwitchConnector.open(REACTOR, m_CoreSub, &m_SwitchAddr, AF_UNIX) < 0)
    {
        DBG_LOG((COREPROC, "Failed to open connector to datahub!\n"));
    }
    else
    {
        m_SwitchConnector.connect();
    }

    m_DataTransmit = new CTransmit();
    m_DataTransmit->setId("Core Data Transmiter");

    if (m_TransCtrlConn.open(REACTOR, m_DataTransmit, &m_TransAddr, AF_UNIX) < 0)
    {
        DBG_LOG((COREPROC, "Failed to open connector to transctrl!\n"));
    }
    else
    {
        m_TransCtrlConn.connect();
    }

    m_CoreSub->setTransmiter(m_DataTransmit);

    m_Publisher = new CPublisher(REACTOR);
    m_Publisher->init();
}

void CCoreApp::processEvents()
{
    while (serviceActive())
    {
        m_Reactor.waitForEvents();
    }

    m_Reactor.stopReactor();
}
