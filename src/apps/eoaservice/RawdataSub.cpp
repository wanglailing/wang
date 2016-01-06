/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: RawdataSub.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月13日 11时24分16秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Transmit.h"
#include "RawdataSub.h"
#include "EoaservApp.h"

#include "MemDump.h"
#include "Logger.h"
#include "SlaveDevManager.h"

using namespace HELIUM;
using namespace EXTSERVICE;

static uint32_t CommTagHead = 0xA0A0A0A0;
static uint16_t CommTagTail = 0x0A0A;

CRawdataSub::CRawdataSub() : m_RespData(CExchangePacket((uint8_t)PROG_EOASERV)),
    m_FirmwareTotalBytes(0), m_FirmwareRecvdBytes(0), m_bUpgrading(false),
    m_UpgradeOverTime(0), m_CmdSenderId(0)
{
    m_EoasCommId = APPCONFIG->getSlaveDevType("COMMEOAS");
    m_AtpId      = APPCONFIG->getSlaveDevType("ATP300T");

    memset(&m_EoasCommItem, 0, sizeof(m_EoasCommItem));

    m_SubSet[0]  = (unsigned char)m_EoasCommId;
    m_SubSet[1]  = (unsigned char)m_AtpId;

    m_ConfirmLayer = new CTransConfirm();

    CTimeVal resend_tv(1.02f);
    m_ResendTid = REACTOR->registerTimerHandler(this, resend_tv, "Resend Timer");

    CTimeVal push_tv(0.6f);
    m_CorePushTid = REACTOR->registerTimerHandler(this, push_tv, "Core Data Push Timer");
}

CRawdataSub::~CRawdataSub()
{
    delete m_ConfirmLayer; m_ConfirmLayer = NULL;
}

void CRawdataSub::setTransmiter(CTransmit* transmit)
{ 
    m_Transmiter = transmit; 
    m_Transmiter->setDataCommiter(this);
}

void CRawdataSub::processData(const char* data, int length)
{
    /**
     * 升级结束后，20分钟内不向EOAS发送业务数据
     *
     */
    if (m_bUpgrading == true && m_UpgradeOverTime != 0)
    {
        int time_elapsed = time(NULL) - m_UpgradeOverTime;
        if (time_elapsed >= 1200)
        {
            m_bUpgrading = false;
            m_UpgradeOverTime = 0;
        }
    }

    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = (data + offset + 4);
        int payload_len = subpack_hdr->len - 4;

        if ((uint8_t)(subpack_hdr->dataid) == (uint8_t)m_AtpId && m_bUpgrading == false)
        {
            m_DataBuf.retrieveAll();
            m_DataBuf.append(payload_ptr, payload_len);

            /// 封装数据存储协议
            wrapStoreProto(&m_DataBuf, 1);

            /// 封装EOAS通信协议
            wrapCommProto(&m_DataBuf, (uint8_t)RAW_ATP);

            /// 封装传输确认协议
            m_ConfirmLayer->capData(m_DataBuf.peek(), m_DataBuf.readableBytes());

            /// 发送传输确认帧
            sendConfirmingFrame();
        }
        else if (subpack_hdr->dataid == m_EoasCommId)
        {
            /// 解封传输确认协议
            char acked[64];
            int ack_len = 0;
            int decaped = m_ConfirmLayer->decapData(payload_ptr, payload_len, acked, &ack_len);
            if (ack_len > 0)
            {
                responseData(m_EoasCommItem.devid.id, acked, ack_len);
            }

            int confirm_len = m_ConfirmLayer->smallestFrameSize();
            if (decaped == 0 && payload_len > confirm_len)
            {
                /// 跳过外层的传输确认封装
                parseEoasData(payload_ptr + sizeof(CTransConfirm::PROTO_HEADER), 
                        payload_len - confirm_len);
            }
            else if (decaped < 0)
            {
                parseEoasData(payload_ptr, payload_len);
            }
        }
        else if (subpack_hdr->dataid == DMS_STATE)
        {
            HELIUM::SLAVEDEV_MGR->update(reinterpret_cast<const unsigned char *>(payload_ptr),
                    payload_len);

            m_EoasCommItem = HELIUM::SLAVEDEV_MGR->queryDeviceItem(m_EoasCommId);
        }

        offset += subpack_hdr->len;
    }

    return;
}

