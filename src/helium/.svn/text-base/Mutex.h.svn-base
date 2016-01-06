/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Mutex.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月25日 11时27分31秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _MUTEX_H
#define _MUTEX_H

#include <pthread.h>
#include <errno.h>

namespace HELIUM
{
    /**
     * 互斥锁类
     *
     */
    class CMutex
    {
        public:
            inline CMutex()
            {
                pthread_mutex_init(&m_Mutex, NULL);
            }

            inline ~CMutex()
            {
                pthread_mutex_destroy(&m_Mutex);
            }

            inline void lock(void)
            {
                pthread_mutex_lock(&m_Mutex);
            }

            inline bool tryLock(void)
            {
                int rc = pthread_mutex_trylock(&m_Mutex);
                if (rc == EBUSY)
                    return false;

                return true;
            }

            inline void unlock(void)
            {
                pthread_mutex_unlock(&m_Mutex);
            }

        private:
            CMutex(const CMutex&);
            CMutex& operator=(const CMutex&);

        private:
            pthread_mutex_t m_Mutex;
    };

    /**
     * 锁定某个范围
     *
     */
    class CScopedLock
    {
        public:
            explicit CScopedLock(CMutex& mutex) : m_Mutex(mutex)
            {
                m_Mutex.lock();
            }

            ~CScopedLock()
            {
                m_Mutex.unlock();
            }

        private:
            CScopedLock(const CScopedLock&);
            CScopedLock& operator=(const CScopedLock&);

            CMutex& m_Mutex;
    };
}

#endif //_MUTEX_H
