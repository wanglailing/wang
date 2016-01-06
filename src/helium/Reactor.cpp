/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Reactor.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月24日 10时37分11秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <sys/time.h>
#include <sys/resource.h>

#include <iostream>
#include <sstream>
#include <string>

#include "Reactor.h"
#include "Logger.h"
#include "Assure.h"

using namespace HELIUM;

CReactor::CReactor() : m_FdSetsize(1024), m_MaxFdPlus1(0), m_bActive(true)
{
    struct rlimit rlim;
    rlim.rlim_max = 0;
    if (getrlimit(RLIMIT_NOFILE, &rlim) == 0)
    {
        m_FdSetsize = rlim.rlim_cur;
    }
}

CReactor::~CReactor()
{
    m_ReadHandlers.clear();
    m_WriteHandlers.clear();
    m_ExceptHandlers.clear();

    deactivate();
}

TimerId CReactor::registerTimerHandler(CEventHandler *eh, 
                                       const CTimeVal &tv, 
                                       const std::string &name)
{
    ASSERT_RETURN(eh);
    CTimeVal now(CTimeVal::getMonotonicTime());
    CTimeVal t(now + tv);

    TimerId tid = m_TimerQueue.insert(eh, t, tv, name);

    return (tid);
}

bool CReactor::registerIOHandler(CEventHandler *eh, handler_t fd, EventType type)
{
    std::ostringstream msg;
    ASSERT_RETURN(eh && !isSignalEvent(type) && !isTimeoutEvent(type));

    if (isReadEvent(type))
    {
        if (!m_WaitSet.m_ReadSet.setFd(fd))
        {
            DBG_LOG((FATALERR, "ReadSet: fd [%d] out of range!\n", fd));
            return (false);
        }

        m_ReadHandlers[fd] = eh;
        msg << "READ_EVENT";
    }

    if (isWriteEvent(type))
    {
        if (!m_WaitSet.m_WriteSet.setFd(fd))
        {
            DBG_LOG((FATALERR, "WriteSet: fd [%d] out of range!\n", fd));
            return (false);
        }

        m_WriteHandlers[fd] = eh;
        msg << "WRITE_EVENT";
    }

    if (isExceptEvent(type))
    {
        if (!m_WaitSet.m_ExceptSet.setFd(fd))
        {
            DBG_LOG((FATALERR, "ExceptSet: fd [%d] out of range!\n", fd));
            return (false);
        }

        m_ExceptHandlers[fd] = eh;
        msg << "EXCEPT_EVENT";
    }

    msg << std::ends;
    DBG_LOG((REACTOR, "Registered EvtH(%s) fd=[%d] (0x%x) for event(s) [%s]\n",
            eh->getId().c_str(), fd, (unsigned long)eh, msg.str().c_str()));

    if (m_MaxFdPlus1 < fd + 1)
    {
        m_MaxFdPlus1 = fd + 1;
    }

    return (true);
}

bool CReactor::removeHandler(CEventHandler *eh, EventType type)
{
    bool ret = false;
    handler_t fd = BAD_HANDLER;
    Fd2EhMapIter iter;

    if (eh == NULL)
    {
        return false;
    }

    if (isTimeoutEvent(type))
    {
        ret = m_TimerQueue.remove(eh);
        ret = true;
    }

    if (isReadEvent(type))
    {
        iter = m_ReadHandlers.begin();
        while (iter != m_ReadHandlers.end())
        {
            if ((*iter).second == eh)
            {
                fd = (*iter).first;
                m_ReadHandlers.erase(iter);
                m_WaitSet.m_ReadSet.clear(fd);
                ret = true;
                break;
            }
            iter++;
        }
    }

    if (isWriteEvent(type))
    {
        iter = m_WriteHandlers.begin();
        while (iter != m_WriteHandlers.end())
        {
            if ((*iter).second == eh)
            {
                fd = (*iter).first;
                m_WriteHandlers.erase(iter);
                m_WaitSet.m_WriteSet.clear(fd);
                ret = true;
                break;
            }
            iter++;
        }
    }

    if (isExceptEvent(type))
    {
        iter = m_ExceptHandlers.begin();
        while (iter != m_ExceptHandlers.end())
        {
            if ((*iter).second == eh)
            {
                fd = (*iter).first;
                m_ExceptHandlers.erase(iter);
                m_WaitSet.m_ExceptSet.clear(fd);
                ret = true;
                break;
            }
            iter++;
        }
    }

    if (ret == true)
    {
        eh->handleClose(fd);
    }

    adjustMaxFdp1(fd);
    return (ret);
}

