/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: LogService.h
 *
 *       版本号: 1.0
 *     创建日期: 2015-11-11 13:49:26
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _LOG_SERVICE_H
#define _LOG_SERVICE_H

#include "Thread.h"

namespace STORESERVICE
{
    /**
     * @def 日志服务
     */
    class CLogService
    {
    public:
        explicit CLogService(bool storedev_mounted);
        ~CLogService();

        void stop(void) { m_bRunning = false; }

    private:
        /**
         * @brief 创建接收日志信息的套接字
         *
         * @param void
         * @return 套接字描述符
         */
        int createSocket(void);

        /**
         * @brief 打开日志文件
         *
         * @param void
         * @return 日志文件描述符
         */
        int openLogFile(void);

        /**
         * @brief 日志记录线程执行体
         *
         * @param args 线程参数
         * @return void
         */
        static void workerRoutine(void* args);

        /**
         * @brief 将缓冲区中的数据写入文件
         *
         * @param fd  文件描述符
         * @param buf 缓冲区地址
         * @param len 缓冲区长度
         *
         * @return 写入的数据长度
         */
        static ssize_t fullWrite(int fd, const void *buf, size_t len);
        static ssize_t safeWrite(int fd, const void *buf, size_t count);

    private:
        CLogService(const CLogService&);
        CLogService& operator=(const CLogService&);

    private:
        /// 日志记录线程
        HELIUM::CThread m_Worker;

        /// 线程运行标志
        volatile bool m_bRunning;

        /// 套接字描述符
        int m_SockFd;

        /// 日志文件描述符
        int m_LogFileFd;

        static int MAX_LOG_SIZE;
    };
}

#endif //_LOG_SERVICE_H
