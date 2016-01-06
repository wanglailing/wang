/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Thread.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月24日 13时48分00秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _THREAD_H
#define _THREAD_H

#include <stdint.h>
#include <pthread.h>

namespace HELIUM
{
    /**
     * 线程类
     *
     */
    class CThread
    {
        public:
            typedef void (*ThreadFunc)(void*);

        public:
            CThread() {}
            ~CThread() {}

            /// 启动线程的执行
            void start(ThreadFunc func, void* args);

            /// 等待线程的结束
            void stop(void);

            /// 线程执行体指针
            ThreadFunc m_Func;

            /// 线程执行体参数
            void* m_FuncArgs;

        private:
            CThread(const CThread&);
            CThread& operator=(const CThread&);

        private:
            /// 线程标识
            pthread_t m_Tid;
    };
}

#endif //_THREAD_H
