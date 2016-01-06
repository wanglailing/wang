/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: BoardBusOp.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月13日 15时44分40秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _BOARDBUS_OP_H_
#define _BOARDBUS_OP_H_

#include "../BusProtocol.h"
#include "Singleton.h"

namespace DMS
{
    /**
     * 底板总线操作类的接口。
     * 底板总线不支持异步IO操作，但是它也不会阻塞到IO上。在读数据的时候，如果当前没有数据，
     * 它会立刻返回0。因此，只能采用定时读写的方式操作它。
     *
     */
    class CBoardBusOp : public HELIUM::CSingleton<CBoardBusOp>
    {
        public:
            CBoardBusOp();
            ~CBoardBusOp();

            /**
             * 以异步方式打开底板总线
             *
             * @return 0 -- 成功; -1 -- 失败
             *
             */
            int openBus(void);

            /**
             * 关闭底板总线
             *
             */
            void closeBus(void);

            /**
             * 获取底板总线设备描述符
             *
             * @return 底板总线描述符
             */
            int getBusFd(void) const;

            /**
             * 请求从设备的状态
             *
             * @return 0 -- 成功; -1 -- 出错
             */
            int queryDeviceState(void);

            /**
             * 向底板总线的某个从板写入数据
             *
             */
            void flushData(const char *buf, int len);

            /**
             * 向底板总线相同类型的从板写入数据
             *
             */
            void multicastData(const char *buf, int len);

            /**
             * 开始升级从设备
             *
             * @return 0 -- 成功; -1 -- 出错
             */
            int beginUpgradeSlave(void) { return -1; }

            /**
             * 从设备升级结束
             *
             * @return 0 -- 成功; -1 -- 出错
             */
            int finishUpgradeSlave(void) { return -1; }

            /**
             * 将数据添加帧头和帧尾，然后发送给底板总线
             *
             * @param bus_servtype 底板总线业务类型
             * @param buf          数据缓冲区需预留出帧头和帧尾的空间
             * @param len          数据负载的长度
             *
             * @return 发送的数据长度; -1 -- 出错
             */
            int sendToBus(BUS_SERVTYPE bus_servtype, char *buf, int len);

            /**
             * 向底板总线写入数据
             *
             * @param buf          数据缓冲区地址
             * @param len          数据长度
             *
             * @return 写入的数据长度; -1 -- 出错
             */
            int writeData(const char *buf, int len);

            /**
             * 从底板总线读取数据
             *
             * @param buf          数据缓冲区地址
             * @param len          缓冲区长度
             *
             * @return 发送的数据长度; -1 -- 出错
             */
            int recvFromBus(char *buf, int len);

        private:
            /// 禁止复制
            CBoardBusOp(const CBoardBusOp&);
            CBoardBusOp& operator=(const CBoardBusOp&);

        private:
            /// 发往底板总线的数据帧编号
            unsigned char m_SendSeq;

            /// 底板总线设备描述符
            int   m_BusFd;

            /// 数据写入缓冲区
            char* m_FlushBuf;

            /// 写入缓冲区的容量
            static const int FlushBufCount;

            /// 底板总线设备名称
            static const char* m_BusDevName;
    };

    #define BOARD_BUS CBoardBusOp::getInstance()
}

#endif //_BOARDBUS_OP_H_
