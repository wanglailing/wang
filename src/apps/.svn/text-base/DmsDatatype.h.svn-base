/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DmsDatatype.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月24日 09时55分43秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DMS_DATATYPE_H
#define _DMS_DATATYPE_H

/**
 * 数据标识的低字节是数据类型，高字节是数据子类型。
 *
 */
typedef unsigned short DataId;

/// 定义ATP数据类型
#define ATP_300S            0x0106
#define ATP_300T            0x0206
#define ATP_200C            0x0306
#define ATP_200H            0x0406
#define ATP_300D            0x0506
#define ATP_300S_C2         0x0606
#define ATP_300S_C3_485     0x0806
#define ATP_300S_C3_485TEST 0x0906
#define ATP_300H            0x0A06
#define ATP_200T            0x1206
#define ATP_300S_C2_TEST    0x7706

#define DMS_GPRS            0x0105
#define DMS_GSMR            0x0205

#define DMS_CIR             0x010C
#define DMS_GPS             0x0104
#define DMS_TCR             0x0110
#define DMS_BATT            0x0115
#define DMS_GNET            0x0170

#define COMM_TCAS           0x011D

/// 设备写操作类型
#define DEV_UNICAST         0x01E1
#define DEV_MULTICAST       0x02E1
#define DEV_RAWDATA         0x03E1

/// DMS设备状态及模块运行状态
#define DMS_STATE           0x00E2

/// 远程命令
#define REMOTE_CMD          0x00E3
#define REG_CMD             0x01E3

/// 存储服务数据类型
#define FILE_CREATE         0x00E4
#define FILE_WRITE          0x01E4

/// 固件升级数据类型
#define UPGRADE_DESC        0x00E5
#define UPGRADE_CONTENT     0x01E5

/// 核心数据集
#define CORE_DATASETS       0x00E6

/// 心跳数据类型
#define HEARTBEAT           0x00E8

/**
 * @def 可执行程序标识
 */ 
typedef enum emProgId
{
    PROG_DATASWITCH  = 0,     /**< 数据交换 */
    PROG_MTSERVICE,           /**< 设备维护服务 */
    PROG_COREPROC,            /**< 核心业务处理 */
    PROG_TRANSCTRL,           /**< 可靠传输控制 */
    PROG_STORESERV,           /**< 存储服务 */
    PROG_TCASERV,             /**< 追踪预警服务 */
    PROG_EOASERV,             /**< EOAS服务 */
    PROG_GNETMON              /**< G网检测服务 */
}PROG_ID;

/**
 * @def DMS报文标识
 */ 
typedef enum emDmsMsgId
{
    DMSMSG_HEADER       = 0,
    DMSMSG_BASE         = 1,
    DMSMSG_TRACKVOLTAGE = 2,
    DMSMSG_TRACKINFO    = 3,
    DMSMSG_GPS          = 4,
    DMSMSG_BALISE       = 6,
    DMSMSG_CTCS2        = 8,
    DMSMSG_JRU          = 10,
    DMSMSG_STATUS       = 11,
    DMSMSG_CUSTOM       = 12,
    DMSMSG_DATETIME     = 17
}DMSMSG_ID;

/**
 * @def 数据记录类型
 */ 
typedef enum emDataRecType
{
    DATAREC_CIR = 0,
    DATAREC_ATP,
    DATAREC_ATPC2,
    DATAREC_GNET
}DATAREC_TYPE;

#endif //_DMS_DATATYPE_H
