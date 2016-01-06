/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Atp200C.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月29日 14时36分33秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ATP_200C_H
#define _ATP_200C_H

#include "AtpNode.h"

namespace COREBUSINESS
{
    class CAtp200C : public CAtpNode
    {
        public:
            /// 消息标识
            typedef enum emNIDMessage
            {
                NID_ENGINE          = 3,    /**< 设备标识 */
                NID_DRIVERINFO      = 4,    /**< 司机信息 */
                NID_DRIVEROP        = 5,    /**< 司机操作 */
                NID_BALISEPASSING   = 6,    /**< 废弃应答器 */
                NID_CTRLCMDOUTSTATE = 7,    /**< 输出状态控制命令 */
                NID_ALARMINFO       = 8,    /**< 报警信息 */
                NID_TARGETDISTANCE  = 9,    /**< 目标距离 */
                NID_TVMINFO         = 10,   /**< TVM信息 */
                NID_LINKSTATISTICS  = 11,   /**< 链路状态统计 */
                NID_SLIPSLIDEINFO   = 12,   /**< 空转滑行 */
                NID_ATPDEFAULTINFO  = 13,   /**< ATP缺省信息 */
                NID_ABNORMALINFOGND = 14,   /**< 地面异常信息 */
                NID_BALISETELEGRAM  = 15,   /**< 应答器报文 */
                NID_DEVICESTATUS    = 16,   /**< 设备状态 */
                NID_DRIVERINTERACT  = 17    /**< 司机操作列车时的动作 */
            }NID_MSG;

            /// 头部各域的索引名, 这些枚举值必须是从0开始的连续值
            typedef enum emHeaderFieldIndex
            {
                HDRFIELD_NIDMSG = 0,
                HDRFIELD_MSGLEN,
                HDRFIELD_YEAR,
                HDRFIELD_MONTH,
                HDRFIELD_DAY,
                HDRFIELD_HOUR,
                HDRFIELD_MINUTE,
                HDRFIELD_SECOND,
                HDRFIELD_TTS,
                HDRFIELD_STATUS,
                HDRFIELD_QSCALE,
                HDRFIELD_NIDLRBG,
                HDRFIELD_DLRBG,
                HDRFIELD_QDIRLRBG,
                HDRFIELD_QDLRBG,
                HDRFIELD_VTRAIN,
                HDRFIELD_VLIMITSPEED,
                HDRFIELD_VTARGET,
                HDRFIELD_LEVEL,
                HDRFIELD_MODE,
                HDRFIELD_CABSIGCODE,
                HDRFIELD_CABSIGSPEED,
                HDRFIELD_NIDJOINT,
                HDRFIELD_TOTAL
            }HDRFIELD_INDEX;

            /// 司机信息各域的索引名
            typedef enum emDriverInfoIndex
            {
                DRIVERINFO_LANGUAGE = 0,           /**< 语言标识 */
                DRIVERINFO_COMPANY,                /**< 公司标识(未使用) */
                DRIVERINFO_DRIVERID,               /**< 司机标识 */
                DRIVERINFO_MISSION_LETTERS,        /**< 车次的字母部分(0-3个字母) */
                DRIVERINFO_MISSION_DIGITS,         /**< 车次的数字部分(BCD码, 1-5个数字) */
                DRIVERINFO_TOTAL
            }DRIVERINFO_INDEX;

            /// 链路状态统计各域的索引名
            typedef enum emLinkStatIndex
            {
                LINKSTAT_INIT = 0,                 /**< 链接初始化速度 */
                LINKSTAT_FFFE_ERR,                 /**< 链接处于FFFE_ERR状态的次数 */
                LINKSTAT_CRC_ERR,                  /**< 链接处于CRC_ERR状态的次数 */
                LINKSTAT_DYN,                      /**< 链接处于DYN状态的次数*/
                LINKSTAT_MSG_LOSS,                 /**< 链接处于MSG_LOSS状态的次数*/
                LINKSTAT_DELAY,                    /**< 链接处于延迟状态的次数*/
                LINKSTAT_OTHER,                    /**< 链接因与之前原因不同而丢弃报文的次数 */
                LINKSTAT_TOTAL
            }LINKSTAT_INDEX;

