/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: RawdataSub.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月31日 17时02分39秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _RAWDATA_SUB_H
#define _RAWDATA_SUB_H

#include <stdint.h>

#include "Buffer.h"
#include "DataSubService.h"
#include "../DmsDatatype.h"

namespace STORESERVICE
{
    class CRawdataSub : public HELIUM::CDataSubscriber
    {
        public:
#pragma pack(push, 1)
            typedef struct stAtpHeader
            {
                uint8_t  timeflag;           /// 时间戳标志
                double   timestamp;          /// 时间戳
                uint8_t  gpsflag;            /// GPS信息标志
                uint32_t gpstatus;           /// GPS数据状态
                uint32_t gpspeed;            /// GPS速度(km/h)
            }AtpHeader;
#pragma pack(pop)

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

        private:
            CRawdataSub(const CRawdataSub&);
            CRawdataSub& operator=(const CRawdataSub&);

        private:
            /// 存储CIR数据
            int storeCirData(const char *buf, int len, time_t now);

            /// 存储G网检测数据
            int storeGNetData(const char *buf, int len, time_t now);

            /**
             * 存储ATP数据
             *
             * @param buf       ATP数据地址
             * @param len       ATP数据长度
             * @param clock_now 挂钟时间(用于写入时间戳)
             * @param nono_now  单调时间(用于计算GPS时效性)
             *
             */
            int storeAtpData(const char *buf, int len, time_t clock_now, uint64_t mono_now);

            /// 解析GPS数据
            int parseGpsData(const char *buf, int len, uint64_t now);

            /// 获取GPS状态
            uint32_t getGpsState(uint64_t now);

        private:
            /// 数据标识
            DataId m_CirId;
            DataId m_GpsId;
            DataId m_AtpId;
            DataId m_AtpC2Id;
            DataId m_GNetId;
            DataId m_BatteryId;

            AtpHeader m_AtpHeader;

            /// GPS数据接收时间
            uint64_t m_RecvGpsTimestamp;
    };
}

#endif //_RAWDATA_SUB_H
