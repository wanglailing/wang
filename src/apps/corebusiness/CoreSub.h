/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CoreSub.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月24日 17时10分39秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _CORE_SUBSCRIBE_H
#define _CORE_SUBSCRIBE_H

#include <map>
#include <string>

#include "DataSubService.h"
#include "DataNode.h"
#include "StepRunner.h"
#include "Transmit.h"
#include "Buffer.h"
#include "Packet.h"
#include "../DmsDatatype.h"

namespace COREBUSINESS
{
    /**
     * 核心模块的DMS业务数据订阅
     *
     */
    class CCoreSub : public HELIUM::CDataSubscriber
    {
        public:
            CCoreSub();
            ~CCoreSub();

            int open(HELIUM::CReactor* r);

            int handleTimeout(HELIUM::TimerId tid);

            /**
             * 处理DMS业务数据
             *
             * @param data    数据地址
             * @param length  数据长度
             *
             */
            void processData(const char* data, int length);

            void setTransmiter(CTransmit* transmit) { m_Transmiter = transmit; }

        private:
            CCoreSub(const CCoreSub&);
            CCoreSub& operator=(const CCoreSub&);

            /// 发送保活数据
            void sendKeepaliveData(void);

            /// 管理GSMR网络
            void manageGsmrNet(void);

            /// 放置状态、时间等数据包
            void pushMiscData(HELIUM::CBuffer* pbuf, uint64_t mono_now, bool need_state);

            /// 收集当前时刻的数据状态
            void collectDataState(uint64_t now);

            void prepareCoreData(HELIUM::CExchangePacket* container);

            /**
             * 更新并广播共享数据
             *
             * @param has_transmited 转发给传输控制模块的标志(如果没有转发，则向其转发一份)
             */
            void pushShareData(bool has_transmited);

            /// 获取GPS信息
            unsigned char* getGpsInfo(int* length);

            uint8_t getTcasState(uint64_t now);

            uint8_t parseDeviceState(DataId dataid);

        private:
            /// 读取套接字数据缓冲区
            HELIUM::CBuffer m_InputBuffer;

            /// 存储解析过的数据节点
            HELIUM::CBuffer m_NodesBuffer;

            /// 与地面服务器之间的保活定时器
            HELIUM::TimerId m_KeepaliveTid;

            /// 保活间隔
            HELIUM::CTimeVal m_KeepaliveInterval;

            /// 定义数据节点和数据标识之间的映射
            typedef std::map<DataId, CDataNode*> DataNodeMap;

            /// 业务数据集
            DataNodeMap m_BusinessNodes;

            /// 数据状态
            DataStatus m_DataStatus;

            /// 封装过的核心业务数据
            HELIUM::CExchangePacket m_CoreData;

            /// 封装过的共享数据
            HELIUM::CExchangePacket m_SharedData;

            /// 数据转发服务
            CTransmit* m_Transmiter;

            /// 走行距离计算线程
            CStepRunner* m_StepRunner;

            /// 数据标识
            DataId m_CirId;
            DataId m_GpsId;
            DataId m_TcrId;
            DataId m_AtpId;
            DataId m_TcasCommId;
            DataId m_GprsId;
            DataId m_GsmrId;

            /// GPRS网络状态
            uint8_t m_GprsNetState;

            /// GSMR网络状态
            uint8_t m_GsmrNetState;

            /// 有效数据的发送时间戳(毫秒)
            uint64_t m_EffectiveStamp;

            /// 保活数据的发送时间戳(毫秒)
            uint64_t m_KeepaliveStamp;

            /// TCAS心跳时间戳
            uint64_t m_TcasHeartStamp;

            std::string m_AtpTypeStr;
    };
}

#endif //_CORE_SUBSCRIBE_H
