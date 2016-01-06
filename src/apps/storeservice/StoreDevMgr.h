/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: StoreDevMgr.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月18日 17时38分21秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _STOREDEV_MGR_H
#define _STOREDEV_MGR_H

#include <stdio.h>

#include "Thread.h"

namespace STORESERVICE
{
    /**
     * 管理存储设备
     *
     */
    class CStoreDevMgr
    {
        public:
            CStoreDevMgr();
            ~CStoreDevMgr();

            /// 获取设备挂载状态
            bool isCFCardMounted(void) const { return m_bDevMounted; }

            void stop(void) { m_bRunning = false; }

            /// 文件存放路径
            static const char* DataDir;

        private:
            CStoreDevMgr(const CStoreDevMgr&);
            CStoreDevMgr& operator=(const CStoreDevMgr&);

            static void workerRoutine(void* args);
            void deleteOldFiles(void);

        private:
            /// 设备管理线程
            HELIUM::CThread m_Worker;

            /// 线程运行标志
            volatile bool m_bRunning;

            /// 设备挂载标志
            bool m_bDevMounted;

            /// 剩余空间
            unsigned int m_FreeSpace;

            /// 最小剩余空间
            static unsigned int SpaceAlarmLevel;
    };
}

#endif //_STOREDEV_MGR_H
