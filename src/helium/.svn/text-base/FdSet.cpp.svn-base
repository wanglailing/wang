/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: FdSet.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月25日 10时37分53秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "FdSet.h"

using namespace HELIUM;

bool CFdSet::setFd(handler_t fd)
{
    FD_SET(fd, this);

    ActiveFdsIter iter;
    iter = std::find(m_ActiveFds.begin(), m_ActiveFds.end(), fd);
    if (iter == m_ActiveFds.end())
    {
        m_ActiveFds.push_back(fd);
    }

    return (true);
}

bool CFdSet::clear(handler_t fd)
{
    if (!isSet(fd))
    {
        DBG_LOG((REACTOR, "Not found %d. ignoring!\n", fd));
        return (false);
    }

    FD_CLR(fd, this);
    if (FD_ISSET(fd, this))
    {
        DBG_LOG((REACTOR, "FD_CLR failed!\n"));
    }

    ActiveFdsIter iter;
    iter = std::find(m_ActiveFds.begin(), m_ActiveFds.end(), fd);
    if (iter != m_ActiveFds.end())
    {
        m_ActiveFds.erase(iter);
    }

    return (true);
}

void CFdSet::sync(void)
{
    ActiveFdsIter iter;

restart:
    iter = m_ActiveFds.begin();
    while (iter != m_ActiveFds.end())
    {
        if (!isSet(*iter))
        {
            m_ActiveFds.erase(iter);
            goto restart;
        }
        iter++;
    }
}

void CFdSet::reset(void)
{
    memset(this, 0, sizeof(fd_set));
    m_ActiveFds.clear();
}

int CFdSet::maxInSet(void)
{
    if (m_ActiveFds.size() == 0)
    {
        return 0;
    }

    ActiveFdsIter iter = std::max_element(m_ActiveFds.begin(), m_ActiveFds.end());
    return (*iter);
}
