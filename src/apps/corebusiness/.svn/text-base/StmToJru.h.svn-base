/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: StmToJru.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月16日 17时22分02秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _STM_TO_JRU_H
#define _STM_TO_JRU_H

#include <stdint.h>
#include <time.h>
#include <map>

#include "Buffer.h"
#include "CommonData.h"
#include "DataNode.h"
#include "../DmsBusinessDat.h"

namespace COREBUSINESS
{
    /**
     * C2 STM发往JRU的数据(只对300T有效)。
     *
     */
    class CSTMToJRU : public CDataNode
    {
        public:
            typedef enum emMessageId
            {
                NID_GENERAL     = 1,    /**< 通用数据 */
                NID_TRAINDRIVER = 2,    /**< 车次和司机号 */
                NID_DMI         = 3,    /**< DMI信息 */
                NID_APPRUN      = 4,    /**< 应用层运行信息 */
                NID_STATION     = 7,    /**< 站名显示 */
                NID_BALISE1     = 30,   /**< 应答器信息包1 */
                NID_BALISE2     = 31,   /**< 应答器信息包2 */
                NID_SYSRUN      = 32,   /**< 系统层运行信息包 */
                NID_TIME        = 33,   /**< 时间信息包 */
                NID_TRAINDATA   = 128,  /**< 列车数据 */
                NID_EXCEPTION   = 130   /**< 异常信息 */
            }NID_MSG;

            /// 头部各域的索引名, 这些枚举值必须是从0开始的连续值
            typedef enum emHeaderFieldIndex
            {
                HDRFIELD_NIDPACKET = 0,   /**< JRU数据包标识 */
                HDRFIELD_PACKETLEN,       /**< JRU数据包长度 */
                HDRFIELD_JRUTIME,         /**< ATP系统参考时间(ms) */
                HDRFIELD_NLITER,          /**< JRU有效数据的字节长度 */
                HDRFIELD_FSTORAGETIME,    /**< JRU数据的存储区域 */
                HDRFIELD_POSITION,        /**< 列车位置(m) */
                HDRFIELD_PEROID,          /**< 系统周期号 */
                HDRFIELD_TOTAL            /**< 头部域的个数 */
            }HDRFIELD_INDEX;

            /// 通用信息各域的索引名
            typedef enum emGeneralInfoIndex
            {
                GENERALINFO_RUNSPEED = 0,    /**< 列车当前速度(km/h) */
                GENERALINFO_TRAINPOS,        /**< 列车绝对位置(m) */
                GENERALINFO_CARRFREQ,        /**< 轨道载频 */
                GENERALINFO_LOWFREQ,         /**< 轨道低频 */
                GENERALINFO_PERSPEED,        /**< 允许速度(km/h) */
                GENERALINFO_TARGETSPEED,     /**< 目标速度(km/h) */
                GENERALINFO_EBSPEED,         /**< 紧急制动速度(km/h) */
                GENERALINFO_SBSPEED,         /**< 常用制动速度(km/h) */
                GENERALINFO_TARGETDIS,       /**< 目标距离 */
                GENERALINFO_MILEAGE,         /**< 公里标 */
                GENERALINFO_BRAKECMD,        /**< 制动命令 */
                GENERALINFO_TOTAL
            }GENERALINFO_INDEX;

            /// 日期时间各域的索引名
            typedef enum emDateTimeIndex
            {
                DATETIME_YEAR = 0,
                DATETIME_MONTH,
                DATETIME_DAY,
                DATETIME_HOUR,
                DATETIME_MINUTE,
                DATETIME_SECOND,
                DATETIME_TTS,
                DATETIME_TOTAL
            }DATETIME_INDEX;

            /// 列车数据各域的索引名
            typedef enum emTrainDataIndex
            {
                TRAINDATA_RUNUM = 0,     /**< 车次 */
                TRAINDATA_TYPE,          /**< 列车类型 */
                TRAINDATA_LENGTH,        /**< 列车长度 */
                TRAINDATA_MAXSPEED,      /**< 最大构造速度 */
                TRAINDATA_LOADGAUGE,     /**< 载重种类 */
                TRAINDATA_BRAKPERC,      /**< 制动百分比 */
                TRAINDATA_ADHERECOFF,    /**< 粘着系数 */
                TRAINDATA_AXLELOAD,      /**< 轴重 */
                TRAINDATA_ARITIGHT,      /**< 密封性种类 */
                TRAINDATA_TRACTION,      /**< 牵引类型 */
                TRAINDATA_MAXACC,        /**< 最大加速度 */
                TRAINDATA_TOTAL
            }TRAINDATA_INDEX;

