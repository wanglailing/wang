/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ConnService.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月22日 14时50分44秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <string.h>
#include <string>

#include "Packet.h"
#include "ConnService.h"
#include "MaintainApp.h"
#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace DMS;

ConnService::ConnService()
{
}

ConnService::~ConnService()
{
}

void ConnService::processData(const char* data, int length)
{
    if (data == NULL || length <= 0)
        return;

    /// 报文中可能包含多个子包
    unsigned subhdr_len = sizeof(SUBPACK_HEADER);
    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = data + offset + subhdr_len;
        int payload_len = subpack_hdr->len - subhdr_len;

        /// 从板设备状态
        if (subpack_hdr->dataid == DMS_STATE)
        {
            HELIUM::SLAVEDEV_MGR->update(reinterpret_cast<const unsigned char *>(payload_ptr),
                    payload_len);
        }
        else   /// 向维护软件转发从板数据
        {
            uint16_t serv_type = (uint16_t)DAT_EXCHANGE;
            DeviceItem dev_item = HELIUM::SLAVEDEV_MGR->queryDeviceItem(subpack_hdr->dataid);

            m_ExchangeBuf.retrieveAll();
            m_ExchangeBuf.append(&dev_item.devid.id, sizeof(uint32_t));
            m_ExchangeBuf.append(&serv_type, sizeof(uint16_t));
            m_ExchangeBuf.append(payload_ptr, payload_len);
            MAINTAIN_APP->getMaintainServ()->responseData(m_ExchangeBuf.peek(), m_ExchangeBuf.readableBytes());
        }

        offset += subpack_hdr->len;
    }

    return;
}

void ConnService::pushData(const char* data, int length)
{
    if (data == NULL || length <= 0)
        return;

    getStream().write(data, length);
}

