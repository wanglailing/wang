/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GNetFrame.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月06日 09时45分54秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _GNETFRAME_H
#define _GNETFRAME_H

#include <stdint.h>

/**
 * @def
 * 发往地面服务器的自定义数据类型
 *
 */
typedef enum emCustomDatatype
{
    CUSTOM_LEVEL       = 0x06,    /**< 信号强度 */
    CUSTOM_WORKMODE    = 0x0C,    /**< 工作模式 */
    CUSTOM_QUALITY     = 0x0D,    /**< 信号质量 */
    CUSTOM_CARRTOINTER = 0x0E     /**< 载干比   */
}CUSTOM_TYPE;

/**
 * @def
 * 模块工作模式
 *
 */
typedef enum emWorkMode
{
    NORMAL_MODE = 0,              /**< 正常检测模式 */
    SCAN_MODE,                    /**< 扫描模式 */
    SPECIAL_MODE                  /**< 专项测试模式 */
}WORK_MODE;

/**
 * @def
 * 扫描子模式
 *
 */
typedef enum emSubmodeScan
{
    NORMAL_SCAN = 0,              /**< 正常扫描子模式 */
    RF_SCAN,                      /**< RF扫描 */
    BCCH_SCAN,                    /**< BCCH扫描 */
    PLMN_SCAN                     /**< PLMN扫描 */
}SUBMODE_SCAN;

/**
 * @def
 * 专项测试子模式
 *
 */
typedef enum emSubmodeSpecial
{
    NORMAL_SPECIAL = 0,           /**< 正常测试子模式 */
    ARFCN_SPECIAL,                /**< 公网频点测试 */
    SHORTCALL_SPECIAL,            /**< 短呼测试 */
    LONGCALL_SPECIAL,             /**< 长呼测试 */
    CSDCONN_SPECIAL,              /**< CSD连接建立测试 */
    CSDTRANS_SPECIAL,             /**< CSD传输测试 */
    GPRS_SPECIAL                  /**< GPRS测试 */
}SUBMODE_SPECIAL;

#pragma pack(push, 1)
/**
 * @def
 * 数据帧头
 *
 */
typedef struct stFrameHeader
{
    uint8_t  stx;                 /// 帧起始标记
    uint8_t  modeid;              /// 工作模式及应用标识
    uint16_t msglen;
    uint16_t msgid;               /// 包含类别及子类型
    uint32_t timestamp;           /// 时间戳
    uint8_t  resvmode;            /// 模式
}FrameHeader;

/**
 * @def
 * 数据帧尾
 *
 */
typedef struct stFrameTail
{
    uint8_t checksum;
    uint8_t etx;                  /// 帧结束标记
}FrameTail;

/**
 * @def
 * 服务小区的通话数据
 *
 */
typedef struct stServCallData
{
    uint8_t  ta;                  /// 时间提前量(Timing Advance)
    uint16_t qual_lev;            /// 信号接收质量及强度
    uint8_t  bcch_lev;            /// 广播控制信道的强度
    uint16_t bcch;                /// 广播控制信道的频点号
    uint8_t  bsic;                /// 基站识别码(ncc.bcc)
    uint16_t cellid;              /// 小区标识
    uint16_t lac;                 /// 位置区码
    uint8_t  channel;             /// 信道类型
    uint16_t tch;                 /// 业务信道的频点号
    uint8_t  carrtointer;         /// 载干比
}ServCallData;

/**
 * @def
 * 邻区的通话数据
 *
 */
typedef struct stNearCallData
{
    uint8_t  bsic;                /// 基站识别码(ncc.bcc)
    uint16_t bcch;                /// 广播控制信道的频点号
    char     rx_lev;              /// 接收信号强度
}NearCallData;

/**
 * @def
 * 通话状态的数据
 *
 */
typedef struct stDedicatedData
{
    uint8_t  servcellen;          /// 服务小区的个数
    ServCallData serving;         /// 服务小区的通话数据
    uint8_t  ncellen;             /// 邻区的个数
    NearCallData neighbours[6];   /// 邻区通话数据 
}DedicatedData;

/**
 * @def
 * 服务小区的待机数据
 *
 */
typedef struct stServIdleData
{
    uint8_t  bsic;                /// 基站识别码(ncc.bcc)
    uint16_t bcch;                /// 广播控制信道的频点号
    char     rx_lev;              /// 接收信号强度
    uint8_t  carrtointer;         /// 载干比
    uint16_t cellid;              /// 小区标识
    uint16_t lac;                 /// 位置区码
}ServIdleData;

/**
 * @def
 * 邻区的待机数据
 *
 */
typedef struct stNearIdleData
{
    uint8_t  bsic;                /// 基站识别码(ncc.bcc)
    uint16_t bcch;                /// 广播控制信道的频点号
    char     rx_lev;              /// 接收信号强度
    uint16_t cellid;              /// 小区标识
    uint16_t lac;                 /// 位置区码
}NearIdleData;

/**
 * @def
 * 待机状态的数据
 *
 */
typedef struct stIdleData
{
    uint8_t  servcellen;          /// 服务小区的个数
    ServIdleData serving;         /// 服务小区的待机数据
    uint8_t  ncellen;             /// 邻区的个数
    NearIdleData neighbours[6];   /// 邻区待机数据 
}IdleData;

/**
 * @def
 * RF扫描的结果
 *
 */
typedef struct stRFTrace
{
    uint16_t arfcn;
    char     rx_lev; 
}RFTrace;
#pragma pack(pop)

#endif //_GNETFRAME_H
