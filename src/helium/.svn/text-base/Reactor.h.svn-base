/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Reactor.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月18日 11时00分45秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _REACTOR_H
#define _REACTOR_H

#include <map>

#include "EventHandler.h"
#include "FdSet.h"
#include "MaskSet.h"
#include "TimerQueue.h"
#include "TimerCountdown.h"

namespace HELIUM
{
    /**
     * 反应堆模式的实现
     *
     */
    class CReactor
    {
        public:
            CReactor();
            ~CReactor();

            /**
             * 注册定时器事件响应器
             *
             * @param eh 定时器事件响应器
             * @param tv 定时间隔
             * @param name 定时器名称
             *
             * @return 定时器ID
             */
            TimerId registerTimerHandler(CEventHandler *eh, const CTimeVal &tv, const std::string &name = "unknown");

            /**
             * 注册I/O事件响应器
             *
             * @param eh I/O事件响应器
             * @param fd 文件描述符
             * @param type 事件类型
             *
             * @return true -- 成功；false -- 失败
             */
            bool registerIOHandler(CEventHandler *eh, handler_t fd, EventType type = RWE_EVENT);

            /**
             * 删除事件响应器
             *
             * @param eh 事件响应器
             * @param type 事件类型
             *
             * @return true -- 成功；false -- 失败
             */
            bool removeHandler(CEventHandler *eh, EventType type = ALL_EVENTS);

            /**
             * 删除定时器事件响应器
             *
             * @param tid 定时器ID
             *
             * @return true -- 成功；false -- 失败
             */
            bool removeTimerHandler(TimerId tid);

            /**
             * 删除I/O事件响应器
             *
             * @param fd 文件描述符
             *
             * @return true -- 成功；false -- 失败
             */
            bool removeIOHandler(handler_t fd);

            /// 永久阻塞并处理事件的主循环
            void waitForEvents(void);

            /**
             * 在一定时间内等待事件的发生
             *
             * @param tv 等待时长
             *
             */
            void waitForEvents(CTimeVal *tv);

            /// 停止反应堆
            void stopReactor(void);

            void deactivate(void);

        private:
            /// 禁止拷贝
            CReactor(const CReactor&);
            CReactor& operator=(const CReactor&);

        private:
            typedef std::map<unsigned int, CEventHandler*> Fd2EhMapType;    /// 定义文件描述符到事件响应器的映射类型
            typedef Fd2EhMapType::iterator Fd2EhMapIter;            /// 定义映射的迭代器

        private:
            /// 捕捉select()循环中的错误
            bool handleError(void);

            /**
             * 将已发生的事件通知到合适的事件响应器
             *
             * @param ready_fds 已就绪的文件描述符
             */
            bool dispatch(int ready_fds);

            /// 返回已就绪的文件描述符的个数
            int isAnyReady(void);

            bool checkFDs(void);

            /// 调用事件响应器的回调函数
            void dispatchHandler(CFdSet &mask, Fd2EhMapType &fdehs, EH_IO_CALLBACK callback);

            /**
             * 计算最近的超时。如果定时器队列非空，返回...
             * 
             * @param howlong (in) 期望等待的时长
             * @param maxwait (out) 实际等待的时长
             */
            void calcTimeout(CTimeVal*& howlong, CTimeVal *maxwait);

            /**
             * 调整最大的文件描述符
             * 
             * @param fd 被删除的文件描述符
             */
            void adjustMaxFdp1(handler_t fd);

        private:
            /// 一个进程能够打开的最大描述符个数
            int m_FdSetsize;

            /// 最大文件描述符加1
            handler_t m_MaxFdPlus1;

            /// 反应堆是否处于活动状态的标志
            bool m_bActive;

            /// 等待读事件的响应器
            Fd2EhMapType m_ReadHandlers;

            /// 等待写事件的响应器
            Fd2EhMapType m_WriteHandlers;

            /// 等待异常事件的响应器
            Fd2EhMapType m_ExceptHandlers;

            /// 等待描述符集
            CMaskSet m_WaitSet;

            /// 就绪描述符集
            CMaskSet m_ReadySet;

            /// 定时器队列
            CTimerQueue m_TimerQueue;
    };

    inline void CReactor::deactivate(void) { m_bActive = false; }
}

#endif //_REACTOR_H
