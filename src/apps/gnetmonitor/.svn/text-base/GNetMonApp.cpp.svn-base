/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GNetMonApp.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月10日 14时16分35秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "GNetMonApp.h"

#include "../CommAddress.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace EXTSERVICE;

HELIUM_DECL_SINGLETON(CGNetMonApp);

CGNetMonApp::CGNetMonApp() : m_RawdataSub(NULL), 
    m_HubAddr(DATASWITCH_ADDRESS)
{
    m_ProcName = "gnetmon";
    m_Version = "2.0.2.491";
}

CGNetMonApp::~CGNetMonApp()
{
    delete m_RawdataSub; m_RawdataSub = NULL;
}

void CGNetMonApp::initService()
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
    m_RawdataSub->creatHeartbeatPacket((uint8_t)PROG_GNETMON, HEARTBEAT, name_ptr, strlen(name_ptr));

    if (m_HubConn.open(REACTOR, m_RawdataSub, &m_HubAddr, AF_UNIX) < 0)
    {
        DBG_LOG((FATALERR, "Failed to open connector to datahub!\n"));
    }
    else
    {
        m_HubConn.connect();
    }
}

void CGNetMonApp::processEvents()
{
    while (serviceActive())
    {
        m_Reactor.waitForEvents();
    }

    m_Reactor.stopReactor();
}

