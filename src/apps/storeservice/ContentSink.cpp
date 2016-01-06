/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ContentSink.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月05日 10时42分54秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <string.h>
#include <unistd.h>

#include "ContentSink.h"
#include "UnixAddress.h"
#include "StoreApp.h"
#include "Assure.h"

#include "../CommAddress.h"

using namespace HELIUM;
using namespace STORESERVICE;

const char* CContSink::m_UnixBindPath = STORE_SINK_ADDRESS;

CContSink::CContSink(CReactor *r) : m_Reactor(r)
{
    m_DataAcceptor = new CAcceptor<CContServ, CTcpSocket>(r);
}

CContSink::~CContSink()
{
    delete m_DataAcceptor;
}

int CContSink::init(void)
{
    m_DataAcceptor->setId("Content Sink Service");
    CUnixAddress listenAddr(m_UnixBindPath);
    ASSERT_EXIT(!listenAddr.bad());
    ASSERT_EXIT(m_DataAcceptor->open(listenAddr) == 0);

    return 0;
}

