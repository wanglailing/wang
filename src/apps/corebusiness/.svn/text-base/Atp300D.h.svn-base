/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Atp300D.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月29日 10时43分58秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ATP_300D_H
#define _ATP_300D_H

#include "AtpNode.h"

namespace COREBUSINESS
{
    /**
     * 300D型ATP数据
     *
     */
    class CAtp300D : public CAtpNode
    {
        public:
#pragma pack(push, 1)
            typedef struct stTargetValues
            {
                uint16_t permit_speed;
                uint16_t target_speed;
                uint32_t target_dist;
            }TargetValues;
#pragma pack(pop)

            /// 报文标识
            typedef enum emNidPacket
            {
                NID_ETCSMODE     = 0x01,
                NID_CURRSPEED    = 0x02,
                NID_TARGETVALUES = 0x03,
                NID_BRAKEAPP     = 0x04,
                NID_ATPSYSWITCH  = 0x05,
                NID_ETCSOBUDIAG  = 0x06,
                NID_SHORTBALISE  = 0x07,
                NID_LONGBALISE   = 0x08,
                NID_DRIVERTEXT   = 0x09,
                NID_TCRSYSWITCH  = 0x0A,
                NID_PROCIMAGE    = 0x10
            }NID_PACKET;

            typedef int (*PARSE_FUNC)(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);
            typedef struct stPacketParser
            {
                NID_PACKET nid;
                PARSE_FUNC callback;
            }PACKET_PARSER;

        public:
            CAtp300D();
            ~CAtp300D();

        private:
            CAtp300D(const CAtp300D&);
            CAtp300D& operator=(const CAtp300D&);

            /// 解析ETCS模式
            static int parseETCSMode(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            /// 解析列车当前速度
            static int parseCurrSpeed(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            /// 解析目标信息
            static int parseTargetValues(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            /// 解析制动动作
            static int parseBrakeApp(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            /// 解析ATP系统切换
            static int parseATPSysSwitch(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            static int parseETCSObuDiagnosis(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            /// 解析短应答器报文
            static int parseShortBaliseTele(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            /// 解析长应答器报文
            static int parseLongBaliseTele(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            static int parseProcessImage(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            /// 解析司机文本信息
            static int parseDriverText(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            /// 解析TCR系统切换
            static int parseTCRSysSwitch(CAtp300D* obj, const unsigned char* data, int bitpos, int bitlen);

            /// 报文处理函数集
            static PACKET_PARSER PacketParsers[];
    };
}

#endif //_ATP_300D_H
