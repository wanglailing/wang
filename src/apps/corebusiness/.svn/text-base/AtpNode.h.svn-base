/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: AtpNode.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月27日 15时56分47秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ATP_NODE_H
#define _ATP_NODE_H

#include <string>
#include <stdlib.h>
#include <string.h>

#include "DataNode.h"
#include "CommonData.h"
#include "Packet.h"
#include "TimeVal.h"
#include "../DmsBusinessDat.h"
#include "../DmsDatatype.h"

#define BALISE_LENGTH 104

namespace COREBUSINESS
{
    /**
     * ATP数据的抽象
     *
     */
    class CAtpNode : public CDataNode
    {
        public:
            CAtpNode(int interval);
            virtual ~CAtpNode() {}

            virtual const char* getVersion(void) { return m_VerInfo.c_str(); }

            /// 获取ATP运行速度
            unsigned short getRunSpeed(uint64_t now);

            /**
             * @brief 更新走行距离
             *
             * @param distance 自身计算的走行距离
             *
             */
            virtual void updateRunDistance(unsigned int distance);

            /**
             * @brief 更新车次
             *
             * @param trainum 车次信息
             * @param timestamp 车次信息的接收时间
             *
             */
            virtual void updateTrainum(const void* trainum, unsigned int timestamp);

            /// 更新数据状态
            void updateDataState(const void* pstate);

            /**
             * @brief 获取ATP实时信息
             *
             * @param [out] length ATP实时信息的长度
             *
             * @return ATP实时信息的地址
             *
             */
            virtual unsigned char* getRealtimeInfo(int* length);

            /// 将日期时间转换成整数
            static uint32_t convertDateTime(const uint32_t* date_time);
            static uint32_t convertDateTime(const DATE_TIME& datetime);

            void dump(void);

        protected:
            /**
             * @brief 检查基础信息是否发生变化
             *
             * @param more_changed 其它变化项是否触发
             * @param now          当前时刻
             *
             * @return true -- 基础信息发生变化; false -- 基础信息没有变化
             */
            bool opbaseInfoChanged(bool more_changed, uint64_t now);

            /// 将基础包和状态包放入缓冲区
            void pushBaseWithDataState(HELIUM::CBuffer* nodes_buf);

            /**
             * @brief 复位ATP中的基础信息
             *
             */
            virtual void resetDataInfo(uint8_t reset_area = 0);

        protected:
            /// 版本信息
            std::string m_VerInfo;

            /// 列车运行信息及ATP基础信息
            OperBasic m_OperateBase;

            /// ATP实时信息(用于判断数据变化)
            AtpRTInfo m_RealtimeInfo;

            /// 数据状态
            DataStatus m_DataStatus;

            /// 列车速度(包含运行速度、紧急制动和常用制动速度)
            volatile uint32_t m_SpeedInfo;

            /// 应答器报文
            uint8_t m_BaliseTelegram[BALISE_LENGTH];

            /// ATP类型标识
            uint8_t m_TypeId;

        private:
            /// 上次更新走行距离的时间戳
            HELIUM::CTimeVal m_LastUpdateStamp;

        private:
            /// 初始化车次、司机号和基础信息
            void initOpBaseInfo(OperBasic &opbase);

            /// 检查列车运行速度、常用制动速度和紧急制动速度是否发生变化
            bool speedInfoChanged(uint32_t infoa, uint32_t infob);

            /// 检查速度是否变化
            static bool isSpeedChanged(uint16_t speeda, uint16_t speedb);

            /// 去掉字符串中的空格
            void trimString(char* str, int strlen);

        private:
            CAtpNode(const CAtpNode&);
            CAtpNode& operator=(const CAtpNode&);
    };
}

#endif //_ATP_NODE_H
