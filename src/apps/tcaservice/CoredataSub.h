/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CoredataSub.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月26日 14时04分21秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _COREDATA_SUB_H
#define _COREDATA_SUB_H

#include "ServiceHandler.h"
#include "TcpSocket.h"
#include "Buffer.h"
#include "Reactor.h"
#include "Packet.h"

#include "../DmsBusinessDat.h"

namespace EXTSERVICE
{
    /**
     * @def
     * 预警数据的发送方向
     *
     */
    typedef enum emTcasDirection
    {
        TO_SERVER = 0,
        TO_DMI
    }TCAS_DIRECT;

#pragma pack(push, 1)
    /**
     * @def
     * 预警数据包头部
     *
     */
    typedef struct stTcasHeader
    {
        uint16_t flag;           /// 头部标志
        uint16_t len;            /// 数据长度
        uint16_t protover;       /// 协议版本
        uint16_t seq;            /// 数据编号
    }TcasHeader;

    /**
     * @def
     * 基础数据
     *
     */
    typedef struct stBaseData
    {
        uint8_t  type;
        uint8_t  len;
        uint8_t  aging;         /// 时效性
        uint32_t timestamp;     /// 发送时间戳(取系统单调时间，单位毫秒)
        uint32_t trainid;       /// 车号
        uint8_t  devstate;      /// 设备状态
        uint32_t rundist;       /// 走行距离
    }BaseData;

    /**
     * @def
     * 列车运营数据(仅包含车次)
     *
     */
    typedef struct stOperateData
    {
        uint8_t  type;
        uint8_t  len;
        uint8_t  aging;         /// 时效性
        uint32_t trainum;       /// 车次
    }OpData;

    /**
     * @def
     * ATP数据
     *
     */
    typedef struct stAtpData
    {
        uint8_t  type;
        uint8_t  len;
        uint8_t  aging;         /// 时效性
        uint16_t speed;         /// ATP速度(km/h)
    }AtpData;

    /**
     * @def
     * GPS位置数据
     *
     */
    typedef struct stGpsPosData
    {
        uint8_t  type;
        uint8_t  len;
        uint8_t  aging;         /// 时效性
        int longitude;          /// 经度
        int latitude;           /// 纬度
        uint16_t speed;         /// GPS速度(km/h)
    }GpsPosData;

    /**
     * @def
     * 应答器数据
     *
     */
    typedef struct stBaliseData
    {
        uint8_t  type;
        uint8_t  len;
        uint8_t  aging;         /// 时效性
        uint32_t passbalino;    /// 过应答器编号
        uint16_t passdist;      /// 过应答器距离
        uint32_t priorbalino;   /// 上一个过应答器编号
        uint8_t  linkcount;     /// 链接应答器个数(该信息不再使用，填充为0)
    }BaliseData;

    /**
     * @def
     * 日期时间数据
     *
     */
    typedef struct stDateTimeData
    {
        uint8_t  type;
        uint8_t  len;
        uint16_t year;          /// 年
        uint8_t  month;         /// 月
        uint8_t  day;           /// 日
        uint8_t  hour;          /// 时
        uint8_t  minute;        /// 分
        uint8_t  second;        /// 秒
    }DateTimeData;

    /**
     * @def
     * 车载程序发往服务器的预警数据帧
     *
     */
    typedef struct stTcasFrame
    {
        TcasHeader   header;
        BaseData     base;
        OpData       operate;
        BaliseData   balise;
        GpsPosData   gpspos;
        AtpData      atp;
        DateTimeData datetime;
    }TcasFrame;

    /**
     * @def
     * 车载程序发往显示终端的基础数据帧
     *
     */
    typedef struct stBaseFrame
    {
        TcasHeader   header;
        BaseData     base;
    }BaseFrame;
#pragma pack(pop)

    /**
     * 订阅DMS核心业务数据
     *
     */
    class CCoredataSub : public HELIUM::CServiceHandler<HELIUM::CTcpSocket>
    {
        public:
            CCoredataSub();
            ~CCoredataSub();

            int open(HELIUM::CReactor* r);
            void close(void);
            int handleRead(int fd);

            TcasFrame* getTcasFrame(TCAS_DIRECT direction);

        private:
            CCoredataSub(const CCoredataSub&);
            CCoredataSub& operator=(const CCoredataSub&);

            void parseFrame(const char* data, int length);

            void initTcasFrame(void);

        private:
            HELIUM::CBuffer m_InputBuffer;

            TcasFrame m_TcasFrame;

            /// ATP实时信息
            AtpRTInfo m_RealtimeAtp;

            /// GPS实时信息
            GpsRTInfo m_RealtimeGps;
    };
}

#endif //_COREDATA_SUB_H
