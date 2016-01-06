/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataSink.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月05日 10时33分03秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DATA_SINK_H
#define _DATA_SINK_H

#include "Reactor.h"
#include "TcpSocket.h"
#include "Acceptor.h"
#include "DataService.h"
#include "Buffer.h"
#include "TcasProcessor.h"
#include "../DmsBusinessDat.h"

namespace TRANSPORTCTRL
{
    /**
     * 接受业务处理客户端的连接，收集并处理各种业务数据。
     *
     */
    class CDataSink
    {
        friend class CDataService;

        public:
            typedef HELIUM::CAcceptor<CDataService, HELIUM::CTcpSocket> ACCEPTOR_TYPE;

        public:
            explicit CDataSink(HELIUM::CReactor *r);
            ~CDataSink();

            /**
             * 初始化数据交换服务。
             *
             * @return 0 -- 成功; -1 -- 失败
             */
            int init(void);

            /// 合并基础信息包和其它业务数据包
            void combinePackets(void);

            /// 封装数据包
            void pushPacket(void);

            uint64_t getPushStamp(void) { return m_PushStamp; }

            /// 设置ATP类型标识
            void setAtpId(unsigned char atp_id) { m_BusinessHeader.atpid = atp_id; }

            /**
             * 向扩展模块转发数据
             *
             * @param dst_id   扩展模块的标识
             * @param data_id  数据标识
             * @param data     数据地址
             * @param length   数据长度
             *
             */
            void forwardData(uint8_t dst_id, unsigned short data_id, const char* data, int length);

            /**
             * 向扩展模块转发命令
             *
             * @param psubhdr 子包头部 
             * @param source  数据源标识
             * @param command 命令地址
             * @param cmdlen  命令长度
             *
             * @return 0 -- 转发成功；-1 -- 转发失败
             *
             */
            int forwardCommand(const uint8_t* psubhdr, unsigned short source, const char* command, int cmdlen);

            CTcasProc* getTcaser(void) { return m_TcasProc; }

        private:
            CDataSink(const CDataSink&);
            CDataSink& operator=(const CDataSink&);

        private:
            /// 反应堆对象
            HELIUM::CReactor *m_Reactor;

            /// 业务数据发送者接受器
            ACCEPTOR_TYPE *m_DataAcceptor;

            /// 业务数据缓冲区
            HELIUM::CBuffer m_BusinessDataBuf;

            /// 完整的封装数据缓冲区
            HELIUM::CBuffer m_FinalBuf;

            /// 与扩展模块交互的缓冲区
            HELIUM::CExchangePacket m_ExchData;

            /// TCAS业务处理
            CTcasProc* m_TcasProc;

            /// 业务数据封包头部
            BUSINESS_HDR m_BusinessHeader;

            /// 封装数据包的时间戳
            uint64_t m_PushStamp;

            /// 超过上限后，将报文发送出去
            static const unsigned int BufUpperbound;

            /// UNIX域地址的绑定路径
            static const char *m_UnixBindPath;
    };
}

#endif //_DATA_SINK_H
