/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ContentService.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月05日 11时06分36秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ContentService.h"
#include "DataRecorder.h"
#include "SharedBuffer.h"
#include "StoreApp.h"
#include "Logger.h"
#include "MemDump.h"
#include "../DmsDatatype.h"
#include "../DmsBusinessDat.h"

using namespace HELIUM;
using namespace STORESERVICE;

int CContServ::open(HELIUM::CReactor* r)
{
    r->registerIOHandler(this, getStream().getHandler(), READ_EVENT);
    SERV_REPO->push_back(this);
    return 0;
}

int CContServ::handleRead(int fd)
{
    if (getStream().getHandler() != fd)
        return -1;

    int save_error;
    int read_len = m_InputBuffer.readFd(fd, &save_error);
    if (read_len < 0)
    {
        ERR_LOG((SOCKET, "Socket read error.[%d]\n", read_len));
        if (save_error != EWOULDBLOCK)
        {
            return -1;
        }
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

void CContServ::parseFrame(const char* data, int length)
{
    if (data == NULL || length <= 0)
        return;

    CExchangePacket resp_info((uint8_t)PROG_STORESERV);
    resp_info.reset();

    /// 报文中可能包含多个子包
    unsigned subhdr_len = sizeof(SUBPACK_HEADER);
    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = data + offset + subhdr_len;
        int payload_len = subpack_hdr->len - subhdr_len;

        if (subpack_hdr->dataid == FILE_CREATE)
        {
            //int file_fd = DATA_REC->createFile(payload_ptr);
            int file_fd = -1;

            /// 将文件描述符反馈给客户端
            SUBPACK_HEADER hdr;
            hdr.dataid = FILE_CREATE;
            hdr.len = payload_len + sizeof(int) + sizeof(SUBPACK_HEADER);

            resp_info.append((uint8_t*)&hdr, sizeof(SUBPACK_HEADER));         /// 子包头部
            resp_info.append((uint8_t*)&file_fd, sizeof(int));                /// 文件描述符
            resp_info.append((uint8_t*)payload_ptr, payload_len);             /// 数据负载
        }
        else if (subpack_hdr->dataid == FILE_WRITE)
        {
#if 0
            int fd;
            char compress_flag;
            ::memcpy(&fd, payload_ptr, sizeof(fd));
            compress_flag = payload_ptr[sizeof(fd)];

            SHARED_BUFFER->pushData(fd, payload_ptr + 5, payload_len - 5, compress_flag);
#endif
        }

        offset += subpack_hdr->len;
    }

    if (resp_info.readableBytes() > sizeof(CAPSUL_HEADER))
    {
        resp_info.finish();
        getStream().write((const char*)(resp_info.begin()), resp_info.readableBytes());
    }
}

int CContServ::handleClose(int /*fd*/)
{
    SERV_REPO->erase(this);
    delete this;
    return 0;
}


