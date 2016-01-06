/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: RawdataSub.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月26日 13时40分04秒
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
#include "Buffer.h"
#include "Packet.h"
#include "DataSubService.h"
#include "GprsParser.h"
#include "../DmsDatatype.h"
#include "../BusProtocol.h"

namespace EXTSERVICE
{
    class CRawdataSub : public HELIUM::CDataSubscriber
    {
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

            void setCoreSubscriber(CCoredataSub* coresub) { m_CoreSub = coresub; }

        private:
            CRawdataSub(const CRawdataSub&);
            CRawdataSub& operator=(const CRawdataSub&);

            void pushBaseData(void);

            void pushRunningData(void);

        private:
            /// TCAS通信数据标识
            DataId m_TcasCommId;

            /// GPRS数据标识
            DataId m_GprsId;

            /// 业务数据缓冲区
            HELIUM::CBuffer m_DataBuf;

            /// 封装好的预警场景数据
            HELIUM::CExchangePacket m_SceneData;

            /// GPRS数据解析
            CGprsParser *m_GprsParser;

            /// 列车运行数据发送定时器标识
            HELIUM::TimerId m_RunningTid;

            /// 基础数据推送定时器标识
            HELIUM::TimerId m_BasePushTid;

            /// 核心业务订阅服务
            CCoredataSub* m_CoreSub;
    };
}

#endif //_RAWDATA_SUB_H