int CRawdataSub::handleTimeout(HELIUM::TimerId tid)
{
    if (tid == m_CorePushTid)
    {
        sendCoreData();
    }
    else if (tid == m_ResendTid)
    {
        sendConfirmingFrame();
    }
    else if (tid == m_HeartbeatTid && m_HeartbeatInfo != NULL)
    {
        getStream().write((const char*)(m_HeartbeatInfo->begin()), m_HeartbeatInfo->readableBytes());
        return 0;
    }

    return 1;
}

void CRawdataSub::sendCoreData(void)
{
    if (m_bUpgrading == true)
        return;

    CBuffer buf;
    PARSED_ATP* core_data = m_CoreSub->getParsedAtp();
    buf.append(core_data, sizeof(PARSED_ATP));

    wrapCommProto(&buf, (uint8_t)ANALYZED_ATP);
    m_ConfirmLayer->wrapConfirmProto(&buf, CTransConfirm::NOACK);

    responseData(m_EoasCommItem.devid.id, buf.peek(), buf.readableBytes());
}

void CRawdataSub::startUpgrade(const char* desc, int length)
{
    /**
     * 升级描述信息由两部分组成：
     * 文件名(64 bytes) +
     * 文件长度(4 bytes)
     *
     */
    if (length != 68)
    {
        DBG_LOG((DATAWATCHER, "The EOAS upgrade description info is error<%d>\n", length));
        return;
    }
    DBG_LOG((DATAWATCHER, "Begin to upgrade the EOAS firmware ...\n"));

    /// 传输确认层要清空发送列表
    m_ConfirmLayer->resetSendList();

    memcpy(&m_FirmwareTotalBytes, desc + 64, sizeof(m_FirmwareTotalBytes));
    m_FirmwareRecvdBytes = 0;
    m_bUpgrading = true;
    m_UpgradeOverTime = 0;
}

void CRawdataSub::upgrading(int numbytes)
{
    m_FirmwareRecvdBytes += numbytes;
    if (m_FirmwareRecvdBytes == m_FirmwareTotalBytes)
    {
        m_UpgradeOverTime = time(NULL);
        DBG_LOG((DATAWATCHER, "Upgrade the EOAS firmware over!\n"));
    }
}

void CRawdataSub::commitData(COMMDAT_TYPE type, const char* data, int length, CTransConfirm::RESPONSE_TYPE ack_flag)
{
    CBuffer buf;

    buf.append(data, length);
    wrapCommProto(&buf, (uint8_t)type);

    /// 无需确认的数据可以直接发出去
    if (ack_flag == CTransConfirm::NOACK)
    {
        m_ConfirmLayer->wrapConfirmProto(&buf, ack_flag);
        responseData(m_EoasCommItem.devid.id, buf.peek(), buf.readableBytes());
    }
    else  /// 待确认的数据进入发送队列(数据无需缓存)
    {
        m_ConfirmLayer->capData(buf.peek(), buf.readableBytes(), false);
        sendConfirmingFrame();
    }
}

void CRawdataSub::sendConfirmingFrame(void)
{
    const char* confirm_frame = NULL;
    int frmlen = m_ConfirmLayer->getSendingFrame(&confirm_frame);
    if (confirm_frame && frmlen > 0)
    {
        responseData(m_EoasCommItem.devid.id, confirm_frame, frmlen);
    }
}

void CRawdataSub::responseData(uint32_t data_id, const char* data, int length)
{
    if (data_id == 0)
        return;

    m_RespData.reset();
    {
        uint16_t serv_type = (uint16_t)DAT_EXCHANGE;
        SUBPACK_HEADER subpack_hdr;
        subpack_hdr.dataid = DEV_UNICAST;
        subpack_hdr.len = length + sizeof(SUBPACK_HEADER) + sizeof(uint32_t) + sizeof(uint16_t);

        m_RespData.append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));
        m_RespData.append((uint8_t*)&data_id, sizeof(uint32_t));
        m_RespData.append((uint8_t*)&serv_type, sizeof(uint16_t));
        m_RespData.append((uint8_t*)data, length);
    }
    m_RespData.finish();

    getStream().write((const char*)(m_RespData.begin()), m_RespData.readableBytes());
}

