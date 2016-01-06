/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Packet.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月23日 14时46分12秒
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
#include <algorithm>

#include "Packet.h"
#include "DataCoder.h"

using namespace HELIUM;

const uint16_t CExchangePacket::m_PacketFlag = 0x7B7D;
const size_t CExchangePacket::m_initialSize = 64;

CExchangePacket::CExchangePacket(uint8_t sender) : m_Buffer(m_initialSize), m_writerIndex(0)
{
    initPacketHeader(sender);
}

CExchangePacket::CExchangePacket(uint8_t sender, uint16_t dataid, const char*content, size_t len) : 
    m_Buffer(m_initialSize), m_writerIndex(0)
{
    initPacketHeader(sender);

    append((uint8_t*)&dataid, sizeof(dataid));
    uint16_t subpack_len = sizeof(uint16_t) + sizeof(uint16_t) + len;
    append((uint8_t*)(&subpack_len), sizeof(subpack_len));
    append((uint8_t*)(content), len);

    finish();
}

void CExchangePacket::initPacketHeader(uint8_t sender)
{
    CAPSUL_HEADER header;
    header.hdrflag = m_PacketFlag;
    header.length = sizeof(CAPSUL_HEADER);
    header.seqnum = 0;
    header.senderid = sender;

    append((uint8_t*)(&header), sizeof(CAPSUL_HEADER));
}

void CExchangePacket::reset(void)
{
    m_writerIndex = sizeof(CAPSUL_HEADER);
    CAPSUL_HEADER* header = (CAPSUL_HEADER*)begin();
    header->seqnum++;
}

void CExchangePacket::finish(void)
{
    /// 更新报文总长度
    CAPSUL_HEADER* header = (CAPSUL_HEADER*)begin();
    header->length = m_writerIndex;

    /// 计算校验和
    uint32_t crc = CDataCoder::Crc32.calcLOPP(begin(), m_writerIndex, 0xFFFFFFFF);
    append((uint8_t*)(&crc), sizeof(crc));
}

const char* CExchangePacket::findFrame(const char* begin, const char* const end, uint16_t* frame_len)
{
    if (begin == NULL || end == NULL)
        return NULL;

    const char* flag = (const char*)&m_PacketFlag;
    while (begin < end)
    {
        const char* packid_pos = std::search(begin, end, flag, flag + sizeof(m_PacketFlag));
        if ( (int)sizeof(CAPSUL_HEADER) > (end - packid_pos) )    /// 报文过短
            break;

        CAPSUL_HEADER* header = (CAPSUL_HEADER*)packid_pos;
        if ( (int)(header->length + sizeof(uint32_t)) > (end - packid_pos) )    /// 报文位置错误
        {
            begin = packid_pos + 2;
            continue;
        }

        /// 验证校验和
        uint32_t crc = CDataCoder::Crc32.calcLOPP(packid_pos, header->length, 0xFFFFFFFF);
        uint32_t orignal_crc;
        memcpy(&orignal_crc, packid_pos + header->length, sizeof(uint32_t));
        if (crc == orignal_crc)    /// 找到一帧数据
        {
            *frame_len = header->length;
            return packid_pos;
        }
        else
        {
            begin = packid_pos + 2;
        }
    }

    return NULL;
}

void CExchangePacket::ensureWritableBytes(size_t len)
{
    if (writableBytes() < len)
    {
        makeSpace(len);
    }
    assert(writableBytes() >= len);
}

void CExchangePacket::append(const uint8_t* data, size_t len)
{
    ensureWritableBytes(len);
    std::copy(data, data + len, beginWrite());
    hasWritten(len);
}

void CExchangePacket::makeSpace(size_t len)
{
    if (writableBytes() < len)
    {
        m_Buffer.resize(m_writerIndex + len);
    }
}
