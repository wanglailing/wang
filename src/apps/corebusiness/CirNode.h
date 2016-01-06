/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CirNode.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月18日 08时44分44秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _CIR_NODE_H
#define _CIR_NODE_H

#include "DataNode.h"
#include "../DmsBusinessDat.h"

namespace COREBUSINESS
{
    class CCirNode : public CDataNode
    {
        public:
#pragma pack(push, 1)
            /// CIR发往DMS的数据格式
            typedef struct stCirToDms
            {
                uint8_t  trainumflag[4];    /// 车次种类标识符
                uint8_t  trainumpart[3];    /// 车次的数字部分
                uint16_t trainmodel;        /// 机车型号
                uint16_t trainseq;          /// 机车号
                uint8_t  workmode;          /// CIR工作模式
                uint8_t  state[10];         /// CIR各单元的状态
                uint8_t  reserved[20];      /// 预留
            }CIR_TO_DMS;

            /// CIR转发TAX箱(DMS发往CIR)的数据格式
            typedef struct stCirTransTax
            {
                uint8_t addr1;              /// 板卡地址1
                uint8_t magicode1;          /// 特征码1
                uint8_t flag;               /// 标志
                uint8_t dmsverno;           /// DMS软件版本号
                uint8_t reserved;           /// 预留
                uint8_t stanum1;            /// 车站号1
                uint8_t trainumflag[4];     /// 车次种类标识符
                uint8_t drivernum1;         /// 司机号1
                uint8_t vicedrvnum1;        /// 副司机号1
                uint8_t reserved1[2];       /// 预留
                uint8_t modelextend;        /// 机车型号扩充字节
                uint8_t actualroutenum;     /// 实际交路号
                uint8_t reserved2[11];      /// 预留
                uint8_t businesstype1;      /// 业务种类1
                uint8_t trainumpart[3];     /// 车次的数字部分
                uint8_t checksum1;          /// 上述字段的累加和

                uint8_t addr2;              /// 板卡地址2
                uint8_t magicode2;          /// 特征码2
                uint8_t checkunitnum;       /// 检查单元代号
                uint8_t datetime[4];        /// 日期时间
                uint8_t runspeed[3];        /// 列车运行速度
                uint8_t trainsem;           /// 机车信号
                uint8_t trainsitutation;    /// 机车工况
                uint16_t semnum;            /// 信号机编号
                uint8_t semtype;            /// 信号机种类
                uint8_t mileage[3];         /// 里程标
                uint16_t weight;            /// 总重
                uint16_t length;            /// 记长
                uint8_t traincount;         /// 辆数
                uint8_t businesstype2;      /// 业务种类2
                uint16_t trainum;           /// 车次
                uint8_t routenum;           /// 交路号
                uint8_t stanum2;            /// 车站号2
                uint16_t drivernum2;        /// 司机号2
                uint16_t vicedrvnum2;       /// 副司机号2
                uint16_t trainseq;          /// 机车号
                uint8_t modellow;           /// 机车型号低字节部分
                uint16_t tubediff;          /// 管压
                uint8_t devstate;           /// 设备状态
                uint8_t reserved3;          /// 预留
                uint8_t checksum2;          /// addr2-reserved3字段的累加和
            }CIRTRANS_TAX;

            /// 包含时间戳的CIR车次
            typedef struct stCirTrainNum
            {
                TrainNumber  parsed;        /// 解析后的车次
                unsigned int stamp;         /// 接收时间戳
            }CirTrainum;
#pragma pack(pop)

        public:
            CCirNode(int interval);
            ~CCirNode();

            int parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            void dump(void);

            /// 获取CIR车次
            unsigned char* getParsedInfo(int* length);

        private:
            CCirNode(const CCirNode&);
            CCirNode& operator=(const CCirNode&);

            /// 重设车次
            void resetTrainNum(CirTrainum *trainum);

            /// 车次
            CirTrainum m_TrainNum;

            /// 列车运行速度
            uint16_t m_RunSpeed;
    };
}

#endif //_CIR_NODE_H
