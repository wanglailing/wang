/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: Sub027JuridicalRec.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月27日 11时34分55秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _SUBSET027_JURIDICAL_RECORDER_H
#define _SUBSET027_JURIDICAL_RECORDER_H

#include "Buffer.h"
#include "DataNode.h"
#include "AtpNode.h"
#include "../DmsBusinessDat.h"

/// 固定文本的最大长度
#define FIXED_TEXT_LEN 128

/// 纯文本的最大长度
#define PLAIN_TEXT_LEN 256

/// RBC消息的最大长度
#define RBC_MSG_LEN 512

namespace COREBUSINESS
{
    /**
     * 司法记录数据解析。请参考SUBSET-027(司法记录)协议。
     *
     */
    class CJuridicalRecParser : public CDataNode
    {
        public:
#pragma pack(push, 1)
            typedef struct stJruMsgHeader
            {
                uint8_t id;            /// 消息标识
                uint16_t payload_len;  /// 数据负载的长度
            }JRUMSG_HEADER;
#pragma pack(pop)

            typedef struct stMsgInfo
            {
                uint32_t nid;          /// 消息标识
                uint32_t len;          /// 消息长度
            }MSG_INFO;

            /**
             * JRU协议版本
             *
             */
            typedef enum emJRUVersion
            {
                JRU_200 = 0,
                JRU_224,
                JRU_229,
                JRU_230
            }JRU_VERSION;

            /**
             * ATP等级
             *
             */
            typedef enum emAtpLevel
            {
                LEVEL_0 = 0,
                LEVEL_STM,                       /**< C2等级 */
                LEVEL_1,
                LEVEL_2,
                LEVEL_3,
                LEVEL_4,
            }ATP_LEVEL;

            /**
             * 消息标识
             *
             */
            typedef enum emNIDMessage
            {
                NID_HEADER = 0,                  /**< 用于标记头部信息，实际上没有此标识 */
                NID_GENERALMSG,
                NID_DATAENTRYOVER,
                NID_EBORDER,                     /**< 紧急制动命令 */
                NID_SBORDER,                     /**< 常用制动命令 */
                NID_EVENTS,                      /**< 关于行车安全的重要事件 */
                NID_TELEFROMBALISE,              /**< 应答器报文 */
                NID_MSGFROMEUROLOOP,
                NID_MSGFROMRADIO,
                NID_MSGFROMRBC,                  /**< 来自RBC的消息 */
                NID_MSGTORBC,                    /**< 发往RBC的消息 */
                NID_DRIVERACT,
                NID_BALISEGRPERR,                /**< 应答器组出错 */
                NID_RADIOERROR,                  /**< 与RBC通信出错 */
                NID_STMINFO,                     /**< STM信息 */
                NID_DATAFROMEXTSRC,
                NID_STARTFIXEDTXT,
                NID_STOPFIXEDTXT,
                NID_STARTPLAINTXT,
                NID_STOPPLAINTXT,
                NID_RSPVALUE,
                NID_TARGETSPEED,                 /**< 目标速度 */
                NID_TARGETDISTANCE,              /**< 目标距离 */
                NID_RELEASESPEED,
                NID_WARNING,
                NID_SRSPEEDDIST,
                NID_STMSELECTED,
                NID_PERMITSPEED
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
                HDRFIELD_QSCALE,
                HDRFIELD_NIDLRBG,
                HDRFIELD_DLRBG,
                HDRFIELD_QDIRLRBG,
                HDRFIELD_QDLRBG,
                HDRFIELD_LDOUBTOVER,
                HDRFIELD_LDOUBTUNDER,
                HDRFIELD_VTRAIN,
                HDRFIELD_DRIVERID,
                HDRFIELD_NIDENGINE,
                HDRFIELD_LEVEL,
                HDRFIELD_MODE,
                HDRFIELD_TOTAL
            }HDRFIELD_INDEX;

            /// 定义头部各字段的布局
            typedef struct stHdrFieldLayout
            {
                HDRFIELD_INDEX index;
                int bitlen;
            }HDR_LAYOUT;

            /// 定义Data Entry Completed各字段的布局
            typedef struct stDatEntryCompLayout
            {
                int bitlen;
                int increment;
            }DATAENTRY_LAYOUT;

            typedef int (*PARSER_FUNC)(CJuridicalRecParser*, const unsigned char*, int, int, int);
            typedef struct stMessageParser
            {
                NID_MSG msgid;
                PARSER_FUNC callback;
            }MSG_PARSER;

        public:
            explicit CJuridicalRecParser(int interval, JRU_VERSION jru_ver, bool use_permited,
                                         AtpRTInfo* rtinfo);
            ~CJuridicalRecParser();

            int parseData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            /// 解析一帧完整的数据
            int parseFrame(const unsigned char* buf, int len, MSG_INFO* msg_info, uint64_t now);

            uint32_t getC3Timestamp(void) const { return m_C3Timestamp; }

            int getHeaderLen(void) const { return m_MsgHdrLength; }

            bool recvdGeneralInfo(void) const { return m_bGeneralInfoRecvd; }
            void resetGeneralInfoRecvFlag(void) { m_bGeneralInfoRecvd = false; }

