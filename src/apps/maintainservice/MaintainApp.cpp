/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: MaintainApp.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月20日 17时21分52秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "Logger.h"
#include "MaintainApp.h"
#include "../CommAddress.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace DMS;

HELIUM_DECL_SINGLETON(CMaintainApp);

CMaintainApp::CMaintainApp() : m_AcceptServ(NULL), m_SwitchAddr(DATASWITCH_ADDRESS), m_ConnServ(NULL)
{
    m_ProcName = "maintainer";
    m_Version = "2.0.2.820";
}

CMaintainApp::~CMaintainApp()
{
    delete m_AcceptServ; m_AcceptServ = NULL;
    delete m_ConnServ; m_ConnServ = NULL;
}

void CMaintainApp::initService()
{
    DBG_LOG((DATAWATCHER, "[%s v%s] START RUNNING...\n", m_ProcName.c_str(), m_Version.c_str()));
    m_AcceptServ = new CAcceptServer(getReactor());
    if (m_AcceptServ->init() < 0)
    {
        ERR_LOG((MTSERVICE, "Initialize the maintainer acceptor error!\n"));
    }

    m_ConnServ = new ConnService();
    std::string app_fullname = m_ProcName + ":" + m_Version;
    app_fullname.append("\n|*");
    const char* name_ptr = app_fullname.c_str();
    m_ConnServ->creatHeartbeatPacket((uint8_t)PROG_MTSERVICE, HEARTBEAT, name_ptr, strlen(name_ptr));

    if (m_SwitchConnector.open(REACTOR, m_ConnServ, &m_SwitchAddr, AF_UNIX) < 0)
    {
        DBG_LOG((MTSERVICE, "Failed to open connector to datahub!\n"));
    }
    else
    {
        m_SwitchConnector.connect();
    }
}

void CMaintainApp::processEvents()
{
    while (serviceActive())
    {
        m_Reactor.waitForEvents();
    }

    m_Reactor.stopReactor();
}
