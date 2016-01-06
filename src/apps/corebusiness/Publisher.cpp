/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Publisher.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月11日 09时04分40秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Publisher.h"
#include "UnixAddress.h"
#include "CoreApp.h"
#include "Assure.h"

#include "../CommAddress.h"

using namespace HELIUM;
using namespace COREBUSINESS;

const char* CPublisher::m_UnixBindPath = DMSPUB_ADDRESS;

CPublisher::CPublisher(CReactor *r) : m_Reactor(r)
{
    m_SubAcceptor = new CAcceptor<CPubService, CTcpSocket>(r);
}

CPublisher::~CPublisher()
{
    delete m_SubAcceptor;
}

int CPublisher::init(void)
{
    m_SubAcceptor->setId("Core Data Publish Service");
    CUnixAddress listenAddr(m_UnixBindPath);
    ASSERT_EXIT(!listenAddr.bad());
    ASSERT_EXIT(m_SubAcceptor->open(listenAddr) == 0);

    return 0;
}

int CPublisher::broadcastData(const char *buf, int len)
{
    if (SERV_REPO->empty() == true || buf == NULL || len < 4)
    {
        return -1;
    }

    CRepository<CPubService>::const_iterator iter = SERV_REPO->begin();
    while (iter != SERV_REPO->end())
    {
        if (*iter)
        {
            (*iter)->getStream().write(buf, len);
        }

        iter++;
    }

    return 0;
}

