/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: Atp300H.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月29日 14时33分40秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ATP_300H_H
#define _ATP_300H_H

#include <vector>

#include "AtpNode.h"
#include "CommonData.h"

namespace COREBUSINESS
{
    class CAtp300H : public CAtpNode
    {
        public:
#pragma pack(push, 1)
            typedef struct stTrackCircuitInfo
            {
                uint16_t property;              /// 列车所在轨道电路的属性
                uint16_t balisepos_offset;      /// 应答器位置校正信息
                uint16_t tracuitpos_offset;     /// 轨道电路位置校正信息(200H无此信息)
                uint16_t d_intracuit;           /// 轨道电路内运行距离
                uint16_t curr_length;           /// 当前轨道电路长度
                uint16_t next_length;           /// 下一轨道电路长度

                uint8_t curr_loadfreqswitch;    /// 当前轨道电路载频切换信息

                uint8_t next_loadfreqswitch;    /// 下一轨道电路载频切换信息
                uint32_t trainpos;              /// 列车位置

                uint32_t lma;                   /// 移动授权极限
            }TRACUIT_INFO;

            /**
             * @def
             * 共通数据1
             *
             */
            typedef struct stCommonData1
            {
                uint8_t reserved1;              /// 备用
                uint8_t ctrlmode;               /// 控制模式
                uint32_t eopstate;              /// EOP状态
                uint8_t reserved30[30];         /// 备用
                uint8_t brake_cmd;              /// 制动命令
                uint8_t acceleration;           /// 加速度
                uint16_t ctrl_speed;            /// 在速度校验中使用的速度
                uint32_t reserved4;             /// 备用
                uint32_t cumulative_distance;   /// 从控制速度计算出的走行距离

                uint32_t fail_info1;            /// 故障信息1
                uint32_t eb_reason;             /// 紧急制动的原因
                uint32_t nb_reason;             /// B7N制动的原因

                uint16_t wheel_diameter;        /// 车轮径

                uint8_t  di_info[6];            /// DI信息

                uint32_t trainum;               /// 列车编号
                uint32_t driverid;              /// 司机号
                uint16_t trainlen;              /// 列车长度
                uint8_t priority;               /// 优先设定、等级设定
                uint8_t reserved2;              /// 备用
            }COMMON_DATA1;

            /**
             * @def
             * 共通数据2
             *
             */
            typedef struct stCommonData2
            {
                uint32_t fail_info2;            /// 故障信息2
                uint8_t reserved12[12];         /// 备用
                uint32_t internal_time;         /// 车载内部时间
                uint8_t reserved8[8];           /// 备用
            }COMMON_DATA2;

            /**
             * @def
             * CTCS2数据
             *
             */
            typedef struct stCTCS2Data
            {
                uint8_t ssp;
                uint8_t tsr;
                TRACUIT_INFO tracuit;           /// 轨道电路相关信息
                uint8_t curr_tracuitinfo;       /// 当前轨道电路信息
                uint8_t ctrlmode;               /// 控制模式
                SPEED_INFO speed_info;          /// 速度相关信息
                uint16_t reserved1;             /// 备用
                uint8_t tcr_signal[8];          /// TCR信号
                uint16_t ground_info;           /// 地面信息
                uint32_t balise_pos;            /// 应答器位置
                uint16_t reserved2;             /// 备用
            }CTCS2_DATA;

            /**
             * @def
             * CTCS3数据
             *
             */
            typedef struct stCTCS3Data
            {
                uint8_t ssp;
                uint8_t tsr;                    /// 临时限速
                uint16_t trainpos_offset;       /// 列车位置校正量
                uint16_t d_lrbg;
                uint8_t nid_lrbg[3];            /// LRBG标识
                uint8_t posreport;              /// 包含Q_DIRLRBG, Q_DLRBG, Q_DIRTRAIN
                uint16_t reserved2;             /// 备用
                uint32_t d_eoa;                 /// 从列车位置到EoA(行车许可终点)的距离
                SPEED_INFO speed_info;          /// 速度相关信息
                uint32_t mileage;               /// 里程
                uint8_t nid_radio[8];           /// 连接RBC的电话号码
                uint8_t nid_rbc[3];             /// RBC标识
                uint8_t ctrlmode;               /// 控制模式
                uint16_t l_doubtover;
                uint16_t l_doubtunder;
                uint32_t balise_pos;            /// 应答器位置
                uint32_t expect_startpos;       /// 预计起始位置
                uint8_t reserved28[28];         /// 备用
            }CTCS3_DATA;

