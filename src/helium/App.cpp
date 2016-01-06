/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: App.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月12日 13时17分41秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "App.h"
#include "AppConfig.h"

using namespace HELIUM;

CApp::CApp() : m_GracefulQuit(false), m_Version("2.0.1.1"), m_HelpMsg("nothing"),
    m_LogFlag(KEEPLOG), m_LogMask(ALL), m_LogStdout("no"), m_ExitValue(0)
{
}

CApp::~CApp()
{
    Log::logClose();
}

void CApp::init(int* argc, char* argv[], const char* help_msg)
{
    m_HelpMsg = help_msg;

    int result;
    while ((result = getopt(*argc, argv, "v")) != -1)
    {
        switch (result)
        {
            case 'v':
                printf("%s v%s\nBuilt on [%s at %s]\n", m_ProcName.c_str(), m_Version.c_str(),
                        __DATE__, __TIME__);
                exit(0);
            default:
                break;
        }
    }

    initInternals();
}

void CApp::initInternals(void)
{
    /// 初始化日志掩码
    int dbg_fd = ::open(".loglevel", O_RDONLY);
    if (dbg_fd > 0)
    {
        char buf[8];
        ssize_t read_len = ::read(dbg_fd, buf, sizeof(buf));
        m_LogMask = CAppConfig::convertHexString(buf, read_len);

        ::close(dbg_fd);
    }

    /// 如果需要的话，删除已存在的日志文件
    if (m_LogFlag == RMLOG && m_LogStdout == "no")
    {}

    if (m_LogStdout == "yes")
    {
        Log::openLogStdout(m_LogMask);
    }
    else
    {
        Log::openLogRemote(NULL, m_LogMask);
    }
    Log::setAppName(getProcName());
}

int CApp::handleSignal(int /*signum*/)
{
    return -1;
}

inline void CApp::stopService(void)
{
    m_GracefulQuit = true;
    m_Reactor.deactivate();
}

inline void CApp::displayHelp(void)
{
    std::cout << m_HelpMsg << std::endl;
}

