/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: WindowTransClient.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月14日 10时47分34秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _WINDOWTRANS_CLIENT_H
#define _WINDOWTRANS_CLIENT_H

#include "WindowTransport.h"

namespace TRANSPORTCTRL
{
    /**
     * 窗口传输客户端。
     *
     */
    class CWTransClient : public CWindowTrans
    {
        public:
            CWTransClient();
            ~CWTransClient();

            /// 插入报文
            void insertPacket(std::string* packet);

            /// 获取待发送的报文
            int getSendingPacket(char* dst, int dst_len);

            /**
             * 解析确认报文
             *
             * @param ack 确认报文
             */ 
            void parseACK(std::string& ack);

        protected:
            void reset(void);

        private:
            CWTransClient(const CWTransClient&);
            CWTransClient& operator=(const CWTransClient&);

        private:
            /// 待发送的报文的标识符
            unsigned int m_SendID;

            /// 当前报文的标识符
            unsigned int m_CurrentID;
    };
}

#endif //_WINDOWTRANS_CLIENT_H