            /**
             * @def
             * 控制信息记录数据
             *
             */
            typedef struct stVCRecord
            {
                COMMON_DATA1 common_data1;
                CTCS2_DATA   ctcs2_data;
                CTCS3_DATA   ctcs3_data;
                COMMON_DATA2 common_data2;
            }VC_RECORD;

            typedef struct stBaliseRecord
            {
                uint8_t reserved36[36];         /// 备用
                uint8_t balise[104];            /// 应答器报文
                uint8_t reserved100[100];       /// 备用
            }BALISE_RECORD;

            typedef struct stWirelessRecord
            {
                uint8_t direction;              /// OBU及RBC之间的数据传送方向
                uint8_t rtmtype;                /// RTM系
                uint8_t reserved6[6];           /// 备用
                uint8_t dataseq;                /// 一条无线信息的分组编号
                uint8_t datacount;              /// 一条无线信息的分组数
                uint8_t payload[226];           /// 一条完整的无线信息包括：数据长度+数据负载+FCS。该信息将被分割成226个字节。
                uint32_t reserved4;             /// 备用
            }WIRELESS_RECORD;

            typedef union unMSUPayload
            {
                VC_RECORD vc;
                BALISE_RECORD balise;
                WIRELESS_RECORD wireless;
            }MSU_PAYLOAD;

            /// MSU向DMS发送的报文结构
            typedef struct stMSUData
            {
                DATE_TIME datetime;
                uint16_t msgtype;
                uint16_t reserved;
                MSU_PAYLOAD payload;
                uint32_t crc;
            }MSU_DATA;

            ///////////////////////////////////////////////
            typedef struct stMiscInfo
            {
                uint32_t fail_info1;            /// 故障信息1
                uint32_t eb_reason;             /// 紧急制动的原因
                uint32_t nb_reason;             /// B7N制动的原因
                uint8_t  di_info[6];            /// DI信息
                uint32_t lma;                   /// 移动授权极限
                uint8_t  curr_loadfreqswitch;   /// 当前轨道电路载频切换信息
                uint32_t fail_info2;            /// 故障信息2
            }MISC_INFO;

            typedef struct stAssistInfo
            {
                uint8_t   type;
                MISC_INFO misc;
                uint8_t   brake;
                uint8_t   systype;
            }ASSIST_INFO;
#pragma pack(pop)

            /// 信息标识
            typedef enum emInformationId
            {
                INFO_CTRLMSG  = 0x0921,    /**< 控制消息 */
                INFO_BALISE   = 0x0927,    /**< 应答器消息 */
                INFO_WIRELESS = 0x0929     /**< 无线消息 */
            }INFO_ID;

        public:
            CAtp300H(int interval);
            ~CAtp300H();

            int parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

        private:
            CAtp300H(const CAtp300H&);
            CAtp300H& operator=(const CAtp300H&);

            void parseBaseFromVC(const VC_RECORD* vc_rec);
            bool parseWirelessRec(const WIRELESS_RECORD* wireless_rec);
            bool parseBaliseRec(const BALISE_RECORD* balise_rec);

            void parseSpeedInfo(const SPEED_INFO* speed_info);

            void convertMode(uint8_t& mode);

            bool checkCtrlMsgTriggers(MISC_INFO* misc, const VC_RECORD* vc_rec);

            /**
             * @brief 解析制动命令
             *
             * @param brake_info 制动指令
             *
             * @return
             */
            BRAKE_ORDER parseBrakeOrder(uint8_t brake_info) const;

            /**
             * @brief 解析低频灯码信息
             *
             * @param tcr_signal TCR信号
             *
             * @return
             */
            LOWFREQ_CODER decodeLowFreq(const uint8_t* tcr_signal) const;

            /**
             * @brief 解析载频信息
             *
             * @param tcr_signal TCR信号
             *
             * @return
             */
            CARRIER_FREQ decodeCarrFreq(const uint8_t* tcr_signal) const;

            /// 处理无线数据
            bool procWirelessData(void);

        private:
            static LOWFREQ_CODER LowFreqDictionary[];
            static CARRIER_FREQ CarrFreqDictionary[];

        private:
            /// 杂项数据
            MISC_INFO m_MainData;

            /// 备系数据
            ASSIST_INFO m_AssistData;

            /// 无线数据缓冲区
            unsigned char m_WirelessBuf[1024];

            /// 无线数据的总长度
            int m_WirelessLen;

            /// 已接收的无线数据长度
            int m_RecvLen;
    };
}

#endif //_ATP_300H_H
