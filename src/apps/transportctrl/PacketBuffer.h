/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PacketBuffer.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月14日 11时32分30秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _PACKET_BUFFER_H
#define _PACKET_BUFFER_H

#include <string>
#include <map>

namespace TRANSPORTCTRL
{
    /**
     * 报文缓冲区。
     *
     */
    class CPacketBuff
    {
        public:
            typedef enum emAckType
            {
                NAK = 0,              /**< 未确认 */
                ACK                   /**< 已确认 */
            }ACK_TYPE;

            typedef struct stPackItem
            {
                ACK_TYPE acked;        /// 报文确认标志
                std::string* data;     /// 报文地址

                stPackItem(ACK_TYPE ack_type, std::string* pack)
                {
                    acked = ack_type;
                    data = pack;
                }
            }PACK_ITEM;

            typedef std::map<unsigned int, PACK_ITEM> ITEM_MAPPER;

        public:
            CPacketBuff();
            ~CPacketBuff();

        public:
            /**
             * 将报文添加到缓冲区
             *
             * @param id       报文ID
             * @param ack_type 报文确认标志
             * @param data     报文
             *
             * @return 报文ID
             */ 
            unsigned int addToBuf(unsigned int id, ACK_TYPE ack_type, std::string* data);

            /**
             * 从缓冲区中读取报文
             *
             * @param id 报文ID
             *
             * @return 目标报文。如果不存在，则返回空字符串
             */ 
            std::string* getItemFromBuf(unsigned int id);

            /**
             * 删除已确认报文
             *
             * @param id 报文ID
             *
             * @return 0 -- 成功；-1 -- 失败
             */ 
            int removeItem(unsigned int id);

            /**
             * 删除给定范围内已确认的报文
             *
             * @param begin_id 起始范围
             * @param end_id   结束范围
             *
             * @return 0 -- 成功；-1 -- 失败
             */ 
            int removeItems(unsigned int begin_id, unsigned int end_id);

            /// 将报文状态改为未确认
            void reset(void);

            /// 清空报文缓冲区
            void clear(void);

            /// 获取第一包未确认数据的标识
            unsigned int getFirstNakID(void);

            /// 获取下一包未确认数据的标识
            unsigned int getNextNakID(unsigned int current_id);

            /// 获取最后一包数据的标识
            unsigned int getLastID(void);

            /// 获取已确认的报文标识
            std::string getAckIDs(unsigned int need_id);

            /// 将报文设为已确认
            void setToAcked(unsigned int packet_id);

        private:
            /// 从头开始删除连续的, ID值<=last_id的已确认报文
            void removeContinuedAckItems(unsigned int last_id);

        private:
            ITEM_MAPPER* m_Container;
            unsigned int m_MaxID;
    };
}

#endif //_PACKET_BUFFER_H
