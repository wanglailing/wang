/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: BusProtocol.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月02日 13时45分04秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _BUS_PROTOCOL_H
#define _BUS_PROTOCOL_H

#include <stdint.h>

/**
 * 底板总线数据的业务类型
 *
 */
typedef enum emBusServiceType
{
    NORMAL_SERVICE   = 0x0A,    /**< 普通业务 */
    INTERNAL_SERVICE = 0xE0,    /**< 内部业务 */
    EXTEND_SERVICE   = 0xFF     /**< 扩展业务 */
}BUS_SERVTYPE;

/**
 * 底板总线内部数据的业务类型
 *
 */
typedef enum emInternalServType
{
    INTER_DEVSTATUS     = 0x10,    /**< 设备状态(驱动层发给应用层) */
    INTER_GET_DEVSTATUS = 0x11,    /**< 获取设备状态(应用层发给驱动层) */
    INTER_TO_SLAVE      = 0xA1,    /**< 发往从设备 */
    INTER_FROM_SLAVE    = 0xA2,    /**< 接收自从设备 */
    INTER_EXTEND        = 0xFF     /**< 内部扩展 */
}BUS_INTER_SERVT;

/**
 * 底板总线普通数据的业务类型
 *
 */
typedef enum emDataServType
{
    DAT_EXCHANGE      = 0x0101,    /**< 外部交换数据(主从互发) */
    DAT_SPECIAL       = 0x0201,    /**< 特殊数据(主从互发) */
    DAT_UPGRADE_BEGIN = 0xF300,    /**< 从设备升级开始(主设备发给从设备) */
    DAT_UPGRADE_ACK   = 0xF301,    /**< 升级数据确认(从设备发给主设备) */
    DAT_UPGRADE_DATA  = 0xF302,    /**< 升级数据(主设备发给从设备) */
    DAT_UPGRADE_OVER  = 0xF303     /**< 从设备升级结束(从设备发给主设备) */
}DATA_SERVTYPE;

#pragma pack(push, 1)
/**
 * 底板总线数据帧头部
 *
 */
typedef struct stBusDataHeader
{
    uint16_t headFlag;              /// 头部标识
    uint16_t payloadLen;            /// 数据负载的长度
    uint8_t  seq;                   /// 帧编号
    uint8_t  ToS;                   /// 业务类型
    uint8_t  reserved;              /// 预留
}BusDataHeader;

/**
 * 设备标识符共有4个字节，依次为：地址、端口号、设备类型和设备子类型
 *
 */
typedef union unDeviceId
{
    uint32_t id;
    struct {
        uint8_t address;
        uint8_t port;
        uint8_t devtype;
        uint8_t subtype;
    }fields;
}DeviceId;

/**
 * 设备状态共有4个字节，依次为：固件版本号、软件版本号1、软件版本号2和设备状态
 *
 */
typedef union unDeviceStatus
{
    uint32_t status;
    struct {
        uint8_t romver;
        uint8_t softver1;
        uint8_t softver2;
        uint8_t state;
    }fields;
}DeviceStatus;

typedef struct stDeviceItem
{
    DeviceId     devid;
    DeviceStatus devstatus;
}DeviceItem;

#pragma pack(pop)

#endif //_BUS_PROTOCOL_H
