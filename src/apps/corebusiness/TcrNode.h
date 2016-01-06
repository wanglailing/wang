/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TcrNode.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月18日 08时46分48秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _TCR_NODE_H
#define _TCR_NODE_H

#include "DataNode.h"

namespace COREBUSINESS
{
    class CTcrNode : public CDataNode
    {
        public:
#pragma pack(push, 1)
            typedef struct stTcrProtoData
            {
                uint16_t carrfreq;            /// 载频(单位:0.1HZ)
                uint16_t lowfreq;             /// 低频(单位:0.01HZ)
                uint16_t uppersidefreq;       /// 上边频(单位:0.1HZ)
                uint16_t downsidefreq;        /// 下边频(单位:0.1HZ)
                uint16_t sigampling1;         /// 信号幅值1(时域)(单位:1mv)
                uint16_t sigampling2;         /// 信号幅值2(频域)(单位:1mv)
            }TCRPROTO_DATA;

            typedef struct stTcrInfo
            {
                uint8_t status;               /// TCR状态
                float   carrfreq;             /// 载频(单位:1HZ)
                float   lowfreq;              /// 低频(单位:1HZ)
                float   voltage;              /// 轨道电压(单位:0.02v)
            }TCR_INFO;
#pragma pack(pop)

        public:
            CTcrNode(int interval);
            ~CTcrNode();

            int parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            unsigned char getVoltage(uint64_t now)
            {
                if (recvTimeout(now) == false)
                {
                    return m_TrackVoltage;
                }

                return 0;
            }

            void dump(void);

            unsigned char* getParsedInfo(int* length);
            void clearParsedInfo(void);

        private:
            CTcrNode(const CTcrNode&);
            CTcrNode& operator=(const CTcrNode&);

            void encodeTcrInfo(float carrfreq, float lowfreq);

        private:
            TCR_INFO m_TcrInfo;

            /// 轨道电压
            volatile unsigned char m_TrackVoltage;
    };
}

#endif //_TCR_NODE_H
