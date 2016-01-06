/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GprsParser.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月31日 10时20分37秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _GPRS_PARSER_H
#define _GPRS_PARSER_H

#include "Buffer.h"
#include "Packet.h"
#include "GprsPackage.h"

#include "../BusProtocol.h"

namespace TRANSPORTCTRL
{
    class CGprsParser : public HELIUM::CGprsPackage
    {
        public:
            explicit CGprsParser(bool stuff_data);
            ~CGprsParser();

            void procBusinessData(unsigned short source, const char *buf, int length, void* response);

            /// 封装发往DMI显示屏的数据包
            void wrapDataToDMI(HELIUM::CExchangePacket* container, HELIUM::CBuffer* data_buf);

            /**
             * 封装报文并放入缓冲区容器中
             *
             * @param container   缓冲区容器
             * @param data_type   数据类型
             * @param devid       设备标识符
             * @param buf         数据地址
             * @param payload_len 数据长度
             *
             */
            void pushPacket(HELIUM::CExchangePacket* container, unsigned short data_type, uint32_t devid, 
                    const char* buf, int payload_len);

            /// 更新GPRS设备信息
            void updateGprsItem(DeviceItem item) { m_GprsItem = item; }

            /// 更新TCAS通信板设备信息
            void updateTcasCommItem(DeviceItem item) { m_TcasCommItem = item; }

            uint32_t getGprsDevId(void) { return m_GprsItem.devid.id; }

        private:
            CGprsParser(const CGprsParser&);
            const CGprsParser& operator=(const CGprsParser&);

            /// 转发命令
            void transferCmd(HELIUM::CExchangePacket* hub, unsigned short source, const char* command, int cmdlen);

        private:
            /// GPRS设备状态
            DeviceItem m_GprsItem;

            /// TCAS通信设备状态
            DeviceItem m_TcasCommItem;
    };
}

#endif //_GPRS_PARSER_H
