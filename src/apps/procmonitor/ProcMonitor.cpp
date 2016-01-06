/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ProcMonitor.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年03月31日 16时58分07秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

#include "tinyxml/tinyxml.h"

#define INVALID_PID    -1
#define PROCPS_BUFSIZE 1024

/**
 * @def 进程相关信息
 */
typedef struct stProcInfo
{
    pid_t pid;             /// 进程标识
    char image_name[32];   /// 进程名
}ProcInfo;

static std::vector<ProcInfo> gProcVec;

static void loadConfig(void)
{
    TiXmlDocument doc("processes.xml");
    if (doc.LoadFile() == false)
    {
        return;
    }

    TiXmlElement *rootElement = doc.RootElement();

    TiXmlNode *pChild;
    for (pChild = rootElement->FirstChild(); pChild != NULL; pChild = pChild->NextSibling())
    {
        TiXmlElement *pChildElement = pChild->ToElement();
        if (pChildElement == NULL)
            continue;

        ProcInfo proc_info;
        memset(&proc_info, 0, sizeof(proc_info));
        proc_info.pid = INVALID_PID;
        snprintf(proc_info.image_name, sizeof(proc_info.image_name) - 1, "%s", pChild->Value());

        gProcVec.push_back(proc_info);
    }
}

int readToBuf(const char* file_name, void* buf)
{
    int fd;
    ssize_t ret = -1;
    fd = open(file_name, O_RDONLY);
    if (fd > 0)
    {
        ret = read(fd, buf, PROCPS_BUFSIZE - 1);
        close(fd);
    }

    ((char *)buf)[ret > 0 ? ret : 0] = '\0';
    return ret;
}

pid_t findProcessID(const char* prog_name)
{
    pid_t pid = INVALID_PID;
    char namebuf[PROCPS_BUFSIZE];
    DIR* proc_dir = NULL;
    struct dirent* dir_entry = NULL;

    proc_dir = opendir("/proc");
    while ((dir_entry = ::readdir(proc_dir)) != NULL)
    {
        struct stat statbuf;
        stat(dir_entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode) && isalnum(dir_entry->d_name[0]))
        {
            char cmdline_fname[64];
            snprintf(cmdline_fname, sizeof(cmdline_fname), "/proc/%s/cmdline", dir_entry->d_name);
            if (readToBuf(cmdline_fname, namebuf) > 0 && strstr(namebuf, prog_name))
            {
                pid = strtoul(dir_entry->d_name, NULL, 10);
                break;
            }
        }
    }
    closedir(proc_dir);

    return pid;
}

void monitorProcess(void)
{
    for (unsigned int i = 0; i < gProcVec.size(); ++i)
    {
        /// 1. 进程信息未初始化(进程标识为非法)
        /// 2. 进程异常退出(进程标识合法，但在/proc中找不到)
        /// 3. 进程正常运行(在/proc中能查找到进程标识)
        if (gProcVec[i].pid == INVALID_PID)
        {
            gProcVec[i].pid = findProcessID(gProcVec[i].image_name);
        }

        struct stat stat_buf;
        char process_dir[64];
        snprintf(process_dir, sizeof(process_dir), "/proc/%u", gProcVec[i].pid);

        if (gProcVec[i].pid == INVALID_PID || stat(process_dir, &stat_buf) < 0 || !S_ISDIR(stat_buf.st_mode))
        {
            /// 启动进程
            char cmd_buf[64];
            snprintf(cmd_buf, sizeof(cmd_buf), "./%s&", gProcVec[i].image_name);
            if (system(cmd_buf) < 0)
            {
                printf("Failed to start the program[%s]\n", gProcVec[i].image_name);
            }

            /// 进程启动后还不知道它的ID号，在下次检查进程状态时更新
            gProcVec[i].pid = INVALID_PID;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "procmon v2.0.1.1021\nUsage: procmon <monitor interval>\n");
        return -1;
    }

    int interval = atoi(argv[1]);
    if (interval <= 0)
    {
        fprintf(stderr, "error: monitor interval [%s] is less than zero!\n", argv[1]);
        return -1;
    }

    loadConfig();

    if (gProcVec.empty() == true)
    {
        fprintf(stderr, "There are no processes to monitor!\n");
        return -1;
    }

    for (;;)
    {
        sleep(interval);
        monitorProcess();
    }

    return 0;
}

