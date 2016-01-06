/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Upgrader.h
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

#ifndef _UPGRADER_H
#define _UPGRADER_H

#include <stdio.h>

#include "Singleton.h"
#include "Thread.h"

namespace TRANSPORTCTRL
{
#pragma pack(push, 1)
    /**
     * @def
     * 升级文件描述信息
     *
     */
    typedef struct stFileDescription
    {
        char name[64];    /// 文件名
        int  size;        /// 文件长度
    }FILE_DESCRIPTION;
#pragma pack(pop)

    /**
     * 升级车载程序
     *
     */
    class CUpgrader : public HELIUM::CSingleton<CUpgrader>
    {
        public:
            CUpgrader();
            ~CUpgrader();

            /// 创建升级文件
            int createUpgradeFile(const FILE_DESCRIPTION* file_desc);

            /// 将升级内容写入文件
            int writeToFile(const char* data, int length);

        private:
            CUpgrader(const CUpgrader&);
            CUpgrader& operator=(const CUpgrader&);

            /// 初始化升级所需要的文件结构
            void initFileStructs(void);

            /// 升级脚本执行体
            static void workerRoutine(void* args);

        private:
            /// 升级文件指针
            FILE* m_Fp;

            /// 升级文件名
            char m_FileName[32];

            /// 文件总长度
            int m_FileSize;

            /// 已接收长度
            int m_RecvSize;

            /// 升级脚本执行线程
            HELIUM::CThread m_Worker;

            static const char *UnpackScript;
    };

    #define UPGRADER CUpgrader::getInstance()
}

#endif //_UPGRADER_H
