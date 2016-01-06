/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Timer.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月20日 14时07分00秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _TIMER_H
#define _TIMER_H

#include <sys/time.h>

#include "EventHandler.h"
#include "TimeVal.h"
#include "Logger.h"

namespace HELIUM
{
    /**
     * 定时器类
     *
     */
    class CTimer
    {
        public:
            /// 缺省构造函数
            CTimer();

            /**
             * 由定时器队列调用的构造函数
             *
             * @param eh    事件响应器
             * @param tv    绝对超时时间
             * @param delta 相对超时时间
             * @param name  定时器名
             *
             */
            CTimer(const CEventHandler *eh, const CTimeVal &tv, const CTimeVal &delta, const std::string &name);

            /// 复制构造函数
            CTimer(const CTimer &t);

            ~CTimer();

            CTimer& operator=(const CTimer &t);

            bool operator<(const CTimer &t) const;
            bool operator==(const CTimer &t) const;

            /// 获取事件响应器指针
            CEventHandler* getHandler(void) const { return m_evHandler; }

            /// 获取过期时间
            const CTimeVal& getExpireTime(void) const { return m_trigTime; }

            /// 获取相对超时时间
            const CTimeVal& getDeltaTime(void) const { return m_delta; }

            /// 使用delta值重设过期时间
            void rescheduleExpireTime(void);

            /// 将定时器信息转储到日志
            void dump(void);

            /// 设置定时器ID
            void setId(const std::string &id) { m_Id = id; }

            /// 获取定时器ID
            std::string getId(void) const { return m_Id; }

        private:
            /// 定时器事件响应器
            CEventHandler *m_evHandler;

            /// 定时器触发时间
            CTimeVal m_trigTime;

            /// 反应器需要重设定时器时用到的相对时间
            CTimeVal m_delta;

            /// 定时器ID
            std::string m_Id;
    };

    inline CTimer::CTimer() : m_evHandler(NULL), m_Id("<unknown>")
    {
    }

    inline CTimer::CTimer(const CEventHandler *eh, const CTimeVal &tv, 
            const CTimeVal &delta, const std::string &name) : 
        m_evHandler((CEventHandler *)eh), m_trigTime(tv), m_delta(delta), m_Id(name)
    {
    }

    inline CTimer::CTimer(const CTimer &t) :
        m_evHandler(t.m_evHandler), m_trigTime(t.m_trigTime), 
        m_delta(t.m_delta), m_Id(t.m_Id)
    {
    }

    inline CTimer::~CTimer()
    {
    }

    inline CTimer& CTimer::operator=(const CTimer &t)
    {
        m_evHandler = t.m_evHandler;
        m_trigTime = t.m_trigTime;
        m_delta = t.m_delta;
        m_Id = t.m_Id;

        return *this;
    }

    inline bool CTimer::operator<(const CTimer &t) const
    {
        return m_trigTime < t.m_trigTime;
    }

    inline bool CTimer::operator==(const CTimer &t) const
    {
        return m_trigTime == t.m_trigTime;
    }

    inline void CTimer::rescheduleExpireTime(void)
    {
        CTimeVal now(CTimeVal::getMonotonicTime());
        m_trigTime = now + m_delta;
    }

    inline void CTimer::dump(void)
    {
        DBG_LOG((REACTOR, "Timer %s (EH=%s) expires at %s (delta=%s)\n",
                    getId().c_str(),
                    m_evHandler->getId().c_str(), 
                    m_trigTime.fmtHHMMSS().c_str(),
                    m_delta.fmtMMSSMLS().c_str()));
    }

    /**
     * 基于过期时间比较两个定时器
     *
     */
    struct TimerCompare
    {
        bool operator() (const CTimer *t1, const CTimer *t2) const
        {
            return *t1 < *t2;
        }
    };
}

#endif //_TIMER_H
