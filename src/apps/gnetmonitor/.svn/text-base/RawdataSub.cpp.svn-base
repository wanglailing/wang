/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: RawdataSub.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月10日 14时01分16秒
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
#include "GNetMonApp.h"

#include "MemDump.h"
#include "Logger.h"
#include "SlaveDevManager.h"

using namespace HELIUM;
using namespace EXTSERVICE;

CRawdataSub::CRawdataSub()
{
    uint16_t module_a = APPCONFIG->getSlaveDevType("GNETMONA");
    uint16_t module_b = APPCONFIG->getSlaveDevType("GNETMONB");

    m_GNetDatatype = (uint8_t)module_a;
    m_SubSet[0] = m_GNetDatatype;
    m_GNetProbe.setModulesId(module_a, module_b);
}

CRawdataSub::~CRawdataSub()
{
}

void CRawdataSub::processData(const char* data, int length)
{
    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const char* payload_ptr = (data + offset + 4);
        int payload_len = subpack_hdr->len - 4;

        if ((uint8_t)subpack_hdr->dataid == m_GNetDatatype)
        {
            m_GNetProbe.parseFrame(subpack_hdr->dataid, payload_ptr, payload_len);
        }
        else if (subpack_hdr->dataid == DMS_STATE)
        {
            HELIUM::SLAVEDEV_MGR->update(reinterpret_cast<const unsigned char *>(payload_ptr),
                    payload_len);
        }

        offset += subpack_hdr->len;
    }

    return;
}

