/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TimerQueue.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月20日 10时35分26秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _TIMER_QUEUE_H
#define _TIMER_QUEUE_H

#include "EventHandler.h"
#include "PriorityQueue.h"
#include "Timer.h"

namespace HELIUM
{
    /**
     * 定时器队列
     *
     */
    class CTimerQueue
    {
        public:
            CTimerQueue();
            ~CTimerQueue();

            /**
             * 定时器队列是否为空
             *
             * @return true -- 空；false -- 非空
             */
            bool isEmpty(void);

            /**
             * 插入定时器
             *
             * @param eh    事件响应器
             * @param tv    绝对超时时间
             * @param delta 相对超时时间
             * @param name  定时器名
             *
             * @return 定时器ID
             */
            TimerId insert(CEventHandler *eh, const CTimeVal &tv, const CTimeVal &delta, const std::string &name);

            /**
             * 取消针对事件响应器eh的所有定时器
             *
             * @param eh    事件响应器
             *
             * @return 被取消的定时器个数
             */
            int remove(CEventHandler *eh);

            /**
             * 取消某个定时器
             *
             * @param tid 定时器ID
             *
             * @return true -- 定时器在队列中被找到; false -- 其它
             */
            bool remove(TimerId tid);

            /**
             * 遍历队列，触发时间在tv之前的所有定时器。执行完定时器回调后，定时器将被删除
             *
             * @param tv 过期时间
             *
             * @return 被派发的定时器回调的个数
             */
            int expire(const CTimeVal &tv);

            /// 返回队列顶端定时器的过期时间
            CTimeVal& top(void);

            /// 将队列消息转储到日志文件
            void dump(void);

        private:
            /// 定时器队列自身
            CPriorityQueue<CTimer*, TimerCompare> m_Queue;
    };

    inline CTimerQueue::CTimerQueue()
    {
    }

    inline bool CTimerQueue::isEmpty(void)
    {
        return (m_Queue.size() == 0);
    }

    inline CTimeVal& CTimerQueue::top(void)
    {
        return (CTimeVal&)m_Queue.top()->getExpireTime();
    }
}

#endif //_TIMER_QUEUE_H
