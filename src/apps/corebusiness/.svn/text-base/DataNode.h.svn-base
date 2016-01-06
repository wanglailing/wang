/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataNode.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月27日 15时10分45秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DATA_NODE_H
#define _DATA_NODE_H

#include <stdint.h>
#include <time.h>

#include "PacketManager.h"
#include "Logger.h"
#include "Buffer.h"

#include "../DmsBusinessDat.h"

namespace COREBUSINESS
{
    /**
     * 各种数据的抽象, 包含数据的时序，以及解析接口。
     *
     */
    class CDataNode
    {
        public:
            explicit CDataNode(int interval) : m_TimeoutInterval(interval), m_RecvCount(0), m_RecvTimeout(true),
                                               m_Status((uint8_t)STATUS_TIMEOUT), m_RecvTimestamp(0)
            {
            }

            virtual ~CDataNode() {}

        public:
            /**
             * @brief 获取当前时刻的数据状态
             *
             * @param now 当前时刻(毫秒)
             *
             * @return 数据状态
             */
            virtual unsigned int getState(uint64_t now) {
                /// 更新数据接收超时标志
                if (m_RecvTimeout == false)
                {
                    m_RecvTimeout = recvTimeout(now);
                }

                if (m_RecvTimeout == true)
                {
                    m_Status = (uint8_t)STATUS_TIMEOUT;
                    resetDataInfo();
                }

                return m_Status;
            }

            /**
             * @brief 数据接收超时
             *
             * @param now 当前时刻(毫秒)
             *
             * @return true -- 超时; false -- 未超时。
             */
            bool recvTimeout(uint64_t now) const {
                if (m_RecvTimestamp > 0 && (int)(now - m_RecvTimestamp) > m_TimeoutInterval)
                    return true;

                return false;
            }

            bool isRecvTimeout(void) const {
                return m_RecvTimeout;
            }

            /**
             * @brief 解析数据
             *
             * @param buf       缓冲区地址
             * @param len       数据长度
             * @param nodes_buf 存储解析结果
             * @param now       当前时刻(毫秒)
             *
             * @return 解析结果的长度
             */
            virtual int parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now) = 0;

            /// 打印数据信息(调试用)
            virtual void dump(void) = 0;

            /**
             * @brief 获取解析后的信息(由子类实现)
             *
             * @param [out] length 信息长度
             *
             * @return 信息地址
             */
            virtual unsigned char* getParsedInfo(int* length) { (void)length; return NULL; }

            /// 获取数据接收时间
            uint64_t getRecvTimestamp(void) { return m_RecvTimestamp; }

            /**
             * @brief 清理解析后的信息(由子类实现)
             *
             */
            virtual void clearParsedInfo(void) {}

            /**
             * @brief 检查数据时效性(更新数据接收时间戳)
             *
             * @param now 当前时刻(毫秒)
             *
             * @return true -- 数据接收超时; false -- 数据接收正常
             */
            bool checkTimeliness(uint64_t now);

        protected:
            /**
             * @brief 复位数据信息
             *
             * @param reset_area 要复位的数据范围
             *
             */
            virtual void resetDataInfo(uint8_t reset_area = 0);

        protected:
            /// 接收超时间隔
            int m_TimeoutInterval;

            /// 接收计数
            int m_RecvCount;

            /// 接收接收超时标志
            bool m_RecvTimeout;

            /// 数据状态
            uint8_t m_Status;

            /// 接收时间戳(毫秒)
            uint64_t m_RecvTimestamp;

        private:
            CDataNode(const CDataNode&);
            CDataNode& operator=(const CDataNode&);
    };

    inline bool CDataNode::checkTimeliness(uint64_t now)
    {
        m_RecvTimeout = recvTimeout(now);
        m_RecvTimestamp = now;

        return m_RecvTimeout;
    }

    inline void CDataNode::resetDataInfo(uint8_t reset_area)
    {
        (void)reset_area;
    }
}

#endif //_DATA_NODE_H
