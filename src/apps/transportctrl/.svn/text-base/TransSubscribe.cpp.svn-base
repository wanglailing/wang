/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TransSubscribe.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月31日 17时17分17秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "TransSubscribe.h"
#include "SlaveDevManager.h"
#include "MemDump.h"
#include "Logger.h"
#include "TransportApp.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

const int CTransSubscribe::BuffLength = 2048;

CTransSubscribe::CTransSubscribe() : m_TransData(CExchangePacket((uint8_t)PROG_TRANSCTRL))
{
    m_TcasCommId = APPCONFIG->getSlaveDevType("COMMTCAS");
    m_GprsId     = APPCONFIG->getSlaveDevType("GPRS");
    m_GsmrId     = APPCONFIG->getSlaveDevType("GSMR");

    m_SubSet[0]  = (unsigned char)m_GprsId;

    m_EncodedBuf = new char[BuffLength];
    m_GprsParser = new CGprsParser(APPCONFIG->isGprsStuffed());

    CTimeVal tv(4.0f);
    m_SendTid = REACTOR->registerTimerHandler(this, tv, "Reliable Sender");

    CTimeVal push_tv(1.0f);
    m_BasePushTid = REACTOR->registerTimerHandler(this, push_tv, "TCAS Base Sender");
}

CTransSubscribe::~CTransSubscribe()
{
    delete [] m_EncodedBuf; m_EncodedBuf = NULL;
    delete m_GprsParser; m_GprsParser = NULL;
}

void CTransSubscribe::processData(const char* data, int length)
{
    if (data == NULL || length <= 0)
        return;

    m_TransData.reset();
    bool recv_gprs = false;

    /// 报文中可能包含多个子包
    unsigned subhdr_len = sizeof(SUBPACK_HEADER);
    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = data + offset + subhdr_len;
        int payload_len = subpack_hdr->len - subhdr_len;

        /// 只处理GPRS/GSM-R数据
        if ((uint8_t)(subpack_hdr->dataid) == (uint8_t)m_GprsId)
        {
            recv_gprs = true;
            m_GprsParser->parseData(subpack_hdr->dataid, payload_ptr, payload_len, &m_TransData);
        }
        else if (subpack_hdr->dataid == DMS_STATE)
        {
            HELIUM::SLAVEDEV_MGR->update(reinterpret_cast<const unsigned char *>(payload_ptr),
                    payload_len);

            DeviceItem dev_item;
            dev_item = HELIUM::SLAVEDEV_MGR->queryDeviceItem(m_GprsId);
            if ((uint16_t)(dev_item.devid.id >> 16) != m_GprsId)
            {
                dev_item = HELIUM::SLAVEDEV_MGR->queryDeviceItem(m_GsmrId);
            }
            m_GprsParser->updateGprsItem(dev_item);

            m_GprsParser->updateTcasCommItem(HELIUM::SLAVEDEV_MGR->queryDeviceItem(m_TcasCommId));
        }

        offset += subpack_hdr->len;
    }

    /// 先处理反馈信息
    if (m_TransData.readableBytes() > sizeof(CAPSUL_HEADER))
    {
        m_TransData.finish();
        getStream().write((const char*)(m_TransData.begin()), m_TransData.readableBytes());
    }

    /// 再处理做为服务端时的已排序报文
    if (recv_gprs == true)
    {
        RELIABLE_CTRL->procSortedPackets();
    }
}

void CTransSubscribe::pushBaseData(void)
{
    CBuffer datbuf;
    BaseFrame base_frame;

    TcasFrame* tcas_frame = DATA_SINK->getTcaser()->getTcasFrame(TO_DMI);
    memcpy(&base_frame, tcas_frame, sizeof(base_frame));
    base_frame.header.len = sizeof(base_frame);
    base_frame.header.seq = 0;

    datbuf.append(&base_frame, sizeof(base_frame));

    m_TransData.reset();
    m_GprsParser->wrapDataToDMI(&m_TransData, &datbuf);
    m_TransData.finish();
    getStream().write((const char*)(m_TransData.begin()), m_TransData.readableBytes());
}

int CTransSubscribe::handleTimeout(HELIUM::TimerId tid)
{
    if (tid == m_BasePushTid)
    {
        pushBaseData();
    }
    else if (tid == m_SendTid)
    {
        memset(m_EncodedBuf, 0, BuffLength);

        /// 留出第一个字节，用于放置通道字符
        int len = RELIABLE_CTRL->getSendingPacket(m_EncodedBuf + 1, BuffLength);
        int tcas_len = DATA_SINK->getTcaser()->encodePacket(m_EncodedBuf + len + 1, BuffLength - len - 1);
        len += tcas_len;
        if (len <= 0)
            return 1;

        int act_len;
        char *final_buf = m_GprsParser->stuffData(m_EncodedBuf, len + 1, &act_len);
        m_TransData.reset();
        {
            m_GprsParser->pushPacket(&m_TransData, DEV_MULTICAST, m_GprsParser->getGprsDevId(), final_buf, act_len);
        }
        m_TransData.finish();

        getStream().write((const char*)(m_TransData.begin()), m_TransData.readableBytes());
    }
    else if (tid == m_HeartbeatTid && m_HeartbeatInfo != NULL)
    {
        getStream().write((const char*)(m_HeartbeatInfo->begin()), m_HeartbeatInfo->readableBytes());
        return 0;
    }

    return 1;
}