bool CReactor::removeTimerHandler(TimerId tid)
{
    bool ret;

    if ( (ret = m_TimerQueue.remove(tid)) )
    {
    }
    else
    {
        ERR_LOG((FATALERR, "TimerId [0x%x] was not found!\n", (unsigned long)tid));
    }

    return ret;
}

bool CReactor::removeIOHandler(handler_t fd)
{
    bool ret = false;
    CEventHandler *ehp = NULL;
    Fd2EhMapIter iter;

    ASSERT_RETURN(HELIUM::isValidHandler(fd));

    if ( (iter = m_ReadHandlers.find(fd)) != m_ReadHandlers.end() )
    {
        ehp = (*iter).second;
        m_ReadHandlers.erase(iter);
        m_WaitSet.m_ReadSet.clear(fd);
        m_ReadySet.m_ReadSet.clear(fd);
        if (m_ReadHandlers.size() > 0)
        {
            iter = m_ReadHandlers.end();
            iter--;
        }
        ret = true;
    }

    if ( (iter = m_WriteHandlers.find(fd)) != m_WriteHandlers.end() )
    {
        ehp = (*iter).second;
        m_WriteHandlers.erase(iter);
        m_WaitSet.m_WriteSet.clear(fd);
        m_ReadySet.m_WriteSet.clear(fd);
        if (m_WriteHandlers.size() > 0)
        {
            iter = m_WriteHandlers.end();
            iter--;
        }
        ret = true;
    }

    if ( (iter = m_ExceptHandlers.find(fd)) != m_ExceptHandlers.end() )
    {
        ehp = (*iter).second;
        m_ExceptHandlers.erase(iter);
        m_WaitSet.m_ExceptSet.clear(fd);
        m_ReadySet.m_ExceptSet.clear(fd);
        if (m_ExceptHandlers.size() > 0)
        {
            iter = m_ExceptHandlers.end();
            iter--;
        }
        ret = true;
    }

    if (ret == true && ehp != NULL)
    {
        DBG_LOG((REACTOR, "Remove event handler <%s> (%p)\n", ehp->getId().c_str(), ehp));
        ehp->handleClose(fd);
    }

    adjustMaxFdp1(fd);
    return (ret);
}

void CReactor::adjustMaxFdp1(handler_t fd)
{
    if ( m_MaxFdPlus1 == (fd + 1) )
    {
        m_MaxFdPlus1 = m_WaitSet.maxFd() + 1;
        DBG_LOG((REACTOR, "Adjust max fd to %d\n", m_MaxFdPlus1));
    }
}

void CReactor::waitForEvents(void)
{
    while (m_bActive)
    {
        waitForEvents((CTimeVal *)NULL);
    }
}

void CReactor::waitForEvents(CTimeVal *tv)
{
    CTimerCountdown traceTime(tv);
    m_TimerQueue.expire(CTimeVal::getMonotonicTime());
    if (!m_bActive)
    {
        return;
    }

    int ready;
    CTimeVal delay;
    CTimeVal *dlp = &delay;

    if ( (ready = isAnyReady()) )
    {
        DBG_LOG((REACTOR, "isAnyReady() returned: %d\n", ready));
        dispatch(ready);
        return;
    }

    do
    {
        m_ReadySet.reset();
        m_ReadySet = m_WaitSet;

        calcTimeout(dlp, tv);

        /**
         * 在没有需要检测的文件描述符时，select()相当于是一个定时器
         */
        ready = select(m_MaxFdPlus1, 
                                    &m_ReadySet.m_ReadSet, 
                                    &m_ReadySet.m_WriteSet, 
                                    &m_ReadySet.m_ExceptSet, 
                                    dlp);
        m_ReadySet.sync();
    } while (ready < 0 && handleError());

    dispatch(ready);
}

