/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ReliableCtrl.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月14日 15时56分31秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _WIRELESS_CTRL_H
#define _WIRELESS_CTRL_H

#include <string>

#include "WindowTransClient.h"
#include "WindowTransServer.h"
#include "Buffer.h"

namespace TRANSPORTCTRL
{
    /**
     * 可靠传输控制。
     *
     */
    class CReliableCtrl
    {
        public:
            CReliableCtrl();
            ~CReliableCtrl();

            /**
             * 将业务数据经ZLIB压缩和BASE64编码后放入发送端缓冲区
             *
             * @param packet 业务数据
             *
             * @return
             */ 
            int procBusinessPacket(HELIUM::CBuffer* packet);

            /**
             * 处理接收的报文
             *
             * @param packet   报文内容
             * @param response 反馈缓冲区
             * @param buflen   反馈缓冲区容量
             *
             * @return 反馈信息的长度
             */ 
            int procReceivedPacket(std::string& packet, char *response, int buflen);

            /// 处理服务端已排序报文
            inline void procSortedPackets(void)
            {
                m_transServer->processPackets();
            }

            inline int getSendingPacket(char* dst, int dst_len)
            {
                return m_transClient->getSendingPacket(dst, dst_len);
            }

        private:
            CReliableCtrl(const CReliableCtrl&);
            CReliableCtrl& operator=(const CReliableCtrl&);

        private:
            /**
             * 重设客户端
             *
             */ 
            void resetClient(void);

            /**
             * 重设服务器
             *
             */ 
            void resetServer(void);

        private:
            /// 传输客户端
            CWTransClient *m_transClient;

            /// 传输服务器
            CWTransServer *m_transServer;
    };
}

#endif //_WIRELESS_CTRL_H
