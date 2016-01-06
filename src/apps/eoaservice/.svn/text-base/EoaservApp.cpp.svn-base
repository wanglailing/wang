/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: EoaservApp.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月13日 10时27分35秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "EoaservApp.h"

#include "../CommAddress.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace EXTSERVICE;

HELIUM_DECL_SINGLETON(CEoaservApp);

CEoaservApp::CEoaservApp() : m_RawdataSub(NULL), m_CoredataSub(NULL), m_DataTransmit(NULL),
    m_HubAddr(DATASWITCH_ADDRESS), m_TransAddr(BUSINESS_SINK_ADDRESS),
    m_CoreAddr(DMSPUB_ADDRESS)
{
    m_ProcName = "eoaservice";
    m_Version = "2.0.2.491";
}

CEoaservApp::~CEoaservApp()
{
    delete m_RawdataSub; m_RawdataSub = NULL;
    delete m_CoredataSub; m_CoredataSub = NULL;
    delete m_DataTransmit; m_DataTransmit = NULL;
}

void CEoaservApp::initService()
{
    DBG_LOG((DATAWATCHER, "[%s v%s] START RUNNING...\n", m_ProcName.c_str(), m_Version.c_str()));
    m_RawdataSub = new CRawdataSub();
    m_RawdataSub->setId("Raw Data Subscriber");

    std::string app_fullname = m_ProcName + ":" + m_Version;
    app_fullname.append("\n|");
    char dataset[256] = {'\0'};
    m_RawdataSub->getSubscribeSet(dataset, sizeof(dataset));
    app_fullname.append(dataset);
    const char* name_ptr = app_fullname.c_str();
    m_RawdataSub->creatHeartbeatPacket((uint8_t)PROG_EOASERV, HEARTBEAT, name_ptr, strlen(name_ptr));

    if (m_HubConn.open(REACTOR, m_RawdataSub, &m_HubAddr, AF_UNIX) < 0)
    {
        DBG_LOG((FATALERR, "Failed to open connector to datahub!\n"));
    }
    else
    {
        m_HubConn.connect();
    }

    m_DataTransmit = new CTransmit();
    m_DataTransmit->setId("Eoas Data Transmiter");

    if (m_TransCtrlConn.open(REACTOR, m_DataTransmit, &m_TransAddr, AF_UNIX) < 0)
    {
        DBG_LOG((FATALERR, "Failed to open connector to transctrl!\n"));
    }
    else
    {
        m_TransCtrlConn.connect();
    }

    m_CoredataSub = new CCoredataSub();
    m_CoredataSub->setId("Core Data Subscriber");
    if (m_CoreConn.open(REACTOR, m_CoredataSub, &m_CoreAddr, AF_UNIX) < 0)
    {
        DBG_LOG((FATALERR, "Failed to open connector to coreproc!\n"));
    }
    else
    {
        m_CoreConn.connect();
    }

    m_RawdataSub->setTransmiter(m_DataTransmit);
    m_RawdataSub->setCoreSubscriber(m_CoredataSub);
}

void CEoaservApp::processEvents()
{
    while (serviceActive())
    {
        m_Reactor.waitForEvents();
    }

    m_Reactor.stopReactor();
}