            /// 定义头部各字段的布局
            typedef struct stHdrFieldLayout
            {
                HDRFIELD_INDEX index;
                int bitlen;
            }HDR_LAYOUT;

            /// 定义通用信息各字段的布局
            typedef struct stGeneralInfoLayout
            {
                GENERALINFO_INDEX index;
                int bitlen;
            }GENERALINFO_LAYOUT;

            /// 定义时间信息各字段的布局
            typedef struct stDateTimeLayout
            {
                DATETIME_INDEX index;
                int bitlen;
            }DATETIME_LAYOUT;

            /// 定义列出数据各字段的布局
            typedef struct stTrainDataLayout
            {
                TRAINDATA_INDEX index;
                int bitlen;
            }TRAINDATA_LAYOUT;

            typedef int (*MSG_CALLBACK)(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);
            typedef std::map<NID_MSG, MSG_CALLBACK> MSG_PARSER;
            typedef std::map<NID_MSG, MSG_CALLBACK>::const_iterator PARSER_ITER;

        public:
            CSTMToJRU(int jru_hdrlen, AtpRTInfo* rtinfo);
            ~CSTMToJRU() {}

            int parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            uint16_t getRunSpeed(void) const { return m_RunSpeed; }

            uint8_t getMode(void) const { return m_Mode; }

            /// 获取数据发送标志
            bool getSendFlag(void) const { return m_bSendTriggered; }

            /// 复位数据发送标志
            void resetSendFlag(void) { m_bSendTriggered = false; }

            /// 复位基础信息
            void resetDataInfo(uint8_t reset_area = 0);

            /// 获取C2中的走行距离
            uint32_t getC2RunDistance(void) const { return m_C2RunDistance; }

            /// 设置C3等级下的ATP时间戳
            void setC3Timestamp(uint32_t stamp) { m_C3Timestamp = stamp; }

            void dump(void) {}

        private:
            CSTMToJRU(const CSTMToJRU&);
            CSTMToJRU& operator=(const CSTMToJRU&);

            /// 获取头部长度
            int getHeaderLength(void);

            /// 将编码时间转换为秒数
            static time_t decodeTimeToSecond(uint32_t encode_date);

            /// 解析制动命令
            static BRAKE_ORDER decodeBrakeCommand(uint8_t brake_cmd);

            /// 解析低频灯码
            static LOWFREQ_CODER decodeLowFreqCode(uint32_t lowfreq);

            /// 解析载频
            static CARRIER_FREQ decodeCarrFreqCode(uint32_t carrfreq);

            /// 解析通用信息
            static int parseGeneralInfo(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析应用层运行信息(控车模式)
            static int parseAppRunInfo(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析时间信息
            static int parseTimeInfo(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 解析列车数据
            static int parseTrainData(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 触发数据发送
            static int triggerSending(CSTMToJRU* obj, const unsigned char* data, int total_bitlen, int bitpos, int bitlen);

            /// 头部各域的布局
            static HDR_LAYOUT HdrFieldsLayout[];

            /// 通用信息各域的布局
            static GENERALINFO_LAYOUT GeneralInfoLayout[];

            /// 时间信息各域的布局
            static DATETIME_LAYOUT DateTimeLayout[];

            /// 列车数据各域的布局
            static TRAINDATA_LAYOUT TrainDataLayout[];

            /// 消息域的解析工具集
            MSG_PARSER m_MessageParsers;

            /// 存放解析后的头部各域的值(每个域的长度都不会超过32个比特位)
            uint32_t m_HdrFieldValues[HDRFIELD_TOTAL];

            /// 存放解析后的通用信息各域的值
            uint32_t m_GeneralInfoValues[GENERALINFO_TOTAL];

            /// 存放解析后的时间信息各域的值
            uint32_t m_DateTimeValues[DATETIME_TOTAL];

            /// 存放解析后的列车数据各域的值
            uint32_t m_TrainDataValues[TRAINDATA_TOTAL];

            /// JRU报文头部长度(STM报文是JRU报文的数据负载)
            int m_JruHdrBitLen;

            /// ATP实时信息
            AtpRTInfo* m_pAtpRealtimeInfo;

            /// STM中的走行距离
            uint32_t m_C2RunDistance;

            /// C3等级下的ATP时间戳
            uint32_t m_C3Timestamp;

            /// 列车运行速度
            uint16_t m_RunSpeed;

            /// 模式
            uint8_t m_Mode;

            /// STM报文头部长度
            int m_HeaderBitLen;

            /// 数据发送触发标志
            bool m_bSendTriggered;
    };
}

#endif //_STM_TO_JRU_H
