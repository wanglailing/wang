/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CommonData.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月05日 15时49分59秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _COMMON_DATA_H
#define _COMMON_DATA_H

#include <stdint.h>

typedef enum emBrakeOrder
{
    BRAKE_NONE    = 0,    /**< 无制动 */
    BRAKE_SB1     = 1,    /**< 1级常用制动 */
    BRAKE_SB2     = 2,    /**< 2级常用制动 */
    BRAKE_SB3     = 3,    /**< 3级常用制动 */
    BRAKE_SB4     = 4,    /**< 4级常用制动 */
    BRAKE_SB5     = 5,    /**< 5级常用制动 */
    BRAKE_SB6     = 6,    /**< 6级常用制动 */
    BRAKE_SB7     = 7,    /**< 7级常用制动 */
    BRAKE_SB8     = 8,    /**< 8级常用制动 */
    BRAKE_SBUN    = 9,    /**< 未知 */
    BRAKE_EB      = 13,   /**< 紧急制动 */
    BRAKE_PULLCUT = 15,   /**< 切断牵引 */
    BRAKE_TOTAL   = 16
}BRAKE_ORDER;

typedef enum emCarrierFrequency
{
    FREQ_UNKNOWN = 0,
    FREQ_0,
    FREQ_550,
    FREQ_650,
    FREQ_750,
    FREQ_850,
    FREQ_1700,
    FREQ_2000,
    FREQ_2300,
    FREQ_2600,
    FREQ_TOTAL
}CARRIER_FREQ;

/// 低频码(灯码)
typedef enum emLowFrequencyCoder
{
    LOWFREQ_UNKNOWN = 0,         /**< 未知 */
    LOWFREQ_0,                   /**< */
    LOWFREQ_H,                   /**< 红 */
    LOWFREQ_HU,                  /**< 红黄 */
    LOWFREQ_HB,                  /**< 红黄闪 */
    LOWFREQ_U,                   /**< 黄 */
    LOWFREQ_U2S,                 /**< 黄2闪 */
    LOWFREQ_U2,                  /**< 黄2 */
    LOWFREQ_UU,                  /**< 双黄 */
    LOWFREQ_UUS,                 /**< 双黄闪 */
    LOWFREQ_LU,                  /**< 绿黄 */
    LOWFREQ_LU2,                 /**< */
    LOWFREQ_L,                   /**< 绿 */
    LOWFREQ_L2,                  /**< 绿2 */
    LOWFREQ_L3,                  /**< 绿3 */
    LOWFREQ_L4,                  /**< 绿4 */
    LOWFREQ_L5,                  /**< 绿5 */
    LOWFREQ_L6,                  /**< 绿6 */
    LOWFREQ_257,                 /**< 客专用于上下行转频 */
    LOWFREQ_279,                 /**< 反向运行，正向时按照无码处理 */
    LOWFREQ_U3,                  /**< */
    LOWFREQ_TOTAL
}LOWFREQ_CODER;

typedef enum emChangeEvent
{
    CHANGE_BASE = 0,       /**< 基本信息改变 */
    CHANGE_BALISE,         /**< 应答器改变 */
    CHANGE_CTCS2,          /**< C2数据改变 */
    CHANGE_CTCS3,          /**< C3数据改变 */
    CHANGE_C3JRU,          /**< C3 JRU改变 */
    CHANGE_DMI,            /**< DMI文本改变 */
    CHANGE_ATPSTATE        /**< ATP状态改变 */
}CHANGE_EVENT;

#pragma pack(push, 1)
typedef struct stDateTime
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t millisec;
}DATE_TIME;

typedef struct stDmsLocalTime
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}DMS_LOCALTIME;

typedef struct stDMIInfo
{
    uint16_t target_speed;          /// 目标速度
    uint16_t target_distance;       /// 目标距离
    uint8_t brakalert_time;         /// 制动警戒时间
    uint8_t alarm;                  /// 警报
    uint8_t sound;                  /// 声音信息
    uint8_t voice;                  /// 语音信息
    uint32_t text;                  /// 文本信息
    uint16_t switch_type;           /// 开关类型
}DMI_INFO;

typedef struct stSpeedInfo
{
    uint16_t ebp;                   /// 紧急制动速度
    uint16_t nbp;                   /// B7N制动速度
    uint16_t warn;                  /// 警报速度
    uint16_t permit;                /// 允许速度
}SPEED_INFO;
#pragma pack(pop)

#endif //_COMMON_DATA_H
