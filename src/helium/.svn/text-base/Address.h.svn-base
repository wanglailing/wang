/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Address.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月23日 09时46分50秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ADDRESS_H
#define _ADDRESS_H

#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#include "Logger.h"

namespace HELIUM
{
    typedef struct sockaddr SA;
    typedef struct sockaddr_in SA_IN;
    typedef struct sockaddr_un SA_UN;

    /**
     * 网络地址和UNIX域地址的统一接口
     *
     */
    class CAddress
    {
        public:
            enum emAddrState
            {
                goodbit = 0,
                badbit = 1
            };

            typedef int addrstate;

        public:
            CAddress() : m_State(CAddress::goodbit) {}
            virtual ~CAddress() {}

            /**
             * 地址是否有效
             *
             * @return true -- 有效； false--无效 
             *
             */
            bool good(void) const { return m_State == 0; }

            /**
             * 构造地址的过程中是否有错误
             *
             * @return true -- 无效； false--有效 
             *
             */
            bool bad(void) const { return m_State & CAddress::badbit; }

            /**
             * bad()的别名
             *
             * @return true -- 无效； false--有效 
             *
             */
            bool operator!() const { return bad(); }

            /// 获取地址结构的长度
            virtual const int getLength(void) const = 0;

            /// 获取地址结构的指针
            virtual SA* getAddress(void) const = 0;

        protected:
            /**
             * 设置地址的状态 
             *
             * @param flag 状态标志
             *
             */
            void setState(addrstate flag) { m_State |= flag; }

        private:
            /// 构造地址的状态
            unsigned char m_State;
    };
}

#endif //_ADDRESS_H