            /// 废弃应答器组各域的索引名
            typedef enum emBalisePassingIndex
            {
                BALISEPASS_QSCALE = 0,
                BALISEPASS_NIDLRBG,
                BALISEPASS_DLRBG,
                BALISEPASS_QDIRLRBG,
                BALISEPASS_QDLRBG,
                BALISEPASS_MERROR,
                BALISEPASS_TOTAL
            }BALISEPASS_INDEX;

            /// 轨道电路各域的索引名
            typedef enum emTrackCircuitIndex
            {
                TRACKCIRCUIT_NIDXUSR = 0,          /**< 信息包标识码 */
                TRACKCIRCUIT_QDIR,                 /**< 验证方向 */
                TRACKCIRCUIT_LPACKET,              /**< 信息包位数 */
                TRACKCIRCUIT_QSCALE,               /**< 长度分辨率 */
                TRACKCIRCUIT_DSIGNAL,              /**< 到前方第一架信号机的距离 */
                TRACKCIRCUIT_TOTAL
            }TRACKCIRCUIT_INDEX;

            /// 定义头部各字段的布局
            typedef struct stHdrFieldLayout
            {
                HDRFIELD_INDEX index;
                int bitlen;
            }HDR_LAYOUT;

            /// 轨道区段
            typedef struct stTrackSection
            {
                uint8_t nidsignal;    /// 信号种类
                uint8_t nidfreq;      /// 轨道载频
                uint16_t length;      /// 轨道区段的长度
            }TRACK_SECTION;

            typedef struct stMessageLayout
            {
                NID_MSG msgid;
                int bitlen;
            }MSG_LAYOUT;

            /// 定义司机信息各字段的布局
            typedef struct stDriverInfoLayout
            {
                DRIVERINFO_INDEX index;
                int bitlen;
            }DRIVERINFO_LAYOUT;

            /// 定义废弃应答器各字段的布局
            typedef struct stBalisePassLayout
            {
                BALISEPASS_INDEX index;
                int bitlen;
            }BALISEPASS_LAYOUT;

            /// 定义轨道电路各字段的布局
            typedef struct stTrackCircuitLayout
            {
                TRACKCIRCUIT_INDEX index;
                int bitlen;
            }TRACKCIRCUIT_LAYOUT;

            typedef int (*MSG_CALLBACK)(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);
            typedef struct stMessageParser
            {
                MSG_LAYOUT layout;
                MSG_CALLBACK callback;
            }MSG_PARSER;

#pragma pack(push, 1)
            typedef struct stCTCS2Data
            {
                uint8_t power_cutoff;        /// 电源切断命令
                uint8_t eb_cmd;              /// 紧急制动命令
                uint8_t sb_cmd;              /// 常用制动命令
                uint8_t driverop;            /// 司机操作
                uint8_t alarm_type;          /// 报警类型
                uint8_t alarm_event;         /// 报警事件种类
                uint8_t atpdefinfo;          /// ATP缺省信息
                uint8_t slipslideinfo;       /// 空转滑行信息
                uint8_t abnormitygndinfo;    /// 地面异常信息
                uint8_t atpstatus;           /// ATP状态
                uint8_t tiustatus;           /// TIU状态
                uint8_t btmstatus;           /// BTM状态
                uint8_t odostatus;           /// ODO状态
                uint8_t dmistatus;           /// DMI状态
                uint8_t cfskstatus;          /// CFSK状态
                uint8_t carrfreq;            /// 载频
                uint8_t drvdmiop;            /// 司机DMI按键操作
                uint8_t trackhandlepos;      /// 牵引手柄位置
                uint8_t direhandlepos;
                uint8_t atpisolate;          /// ATP隔离状态
                uint8_t dmitxtcode;          /// DMI文本码
            }CTCS2_DATA;
#pragma pack(pop)

