/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: DataSwitchApp.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月27日 17时17分52秒
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

#include "SlaveDevManager.h"
#include "DataSwitchApp.h"
#include "GprsPackage.h"
#include "LibVersion.h"
#include "DataCoder.h"
#include "Logger.h"

#include "../DmsBusinessDat.h"

using namespace HELIUM;
using namespace DMS;

HELIUM_DECL_SINGLETON(CDataSwitchApp);

CDataSwitchApp::CDataSwitchApp() : m_DataHubber(NULL), m_300tId(ATP_300T)
{
    m_ProcName = "datahub";
    m_Version = "2.0.3.1030";

    ModulesCaption.insert(make_pair(m_ProcName, m_Version));
    ModulesCaption.insert(make_pair(std::string(LibName), std::string(LibVerno)));

    DataId data_id;
    if ((data_id = m_Config.getSlaveDevType("ATP300T")) != 0)
        m_300tId = data_id;
}

CDataSwitchApp::~CDataSwitchApp()
{
    delete m_DataHubber;
}

void CDataSwitchApp::initService()
{
    DBG_LOG((DATASWITCH, "\n"));
    DBG_LOG((DATASWITCH, "[%s v%s] START RUNNING...\n", m_ProcName.c_str(), m_Version.c_str()));
    m_DataHubber = new CDataHub(getReactor());
    m_DataHubber->setId("DMS Data Hubber");
    if (m_DataHubber->init() < 0)
    {
        DBG_LOG((DATASWITCH, "Initialize the data hubber error!\n"));
    }
}

void CDataSwitchApp::processEvents()
{
    while (serviceActive())
    {
        m_Reactor.waitForEvents();
    }

    m_Reactor.stopReactor();
}

int CDataSwitchApp::stuffCmdResult(unsigned short sender, char *src, int srclen, char channel_type)
{
    /// 字符填充
    CGprsPackage gprs_pack(APP_CONFIG->isGprsStuffed());
    int act_len;
    char *final_buf = gprs_pack.stuffData(src, srclen, &act_len, channel_type);

    /// 底板数据封装
    uint16_t serv_type = (uint16_t)DAT_EXCHANGE;
    DeviceItem dev_item = SLAVEDEV_MGR->queryDeviceItem(sender);
    memcpy(m_ExecResult, &(dev_item.devid.id), sizeof(uint32_t));
    memcpy(m_ExecResult + sizeof(uint32_t), &serv_type, sizeof(uint16_t));
    memcpy(m_ExecResult + sizeof(uint32_t) + sizeof(uint16_t), final_buf, act_len);

    return (act_len + sizeof(uint32_t) + sizeof(uint16_t));
}

char* CDataSwitchApp::configGsmr(unsigned short sender, char config_cmd, int* rc_len)
{
    char tmp[16];
    tmp[1] = config_cmd;
    *rc_len = stuffCmdResult(sender, tmp, 1 + 1, CHANNEL_FROMCPU_CONFIG);
    return m_ExecResult;
}

char* CDataSwitchApp::runCmd(const char* cmd, int len, int* rc_len)
{
    if (cmd == NULL || len <= 0)
    {
        *rc_len = 0;
        return NULL;
    }

    /// 头两个字节为发送方标识
    unsigned short sender_id;
    memcpy(&sender_id, cmd, sizeof(sender_id));

    const char* useful_cmd = cmd + sizeof(sender_id);
    if (!strncasecmp(useful_cmd, "**GETVER##", 10))
    {
        std::string modules = getAllModules();

        char tmp[256];
        ::snprintf(&tmp[1], sizeof(tmp) - 1, "**GETVER##%s", modules.c_str());

        *rc_len = stuffCmdResult(sender_id, tmp, modules.length() + 1 + 10, CHANNEL_FROMCPU_SERVDAT);
        return m_ExecResult;
    }
    if (!strncasecmp(useful_cmd, "**time|", 7))
    {
        /// 长度要去掉前缀"**time|"、后缀"##"以及发送方标识
        adjustByRemoteTime(useful_cmd + 7, len - 11);
    }

    if (!strncasecmp(useful_cmd, "**REBOOT##", 10))
    {
        /// 简单粗暴式重启
        DBG_LOG((DATASWITCH, "Recv REBOOT command, the system will be restarted after 3 seconds...\n"));
        sleep(3);
        system("reboot");
    }
    else if (!strncasecmp(useful_cmd, "**OPENGSMR##", 12))
    {
        return configGsmr(sender_id, 0x39, rc_len);
    }
    else if (!strncasecmp(useful_cmd, "**CLOSEGSMR##", 13))
    {
        return configGsmr(sender_id, 0x3A, rc_len);
    }

    *rc_len = 0;
    return NULL;
}

void CDataSwitchApp::adjustByRemoteTime(const char* time_buf, int len)
{
    if (time_buf == NULL || len <= 0)
        return;

    /**
     * 时间为明文字符串，其格式如下：
     * YYYYMMDDhhmmss|threshold|0
     * 当本地时间与远程时间的误差大于threshold时，用远程时间调整本地时间。
     * 0用于计算报文的传输时延，没有用到。
     */
    char time_str[64] = {'\0'};
    memcpy(time_str, time_buf, len);

    char time_array[3][32];
    memset(time_array, 0, sizeof(time_array));

    int i;
    char *str_ptr, *token, *saveptr;
    const char* delimiter = "|";
    for (i = 0, str_ptr = time_str; i < 3; ++i, str_ptr = NULL)
    {
        token = strtok_r(str_ptr, delimiter, &saveptr);
        if (token == NULL)
            break;

        strcpy(time_array[i], token);
    }

    if (i == 3)
    {
        struct tm remote_time;
        memset(&remote_time, 0, sizeof(remote_time));
        sscanf(time_array[0], "%04d%02d%02d%02d%02d%02d", &remote_time.tm_year, &remote_time.tm_mon, &remote_time.tm_mday, 
                &remote_time.tm_hour, &remote_time.tm_min, &remote_time.tm_sec);
        int adj_threshold = atoi(time_array[1]);

        CTimeVal::adjustTime(&remote_time, adj_threshold, 1, 0);
    }
}

void CDataSwitchApp::setModuleInfo(const char* info, int length)
{
    if (info == NULL || length <= 0)
        return;

    const char* ptr = info;
    char* colon = NULL;
    char* new_line = NULL;
    while (ptr != NULL && (colon = strchr(ptr, ':')) != NULL)
    {
        std::string name;
        std::string verno;

        name.append(ptr, colon - ptr);
        new_line = strchr(colon, '\n');
        if (new_line != NULL)
        {
            verno.append(colon + 1, new_line - colon - 1);
            ptr = new_line + 1;
        }
        else
        {
            verno.append(colon + 1, length - (colon - info + 1));
            ptr = NULL;
        }

        std::map<std::string, std::string>::iterator iter;
        iter = ModulesCaption.find(name);
        if (iter != ModulesCaption.end())
        {
            iter->second = verno;
        }
        else
        {
            ModulesCaption.insert(make_pair(name, verno));
        }
    }
}

std::string CDataSwitchApp::getAllModules(void)
{
    std::string result("");
    std::map<std::string, std::string>::const_iterator iter;
    for (iter = ModulesCaption.begin(); iter != ModulesCaption.end(); ++iter)
    {
        std::string module_info = iter->first + ":" + iter->second + "\n";
        result += module_info;
    }

    if (!result.empty())
        result += "\n";

    return result;
}

