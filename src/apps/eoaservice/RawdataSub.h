/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: RawdataSub.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月13日 11时22分04秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _RAWDATA_SUB_H
#define _RAWDATA_SUB_H

#include "CoredataSub.h"
#include "TransConfirm.h"
#include "Buffer.h"
#include "Packet.h"
#include "DataSubService.h"
#include "../DmsDatatype.h"
#include "../BusProtocol.h"

namespace EXTSERVICE
{
    class CTransmit;

    class CRawdataSub : public HELIUM::CDataSubscriber
    {
        public:
#pragma pack(push, 1)
            /**
             * @def
             * 数据存储头部
             *
             */
            typedef struct stStoreHeader
            {
                uint16_t flag;           /// 头部标志
                uint16_t len;            /// 数据长度
                uint16_t protover;       /// 协议版本
                uint16_t type;           /// 数据类型
                uint8_t  reserved;       /// 预留
                double   timestamp;      /// 时间戳
            }STORE_HEADER;

            /**
             * @def
             * 数据存储尾部
             *
             */
            typedef struct stStoreTail
            {
                uint16_t crc16;          /// CRC16校验和
                uint16_t flag;           /// 尾部标志
            }STORE_TAIL;

            /**
             * @def
             * 与EOAS主机通信协议头部
             *
             */
            typedef struct stCommHeader
            {
                uint32_t flag;           /// 头部标志
                uint16_t len;            /// 数据长度
                uint8_t  type;           /// 数据类型
                uint8_t  reserved;       /// 预留
            }COMM_HEADER;
#pragma pack(pop)

            /**
             * @def
             * 与EOAS主机通信数据类型
             *
             */
            typedef enum emCommDatType
            {
                EB            = 1,
                RAW_ATP       = 2,
                DCMS          = 3,
                ATP300SC2     = 4,
                CIR           = 5,
                ANALYZED_ATP  = 6,
                CIR_AUDIO     = 7,
                WTD           = 8,
                MIS           = 9,
                COMMAND       = 10,
                DEVSTATE      = 11,
                UPGRADE_BEGIN = 241,
                UPGRADING     = 242,
                GETCONFIG     = 244
            }COMMDAT_TYPE;

        public:
            CRawdataSub();
            ~CRawdataSub();

            /**
             * 处理数据交换模块的广播数据
             *
             * @param data    数据地址
             * @param length  数据长度
             *
             */
            void processData(const char* data, int length);

            int handleTimeout(HELIUM::TimerId tid);

            void setTransmiter(CTransmit* transmit);

            void setCoreSubscriber(CCoredataSub* coresub) { m_CoreSub = coresub; }

            /**
             * 开始升级
             *
             * @param desc    升级描述信息
             * @param length  描述信息长度
             *
             */
            void startUpgrade(const char* desc, int length);

            /**
             * 升级进行中
             *
             * @param numbytes 升级数据分片长度
             *
             */
            void upgrading(int numbytes);

            /**
             * 向数据交换模块提交数据
             *
             * @param type     数据类型
             * @param data     数据地址
             * @param length   数据长度
             * @param ack_flag 确认标志
             *
             */
            void commitData(COMMDAT_TYPE type, const char* data, int length, 
                    CTransConfirm::RESPONSE_TYPE ack_flag = CTransConfirm::NEEDACK);

            void setCmdSender(unsigned short sender_id) { m_CmdSenderId = sender_id; }

        private:
            CRawdataSub(const CRawdataSub&);
            CRawdataSub& operator=(const CRawdataSub&);

            /// 发送核心业务数据
            void sendCoreData(void);

            /// 发送待确认的数据帧
            void sendConfirmingFrame(void);

            /// 向数据交换模块反馈数据
            void responseData(uint32_t data_id, const char* data, int length);

            /// 包装数据存储协议
            void wrapStoreProto(HELIUM::CBuffer* buf, uint16_t data_type);

            /// 包装与EOAS主机通信协议
            void wrapCommProto(HELIUM::CBuffer* buf, uint8_t data_type);

            /// 解析EOAS业务数据
            void parseEoasData(const char* data, int length);

            /// 转发EOAS配置信息
            void dumpConfigInfo(const char* config, int length);

        private:
            /// EOAS通信板标识
            DataId m_EoasCommId;

            /// EOAS通信设备状态
            DeviceItem m_EoasCommItem;

            /// ATP数据标识
            DataId m_AtpId;

            /// 业务数据缓冲区
            HELIUM::CBuffer m_DataBuf;

            /// 封装好的反馈数据
            HELIUM::CExchangePacket m_RespData;

            /// 重传定时器标识
            HELIUM::TimerId m_ResendTid;

            /// 核心数据推送定时器标识
            HELIUM::TimerId m_CorePushTid;

            /// 数据转发服务
            CTransmit* m_Transmiter;

            /// 核心业务订阅服务
            CCoredataSub* m_CoreSub;

            /// 传输确认层
            CTransConfirm* m_ConfirmLayer;

            /// 固件数据总长度
            int m_FirmwareTotalBytes;

            /// 固件数据已接收长度
            int m_FirmwareRecvdBytes;

            /// 固件升级标志
            bool m_bUpgrading;

            /// 升级结束时间
            time_t m_UpgradeOverTime;

            unsigned short m_CmdSenderId;
    };
}

#endif //_RAWDATA_SUB_H

