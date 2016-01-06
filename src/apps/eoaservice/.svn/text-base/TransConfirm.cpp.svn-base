/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TransConfirm.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月13日 16时40分56秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <time.h>

#include "TransConfirm.h"
#include "Logger.h"

using namespace HELIUM;
using namespace EXTSERVICE;

unsigned int CTransConfirm::SendCaption = 10;
HELIUM::Crc<uint16_t> CTransConfirm::Crc16_CCITT(0x8408, 0x1021);

static uint32_t TagHead = 0xA5A5A5A5;
static uint32_t TagTail = 0x5A5A5A5A;

CTransConfirm::CTransConfirm() : m_CurrPackId(0), m_RecvPackId(0), m_RecvTimes(0)
{
    m_SmallestLen = sizeof(PROTO_HEADER) + sizeof(PROTO_TAIL);
    initSendList();
}

CTransConfirm::~CTransConfirm()
{
    destroySendList();
}

void CTransConfirm::capData(const char* data, int length, bool cache_flag)
{
    PACKET_ITEM* idle_item = findIdleSlot();
    idle_item->buf->append(data, length);

    unsigned int payload_len = idle_item->buf->readableBytes();
    if ((payload_len > 1200 && cache_flag == true) || cache_flag == false)
    {
        wrapConfirmProto(idle_item->buf, NEEDACK);
        idle_item->capover = 1;
    }
}

void CTransConfirm::wrapConfirmProto(HELIUM::CBuffer* buf, RESPONSE_TYPE ack_flag)
{
    PROTO_HEADER proto_hdr;
    proto_hdr.flag = TagHead;
    proto_hdr.len = buf->readableBytes() + m_SmallestLen;
    proto_hdr.needack = (uint8_t)ack_flag;

    if (ack_flag == NEEDACK)    /// 需要确认
        proto_hdr.seq = m_CurrPackId++;
    else
        proto_hdr.seq = 0;

    proto_hdr.direction = 0;

    /// 插入协议头部
    buf->prepend(&proto_hdr, sizeof(PROTO_HEADER));

    PROTO_TAIL proto_tail;

    /// 校验和的计算不包括协议头部标志
    proto_tail.crc16 = Crc16_CCITT.calcHOPP(buf->peek() + 4, 
            buf->readableBytes() - 4, 0);
    proto_tail.flag = TagTail;

    /// 追加协议尾部
    buf->append(&proto_tail, sizeof(PROTO_TAIL));
}

int CTransConfirm::getSendingFrame(const char** frame_buf)
{
    /// 报文最多发送3次
    if (m_SendingList.front().sendcount >= 3)
    {
        recycleFirstItem();
    }

    std::list<PACKET_ITEM>::iterator iter = m_SendingList.begin();
    CBuffer* data_buf = iter->buf;

    if (iter->capover == 1)
    {
        time_t now;
        time(&now);

        int time_diff = now - iter->timestamp;
        if (time_diff > 1)
        {
            iter->timestamp = now;
            iter->sendcount++;

            if (iter->sendcount > 1)
            {
                const PROTO_HEADER* proto_hdr = reinterpret_cast<const PROTO_HEADER*>(data_buf->peek());
                //DBG_LOG((DATAWATCHER, "<RESEND>seq[%u], count[%u]\n", proto_hdr->seq, iter->sendcount));
            }

            *frame_buf = data_buf->peek();
            return data_buf->readableBytes();
        }
    }

    *frame_buf = NULL;
    return 0;
}

