/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TransSubscribe.h
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

#ifndef _TRANS_SUBSCRIBE_H
#define _TRANS_SUBSCRIBE_H

#include "DataSubService.h"
#include "Buffer.h"
#include "Packet.h"
#include "TimeVal.h"
#include "GprsParser.h"
#include "../DmsDatatype.h"

namespace TRANSPORTCTRL
{
    class CTransSubscribe : public HELIUM::CDataSubscriber
    {
        public:
            CTransSubscribe();
            ~CTransSubscribe();

            int handleTimeout(HELIUM::TimerId tid);

            /**
             * 处理数据交换模块的广播数据
             *
             * @param data    数据地址
             * @param length  数据长度
             *
             */
            void processData(const char* data, int length);

        private:
            CTransSubscribe(const CTransSubscribe&);
            CTransSubscribe& operator=(const CTransSubscribe&);

            void pushBaseData(void);

        private:
            /// 封装好的传输数据
            HELIUM::CExchangePacket m_TransData;

            /// 报文编码缓冲区
            char *m_EncodedBuf;

            /// GPRS数据解析
            CGprsParser *m_GprsParser;

            /// GPRS数据发送定时器ID
            HELIUM::TimerId m_SendTid;

            /// TCAS基础数据包发送定时器ID
            HELIUM::TimerId m_BasePushTid;

            /// GPRS数据标识
            DataId m_GprsId;

            /// GSMR数据标识
            DataId m_GsmrId;

            /// GPRS数据标识
            DataId m_TcasCommId;

            static const int BuffLength;
    };
}

#endif //_TRANS_SUBSCRIBE_H
