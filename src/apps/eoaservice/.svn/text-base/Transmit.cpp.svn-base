/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Transmit.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月03日 10时31分15秒
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

#include "Transmit.h"
#include "RawdataSub.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace EXTSERVICE;

CTransmit::CTransmit() : m_EoasData(CExchangePacket((uint8_t)PROG_EOASERV)), m_DataCommiter(NULL)
{
    m_EoasDatMap.insert(std::make_pair(0x05, 0x02));    /// CIR检测数据
    m_EoasDatMap.insert(std::make_pair(0x08, 0x03));    /// WTD数据
    m_EoasDatMap.insert(std::make_pair(0x09, 0x04));    /// MIS数据
    m_EoasDatMap.insert(std::make_pair(0x0B, 0x07));    /// EOAS状态数据
}

CTransmit::~CTransmit()
{
}

int CTransmit::open(HELIUM::CReactor* r)
{
    CTimeVal regcmd_tv(1.0f);
    m_RegCmdTid = r->registerTimerHandler(this, regcmd_tv, "Register Command Timer");
    return 0;
}

void CTransmit::close(void)
{
}

int CTransmit::handleRead(int fd)
{
    int save_error;
    int read_len = m_InputBuffer.readFd(fd, &save_error);
    if (read_len < 0)
    {
        DBG_LOG((SOCKET, "Transmit read error!\n"));
        return -1;
    }
    else if (read_len == 0)
    {
        DBG_LOG((SOCKET, "Peer has dropped connection!\n"));
        return -1;
    }
    else
    {
        uint16_t frame_length;
        const char* begin_pos = m_InputBuffer.peek();
        const char* end_pos = begin_pos + m_InputBuffer.readableBytes();
        while (begin_pos < end_pos)
        {
            const char* frame_pos;
            if ((frame_pos = CExchangePacket::findFrame(begin_pos, end_pos, &frame_length)) == NULL)
                break;

            parseFrame(frame_pos, (int)frame_length);

            begin_pos = frame_pos + frame_length + sizeof(uint32_t);
            m_InputBuffer.retrieveUntil(begin_pos);
        }
    }

    return 0;
}

int CTransmit::handleTimeout(HELIUM::TimerId tid)
{
    if (tid == m_RegCmdTid)
    {
        const char* cmd_set = "**GETEOASVERSION##";
        int cmdset_len = strlen(cmd_set);

        m_EoasData.reset();
        {
            SUBPACK_HEADER subpack_hdr;
            subpack_hdr.dataid = REG_CMD;
            subpack_hdr.len = sizeof(subpack_hdr) + cmdset_len;

            m_EoasData.append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));
            m_EoasData.append((uint8_t*)cmd_set, cmdset_len);
        }
        m_EoasData.finish();

        getStream().write((const char*)(m_EoasData.begin()), m_EoasData.readableBytes());
        return 0;
    }

    return 1;
}

void CTransmit::procRemoteCmd(const char* cmd, int length)
{
    (void)length;

    unsigned short sender_id;
    memcpy(&sender_id, cmd, sizeof(sender_id));
    m_DataCommiter->setCmdSender(sender_id);

    if (strstr(cmd + sizeof(sender_id), "**GETEOASVERSION##") != NULL)
    {
        m_DataCommiter->commitData(CRawdataSub::GETCONFIG, NULL, 0, CTransConfirm::NOACK);
    }
}

void CTransmit::parseFrame(const char* data, int length)
{
    if (data == NULL || length <= 0 || m_DataCommiter == NULL)
        return;

    int offset = sizeof(CAPSUL_HEADER);
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = data + offset + 4;
        int payload_len = subpack_hdr->len - 4;

        switch (subpack_hdr->dataid)
        {
            case REMOTE_CMD:
                procRemoteCmd(payload_ptr, payload_len);
                break;
            case UPGRADE_DESC:
                m_DataCommiter->startUpgrade(payload_ptr, payload_len);
                m_DataCommiter->commitData(CRawdataSub::UPGRADE_BEGIN, payload_ptr, payload_len);
                break;
            case UPGRADE_CONTENT:
                m_DataCommiter->upgrading(payload_len);
                m_DataCommiter->commitData(CRawdataSub::UPGRADING, payload_ptr, payload_len);
                break;
            default:
                break;
        }

        offset += subpack_hdr->len;
    }
}

int CTransmit::sendPacket(void)
{
    unsigned int packet_len = m_EoasData.readableBytes();
    if (packet_len <= sizeof(CAPSUL_HEADER))
        return -1;

    return getStream().write((const char*)(m_EoasData.begin()), packet_len);
}

void CTransmit::pushData(const char* data, int length)
{
    if (data == NULL || length <= 2)
        return;

    unsigned char data_type = data[0];
    std::map<unsigned char, unsigned char>::const_iterator iter;
    iter = m_EoasDatMap.find(data_type);
    if (iter == m_EoasDatMap.end())
        return;

    SUBPACK_HEADER subpack_hdr;
    memset(&subpack_hdr, 0, sizeof(subpack_hdr));
    subpack_hdr.len = sizeof(subpack_hdr) + length + 2;
    m_EoasData.append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));

    uint8_t msgid = (uint8_t)DMSMSG_CUSTOM;
    m_EoasData.append(&msgid, sizeof(uint8_t));

    uint16_t payload_len = length - 1;
    m_EoasData.append((uint8_t*)&payload_len, sizeof(uint16_t));

    m_EoasData.append(&iter->second, sizeof(uint8_t));
    m_EoasData.append((uint8_t*)(data + 2), length - 2);
}

