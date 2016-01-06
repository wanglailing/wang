/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GprsParser.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月31日 10时26分40秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "DataCoder.h"
#include "GprsParser.h"
#include "TransportApp.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

CGprsParser::CGprsParser(bool stuff_data) : CGprsPackage(stuff_data)
{
    memset(&m_GprsItem, 0, sizeof(DeviceItem));
    memset(&m_TcasCommItem, 0, sizeof(DeviceItem));
}

CGprsParser::~CGprsParser()
{
}

void CGprsParser::wrapDataToDMI(HELIUM::CExchangePacket* container, HELIUM::CBuffer* data_buf)
{
    /// 附加校验和
    uint32_t crc32_decimal = CDataCoder::Crc32.calcHOPP(data_buf->peek(), data_buf->readableBytes(), 0xFFFFFFFF);
    crc32_decimal = htonl(crc32_decimal);
    data_buf->append(&crc32_decimal, sizeof(crc32_decimal));

    pushPacket(container, DEV_UNICAST, m_TcasCommItem.devid.id, data_buf->peek(), data_buf->readableBytes());
}

void CGprsParser::transferCmd(HELIUM::CExchangePacket* hub, unsigned short source, const char* command, int cmdlen)
{
    SUBPACK_HEADER subpack_hdr;
    subpack_hdr.dataid = REMOTE_CMD;
    subpack_hdr.len = sizeof(SUBPACK_HEADER) + sizeof(source) + cmdlen;

    /**
     * 先尝试向数据服务模块转发此命令，如果找不到相应的数据服务，
     * 再转发给数据交换模块。
     *
     */
    if (DATA_SINK->forwardCommand((const uint8_t*)&subpack_hdr, source, command, cmdlen) < 0)
    {
        hub->append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));
        hub->append((uint8_t*)&source, sizeof(source));                  /// 数据源标识
        hub->append((uint8_t*)(command), cmdlen);
    }
}

void CGprsParser::procBusinessData(unsigned short source, const char *buf, int length, void* response)
{
    if (buf == NULL || length <= 0)
        return;

    CExchangePacket* resp_packs = (CExchangePacket*)response;

    int ctrlresp_len = 0;
    char ctrl_resp[256];
    memset(ctrl_resp, 0, sizeof(ctrl_resp));

    char *frame_begin = NULL;
    char *frame_end   = NULL;
    char *cmd_begin   = NULL;
    char *cmd_end     = NULL;
    char *tcas_begin  = NULL;
    char *tcas_crcpos = NULL;
    char *tcas_end    = NULL;

    int dat_len;

    const char *curr_ptr = buf;
    while ((curr_ptr - buf) < length)
    {
        if ( (tcas_begin  = strstr(curr_ptr, CDataCoder::TcasTagHead)) != NULL &&
             (tcas_crcpos = strstr(tcas_begin + 3, CDataCoder::TcasTagCrc)) != NULL &&
             (tcas_end    = strstr(tcas_crcpos + 11, CDataCoder::TcasTagTail)) != NULL )
        {
            std::string crc32;
            crc32.append(tcas_crcpos + 3, 8);

            dat_len = tcas_crcpos - tcas_begin - 3;
            std::string calc_crc32 = CDataCoder::calcCRC32(tcas_begin + 3, dat_len);
            if (crc32 == calc_crc32)
            {
                /// 解码场景数据
                CBuffer decode_buf;
                std::string msg;
                msg.append(tcas_begin + 3, dat_len);
                CDataCoder::decodeData(&decode_buf, &msg);

                wrapDataToDMI(resp_packs, &decode_buf);
            }
            else
            {
                DBG_LOG((DATAWATCHER, "<tcas>raw_crc=0x%s, calc_crc=0x%s\n", crc32.c_str(), calc_crc32.c_str()));
            }

            frame_end = NULL;
            cmd_end = NULL;
        }
        else if ( (frame_begin = strstr(curr_ptr, CDataCoder::DmsTagHead)) != NULL &&
             (frame_end   = strstr(frame_begin + 3, CDataCoder::DmsTagTail)) != NULL )
        {
            dat_len = frame_end - frame_begin - 3;
            if (dat_len > 0)
            {
                std::string msg;
                msg.append(frame_begin + 3, dat_len);

                /// 只保留最新的传输控制反馈信息
                int resp_len = RELIABLE_CTRL->procReceivedPacket(msg, ctrl_resp + 1, sizeof(ctrl_resp));
                if (resp_len > 0)
                {
                    ctrlresp_len = resp_len;
                }
            }

            tcas_end = NULL;
            cmd_end = NULL;
        }
        else if ( (cmd_begin = strstr(curr_ptr, "**")) != NULL &&
                  (cmd_end   = strstr(cmd_begin + 2, "##")) != NULL )
        {
            int cmd_len = cmd_end - cmd_begin + 2;
            transferCmd(resp_packs, source, cmd_begin, cmd_len);

            tcas_end = NULL;
            frame_end = NULL;
        }

        if (tcas_end != NULL)
            curr_ptr = tcas_end + 3;
        else if (frame_end != NULL)
            curr_ptr = frame_end + 3;
        else if (cmd_end != NULL)
            curr_ptr = cmd_end + 2;
        else
            break;
    }

    if (ctrlresp_len > 0)
    {
        int act_len;
        char *final_buf = stuffData(ctrl_resp, ctrlresp_len + 1, &act_len);
        pushPacket(resp_packs, DEV_MULTICAST, m_GprsItem.devid.id, final_buf, act_len);
    }
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

