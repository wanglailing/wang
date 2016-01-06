/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataSink.cpp
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

#include "DataSink.h"
#include "UnixAddress.h"
#include "GprsPackage.h"
#include "TransportApp.h"
#include "Assure.h"

#include "../CommAddress.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

const unsigned int CDataSink::BufUpperbound  = 650;
const char* CDataSink::m_UnixBindPath = BUSINESS_SINK_ADDRESS;

CDataSink::CDataSink(CReactor *r) : m_Reactor(r), m_ExchData(CExchangePacket((uint8_t)PROG_TRANSCTRL)),
    m_PushStamp(0)
{
    m_DataAcceptor = new CAcceptor<CDataService, CTcpSocket>(r);

    ::memset(&m_BusinessHeader, 0, sizeof(m_BusinessHeader));
    ::memset(&m_BusinessHeader.operate, 0xFF, sizeof(TrainOper));
    m_BusinessHeader.gprshdr.dataid = GPRS_OPDATA;
    m_BusinessHeader.gprshdr.reserved = 0xFFFF;
    m_BusinessHeader.protover = 0x00010000;
    m_BusinessHeader.atpid = 0xFF;
    m_BusinessHeader.trainid = APPCONFIG->getTrainIDNum();

    m_TcasProc = new CTcasProc(m_BusinessHeader.trainid);
}

CDataSink::~CDataSink()
{
    delete m_DataAcceptor;
    delete m_TcasProc;
}

int CDataSink::init(void)
{
    m_DataAcceptor->setId("Data Sink Service");
    CUnixAddress listenAddr(m_UnixBindPath);
    ASSERT_EXIT(!listenAddr.bad());
    ASSERT_EXIT(m_DataAcceptor->open(listenAddr) == 0);

    return 0;
}

void CDataSink::combinePackets(void)
{
    if (m_BusinessDataBuf.readableBytes() > 0)
    {
        m_FinalBuf.append(m_BusinessDataBuf.peek(), m_BusinessDataBuf.readableBytes());
        m_BusinessDataBuf.retrieveAll();
    }
}

void CDataSink::pushPacket(void)
{
    m_FinalBuf.prepend(&m_BusinessHeader, sizeof(BUSINESS_HDR));
    RELIABLE_CTRL->procBusinessPacket(&m_FinalBuf);
    m_FinalBuf.retrieveAll();
    m_BusinessHeader.packcount = 0;
    m_PushStamp = CTimeVal::getMonotonicMs();
}

void CDataSink::forwardData(uint8_t dst_id, unsigned short data_id, const char* data, int length)
{
    CDataService* data_service = TRANS_APP->getServiceById(dst_id);
    if (data_service == NULL)
        return;

    SUBPACK_HEADER subpack_hdr;
    subpack_hdr.dataid = data_id;
    subpack_hdr.len = sizeof(subpack_hdr) + length;

    m_ExchData.reset();
    m_ExchData.append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));
    m_ExchData.append((uint8_t*)data, length);
    m_ExchData.finish();

    data_service->getStream().write((const char*)(m_ExchData.begin()), m_ExchData.readableBytes());
}

int CDataSink::forwardCommand(const uint8_t* psubhdr, unsigned short source, const char* command, int cmdlen)
{
    CDataService* data_service = TRANS_APP->getServiceByCmd(command, cmdlen);
    if (data_service == NULL)
        return -1;

    m_ExchData.reset();
    m_ExchData.append(psubhdr, sizeof(SUBPACK_HEADER));
    m_ExchData.append((uint8_t*)&source, sizeof(source));
    m_ExchData.append((uint8_t*)command, cmdlen);
    m_ExchData.finish();

    data_service->getStream().write((const char*)(m_ExchData.begin()), m_ExchData.readableBytes());
    return 0;
}

