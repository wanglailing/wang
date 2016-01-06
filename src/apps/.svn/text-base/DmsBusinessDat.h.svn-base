/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DmsBusinessDat.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月16日 09时46分48秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DMS_BUSINESS_DAT_H
#define _DMS_BUSINESS_DAT_H

#include <stdint.h>

#include "GprsPackage.h"

#pragma pack(push, 1)
/**
 * @def
 * 车次
 *
 */
typedef struct stTrainNumber
{
    char flag[4];
    uint32_t num;
}TrainNumber;

/**
 * @def
 * 列车运行信息
 *
 */
typedef struct stTrainOpInfo
{
    TrainNumber trainum;     /// 车次
    uint32_t    driverid;    /// 司机号
}TrainOper;

/**
 * @def
 * GPRS业务数据头部
 *
 */
typedef struct stGprsHeader
{
    unsigned short dataid;             /// 数据标识(低字节是数据类型，高字节是数据子类型)
    unsigned short reserved;           /// 预留
}GPRS_HEADER;

/**
 * @def
 * 业务数据头部
 *
 */
typedef struct stBusinessHeader
{
    GPRS_HEADER gprshdr;     /// GPRS封装头部
    uint8_t     flag;        /// 头部标识
    uint32_t    protover;    /// 协议版本号
    uint8_t     atpid;       /// ATP标识
    uint32_t    trainid;     /// 车号
    TrainOper   operate;     /// 列车运营信息
    uint16_t    reserved;    /// 预留
    uint8_t     packcount;   /// 子包数量
}BUSINESS_HDR;

/**
 * @def
 * 基础数据
 *
 */
typedef struct stBasicData
{
    /**
     * 秒: 0-5 (bits)
     * 分: 6-11 (bits)
     * 时: 12-16 (bits)
     * 日: 17-21 (bits)
     * 月: 22-25 (bits)
     * 年: 26-31 (bits)
     */
    uint32_t dateTime;    /// ATP时间

    uint8_t  reserved;    /// 预留
    uint32_t mileage;     /// 里程标
    uint32_t runDistance; /// 走行距离

    /**
     * 运行速度:      0-9 (bits)
     * 常用制动速度: 10-19 (bits)
     * 紧急制动速度: 20-29 (bits)
     * 速度方向:     30-31 (bits)
     */
    uint32_t speedInfo;   /// 速度信息

    /**
     * 模式: 0-3 (bits)
     * 等级: 4-7 (bits)
     */
    uint8_t  modeLevel;   /// 模式等级

    /**
     * 载频:  0-3 (bits)
     * 低频:  4-9 (bits)
     * 预留: 10-15 (bits)
     */
    uint16_t trackFreq;  /// 载频和低频灯码

    /**
     * 目标速度:  0-9  (bits)
     * 目标距离: 10-31 (bits)
     */
    uint32_t targetInfo; /// 目标数据

    uint8_t  brake;      /// 制动动作
}BasicData;

/**
 * @def
 * DMS状态信息
 *
 */
typedef struct stDataStatus
{
    uint32_t dms:5;
    uint32_t cir:3;
    uint32_t gps:3;
    uint32_t atp:3;
    uint32_t tcr:3;
    uint32_t gprs:3;
    uint32_t gsmr:2;
    uint32_t atpc2:2;

    uint32_t tcas:1;
    uint32_t port2:1;
    uint32_t port3:1;
    uint32_t port4:1;
    uint32_t port5:1;
}DataStatus;

/**
 * @def
 * 包含车次和司机号的ATP基础信息
 *
 */
typedef struct stOperBasic
{
    BasicData basic;
    TrainOper trainop;
}OperBasic;

/**
 * @def
 * GPS信息
 *
 */
typedef struct stGpsInfo
{
    uint8_t  status;                /// GPS状态及星数(0-5:星数，6-7:状态)
    int      longitude;             /// 经度
    int      latitude;              /// 纬度
    uint16_t speed;                 /// GPS速度(km/h)
}GpsInfo;

/**
 * @def
 * GPS实时信息
 *
 */
typedef struct stGpsRTInfo
{
    GpsInfo  params;
    uint64_t stamp;
}GpsRTInfo;

/**
 * @def
 * 应答器信息
 *
 */
typedef struct stBaliseInfo
{
    uint32_t passbalino;    /// 过应答器编号
    uint16_t passdist;      /// 过应答器距离
    uint32_t priorbalino;   /// 上一个过应答器编号
}BaliseInfo;

/**
 * @def
 * ATP相关数据项的接收时间戳
 *
 */
typedef struct stRecvTimestampInfo
{
    uint64_t devstate;
    uint64_t trainum;
    uint64_t atpspeed;
    uint64_t balise;
}AtpRecvStamp;

/**
 * @def
 * ATP实时信息
 *
 */
typedef struct stAtpSharedInfo
{
    OperBasic    opbase;               /// ATP基础信息
    uint32_t     enctrain;             /// 编码车次
    BaliseInfo   balise;               /// 应答器信息
    AtpRecvStamp stamps;               /// 接收时间戳
}AtpRTInfo;

/**
 * @def
 * 自定义JRU数据封装头部
 *
 */
typedef struct stJruHeader
{
    uint8_t  startChar;     /// 起始标记
    uint16_t tag;           /// 头部标识
    uint16_t listenPort;    /// DMS作为服务器的侦听端口
}JRUHeader;

/**
 * @def
 * GPS协议数据
 *
 */
typedef struct stGpsProtoData
{
    uint8_t status;              /// GPS状态
    uint8_t starnum;             /// 星数
    uint16_t hdprecision;        /// 水平精度因子
    int latitude;                /// 纬度
    int longitude;               /// 经度
    int altitude;                /// 海拔
    uint32_t speed;              /// GPS速度(m/h)
    uint16_t reserved;           /// 预留
    uint16_t heading;            /// 航向
    uint8_t hour;                /// 时
    uint8_t minute;              /// 分 
    uint8_t second;              /// 秒
    uint8_t millisec;            /// 毫秒
    uint8_t reserved1;           /// 预留
    uint8_t year;                /// 年
    uint8_t month;               /// 月
    uint8_t day;                 /// 日
    uint8_t reserved2[8];        /// 预留
}GPSPROTO_DATA;

#pragma pack(pop)

/**
 * @def
 * 数据状态选项
 *
 */
typedef enum emStatusOption
{
    STATUS_TIMEOUT = 0,       /**< 数据超时 */
    STATUS_ERROR,             /**< 数据错误 */
    STATUS_OK                 /**< 数据正确 */
}STATUS_OPTION;

#endif //_DMS_BUSINESS_DAT_H
