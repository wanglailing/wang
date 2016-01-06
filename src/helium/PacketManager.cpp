/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PacketManager.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月21日 13时13分50秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "PacketManager.h"

using namespace HELIUM;

CPacketManager::CPacketManager()
{
}

CPacketManager::~CPacketManager()
{
}

void CPacketManager::packData(CBuffer* container, uint8_t msgid, unsigned char* buf, int length, int packlen_size)
{
    if (!container || !buf || length <= 0)
        return;

    /// 将每个报文封装成统一的格式，不关心数据类型和子类型
    SUBPACK_HEADER subpack_hdr;
    memset(&subpack_hdr, 0, sizeof(subpack_hdr));
    subpack_hdr.len = sizeof(subpack_hdr) + sizeof(uint8_t) + length;
    if (packlen_size == 1)
        subpack_hdr.len += sizeof(uint8_t);
    else if (packlen_size == 2)
        subpack_hdr.len += sizeof(uint16_t);

    /// 包裹头部
    container->append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));

    /// 消息标识
    container->append(&msgid, sizeof(uint8_t));

    /// 消息长度
    if (packlen_size == 1)
        container->append((uint8_t*)&length, sizeof(uint8_t));
    else if (packlen_size == 2)
        container->append((uint16_t*)&length, sizeof(uint16_t));

    /// 消息内容
    container->append(buf, length);
}

