/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GprsParser.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月27日 13时26分40秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Logger.h"
#include "DataCoder.h"
#include "GprsParser.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace EXTSERVICE;

CGprsParser::CGprsParser(bool stuff_data) : CGprsPackage(stuff_data)
{
    memset(&m_GprsItem, 0, sizeof(DeviceItem));
    memset(&m_TcasCommItem, 0, sizeof(DeviceItem));
}

CGprsParser::~CGprsParser()
{
}

void CGprsParser::procBusinessData(unsigned short source, const char *buf, int length, void* response)
{
    (void)source;
    if (buf == NULL || length <= 0)
        return;

    CExchangePacket* scene_buf = (CExchangePacket*)response;

    char *frame_begin = NULL;
    char *crc_pos     = NULL;
    char *frame_end   = NULL;

    const char *curr_ptr = buf;
    while ((curr_ptr - buf) < length)
    {
        if ( (frame_begin = strstr(curr_ptr, CDataCoder::TcasTagHead)) != NULL &&
             (crc_pos     = strstr(frame_begin + 3, CDataCoder::TcasTagCrc)) != NULL &&
             (frame_end   = strstr(crc_pos + 11, CDataCoder::TcasTagTail)) != NULL )
        {
            std::string crc32;
            crc32.append(crc_pos + 3, 8);

            int dat_len = crc_pos - frame_begin - 3;
            std::string calc_crc32 = CDataCoder::calcCRC32(frame_begin + 3, dat_len);
            if (crc32 == calc_crc32)
            {
                /// 解码场景数据
                std::string msg;
                msg.append(frame_begin + 3, dat_len);
                m_DecodeBuf.retrieveAll();
                CDataCoder::decodeData(&m_DecodeBuf, &msg);

                wrapDataToDMI(scene_buf, &m_DecodeBuf);
            }
            else
            {
                DBG_LOG((DATAWATCHER, "<tcas>raw_crc=0x%s, calc_crc=0x%s\n", crc32.c_str(), calc_crc32.c_str()));
            }
        }

        if (frame_end != NULL)
            curr_ptr = frame_end + 3;
        else
            break;
    }
}

void CGprsParser::wrapDataToDMI(CExchangePacket* container, CBuffer* buf)
{
    uint32_t crc32 = CDataCoder::Crc32.calcHOPP(buf->peek(), buf->readableBytes(), 0xFFFFFFFF);
    crc32 = htonl(crc32);
    buf->append(&crc32, sizeof(crc32));

    pushPacket(container, DEV_UNICAST, m_TcasCommItem.devid.id, buf->peek(), buf->readableBytes());
}

void CGprsParser::pushPacket(CExchangePacket* container, unsigned short data_type, uint32_t devid, 
        const char* buf, int payload_len)
{
    if (devid == 0 || buf == NULL || payload_len <= 0)
        return;

    uint16_t serv_type = (uint16_t)DAT_EXCHANGE;
    SUBPACK_HEADER subpack_hdr;
    subpack_hdr.dataid = data_type;
    subpack_hdr.len = payload_len + sizeof(SUBPACK_HEADER) + sizeof(uint32_t) + sizeof(uint16_t);

    container->append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));         /// 子包头部
    container->append((uint8_t*)&devid, sizeof(uint32_t));                  /// 设备标识符
    container->append((uint8_t*)&serv_type, sizeof(uint16_t));              /// 服务类型
    container->append((uint8_t*)buf, payload_len);                          /// 数据负载
}

