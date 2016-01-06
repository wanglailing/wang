/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataSubService.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月24日 16时15分43秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DATA_SUBSERVICE_H
#define _DATA_SUBSERVICE_H

#include <string>

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Reactor.h"
#include "TimeVal.h"
#include "Packet.h"
#include "Buffer.h"

namespace HELIUM
{
    /**
     * 订阅DMS原始数据
     *
     */
    class CDataSubscriber : public CServiceHandler<CTcpSocket>
    {
        public:
            CDataSubscriber();
            virtual ~CDataSubscriber();

            virtual int open(CReactor* r);

            virtual int handleRead(int fd);
            virtual int handleClose(int fd);
            virtual int handleTimeout(TimerId tid);

            /**
             * 创建心跳报文
             *
             * @param senderid 发送者标识 
             * @param dataid   数据标识
             * @param content  数据内容
             * @param len      数据长度
             *
             */
            void creatHeartbeatPacket(uint8_t senderid, uint16_t dataid, const char*content, size_t len);

            /// 数据处理接口
            virtual void processData(const char* data, int length) = 0;

            /**
             * 获取订阅的数据集
             *
             * @param dst    存放数据集的缓冲区
             * @param buflen 缓冲区长度 
             *
             * @return 数据集的长度
             *
             */
            int getSubscribeSet(char* dst, int buflen);

        private:
            CDataSubscriber(const CDataSubscriber&);
            CDataSubscriber& operator=(const CDataSubscriber&);

        protected:
            CReactor* m_Reactor;

            /// 心跳定时器ID
            TimerId m_HeartbeatTid;

            /// 定时器名
            std::string m_TimerName;

            /// 心跳间隔
            CTimeVal m_HeartbeatInterval;

            /// 读取套接字数据缓冲区
            CBuffer m_InputBuffer;

            /// 心跳内容
            CExchangePacket* m_HeartbeatInfo;

            /// 订阅的数据集
            unsigned char m_SubSet[64];
    };
}

#endif //_DATA_SUBSERVICE_H
