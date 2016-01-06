/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: AcceptServer.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月17日 11时16分12秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "AcceptServer.h"
#include "MaintainApp.h"
#include "InetAddress.h"
#include "MemDump.h"
#include "Assure.h"

using namespace HELIUM;
using namespace DMS;

CAcceptServer::CAcceptServer(CReactor *r) : m_Reactor(r)
{
    m_MaintainAcceptor = new CAcceptor<CMaintainService, CTcpSocket>(r);
}

CAcceptServer::~CAcceptServer()
{
    delete m_MaintainAcceptor;

    m_Responser.close();
}

int CAcceptServer::init(void)
{
    m_MaintainAcceptor->setId("DMS Maintain Service");
    CInetAddress listenAddr(2046);
    ASSERT_EXIT(!listenAddr.bad());
    ASSERT_EXIT(m_MaintainAcceptor->open(listenAddr) == 0);

    m_Responser.open(AF_INET);

    return 0;
}

int CAcceptServer::responseData(const char *buf, int len)
{
    if (SERV_REPO->empty() == true)
    {
        return -1;
    }

    CRepository<CMaintainService>::const_iterator iter = SERV_REPO->begin();
    while (iter != SERV_REPO->end())
    {
        if (*iter && (*iter)->hasQueriedDevState())
        {
            CInetAddress dest_addr((*iter)->getStream().getRemoteAddress());
            dest_addr.setPort(8001);

            m_Responser.sendto(buf, len, dynamic_cast<CAddress *>(&dest_addr));
        }

        iter++;
    }

    return 0;
}