            /// 复位基础信息
            void resetDataInfo(uint8_t reset_area = 0);

            void dump(void) {}

            HELIUM::CBuffer* getPacketsBuf(void) { return m_JruPackets; }

        private:
            CJuridicalRecParser(const CJuridicalRecParser&);
            CJuridicalRecParser& operator=(const CJuridicalRecParser&);

            /// 计算消息的头部长度
            int calcMsgHeaderLen(void);

            /// 解析司机号
            void parseDriverId(const unsigned char* buf, int total_bitlen, int bitpos, int bitlen);

            /// 设置制动速度
            static void setBrakeSpeed(CJuridicalRecParser* obj, uint32_t sb_speed);

            /// 解析
            static int parseDataEntryComp(CJuridicalRecParser*, const unsigned char*, int, int, int);

            /// 解析紧急制动命令
            static int parseEBOrder(CJuridicalRecParser*, const unsigned char*, int, int, int);

            /// 解析常用制动命令
            static int parseSBOrder(CJuridicalRecParser*, const unsigned char*, int, int, int);

            /// 解析目标速度
            static int parseTargetSpeed(CJuridicalRecParser*, const unsigned char*, int, int, int);

            /// 解析目标距离
            static int parseTargetDistance(CJuridicalRecParser*, const unsigned char*, int, int, int);

            /// 解析来自应答器的报文
            static int parseTeleFromBalise(CJuridicalRecParser*, const unsigned char*, int, int, int);

            /// 解析来自RBC的消息
            static int parseMsgFromRBC(CJuridicalRecParser*, const unsigned char*, int, int, int);

            /// 解析发给RBC的消息
            static int parseMsgToRBC(CJuridicalRecParser*, const unsigned char*, int, int, int);

            /// 从RBC消息中解析车次
            static void getTrainumInRbcMsg(CJuridicalRecParser*, const unsigned char*, int, int, int);

            static int parseEvents(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parseDriverAction(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parseBGPError(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parseRadioError(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parseStartFixedTxt(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parseStopFixedTxt(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parseStartPlainTxt(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parseStopPlainTxt(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parseRestrictiveSpeed(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parseWarning(CJuridicalRecParser*, const unsigned char*, int, int, int);
            static int parsePermitSpeed(CJuridicalRecParser*, const unsigned char*, int, int, int);

            static void msgGather(HELIUM::CBuffer* jru_container, const unsigned char* raw_data, 
                    uint8_t* msg_buf, int total_bitlen, int bitpos, int bitlen, int max_length);

            /// 头部各域的布局
            static HDR_LAYOUT HdrFieldsLayout[];

            /// Subset027协议版本号
            JRU_VERSION m_ProtoVer;

            /// Subset027即为司法记录协议的引用
            static const char* reference;

            /// 消息域的解析工具集
            static MSG_PARSER MessageParsers[];

            static DATAENTRY_LAYOUT DatEntryCompLayout[];

            /// 存储头部各域的值
            uint32_t m_HdrFieldValues[HDRFIELD_TOTAL];

            /// 消息头部长度
            int m_MsgHdrLength;

            /// C3等级下的ATP时间戳
            uint32_t m_C3Timestamp;

            /// 司机号
            uint8_t m_DriverID[48];

            /// 紧急制动命令
            uint8_t m_EBOrder;

            /// 常用制动命令
            uint8_t m_SBOrder;

            /// 目标速度
            uint16_t m_TargetSpeed;

            /// 限制速度
            uint16_t m_RestrictiveSpeed;

            /// 允许速度
            uint16_t m_PermitSpeed;

            /// 监视收到的基础信息
            bool m_bGeneralInfoRecvd;

            /// 根据允许速度计算紧急制动和常用制动速度
            bool m_bUsePermited;

            /// 应答器报文
            uint8_t m_BalisePacket[BALISE_LENGTH];

            /// 车载事件
            uint8_t m_Events;

            /// 司机操作
            uint8_t m_DrvActions;

            /// 警告信息
            uint8_t m_Warning;

            /// 应答器组出错信息
            uint32_t m_BaliseGrpErr;

            /// RBC出错信息
            uint32_t m_RadioError;

            /// 来自RBC的消息
            uint8_t* m_FromRBCMsg;

            /// 发往RBC的消息
            uint8_t* m_ToRBCMsg;

            /// 将要显示给司机的固定文本信息
            uint8_t* m_StartDisplayFixedTxt;

            /// 不再显示给司机的固定文本信息
            uint8_t* m_StopDisplayFixedTxt;

            /// 将要显示给司机的纯文本信息
            uint8_t* m_StartDisplayPlainTxt;

            /// 不再显示给司机的纯文本信息
            uint8_t* m_StopDisplayPlainTxt;

            /// JRU中的ATP实时信息
            AtpRTInfo* m_pAtpRealtimeInfo;

            /// 存放JRU报文
            HELIUM::CBuffer* m_JruPackets;
    };
}

#endif //_SUBSET027_JURIDICAL_RECORDER_H
