/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Thread.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月24日 17时31分41秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "Thread.h"

using namespace HELIUM;

extern "C"
{
    static void* threadRoutine(void* args)
    {
        CThread* self = (CThread*)args;
        self->m_Func(self->m_FuncArgs);

        return NULL;
    }
}

void CThread::start(ThreadFunc func, void* args)
{
    m_Func = func;
    m_FuncArgs = args;

    pthread_create(&m_Tid, NULL, threadRoutine, this);
}

void CThread::stop(void)
{
    pthread_join(m_Tid, NULL);
}


