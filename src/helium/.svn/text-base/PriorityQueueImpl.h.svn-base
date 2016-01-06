/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PriorityQueueImpl.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月24日 09时34分51秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _PRIORITYQUEUE_IMPLEMENT_H
#define _PRIORITYQUEUE_IMPLEMENT_H

namespace HELIUM
{
    /**
     * 优先队列的抽象接口
     *
     */
    template<class T, class Compare>
    class CPriorityQueueImpl
    {
        public:
            virtual ~CPriorityQueueImpl();

            virtual void insert(const T&) = 0;
            virtual T pop(void) = 0;
            virtual const T& top(void) = 0;
            virtual bool remove(T) = 0;
            virtual size_t size(void) = 0;
            virtual T& operator[](int) = 0;
    };

    template<class T, class Compare>
    inline CPriorityQueueImpl<T, Compare>::~CPriorityQueueImpl()
    {
    }
}

#endif //_PRIORITYQUEUE_IMPLEMENT_H
