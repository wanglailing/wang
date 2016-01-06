/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: WindowTransport.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月14日 10时18分25秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _WINDOW_TRANSPORT_H
#define _WINDOW_TRANSPORT_H

#include "PacketBuffer.h"
#include "DataCoder.h"

namespace TRANSPORTCTRL
{
    /**
     * 窗口传输机制的抽象接口。
     *
     */
    class CWindowTrans
    {
        public:
            CWindowTrans() : m_bConnected(false)
            {
                m_Buffer = new CPacketBuff();
            }

            virtual ~CWindowTrans()
            {
                delete m_Buffer; m_Buffer = NULL;
            }

        private:
            /// 禁止拷贝和赋值
            CWindowTrans(const CWindowTrans&);
            CWindowTrans& operator=(const CWindowTrans&);

        protected:
            /**
             * 复位窗口传输状态
             *
             */ 
            virtual void reset(void) = 0;

        protected:
            /// 逻辑连接是否建立
            bool m_bConnected;

            /// 报文缓冲区
            CPacketBuff* m_Buffer;

            /// 传输窗口大小
            static const int TransWindowSize;
    };
}

#endif //_WINDOW_TRANSPORT_H
