/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TimerCountdown.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月25日 15时37分30秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _TIMER_COUNTDOWN_H
#define _TIMER_COUNTDOWN_H

#include "TimeVal.h"

namespace HELIUM
{
    /**
     * 倒计时功能的实现。将要定时的变量CTimeVal传递给CTimerCountdown类,
     * 后者用自身的析构时间减去创建时间，得到系统的流逝时间。从CTimeVal
     * 中去除这部分流逝时间，即可实现倒计时功能。
     *
     */
    class CTimerCountdown
    {
        public:
            CTimerCountdown(CTimeVal *wait_time);
            ~CTimerCountdown();

        private:
            /// 最长等待时间
            CTimeVal *m_MaxWaitTime;

            /// 倒计时开始的时间
            CTimeVal m_StartTime;
    };

    inline CTimerCountdown::CTimerCountdown(CTimeVal *wait_time)
        : m_MaxWaitTime(wait_time), m_StartTime(CTimeVal::getMonotonicTime())
    {
    }

    inline CTimerCountdown::~CTimerCountdown()
    {
        if (m_MaxWaitTime == NULL)
            return;

        CTimeVal elapsed(CTimeVal::getMonotonicTime());
        elapsed -= m_StartTime;
        if (*m_MaxWaitTime > elapsed)
            *m_MaxWaitTime -= elapsed;
        else
            *m_MaxWaitTime = CTimeVal::zeroTime();
    }
}

#endif //_TIMER_COUNTDOWN_H