int CTransConfirm::decapData(const char* data, int length, char* ack, int* ack_len)
{
    if (data == NULL || length < (int)m_SmallestLen)
        return -1;

    const PROTO_HEADER* proto_hdr = reinterpret_cast<const PROTO_HEADER*>(data);
    if (proto_hdr->flag != TagHead || proto_hdr->len != length)
        return -1;

    const PROTO_TAIL* proto_tail = reinterpret_cast<const PROTO_TAIL*>(data + length - sizeof(PROTO_TAIL));
    if (proto_tail->flag != TagTail)
        return -1;

    uint16_t crc = Crc16_CCITT.calcHOPP(data + sizeof(TagHead), 
            length - sizeof(TagHead) - sizeof(PROTO_TAIL), 0);
    if (crc != proto_tail->crc16)
        return -1;

    if (proto_hdr->direction != 1)
        return -1;

    if (length == (int)m_SmallestLen) /// 确认帧不带数据负载, 且不用回复
    {
        parseAck(proto_hdr->seq);
    }
    else if (proto_hdr->needack == 0)  /// 需要对业务数据进行接收确认
    {
        PROTO_HEADER ack_hdr;
        ack_hdr.flag = TagHead;
        ack_hdr.len = m_SmallestLen;
        ack_hdr.needack = 1;
        ack_hdr.seq = proto_hdr->seq;
        ack_hdr.direction = 0;

        PROTO_TAIL ack_tail;
        ack_tail.crc16 = Crc16_CCITT.calcHOPP((char*)&ack_hdr + 4, 
                sizeof(PROTO_HEADER) - 4, 0);
        ack_tail.flag = TagTail;

        memcpy(ack, &ack_hdr, sizeof(PROTO_HEADER));
        memcpy(ack + sizeof(PROTO_HEADER), &ack_tail, sizeof(PROTO_TAIL));
        *ack_len = m_SmallestLen;

        /// 统计同一包数据接收的次数
        if (m_RecvPackId != proto_hdr->seq)
        {
            m_RecvPackId = proto_hdr->seq;
            m_RecvTimes = 1;
        }
        else
        {
            m_RecvTimes++;
        }

        /// 对于接收到的重复报文不再做处理，但要发送确认帧
        if (m_RecvTimes > 1)
        {
            DBG_LOG((DATAWATCHER, "Packet<%u> received [%u]times\n", proto_hdr->seq, m_RecvTimes));
            return 1;
        }
    }

    return 0;
}

void CTransConfirm::parseAck(uint32_t ack_num)
{
    std::list<PACKET_ITEM>::const_iterator iter = m_SendingList.begin();
    CBuffer* data_buf = iter->buf;
    if (data_buf->readableBytes() < m_SmallestLen)
        return;

    const PROTO_HEADER* proto_hdr = reinterpret_cast<const PROTO_HEADER*>(data_buf->peek());
    if (proto_hdr->seq != ack_num)
        return;

    recycleFirstItem();
}

void CTransConfirm::recycleFirstItem(void)
{
    /// 回收内存
    PACKET_ITEM item = m_SendingList.front();
    item.timestamp = 0;
    item.capover = 0;
    item.sendcount = 0;
    item.buf->retrieveAll();
    m_SendingList.push_back(item);

    /// 删除表首元素
    m_SendingList.pop_front();
}

void CTransConfirm::initSendList(void)
{
    for (unsigned int i = 0; i < SendCaption; ++i)
    {
        PACKET_ITEM item;
        memset(&item, 0, sizeof(item));
        item.buf = new CBuffer();

        m_SendingList.push_back(item);
    }
}

void CTransConfirm::resetSendList(void)
{
    std::list<PACKET_ITEM>::iterator iter;
    for (iter = m_SendingList.begin(); iter != m_SendingList.end(); ++iter)
    {
        iter->capover = 0;
        iter->sendcount = 0;
        iter->buf->retrieveAll();
    }
}

void CTransConfirm::destroySendList(void)
{
    std::list<PACKET_ITEM>::iterator iter;
    for (iter = m_SendingList.begin(); iter != m_SendingList.end(); ++iter)
        delete iter->buf;

    m_SendingList.clear();
}

CTransConfirm::PACKET_ITEM* CTransConfirm::findIdleSlot(void)
{
    std::list<PACKET_ITEM>::iterator iter;
    for (iter = m_SendingList.begin(); iter != m_SendingList.end(); ++iter)
    {
        if (iter->capover == 0)
        {
            return &*iter;
        }
    }

    /// 如果没有找到空闲的表项，则重置整个列表，并返回表首元素
    resetSendList();
    iter = m_SendingList.begin();

    return &*iter;
}