        public:
            CAtp200C(int interval);
            ~CAtp200C() {}

            int parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            /// 获取C2数据
            unsigned char* getCTCS2(void) const { return (unsigned char*)&m_C2Data; }

        private:
            CAtp200C(const CAtp200C&);
            CAtp200C& operator=(const CAtp200C&);

            /// 解析CTCS标识
            static int parseNIDEngine(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析司机信息
            static int parseDriverInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析司机操作
            static int parseDriverOperation(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析废弃应答器组
            static int parseBalisePassing(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析ATP控制命令输出状态
            static int parseCtrlCmdOutState(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析报警信息
            static int parseAlarmInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析目标距离
            static int parseTargetDistance(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析TVM模式下的报文
            static int parseTVMInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析链路状态统计
            static int parseLinkStatistics(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析空转滑行信息
            static int parseSlipSlideInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析ATP缺省信息
            static int parseATPDefaultInfo(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析地面异常信息
            static int parseAbnormalInfoOnGnd(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析应答器报文
            static int parseBaliseTelegram(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析设备状态
            static int parseDeviceStatus(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析司机操作控车设备的动作
            static int parseDriverInteraction(CAtp200C* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 设置轨道区段
            static int setTrackSection(TRACK_SECTION* track_section, const unsigned char* data, int total_bitlen, int bitpos);

            /// 解析常用制动命令
            static BRAKE_ORDER decodeBrakeCommand(uint8_t brake_cmd);

            /// 解析低频灯码
            static LOWFREQ_CODER decodeLowFreqCode(uint32_t lowfreq);

            /// 解析载频
            static CARRIER_FREQ decodeCarrFreqCode(uint8_t carrfreq);

            /// 获取头部相关信息
            void getHeaderInfo(void);

            /// 复位C2数据
            void resetCTCS2Data(void);

            /// 头部各域的布局
            static HDR_LAYOUT HdrFieldsLayout[];

            /// 消息域的解析工具集
            static MSG_PARSER MessageParsers[];

            /// 司机信息各域的布局
            static DRIVERINFO_LAYOUT DriverInfoLayout[];

            /// 废弃应答器各域的布局
            static BALISEPASS_LAYOUT BalisePassLayout[];

            /// 轨道电路各域的布局
            static TRACKCIRCUIT_LAYOUT TrackCircuitLayout[];

            /// 存放解析后的头部各域的值(每个域的长度都不会超过32个比特位)
            uint32_t m_HdrFieldValues[HDRFIELD_TOTAL];

            /// 存放解析后的司机信息各域的值
            uint32_t m_DriverInfoValues[DRIVERINFO_TOTAL];

            /// 存放解析后的废弃应答器各域的值
            uint32_t m_BalisePassValues[BALISEPASS_TOTAL];

            /// 存放解析后的轨道电路各域的值
            uint16_t m_TrackCircuitValues[TRACKCIRCUIT_TOTAL];

            /// 轨道区段的个数在33个以内
            TRACK_SECTION m_TrackSections[33];

            /// 控制命令输出状态
            uint8_t m_CtrlCmdOutState;

            /// 报警信息
            uint8_t m_AlarmInfo;

            /// 设备状态
            uint32_t m_DeviceStatus;

            /// 司机与控车设备的交互信息
            uint32_t m_DriverInteract;

            /// 载频0的接收时间戳
            uint64_t m_ZeroFreqStamp;

            /// C2数据
            CTCS2_DATA m_C2Data;

            /// C2数据变化标志
            bool m_C2Changed;

            /// 应答器报文变化标志
            bool m_BaliseChanged;
    };
}

#endif //_ATP_200C_H
