/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: UnixAddress.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月23日 10时38分01秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "UnixAddress.h"

using namespace HELIUM;

CUnixAddress::CUnixAddress(const char *socket_name)
{
    size_t len;
    m_Address.sun_family = AF_UNIX;

    if ((len = strlen(socket_name)) > sizeof(m_Address.sun_path))
    {
        ERR_LOG((FATALERR, "Socket path name is too long (%d bytes)!\n", len));
        setState(CAddress::badbit);
    }

    strcpy(m_Address.sun_path, socket_name);
}

CUnixAddress::CUnixAddress(SA *socket_address)
{
    SA_UN *sa_un = (SA_UN *)socket_address;
    m_Address.sun_family = AF_UNIX;

    size_t len = strlen(sa_un->sun_path);
    if (len > sizeof(m_Address.sun_path) - 1)
    {
        ERR_LOG((FATALERR, "Socket path name is too long (%d bytes)!\n", len));
        setState(CAddress::badbit);
    }

    strcpy(m_Address.sun_path, sa_un->sun_path);
}

