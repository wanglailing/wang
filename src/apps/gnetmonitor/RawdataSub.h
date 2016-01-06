/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: RawdataSub.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月10日 13时55分04秒
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

#include "Buffer.h"
#include "Packet.h"
#include "DataSubService.h"
#include "GNetScanner.h"
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

        private:
            CRawdataSub(const CRawdataSub&);
            CRawdataSub& operator=(const CRawdataSub&);

        private:
            /// G网检测模块的数据类型
            uint8_t m_GNetDatatype;

            CGNetScan m_GNetProbe;
    };
}

#endif //_RAWDATA_SUB_H

