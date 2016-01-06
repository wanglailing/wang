/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: InetAddress.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月23日 11时30分10秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "InetAddress.h"

using namespace HELIUM;

void CInetAddress::init(void)
{
    memset((char*)&m_Address, 0, sizeof(m_Address));
}

void CInetAddress::createAddress(const char *ip, int port)
{
    if (strlen(ip) == 0)
    {
        m_Address.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        if (inet_pton(AF_INET, ip, &m_Address.sin_addr) != 1)
        {
            ERR_LOG((FATALERR, "Failed to convert the ip:%s\n", ip));
            setState(CAddress::badbit);
            return;
        }
    }

    m_Address.sin_family = AF_INET;
    m_Address.sin_port = htons(port);
}

CInetAddress::CInetAddress()
{
    init();
}

CInetAddress::CInetAddress(const char *ip, int port)
{
    init();
    createAddress(ip, port);
}

CInetAddress::CInetAddress(int port)
{
    init();
    createAddress("", port);
}

CInetAddress::CInetAddress(SA_IN *addr)
{
    init();
    memcpy((void*)&m_Address, (const void*)addr, sizeof(SA_IN));
}

CInetAddress::CInetAddress(SA *addr)
{
    init();
    memcpy((void*)&m_Address, (const void*)addr, sizeof(SA_IN));
}

