/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataHub.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月20日 14时28分47秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DATA_HUB_H
#define _DATA_HUB_H

#include "EventHandler.h"
#include "Reactor.h"
#include "TcpSocket.h"
#include "Acceptor.h"
#include "SwitchService.h"
#include "JruService.h"
#include "Packet.h"

#include "../BusProtocol.h"
#include "../DmsDatatype.h"

namespace DMS
{
    /**
     * 读取底板总线以及网络的数据，并发布给订阅者。
     *
     */
    class CDataHub : public HELIUM::CEventHandler
    {
        public:
            typedef HELIUM::CAcceptor<CSwitchService, HELIUM::CTcpSocket> ACCEPTOR_TYPE;
            typedef HELIUM::CAcceptor<CJRUService, HELIUM::CTcpSocket> JRURECV_TYPE;

        public:
            explicit CDataHub(HELIUM::CReactor *r);
            ~CDataHub();

            /**
             * 初始化数据交换服务。
             *
             * @return 0 -- 成功; -1 -- 失败
             */
            int init(void);

            int handleTimeout(HELIUM::TimerId tid);

            /**
             * 向订阅者广播数据
             *
             * @param subpack_hdr 子报文头部
             * @param buf         数据地址
             * @param len         数据长度
             * @param broadcast   数据广播标志(默认广播)
             *
             * @return 发送的字节数; -1 -- 出错
             */
            int broadcastData(const HELIUM::SUBPACK_HEADER* subpack_hdr, const char *buf, int len, bool broadcast = true);

        private:
            CDataHub(const CDataHub&);
            CDataHub& operator=(const CDataHub&);

            /**
             * 解析底板总线协议
             *
             * @param buf 总线数据
             * @param len 数据长度
             *
             * @return 0 -- 成功; -1 -- 出错
             */
            int parseBusProto(unsigned char *buf, int len);

            /**
             * 附加JRU数据状态
             *
             * @param dst
             * @param dst_len 
             * @param src
             * @param src_len 
             *
             * @return 所有数据状态的总长度
             */
            int appendJruState(char* dst, int dst_len, const char* src, int src_len);

            /**
             * 解析底板总线内部数据
             *
             * @param buf 数据地址
             * @param len 数据长度
             *
             */
            void parseBoardInternalData(const char *buf, int len);

            /// 向JRU客户端发送保活信息
            void keepaliveToJru(void);

        private:
            /// 反应堆对象
            HELIUM::CReactor *m_Reactor;

            /// 数据交换接受器
            ACCEPTOR_TYPE *m_SwitchAcceptor;

            /// 接收JRU数据
            JRURECV_TYPE *m_JruReceiver;

            /// 接收MVB数据
            JRURECV_TYPE *m_MvbReceiver;

            /// 封装好的广播报文
            HELIUM::CExchangePacket m_bdcastPack;

            /// 读取数据定时器标识
            HELIUM::TimerId m_GetDataTid;

            /// 请求设备状态定时器标识
            HELIUM::TimerId m_QueryStateTid;

            /// JRU保活定时器标识(仅限于ATP-T)
            HELIUM::TimerId m_JruKeepaliveTid;

            /// UNIX域地址的绑定路径
            static const char *m_UnixBindPath;
    };
}

#endif //_DATA_HUB_H
