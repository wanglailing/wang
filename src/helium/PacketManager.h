/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PacketManager.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月21日 11时41分26秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef __PACKET_MANAGER_H
#define __PACKET_MANAGER_H

#include <stdint.h>

#include "Buffer.h"
#include "Packet.h"

namespace HELIUM 
{
    class CPacketManager
    {
        public:
            CPacketManager();
            ~CPacketManager();

            /**
             * 封装业务数据
             *
             * @param container       缓冲区容器
             * @param msgid           数据标识
             * @param buf             数据地址
             * @param length          数据长度
             * @param packlen_size    数据长度封装大小(1 -- 1个字节；2 -- 2个字节；其它 -- 不封装数据长度)
             *
             */
            static void packData(CBuffer* container, uint8_t msgid, unsigned char* buf, int length, int packlen_size = 0);

        private:
            CPacketManager(const CPacketManager&);
            CPacketManager& operator=(const CPacketManager&);
    };
}

#endif //__PACKET_MANAGER_H
