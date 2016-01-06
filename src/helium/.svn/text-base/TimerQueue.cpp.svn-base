/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TimerQueue.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月25日 16时12分46秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "TimerQueue.h"
#include "Logger.h"

using namespace HELIUM;

CTimerQueue::~CTimerQueue()
{
    while (m_Queue.size())
    {
        delete m_Queue.pop();
    }
}

TimerId CTimerQueue::insert(CEventHandler *eh, const CTimeVal &tv, const CTimeVal &delta, const std::string &name)
{
    CTimer *t = new CTimer(eh, tv, delta, name);
    m_Queue.insert(t);
    return (TimerId)t;
}

int CTimerQueue::remove(CEventHandler *eh)
{
    int cnt = 0;
    register size_t i;
    bool scan_flag = true;
    register CTimer *tmr;

    while (scan_flag == true)
    {
        scan_flag = false;
        for (i = 0; i < m_Queue.size(); ++i)
        {
            if (m_Queue[i]->getHandler() == eh)
            {
                tmr = m_Queue[i];
                m_Queue.remove(tmr);
                delete tmr;
                cnt++;
                scan_flag = true;
            }
        }
    }

    return (cnt);
}

bool CTimerQueue::remove(TimerId tid)
{
    register size_t i;
    for (i = 0; i < m_Queue.size(); ++i)
    {
        if (m_Queue[i] == (CTimer *)tid)
        {
            CTimer *tmr = m_Queue[i];
            int ret = m_Queue.remove(tmr);
            delete tmr;
            return (ret);
        }
    }

    return (false);
}

int CTimerQueue::expire(const CTimeVal &tv)
{
    register CTimer *tp = (CTimer *)NULL;
    register int cnt = 0;

    while ( m_Queue.size() && ((tp = m_Queue.top()) != (CTimer *)NULL) )
    {
        if (tp->getExpireTime() > tv)
        {
            break;
        }

        m_Queue.pop();

        int ret = tp->getHandler()->handleTimeout((TimerId)tp);
        if (ret == 1)
        {
            tp->rescheduleExpireTime();
            m_Queue.insert(tp);
        }
        else
        {
            delete tp;
            tp = (CTimer *)NULL;
        }

        cnt++;
    }

    return (cnt);
}

void CTimerQueue::dump(void)
{
    if (m_Queue.size() == 0) 
    {
        DBG_LOG((REACTOR,"Queue is empty\n"));
    }
    else 
    {
        for (size_t i = 0; i < m_Queue.size(); ++i) 
        {
            m_Queue[i]->dump();
        }
    }
}

