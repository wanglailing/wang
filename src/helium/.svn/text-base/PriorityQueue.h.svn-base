/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PriorityQueue.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月20日 11时12分49秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _PRIORITY_QUEUE_H
#define _PRIORITY_QUEUE_H

#include "PriorityQueueImpl.h"
#include "PriorityQueueHeap.h"

namespace HELIUM
{
    template<class T, class Compare> class CPriorityQueueImpl;

    template<class T, class Compare>
    class CPriorityQueue
    {
        public:
            CPriorityQueue(size_t max = 20);
            CPriorityQueue(size_t max, const Compare&);

            virtual ~CPriorityQueue();

            virtual void insert(const T&);
            virtual T pop(void);
            virtual const T& top(void) const;
            virtual bool remove(T&);
            virtual size_t size(void);
            virtual T& operator[](int);

            virtual void setHeapImpl(size_t, const Compare&);

        protected:
            /// 禁止拷贝
            CPriorityQueue(const CPriorityQueue&);
            CPriorityQueue& operator=(const CPriorityQueue&);

            const CPriorityQueueImpl<T, Compare>* getPriorityQueueImpl() const
            {
                return (const CPriorityQueueImpl<T, Compare>*)m_Impl;
            }

            Compare m_Compare;

        private:
            /// 优先队列的具体实现
            CPriorityQueueImpl<T, Compare>* m_Impl;
    };

    template<class T, class Compare>
    inline CPriorityQueue<T, Compare>::CPriorityQueue(size_t max)
        : m_Impl(NULL)
    {
        setHeapImpl(max, m_Compare);
    }

    template<class T, class Compare>
    inline CPriorityQueue<T, Compare>::CPriorityQueue(size_t max, const Compare& x)
        : m_Compare(x), m_Impl(NULL)
    {
        setHeapImpl(max, m_Compare);
    }

    template<class T, class Compare>
    inline CPriorityQueue<T, Compare>::~CPriorityQueue()
    {
        delete m_Impl;
    }

    template<class T, class Compare>
    inline void CPriorityQueue<T, Compare>::insert(const T& elem)
    {
        m_Impl->insert(elem);
    }

    template<class T, class Compare>
    inline T CPriorityQueue<T, Compare>::pop(void)
    {
        return m_Impl->pop();
    }

    template<class T, class Compare>
    inline const T& CPriorityQueue<T, Compare>::top(void) const
    {
        return m_Impl->top();
    }

    template<class T, class Compare>
    inline bool CPriorityQueue<T, Compare>::remove(T& elem)
    {
        return m_Impl->remove(elem);
    }

    template<class T, class Compare>
    inline size_t CPriorityQueue<T, Compare>::size(void)
    {
        return m_Impl->size();
    }

    template<class T, class Compare>
    inline T& CPriorityQueue<T, Compare>::operator[](int idx)
    {
        return (*m_Impl)[idx];
    }

    template<class T, class Compare>
    inline void CPriorityQueue<T, Compare>::setHeapImpl(size_t maxsz, 
                                                        const Compare& x)
    {
        if (m_Impl != NULL)
            delete m_Impl;

        m_Impl = new CPriorityQueueHeap<T, Compare>(maxsz, x);
    }
}

#endif //_PRIORITY_QUEUE_H
