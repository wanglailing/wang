/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TransportApp.cpp
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

#include "TransportApp.h"
#include "Logger.h"
#include "../CommAddress.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

HELIUM_DECL_SINGLETON(CTransportApp);

CTransportApp::CTransportApp() : m_TransSub(NULL), m_DataSink(NULL), m_ReliableCtrl(NULL),
    m_SwitchAddr(DATASWITCH_ADDRESS)
{
    m_ProcName = "transctrl";
    m_Version = "2.0.2.607";
}

CTransportApp::~CTransportApp()
{
    delete m_TransSub; m_TransSub = NULL;
    delete m_DataSink; m_DataSink = NULL;
    delete m_ReliableCtrl; m_ReliableCtrl = NULL;
}

void CTransportApp::initService()
{
    DBG_LOG((DATAWATCHER, "[%s v%s] START RUNNING...\n", m_ProcName.c_str(), m_Version.c_str()));
    m_TransSub = new CTransSubscribe();
    m_TransSub->setId("Raw Data Subscriber");
    std::string app_fullname = m_ProcName +  ":" + m_Version;
    app_fullname.append("\n|");
    char dataset[256] = {'\0'};
    m_TransSub->getSubscribeSet(dataset, sizeof(dataset));
    app_fullname.append(dataset);
    const char* name_ptr = app_fullname.c_str();
    m_TransSub->creatHeartbeatPacket((uint8_t)PROG_TRANSCTRL, HEARTBEAT, name_ptr, strlen(name_ptr));

    if (m_SwitchConnector.open(REACTOR, m_TransSub, &m_SwitchAddr, AF_UNIX) < 0)
    {
        DBG_LOG((TRANSCTRL, "Failed to open connector to datahub!\n"));
    }
    else
    {
        m_SwitchConnector.connect();
    }

    m_DataSink = new CDataSink(REACTOR);
    m_DataSink->init();

    m_ReliableCtrl = new CReliableCtrl();
}

void CTransportApp::processEvents()
{
    while (serviceActive())
    {
        m_Reactor.waitForEvents();
    }

    m_Reactor.stopReactor();
}

CDataService* CTransportApp::getServiceById(uint8_t serv_id)
{
    CRepository<CDataService>::iterator iter;
    for (iter = m_DataServiceRepo.begin(); iter != m_DataServiceRepo.end(); ++iter)
    {
        if ((*iter)->getServId() == serv_id)
        {
            return *iter;
        }
    }

    return NULL;
}

CDataService* CTransportApp::getServiceByCmd(const char* cmd, int cmdlen)
{
    CRepository<CDataService>::iterator iter;
    for (iter = m_DataServiceRepo.begin(); iter != m_DataServiceRepo.end(); ++iter)
    {
        if ((*iter)->getCmdSets().find(cmd, 0, cmdlen) != std::string::npos)
        {
            return *iter;
        }
    }

    return NULL;
}

