/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CoredataSub.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月20日 15时29分21秒
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
#pragma pack(push, 1)
    /**
     * @def
     * 解析过的ATP信息
     *
     */
    typedef struct stParsedAtp
    {
        uint32_t trainum;            /// 车次的数字表示
        uint32_t mileage;            /// 里程标
        uint32_t rundist;            /// 走行距离
        uint16_t ebspeed;            /// 紧急制动速度
        uint16_t runspeed;           /// 列车运行速度
        uint8_t  atptime[8];         /// ATP时间
        uint32_t passneutral;        /// 过分相信息
        uint8_t  brake;              /// ATP制动命令
        uint32_t driverno;           /// 司机号
        uint32_t dmsecs;             /// DMS时间(秒)
        uint32_t dmsnaosecs;         /// DMS时间(纳秒)
        uint32_t baliseno;           /// 应答器编号
        uint32_t passdist;           /// 过应答器距离
        uint8_t  atptype;            /// ATP类型
        uint32_t trainid;            /// 车号
        uint8_t  trainstr[7];        /// 车次的字符串表示
    }PARSED_ATP;
#pragma pack(pop)

    /**
     * 订阅DMS核心业务数据
     *
     */
    class CCoredataSub : public HELIUM::CServiceHandler<HELIUM::CTcpSocket>
    {
        public:
            /**
             * @def
             * 自定义ATP类型
             *
             */
            typedef enum emAtpNum
            {
                ATP300T = 0,
                ATP300S,
                ATP200C,
                ATP200H,
                ATP300D,
                ATP300H,
                ATP200T
            }ATP_NUM;

        public:
            CCoredataSub();
            ~CCoredataSub();

            int open(HELIUM::CReactor* r);
            void close(void);
            int handleRead(int fd);

            /// 获取ATP类型对应的数字
            unsigned char getAtpNum(void) { return (unsigned char)m_AtpNum; }

            /// 获取解析过的ATP信息
            PARSED_ATP* getParsedAtp(void);

        private:
            CCoredataSub(const CCoredataSub&);
            CCoredataSub& operator=(const CCoredataSub&);

            void parseFrame(const char* data, int length);

            /// 将ATP日期的各部分转换为BCD码
            void encodeAtpTime(uint8_t* buf, uint32_t atp_time);

            /// 将数字转换成BCD码
            uint8_t convertDecToBCD(uint8_t decimal);

            /// 将ATP类型映射为数字
            void atpTypeToNum(void);

        private:
            HELIUM::CBuffer m_InputBuffer;

            /// ATP类型所对应的数字
            ATP_NUM m_AtpNum;

            PARSED_ATP m_ParsedAtp;

            /// ATP实时信息
            AtpRTInfo m_RealtimeAtp;
    };
}

#endif //_COREDATA_SUB_H
