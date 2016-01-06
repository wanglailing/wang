/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Cpu2ToJru.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月03日 09时03分53秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _CPU2_TO_JRU_H
#define _CPU2_TO_JRU_H

#include "Buffer.h"
#include "CommonData.h"
#include "DataNode.h"
#include "AtpNode.h"
#include "../DmsBusinessDat.h"

namespace COREBUSINESS
{
    /**
     * 300S的CPU2板发往JRU的数据
     */
    class CCpu2ToJru : public CDataNode
    {
        public:
#pragma pack(push, 1)
            typedef struct stSystemInfo
            {
                uint16_t cpu2ver;             /// CPU2版本号
                uint16_t tcrver;              /// TCR版本号
                uint16_t dmiver;              /// DMI版本号
            }SYSTEM_INFO;

            typedef struct stTcrInfo
            {
                uint32_t signal;                /// 低频灯码
                uint16_t carrierfreq;           /// 载频
                uint8_t state;                  /// TCR状态
                uint8_t reserved;               /// 预留
            }TCR_INFO;

            typedef struct stInitialParams
            {
                uint8_t traintype;              /// 列车类型
                uint8_t opitem;                 /// 操作端
                uint16_t trainlen;              /// 列车长度
                uint16_t wheel_diameter;        /// 车轮径
                uint8_t ctcslevel;              /// CTCS等级
                uint8_t ctrlmode;               /// 控车模式
                uint8_t maxspeed;               /// 列车构造速度
                uint8_t reserved;               /// 预留
            }INITIAL_PARAMS;

            typedef struct stBtmInfo
            {
                uint16_t balise_count;          /// 应答器个数
                uint16_t curr_tracuitlen;       /// 当前轨道电路长度
                uint16_t next_tracuitlen;       /// 下一轨道电路长度
                uint8_t curr_loadfreqswitch;    /// 当前轨道电路载频切换信息
                uint8_t next_loadfreqswitch;    /// 下一轨道电路载频切换信息
                uint16_t balise_status;         /// 应答器信息状态
                uint8_t balise_lostflag;        /// 应答器丢失标志
                uint8_t balise_lostnums[3];     /// 丢失应答器编号
                uint16_t runlen_inbgp;          /// 应答器组内行驶距离
            }BTM_INFO;

            typedef struct stRecordData
            {
                uint16_t cpu2status;            /// CPU2状态
                uint16_t cpu2failinfo;          /// CPU2故障信息
                uint32_t mileage;               /// 里程标
                uint8_t ctrlmode;               /// 控制模式
                uint8_t brake_cmd;              /// 制动指令
                uint16_t ctrl_speed;            /// 列车运行速度
                SPEED_INFO speed_info;          /// 速度相关信息
                uint8_t ssp;                    /// 静态速度限制
                uint8_t tsr;                    /// 临时限速
                uint8_t acceleration;           /// 加速度
                uint8_t reserved;               /// 预留
                uint32_t lma;                   /// 移动授权极限
                uint16_t eb_reason;             /// 紧急制动的原因
                uint16_t nb_reason;             /// B7N制动的原因
                uint32_t trainpos;              /// 列车位置
                uint8_t poscorrect_flag;        /// 位置校正标志
                uint8_t correctdir_flag;        /// 校正方向标志
                uint16_t poscorrect_info;       /// 位置校正信息
                uint16_t trainpos_property;     /// 列车位置属性
                DMI_INFO dminfo;                /// DMI信息
                TCR_INFO tcrinfo;               /// TCR信息
                INITIAL_PARAMS initparams;      /// 初始参数
                uint16_t tmminfo;               /// TMM信息
                BTM_INFO btminfo;               /// BTM信息
            }RECORD_DATA;

            /// 定义CPU2板的消息结构
            typedef struct stCpu2Board
            {
                uint8_t trainum[6];           /// 车次
                uint32_t driverid;            /// 司机号
                SYSTEM_INFO sysinfo;          /// 系统信息
                RECORD_DATA recdata;          /// 记录数据
            }CPU2_BOARD;

            typedef struct stBalisePacket
            {
                uint32_t reserved;            /// 预留
                uint8_t balise[104];          /// 应答器报文
            }BALISE_PACKET;

            typedef struct stCpu2Data
            {
                uint8_t seqnum;               /// 序号
                uint8_t reserved;             /// 预留
                DATE_TIME datetime;           /// 日期
                uint32_t distance;            /// 距离

                union {
                    CPU2_BOARD cpu2_board;
                    BALISE_PACKET balise;
                }payload;
            }CPU2_DATA;

            /// CPU2完整帧定义
            typedef struct stCpu2Frame
            {
                uint8_t head;                /// 帧头
                uint8_t flag;                /// 标志位
                CPU2_DATA data;              /// 数据负载
                uint16_t crc;                /// CRC16
                uint8_t tail;                /// 帧尾
            }CPU2_FRAME;

            /////////////////////////////////////////

            typedef struct stCpu2Info
            {
                uint16_t cpu2status;            /// CPU2状态
                uint16_t cpu2failinfo;          /// CPU2故障信息
                uint32_t mileage;               /// 里程标
                uint8_t ctrlmode;               /// 控制模式
                uint32_t lma;                   /// 移动授权极限
                uint16_t eb_reason;             /// 紧急制动的原因
                uint16_t nb_reason;             /// B7N制动的原因
                TCR_INFO tcrinfo;               /// TCR信息
                DMI_INFO dminfo;                /// DMI信息
                uint16_t trainpos_property;     /// 列车位置属性
                uint16_t tmminfo;               /// TMM信息
            }CPU2_INFO;
#pragma pack(pop)

            /// CPU2的帧标志
            typedef enum emFrameFlag
            {
                FRAME_CPU2   = 0x3C,
                FRAME_BALISE = 0x3D,
                FRAME_ERROR  = 0x3E
            }FRAME_FLAG;

        public:
            CCpu2ToJru(AtpRTInfo* rtinfo);
            ~CCpu2ToJru();

            int parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            uint16_t getRunSpeed(void) const { return m_RunSpeed; }

            void dump(void) {}

        private:
            CCpu2ToJru(const CCpu2ToJru&);
            CCpu2ToJru& operator=(const CCpu2ToJru&);

            void parseBaseFromCpu2(const CPU2_BOARD& cpu2);
            int checkCpu2Triggers(const CPU2_BOARD& cpu2, HELIUM::CBuffer* nodes_buf, uint8_t level);
            int parseBalise(const BALISE_PACKET& balise_packet, HELIUM::CBuffer* nodes_buf);

            unsigned char changeLevel(unsigned char level);
            LOWFREQ_CODER decodeLowFreq(uint32_t signal) const;
            CARRIER_FREQ decodeCarrierFreq(uint16_t freq_code) const;
            BRAKE_ORDER parseBrakeOrder(uint8_t brake_info) const;

        private:
            /// ATP实时信息
            AtpRTInfo* m_pAtpRealtimeInfo;

            /// 列车运行速度
            uint16_t m_RunSpeed;

            /// 应答器报文
            uint8_t m_BalisePacket[BALISE_LENGTH];

            /// CPU2触发信息
            CPU2_INFO m_Cpu2Info;
    };
}

#endif //_CPU2_TO_JRU_H
