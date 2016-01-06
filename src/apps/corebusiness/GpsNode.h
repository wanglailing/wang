/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GpsNode.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月18日 08时41分23秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _GPS_NODE_H
#define _GPS_NODE_H

#include "DataNode.h"
#include "../DmsBusinessDat.h"

namespace COREBUSINESS
{
    class CGpsNode : public CDataNode
    {
        public:
            CGpsNode(int interval);
            ~CGpsNode();

            int parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            void dump(void);

            unsigned char* getParsedInfo(int* length);

        private:
            CGpsNode(const CGpsNode&);
            CGpsNode& operator=(const CGpsNode&);

        private:
            GpsRTInfo m_GpsRealtimeInfo;

            /// 校时次数统计
            uint8_t m_AdjustCount;
    };
}

#endif //_GPS_NODE_H
