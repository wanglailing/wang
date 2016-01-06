/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Atp200H.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月29日 14时05分47秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ATP_200H_H
#define _ATP_200H_H

#include "AtpNode.h"
#include "CommonData.h"

namespace COREBUSINESS
{
    class CAtp200H : public CAtpNode
    {
        public:
#pragma pack(push, 1)
            typedef struct stTrackCircuitInfo
            {
                uint16_t property;              /// 列车所在轨道电路的属性
                uint16_t balisepos_offset;      /// 应答器位置校正信息
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
             * FSC基板数据
             *
             */
            typedef struct stFSCData
            {
                uint8_t fsc_romver;             /// FSC基板的固件版本号
                uint8_t actionmode;             /// 动作模式
                uint16_t fsc_state;             /// FSC状态
                uint32_t fsc_error;             /// FSC出错信息
                uint8_t btmdatnum;              /// BTM数据编号
                uint8_t reserved;               /// 备用
                TRACUIT_INFO tracuit;           /// 轨道电路相关信息
                uint32_t tsb;                   /// 临时限速曲线
                uint32_t tentative_speed;       /// 临时限速 
                DMI_INFO dminfo;                /// DMI信息
            }FSC_DATA;

            /**
             * @def
             * OPE基板数据
             *
             */
            typedef struct stOPEData
            {
                uint8_t ope_romver;             /// OPE基板的固件版本号
                uint8_t brake_cmd;              /// 制动命令
                uint32_t ope_state;             /// OPE状态
                uint16_t ctrl_speed;            /// 在速度校验中使用的速度
                uint16_t run_speed;             /// 运行速度
                uint8_t reserved1;              /// 备用
                uint8_t acceleration;           /// 加速度
                SPEED_INFO speed_info;          /// 速度相关信息
                uint32_t cumulative_distance;   /// 从控制速度计算出的走行距离

                uint32_t fail_info;             /// 故障信息
                uint16_t eb_reason;             /// 紧急制动的原因
                uint16_t nb_reason;             /// B7N制动的原因

                uint8_t reserved2;              /// 备用
                uint8_t stm_teststate;          /// STM实验状态
                uint8_t stm_signal[8];          /// STM信号信息
                uint8_t stm_state;              /// STM状态
                uint8_t adiffb;                 /// A/B系不一致
                uint8_t sbus_errinfo;           /// S-BUS出错信息
                uint8_t priority;               /// 0-3位表示CTCS0/2设定；4-7位表示机控人控优先设定
                uint16_t train_property;        /// 0-3位表示车种；4-15位表示车轮径

                uint16_t vc_di;                 /// VC-DI信息

                uint16_t ground_info;           /// 地面信息
            }OPE_DATA;

            typedef struct stBoardData
            {
                FSC_DATA fsc;                   /// FSC基板数据
                OPE_DATA ope;                   /// OPE基板数据
            }BOARD_DATA;

            typedef struct stVCRecord
            {
                BOARD_DATA board_dat[2];        /// 基板1, 基板2数据

                uint16_t druinfo;               /// DRU信息
                uint64_t junction;              /// 接点信息
                uint32_t multidi;               /// 多路DI有效信息

                uint8_t trainum[6];             /// 车次
                uint32_t driverid;              /// 司机号

                uint8_t trndrveffective;        /// 车号和司机号有效标志
                uint8_t reserved1;              /// 备用
                uint16_t lkjinfo;               /// LKJ信息
                uint16_t failtrigger;           /// 故障触发

                uint8_t reserved2[2];           /// 备用
            }VC_RECORD;

            typedef struct stBaliseRecord
            {
                uint16_t reserved;
                uint8_t seq;
                uint8_t datanum;
                uint8_t btmstate[16];
                uint8_t balise[BALISE_LENGTH];
            }BALISE_RECORD;

            typedef struct stVCErrInfo
            {
                uint16_t fscerrinfo;            /// FSC错误信息
                uint8_t failflag;               /// 失败信息标志
                uint8_t reserved0;              /// 备用
                uint8_t operomver;              /// OPE固件版本号
                uint8_t fscromver;              /// FSC固件版本号
                uint8_t baliserrinfo[12];       /// 应答器报文解释错误信息
                uint16_t errownum;              /// 应答器解释错误检测行号
                uint8_t baliselostseq[3];       /// 丢失应答器编号
                uint8_t reserved1;              /// 备用
            }VCERR_INFO;

            typedef struct stDRUErrInfo
            {
                uint16_t fscerrinfo;
                uint8_t failflag;
                uint8_t reserved0[3];
                uint8_t baliserrinfo[12];
                uint16_t errownum;
            }DRUERR_INFO;

            typedef struct stFailInfo
            {
                VCERR_INFO  vc1;
                VCERR_INFO  vc2;
                DRUERR_INFO dru;
            }FAIL_INFO;

            typedef union unDRUPayload
            {
                VC_RECORD     vc_record;
                BALISE_RECORD balise_rec;
                FAIL_INFO     fail_info;
            }DRU_PAYLOAD;

            typedef struct stDRURecord
            {
                DATE_TIME datetime;
                uint16_t msgtype;
                uint8_t druromver;
                uint8_t reserved;
                DRU_PAYLOAD payload;
                uint32_t crc;
            }DRU_RECORD;

            /////////////////////////////////////////

            typedef struct stFailPacket
            {
                uint8_t type;
                FAIL_INFO info;
            }FAIL_PACKET;

            typedef struct stAssistInfo
            {
                uint8_t type;
                BOARD_DATA assist;
            }ASSIST_INFO;

            typedef struct stVCOther
            {
                uint8_t type;
                uint16_t druinfo;               /// DRU信息
                uint64_t junction;              /// 接点信息
                uint32_t multidi;               /// 多路DI有效信息
                uint8_t trndrveffective;        /// 车号和司机号有效标志
                uint8_t reserved1;              /// 备用
                uint16_t lkjinfo;               /// LKJ信息
                uint16_t failtrigger;           /// 故障触发
            }VC_OTHER;
#pragma pack(pop)

            /// 信息标识
            typedef enum emInformationId
            {
                INFO_CTRLMSG = 0x0921,    /**< 控制消息 */
                INFO_BALISE  = 0x0922,    /**< 应答器消息 */
                INFO_FAILURE = 0x0A11     /**< 失败消息 */
            }INFO_ID;

            typedef enum emVCInUse
            {
                VC1_INUSE = 1,            /**< VC1有效 */
                VC2_INUSE = 2             /**< VC2有效 */
            }VC_INUSE;

        public:
            CAtp200H(int interval);
            ~CAtp200H();

            int parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

        private:
            CAtp200H(const CAtp200H&);
            CAtp200H& operator=(const CAtp200H&);

            /**
             * @brief 检查正在使用的VC(安全计算机)
             *
             * @param junction 接点信息
             *
             * @return
             */
            uint8_t checkVCInUse(uint64_t junction);

            /**
             * @brief 解析VC(安全计算机)中的基本信息
             *
             * @param board_data 基板数据
             *
             * @return
             */
            void parseBaseFromBoard(const BOARD_DATA* board_data);

            /**
             * @brief 检查基板数据的触发项
             *
             * @param dst      目标数据
             * @param src      原始数据
             * @param assist   是否为备系
             *
             * @return true -- 变化；false -- 未变化
             */
            bool checkBoardTriggers(BOARD_DATA* dst, const BOARD_DATA* src, bool assist);

            /**
             * @brief 检查VC信息的触发项
             *
             * @param vc_rec VC信息
             *
             * @return true -- 变化；false -- 未变化
             */
            bool checkVCTriggers(const VC_RECORD* vc_rec);

            /**
             * @brief 解析车次
             *
             * @param num_info 车次信息
             *
             */
            void parseTrainNum(const uint8_t* num_info);

            /**
             * @brief 解析制动命令
             *
             * @param brake_info 制动指令
             *
             * @return
             */
            BRAKE_ORDER parseBrakeOrder(uint8_t brake_info) const;

            /**
             * @brief 解析STM低频灯码信息
             *
             * @param stm_signal STM信号
             *
             * @return
             */
            LOWFREQ_CODER decodeSTMLowFreq(const uint8_t* stm_signal) const;

            /**
             * @brief 解析STM载频信息
             *
             * @param stm_signal STM信号
             *
             * @return
             */
            CARRIER_FREQ decodeSTMCarrFreq(const uint8_t* stm_signal) const;

        private:
            static LOWFREQ_CODER LowFreqDictionary[];

            /// 主系基板数据
            BOARD_DATA m_MainData;

            /// 备系基板数据
            ASSIST_INFO m_AssistData;

            /// VC其它数据
            VC_OTHER m_VCOther;

            /// 失败数据
            FAIL_PACKET m_FailData;
    };
}

#endif //_ATP_200H_H
