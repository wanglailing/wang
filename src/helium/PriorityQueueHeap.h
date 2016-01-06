/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PriorityQueueHeap.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月26日 08时54分12秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _PRIORITYQUEUE_HEAP_H
#define _PRIORITYQUEUE_HEAP_H

#include <sys/types.h>
#include <string.h>

#include "PriorityQueueImpl.h"

namespace HELIUM
{
    template<class T, class Compare>
    class CPriorityQueueHeap : public CPriorityQueueImpl<T, Compare>
    {
        public:
            CPriorityQueueHeap(size_t max = 0);
            CPriorityQueueHeap(size_t max, const Compare& x);
            CPriorityQueueHeap(const CPriorityQueueHeap&);

            ~CPriorityQueueHeap();

            CPriorityQueueHeap& operator=(const CPriorityQueueHeap&);

            void insert(const T&);
            T pop(void);
            const T& top(void);
            bool remove(T);
            size_t size(void);
            T& operator[](int idx);

            void dump(void);

        protected:
            void upheap(size_t);
            void downheap(size_t);
            bool resize(size_t);

            Compare m_Comp;

        private:
            /// 为队列分配内存空间
            void allocate(size_t);

            /// 由数组表示的队列的地址 
            T* m_Queue;

            /// 数组大小
            size_t m_Size;

            /// 下一个可用的槽
            size_t m_Curr;

            /// 低水平标志
            size_t m_LWM;
    };

    template<class T, class Compare>
    CPriorityQueueHeap<T, Compare>::CPriorityQueueHeap(size_t max)
        : m_Curr(1), m_LWM(20)
    {
        allocate(max);
    }

    template<class T, class Compare>
    CPriorityQueueHeap<T, Compare>::CPriorityQueueHeap(size_t max, const Compare& x)
        : m_Comp(x), m_Curr(1), m_LWM(20)
    {
        allocate(max);
    }

    template<class T, class Compare>
    CPriorityQueueHeap<T, Compare>::CPriorityQueueHeap(const CPriorityQueueHeap& p)
        : m_Comp(p.m_Comp), m_Size(p.m_Size), m_Curr(p.m_Curr), m_LWM(p.m_LWM)
    {
        allocate(m_Size);
        memcpy(m_Queue, p.m_Queue, sizeof(T) * m_Curr);
    }

    template<class T, class Compare>
    CPriorityQueueHeap<T, Compare>::~CPriorityQueueHeap()
    {
        delete [] m_Queue;
    }

    template<class T, class Compare>
    CPriorityQueueHeap<T, Compare>& CPriorityQueueHeap<T, Compare>::operator=(const CPriorityQueueHeap& p)
    {
        delete [] m_Queue;
        m_Comp = p.m_Comp;
        m_Size = p.m_Size;
        m_Curr = p.m_Curr;
        m_LWM = p.m_LWM;

        allocate(m_Size);
        memcpy(m_Queue, p.m_Queue, sizeof(T) * m_Curr);

        return (*this);
    }

    template<class T, class Compare>
    void CPriorityQueueHeap<T, Compare>::insert(const T& t)
    {
        if ((m_Curr + 1) == m_Size)
        {
            resize(m_Size * 2);
        }

        m_Queue[m_Curr] = t;
        upheap(m_Curr);
        m_Curr++;
    }

    template<class T, class Compare>
    T CPriorityQueueHeap<T, Compare>::pop(void)
    {
        T v = m_Queue[1];
        m_Queue[1] = m_Queue[--m_Curr];

        downheap(1);
        if ((m_Curr * 3 <= m_Size) && (m_Curr * 2 >= m_LWM))
        {
            resize(m_Curr * 2);
        }

        return (v);
    }

    template<class T, class Compare>
    const T& CPriorityQueueHeap<T, Compare>::top(void)
    {
        return (const T&)m_Queue[1];
    }

    template<class T, class Compare>
    bool CPriorityQueueHeap<T, Compare>::remove(T t)
    {
        register size_t i;
        for (i = 1; i < m_Curr; ++i)
        {
            if (m_Queue[i] == t)
                break;
        }

        /// 没有找到
        if (i == m_Curr)
            return false;

        m_Curr--;
        if (i == m_Curr)
            return true;    /// 被删除的是最后一个元素

        m_Queue[i] = m_Queue[m_Curr];
        downheap(i);

        return true;
    }

    template<class T, class Compare>
    size_t CPriorityQueueHeap<T, Compare>::size(void)
    {
        return (m_Curr - 1);
    }

    template<class T, class Compare>
    T& CPriorityQueueHeap<T, Compare>::operator[](int idx)
    {
        return m_Queue[idx + 1];
    }

    template<class T, class Compare>
    void CPriorityQueueHeap<T, Compare>::dump(void)
    {
    }

    template<class T, class Compare>
    void CPriorityQueueHeap<T, Compare>::upheap(size_t idx)
    {
        T v = m_Queue[idx];
        m_Queue[0] = 0;

        while ( idx / 2 != 0 && m_Comp(v, m_Queue[idx / 2]) )
        {
            m_Queue[idx] = m_Queue[idx / 2];
            idx /= 2;
        }
        m_Queue[idx] = v;
    }

    template<class T, class Compare>
    void CPriorityQueueHeap<T, Compare>::downheap(size_t idx)
    {
        register size_t j;
        T v = m_Queue[idx];

        while (idx <= m_Curr / 2)
        {
            j = idx * 2;
            if ( j < m_Curr && m_Comp(m_Queue[j + 1], m_Queue[j]) )
                j++;

            if (m_Comp(v, m_Queue[j]))
                break;

            m_Queue[idx] = m_Queue[j];
            idx = j;
        }
        m_Queue[idx] = v;
    }

    template<class T, class Compare>
    bool CPriorityQueueHeap<T, Compare>::resize(size_t newsz)
    {
        if (m_Size == newsz)
            return true;

        T *new_trunk = new T[newsz];
        memcpy(new_trunk, m_Queue, sizeof(T) * m_Curr);
        delete [] m_Queue;
        m_Queue = new_trunk;
        m_Size = newsz;

        return true;
    }

    template<class T, class Compare>
    void CPriorityQueueHeap<T, Compare>::allocate(size_t s)
    {
        m_Size = s > m_LWM ? s : m_LWM;
        m_Queue = new T[m_Size];
    }
}

#endif //_PRIORITYQUEUE_HEAP_H
