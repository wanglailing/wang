/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: LogService.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015-11-11 14:35:59
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "RemoteLogger.h"
#include "LogService.h"
#include "StoreApp.h"

using namespace HELIUM;
using namespace STORESERVICE;

int CLogService::MAX_LOG_SIZE = 10 * 1024 * 1024;

CLogService::CLogService(bool storedev_mounted) : m_LogFileFd(-1)
{
    m_SockFd = createSocket();
    if (storedev_mounted == true)
    {
        m_LogFileFd = openLogFile();
    }

    m_bRunning = true;
    m_Worker.start(workerRoutine, this);
}

CLogService::~CLogService()
{
    m_Worker.stop();
}

int CLogService::createSocket(void)
{
    struct sockaddr_un sun_addr;
    int sock_fd;

    memset(&sun_addr, 0, sizeof(sun_addr));
    sun_addr.sun_family = AF_UNIX;
    strncpy(sun_addr.sun_path, CRemoteLogger::LOG_PATH, sizeof(sun_addr.sun_path));
    unlink(sun_addr.sun_path);

    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (bind(sock_fd, (struct sockaddr*)&sun_addr, sizeof(sun_addr)) != 0)
    {
        close(sock_fd);
        return -1;
    }

    chmod(CRemoteLogger::LOG_PATH, 0666);
    return sock_fd;
}

int CLogService::openLogFile(void)
{
    int log_fd;
    int flags;

    char log_name[255];
    ::snprintf(log_name, sizeof(log_name), "/CF/LOG2100_12_25_%s_%s.log",
            APPCONFIG->getTrainIDStr().c_str(), APPCONFIG->getAtpType().c_str());

    struct stat stat_buf;
    if (stat(log_name, &stat_buf) == 0 && stat_buf.st_size > MAX_LOG_SIZE)
    {
        flags = O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK;
    }
    else
    {
        flags = O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK;
    }

    log_fd = open(log_name, flags, 0666);

    return log_fd;
}

void CLogService::workerRoutine(void* args)
{
    CLogService* self = (CLogService*)args;
    while (self->m_bRunning)
    {
        ssize_t recv_len;
        char recvbuf[1024];

        /// 从套接字缓冲区中读取日志信息
        recv_len = read(self->m_SockFd, recvbuf, sizeof(recvbuf) - 1);
        if (recv_len < 0)
        {
            printf("Failed to recv the log message!\n");
            break;
        }

        /// 将日志信息记录到文件
        if (self->m_LogFileFd > 0)
        {
            fullWrite(self->m_LogFileFd, recvbuf, recv_len);
        }
    }

    close(self->m_SockFd);
    close(self->m_LogFileFd);

    return;
}

ssize_t CLogService::fullWrite(int fd, const void *buf, size_t len)
{
    ssize_t cc;
    ssize_t total;

    total = 0;

    while (len)
    {
        cc = safeWrite(fd, buf, len);
        if (cc < 0) {
            if (total) {
                return total;
            }

            return cc;
        }

        total += cc;
        buf = ((const char *)buf) + cc;
        len -= cc;
    }

    return total;
}

ssize_t CLogService::safeWrite(int fd, const void *buf, size_t count)
{
    ssize_t n;

    do
    {
        n = write(fd, buf, count);
    } while (n < 0 && errno == EINTR);

    return n;
}
