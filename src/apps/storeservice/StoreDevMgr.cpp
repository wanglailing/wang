/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: StoreDevMgr.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月18日 17时42分26秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include <algorithm>
#include <vector>
#include <string>

#include "StoreDevMgr.h"
#include "Logger.h"

using namespace HELIUM;
using namespace STORESERVICE;

const char* CStoreDevMgr::DataDir = "/CF/Data";
unsigned int CStoreDevMgr::SpaceAlarmLevel = 100 * 1024 * 1024;

CStoreDevMgr::CStoreDevMgr() : m_bRunning(false), m_FreeSpace(SpaceAlarmLevel)
{
    m_bDevMounted = (system("df -h /dev/hda1 > /dev/null") == 0);
    if (m_bDevMounted == true)
    {
        struct stat stat_buf;
        if (stat(DataDir, &stat_buf) < 0 || !S_ISDIR(stat_buf.st_mode))
        {
            mkdir(DataDir, 0755);
        }

        m_bRunning = true;
        m_Worker.start(workerRoutine, this);
    }
}

CStoreDevMgr::~CStoreDevMgr()
{
    m_Worker.stop();
}

void CStoreDevMgr::workerRoutine(void* args)
{
    CStoreDevMgr* self = (CStoreDevMgr*)args;
    while (self->m_bRunning)
    {
        struct statfs fs;
        if (statfs("/CF", &fs) == 0)
        {
            self->m_FreeSpace = fs.f_bsize * fs.f_bfree;
        }

        /// 清理空间
        if (self->m_FreeSpace < SpaceAlarmLevel)
        {
            self->deleteOldFiles();
        }

        sleep(60);
    }

    return;
}

static time_t getDateFromName(const std::string &name)
{
    struct tm tm;
    char date_buf[15];

    ::memset(date_buf, 0, sizeof(date_buf));
    ::memset(&tm, 0, sizeof(tm));

    /// 头三个字符为数据类型标识
    ::strncpy(date_buf, name.c_str() + 3, sizeof(date_buf) - 1);
    ::strptime(date_buf, "%Y_%m_%d", &tm);

    return ::timelocal(&tm);
}

static bool GetTimeOrder(const std::string &str1, const std::string &str2)
{
    time_t time1, time2;
    time1 = getDateFromName(str1);
    time2 = getDateFromName(str2);

    return (time1 < time2);
}

void CStoreDevMgr::deleteOldFiles(void)
{
    DIR* pDir = NULL;
    dirent* pDirent = NULL;

    struct stat cardStatBuf;
    std::vector<std::string> file_vec;

    if (::stat(DataDir, &cardStatBuf) == 0 && S_ISDIR(cardStatBuf.st_mode))
    {
        if ((pDir = ::opendir(DataDir)) == NULL)
        {
            return;
        }

        /// 收集文件名
        char *dot;
        while ((pDirent = ::readdir(pDir)) != NULL)
        {
            if ( (dot = ::strrchr(pDirent->d_name, '.')) != NULL && !::strcmp(dot, ".dat") ) 
            {
                file_vec.push_back(pDirent->d_name);
            }
        }
        ::closedir(pDir);

        /// 按时间排序文件名
        std::sort(file_vec.begin(), file_vec.end(), GetTimeOrder);

        /// 删除文件
        int remove_count = file_vec.size() >> 1;
        for (int i = 0; i < remove_count; ++i)
        {
            char fileName[255] = {0};
            ::snprintf(fileName, sizeof(fileName), "%s/%s", DataDir, file_vec[i].c_str());
            ::unlink(fileName);
        }
    }
}