void CReactor::stopReactor(void)
{
    m_bActive = false;
    Fd2EhMapIter iter;
    CEventHandler *ehp;

    while (m_ReadHandlers.size() > 0)
    {
        iter = m_ReadHandlers.begin();
        ehp = (*iter).second;
        removeHandler(ehp);
    }

    while (m_WriteHandlers.size() > 0)
    {
        iter = m_WriteHandlers.begin();
        ehp = (*iter).second;
        removeHandler(ehp);
    }

    while (m_ExceptHandlers.size() > 0)
    {
        iter = m_ExceptHandlers.begin();
        ehp = (*iter).second;
        removeHandler(ehp);
    }
}

bool CReactor::handleError(void)
{
    if (!m_bActive)
    {
        DBG_LOG((REACTOR, "Received cmd to stop the reactor!\n"));
        return (false);
    }

    if (errno == EINTR)
    {
        return (true);
    }

    if (errno == EBADF)
    {
        DBG_LOG((REACTOR, "Reactor: BAD file descriptors!\n"));
        return (checkFDs());
    }

    ERR_LOG((REACTOR, "Reactor: select(3) error!\n"));
    return (false);
}

bool CReactor::dispatch(int ready_fds)
{
    m_TimerQueue.expire(CTimeVal::getMonotonicTime());

    if (ready_fds < 0)
    {
        ERR_LOG((FATALERR, "select(3) error!\n"));
        return (false);
    }
    else if (ready_fds == 0)
    {
        return (true);
    }

    /**
     * 写->异常->读
     *
     */
    dispatchHandler(m_ReadySet.m_WriteSet, m_WriteHandlers, &CEventHandler::handleWrite);
    dispatchHandler(m_ReadySet.m_ExceptSet, m_ExceptHandlers, &CEventHandler::handleException);
    dispatchHandler(m_ReadySet.m_ReadSet, m_ReadHandlers, &CEventHandler::handleRead);

    return (true);
}

void CReactor::dispatchHandler(CFdSet &mask, Fd2EhMapType &fdehs, EH_IO_CALLBACK callback)
{
    int ret = 0;
    handler_t fd;
    CEventHandler *ehp;
    std::string eh_id;

    Fd2EhMapIter iter = fdehs.begin();
    while (iter != fdehs.end())
    {
        fd = (*iter).first;
        ehp = (*iter).second;

        if (mask.isSet(fd) && ehp != NULL)
        {
            eh_id = ehp->getId();
            ret = (ehp->*callback)(fd);
            if (ret == -1)
            {
                removeIOHandler(fd);
            } 
            else if (ret > 0)
            {
                DBG_LOG((REACTOR, "%d bytes pending on fd=%d <%s>\n", 
                        ret, fd, eh_id.c_str()));
            }
            else
            {
                mask.clear(fd);
            }

            iter = fdehs.begin();
        }
        else
        {
            iter++;
        }
    }
}

int CReactor::isAnyReady(void)
{
    int n = m_ReadySet.m_ReadSet.numSet() +
            m_ReadySet.m_WriteSet.numSet() +
            m_ReadySet.m_ExceptSet.numSet();

    return (n);
}

bool CReactor::checkFDs(void)
{
    bool num_removed = false;
    CFdSet mask;
    timeval poll = {0, 0};

    for (handler_t fd = 0; fd < m_FdSetsize; ++fd)
    {
        if (m_ReadHandlers[fd] != NULL)
        {
            mask.setFd(fd);
            if (select(fd + 1, &mask, NULL, NULL, &poll) < 0)
            {
                removeIOHandler(fd);
                num_removed = true;
            }
            mask.clear(fd);
        }
    }

    return (num_removed);
}

void CReactor::calcTimeout(CTimeVal*& howlong, CTimeVal *maxwait)
{
    CTimeVal now;
    CTimeVal tv;

    if (m_TimerQueue.isEmpty())
    {
        howlong = maxwait;
        return;
    }

    now = CTimeVal::getMonotonicTime();
    tv = m_TimerQueue.top();
    if (tv < now)
    {
        *howlong = 0;
    }
    else
    {
        if (maxwait == NULL || *maxwait == CTimeVal::zeroTime())
        {
            *howlong = tv - now;
        }
        else
        {
            *howlong = ((*maxwait + now) < tv) ? *maxwait : (tv - now);
        }
    }
}
