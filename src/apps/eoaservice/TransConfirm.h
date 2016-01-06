/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TransConfirm.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月13日 16时05分00秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _TRANSPORT_CONFIRM_H
#define _TRANSPORT_CONFIRM_H

#include <stdint.h>
#include <list>

#include "Buffer.h"
#include "CRC.h"

namespace EXTSERVICE
{
    /**
     * 传输确认控制
     *
     */
    class CTransConfirm
    {
        public:
#pragma pack(push, 1)
            /**
             * @def
             * 传输确认协议头部
             *
             */
            typedef struct stConfrimProtoHeader
            {
                uint32_t flag;           /// 头部标志
                uint16_t len;            /// 数据长度
                uint8_t  needack;        /// 数据确认标志(0 -- 需要确认；1 -- 无需确认)
                uint32_t seq;            /// 数据编号
                uint8_t  direction;      /// 传输方向
            }PROTO_HEADER;

            /**
             * @def
             * 传输确认协议尾部
             *
             */
            typedef struct stConfrimProtoTail
            {
                uint16_t crc16;          /// CRC16校验和
                uint32_t flag;           /// 尾部标志
            }PROTO_TAIL;
#pragma pack(pop)

            typedef enum emResponseType
            {
                NEEDACK = 0,
                NOACK   = 1
            }RESPONSE_TYPE;

            typedef struct stPacketItem
            {
                uint32_t timestamp;      /// 首次发送时间戳
                uint8_t  capover;        /// 数据封装完成
                uint8_t  sendcount;      /// 发送次数
                HELIUM::CBuffer* buf;    /// 报文缓冲区
            }PACKET_ITEM;

        public:
            CTransConfirm();
            ~CTransConfirm();

            /**
             * 将业务数据封装后放入缓冲区
             *
             * @param data       数据地址
             * @param length     数据长度
             * @param cache_flag 数据缓存标志
             *
             */
            void capData(const char* data, int length, bool cache_flag = true);

            /**
             * 包装传输确认协议
             *
             * @param buf 缓冲区地址
             * @param ack 是否需要确认
             *
             */
            void wrapConfirmProto(HELIUM::CBuffer* buf, RESPONSE_TYPE ack_flag);

            /**
             * 获取待发送的数据帧
             *
             * @param frame_buf 数据地址
             *
             */
            int getSendingFrame(const char** frame_buf);

            /**
             * 解封传输确认数据帧
             *
             * @param data    数据地址
             * @param length  数据长度
             * @param ack     确认帧
             * @param ack_len 确认帧长度
             *
             * @return 0 -- 解封成功, 报文可用; 
             *         1 -- 解封成功, 但报文重复; 
             *        -1 -- 解封失败
             */
            int decapData(const char* data, int length, char* ack, int* ack_len);

            /// 获取最短数据帧的长度
            const unsigned int smallestFrameSize(void) const { return m_SmallestLen; }

            /// 重设发送列表
            void resetSendList(void);

            /// 16位校验和
            static HELIUM::Crc<uint16_t> Crc16_CCITT;

        private:
            CTransConfirm(const CTransConfirm&);
            CTransConfirm& operator=(const CTransConfirm&);

            /**
             * 根据确认帧编号删除已送达的报文, 并回收内存
             *
             */ 
            void parseAck(uint32_t ack_num);

            /// 回收表首元素
            void recycleFirstItem(void);

            /**
             * 初始化发送列表。
             * 为了避免频繁的内存申请与释放，将列表的长度限制为一个定值，
             * 并一次申请好所有的内存。
             *
             */ 
            void initSendList(void);

            /// 销毁发送列表
            void destroySendList(void);

            /**
             * 找到一个空闲的表项,用于放置业务数据
             *
             * @return 空闲表项的地址
             */ 
            PACKET_ITEM* findIdleSlot(void);

        private:
            /// 发送列表
            std::list<PACKET_ITEM> m_SendingList;

            /// 当前发送报文标识
            unsigned int m_CurrPackId;

            /// 当前收到的报文标识
            unsigned int m_RecvPackId;

            /// 当前报文收到的次数
            unsigned char m_RecvTimes;

            /// 数据包的最小长度
            unsigned int m_SmallestLen;

            /// 发送列表的容量
            static unsigned int SendCaption;
    };
}

#endif //_TRANSPORT_CONFIRM_H
