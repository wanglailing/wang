/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: InetAddress.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月23日 11时00分41秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _INET_ADDRESS_H
#define _INET_ADDRESS_H

#include "Address.h"

namespace HELIUM
{
    /**
     * 封装网络套接字地址
     *
     */
    class CInetAddress : public CAddress
    {
        public:
            /// 缺省构造函数
            CInetAddress();

            /**
             * 根据IP地址和端口号创建客户端套接字地址
             *
             * @param ip   IP地址
             * @param port 端口号
             *
             */
            CInetAddress(const char *ip, int port);

            /**
             * 根据端口号创建服务器端套接字地址
             *
             * @param port 端口号
             *
             */
            CInetAddress(int port);

            /// 复制构造函数
            CInetAddress(SA_IN *addr);

            /// 基于基地址的复制构造函数
            CInetAddress(SA *addr);

            ~CInetAddress() {}

            const int getLength(void) const { return sizeof(m_Address); }

            SA* getAddress(void) const { return (SA*)&m_Address; }

            /// 获取端口号
            int getPort(void) const { return ntohs(m_Address.sin_port); }

            /// 设置端口号
            void setPort(int port) { m_Address.sin_port = htons(port); }

        private:
            /// 初始化套接字地址
            void init(void);

            /// 创建套接字地址
            void createAddress(const char *ip, int port);

        private:
            /// 套接字地址
            SA_IN m_Address;
    };
}

#endif //_INET_ADDRESS_H