void CRawdataSub::wrapStoreProto(HELIUM::CBuffer* buf, uint16_t data_type)
{
    STORE_HEADER store_hdr;
    store_hdr.flag = 0xA0A0;
    store_hdr.len = buf->readableBytes() + sizeof(STORE_HEADER) + sizeof(STORE_TAIL) - 4;
    store_hdr.protover = 0x0001;
    store_hdr.type = data_type;
    store_hdr.reserved = 0xFF;
    store_hdr.timestamp = CTimeVal::localTDateTime();

    buf->prepend(&store_hdr, sizeof(STORE_HEADER));

    STORE_TAIL store_tail;

    /// 校验和的计算不包括存储协议头部标志
    store_tail.crc16 = CTransConfirm::Crc16_CCITT.calcHOPP(buf->peek() + 2, 
            buf->readableBytes() - 2, 0);
    store_tail.flag = 0x0A0A;

    /// 追加协议尾部
    buf->append(&store_tail, sizeof(STORE_TAIL));
}

void CRawdataSub::wrapCommProto(HELIUM::CBuffer* buf, uint8_t data_type)
{
    COMM_HEADER comm_hdr;
    comm_hdr.flag = CommTagHead;
    comm_hdr.len = buf->readableBytes();
    comm_hdr.type = data_type;
    comm_hdr.reserved = m_CoreSub->getAtpNum();
    
    buf->prepend(&comm_hdr, sizeof(COMM_HEADER));
    buf->append(&CommTagTail, sizeof(CommTagTail));
}

void CRawdataSub::parseEoasData(const char* data, int length)
{
    m_Transmiter->resetBuffer();
    int offset = 0;
    while (offset < length)
    {
        const COMM_HEADER* commhdr_ptr = reinterpret_cast<const COMM_HEADER*>(data + offset);
        if (commhdr_ptr->flag != CommTagHead)
        {
            DBG_LOG((DATAWATCHER, "head flag error:%08X, %08X\n", commhdr_ptr->flag, CommTagHead));
            break;
        }

        int frame_len = sizeof(COMM_HEADER) + sizeof(CommTagTail) + commhdr_ptr->len;
        if ((frame_len + offset) > length)
        {
            break;
        }

        const uint8_t* tail_ptr = reinterpret_cast<const uint8_t*>(data + offset + frame_len - sizeof(CommTagTail));
        uint16_t pack_tail = *tail_ptr + ((uint16_t)(*(tail_ptr + 1)) << 8);
        if (pack_tail != CommTagTail)
        {
            DBG_LOG((DATAWATCHER, "tail flag error:%04X, %04X\n", pack_tail, CommTagTail));
            break;
        }

        COMMDAT_TYPE dat_type = (COMMDAT_TYPE)(*(data + offset + 6));
        if (dat_type == GETCONFIG)  /// 将配置信息发往数据交换模块
        {
            dumpConfigInfo(data + offset + 7, commhdr_ptr->len + 2);
        }
        else
        {
            m_Transmiter->pushData(data + offset + 6, commhdr_ptr->len + 2);
        }

        offset += frame_len;
    }
    m_Transmiter->finishBuffer();

    m_Transmiter->sendPacket();
}

void CRawdataSub::dumpConfigInfo(const char* config, int length)
{
    if (length >= 1024)
        return;

    DeviceItem dev_item = SLAVEDEV_MGR->queryDeviceItem(m_CmdSenderId);

    /// 字符填充
    char stuff_buf[1024];
    memcpy(stuff_buf + 1, config, length);
    CGprsPackage gprs_pack(true);
    int act_len;
    char *final_buf = gprs_pack.stuffData(stuff_buf, length + 1, &act_len);

    responseData(dev_item.devid.id, final_buf, act_len);
}

