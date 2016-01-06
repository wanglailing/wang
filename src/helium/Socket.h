/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Socket.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月23日 13时39分56秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _SOCKET_H
#define _SOCKET_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include "Address.h"

namespace HELIUM
{
    /**
     * 套接字的封装
     *
     */
    class CSocket
    {
        public:
            enum opt_t
            {
                REUSEADDR,
                RCVLOWAT,
                SNDLOWAT,
                NONBLOCKING
            };

        public:
            CSocket() : m_Fd(-1), m_Type(0) {}

            virtual ~CSocket() {}

            /**
             * 打开套接字
             *
             * @param domain 套接字域类型
             *
             * @return true -- 成功; false -- 失败.
             */
            virtual bool open(int domain) = 0;

            /// 关闭套接字
            virtual bool close(void) = 0;

            /**
             * 连接到服务器
             *
             * @param addr 服务器地址
             *
             * @return true -- 成功; false -- 失败.
             */
            virtual bool connect(const CAddress& addr);

            /**
             * 服务器在知名端口上绑定套接字地址
             *
             * @param addr 服务器地址
             *
             * @return true -- 成功; false -- 失败.
             */
            virtual bool bind(const CAddress& my_addr) = 0;

            /**
             * 向套接字中写入数据
             *
             * @param buf 数据地址 
             * @param len 数据长度
             *
             * @return 已写入数据的长度.
             */
            virtual int write(const char *buf, int len);

            /**
             * 从套接字中读取数据
             *
             * @param buf 数据地址 
             * @param len 数据长度
             *
             * @return 已读取数据的长度.
             */
            virtual int read(char *buf, int len);

            /// 获取套接字描述符
            virtual handler_t getHandler(void) const = 0;

            /// 获取套接字类型
            virtual const int getDomain(void) const = 0;

            /// 打开套接字选项
            bool turnOptionOn(opt_t opt);

            /// 关闭套接字选项
            bool turnOptionOff(opt_t opt);

            /**
             * 设置选项
             *
             * @param opt 选项名
             * @param arg 选项的值：1--启用；0--禁用。
             *
             * @return true -- 成功; false -- 失败.
             */
            bool setOption(opt_t opt, int arg);

            /// 获取选项的值
            int getOption(opt_t opt) const;

        protected:
            /// 设置套接字选项
            int setSockOption(int level, int optname, int value);

            /**
             * 设置文件描述符选项
             *
             * @param flags 选项值
             *
             * @return 0 -- 成功; -1 -- 错误.
             */
            int setFdOptions(long flags);

            /**
             * 清除文件描述符选项
             *
             * @param flags 选项值
             *
             * @return 0 -- 成功; -1 -- 错误.
             */
            int clearFdOptions(long flags);

        protected:
            /// 套接字描述符
            handler_t m_Fd;

            /// 套接字类型
            int m_Type;

        private:
            CSocket(const CSocket&);
            CSocket& operator=(const CSocket&);
    };

    inline bool CSocket::connect(const CAddress& addr)
    {
        return false;
    }

    inline int CSocket::write(const char* /*buf*/, int /*len*/)
    {
        return -1;
    }

    inline int CSocket::read(char* /*buf*/, int /*len*/)
    {
        return -1;
    }
}

#endif //_SOCKET_H
