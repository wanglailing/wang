/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: UnixAddress.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月23日 10时23分28秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _UNIX_ADDRESS_H
#define _UNIX_ADDRESS_H

#include "Address.h"

namespace HELIUM
{
    /**
     * 封装UNIX域地址
     *
     */
    class CUnixAddress : public CAddress
    {
        public:
            CUnixAddress(const char *socket_name);
            CUnixAddress(SA *socket_address);

            virtual ~CUnixAddress();

            const int getLength(void) const;
            SA* getAddress(void) const;

        private:
            SA_UN m_Address;
    };

    inline CUnixAddress::~CUnixAddress()
    {
    }

    inline const int CUnixAddress::getLength(void) const
    {
        return sizeof(m_Address);
    }

    inline SA* CUnixAddress::getAddress(void) const
    {
        return (SA*)&m_Address;
    }
}

#endif //_UNIX_ADDRESS_H
