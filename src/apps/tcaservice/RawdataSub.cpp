/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: RawdataSub.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月26日 13时51分16秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "RawdataSub.h"
#include "TcaservApp.h"

#include "CharStuff.h"
#include "DataCoder.h"
#include "MemDump.h"
#include "Logger.h"
#include "SlaveDevManager.h"

using namespace HELIUM;
using namespace EXTSERVICE;

CRawdataSub::CRawdataSub() : m_SceneData(CExchangePacket((uint8_t)PROG_TCASERV))
{
    m_TcasCommId = APPCONFIG->getSlaveDevType("COMMTCAS");
    m_GprsId     = APPCONFIG->getSlaveDevType("GPRS");
    m_SubSet[0]  = (unsigned char)m_TcasCommId;
    m_SubSet[1]  = (unsigned char)m_GprsId;


    m_GprsParser = new CGprsParser(APPCONFIG->isGprsStuffed());

    CTimeVal run_tv(4.0f);
    m_RunningTid = REACTOR->registerTimerHandler(this, run_tv, "Running Data Send Timer");

    CTimeVal push_tv(1.02f);
    m_BasePushTid = REACTOR->registerTimerHandler(this, push_tv, "Base Data Push Timer");
}

CRawdataSub::~CRawdataSub()
{
    delete m_GprsParser; m_GprsParser = NULL;
}

void CRawdataSub::processData(const char* data, int length)
{
    m_SceneData.reset();
    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = (data + offset + 4);
        int payload_len = subpack_hdr->len - 4;

        if ((uint8_t)(subpack_hdr->dataid) == (uint8_t)m_GprsId)
        {
            m_GprsParser->parseData(subpack_hdr->dataid, payload_ptr, payload_len, &m_SceneData);
        }
        else if (subpack_hdr->dataid == DMS_STATE)
        {
            HELIUM::SLAVEDEV_MGR->update(reinterpret_cast<const unsigned char *>(payload_ptr),
                    payload_len);

            m_GprsParser->updateGprsItem(HELIUM::SLAVEDEV_MGR->queryDeviceItem(m_GprsId));
            m_GprsParser->updateTcasCommItem(HELIUM::SLAVEDEV_MGR->queryDeviceItem(m_TcasCommId));
        }
        else if (subpack_hdr->dataid == m_TcasCommId)
        {
        }

        offset += subpack_hdr->len;
    }

    if (m_SceneData.readableBytes() > sizeof(CAPSUL_HEADER))
    {
        m_SceneData.finish();
        getStream().write((const char*)(m_SceneData.begin()), m_SceneData.readableBytes());
    }

    return;
}

int CRawdataSub::handleTimeout(HELIUM::TimerId tid)
{
    if (tid == m_BasePushTid)
    {
        pushBaseData();
    }
    else if (tid == m_RunningTid)
    {
        pushRunningData();
    }
    else if (tid == m_HeartbeatTid && m_HeartbeatInfo != NULL)
    {
        getStream().write((const char*)(m_HeartbeatInfo->begin()), m_HeartbeatInfo->readableBytes());
        return 0;
    }

    return 1;
}

void CRawdataSub::pushBaseData(void)
{
    CBuffer datbuf;
    BaseFrame base_frame;

    TcasFrame* tcas_frame = m_CoreSub->getTcasFrame(TO_DMI);
    memcpy(&base_frame, tcas_frame, sizeof(base_frame));
    base_frame.header.len = sizeof(base_frame);
    base_frame.header.seq = 0;

    datbuf.append(&base_frame, sizeof(base_frame));

    m_SceneData.reset();
    m_GprsParser->wrapDataToDMI(&m_SceneData, &datbuf);
    m_SceneData.finish();
    getStream().write((const char*)(m_SceneData.begin()), m_SceneData.readableBytes());
}

void CRawdataSub::pushRunningData(void)
{
    CBuffer packet;
    TcasFrame* tcas_frame = m_CoreSub->getTcasFrame(TO_SERVER);
    packet.append(tcas_frame, sizeof(TcasFrame));
    std::string* enc_str = CDataCoder::encodeData(&packet);
    enc_str->insert(0, CDataCoder::TcasTagHead, 3);
    CDataCoder::wrapData(*enc_str, 3, CDataCoder::TcasTagTail, CDataCoder::TcasTagCrc);

    /**
     * 由于编码字符串中没有边界字符和转义字符，故无需经过字符填充，
     * 只需在首尾添加边界字符即可
     */ 
    if (m_GprsParser->isDataStuffed() == true)
    {
        enc_str->insert(0, 1, CCharStuffer::Boundary);
        enc_str->push_back(CCharStuffer::Boundary);
    }

    delete enc_str;
}

