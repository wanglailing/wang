/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PowerdropDetect.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年10月20日 15时24分30秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "Logger.h"
#include "PowerdropDetect.h"

using namespace HELIUM;
using namespace STORESERVICE;

HELIUM_DECL_SINGLETON(CPowerdropDetecter);

CPowerdropDetecter::CPowerdropDetecter() : m_CurrentState(POWER_ON)
{
}

CPowerdropDetecter::~CPowerdropDetecter()
{
}

void CPowerdropDetecter::decode(const unsigned char* data, int length)
{
    /// data five bytes; temperature: 2 bytes; 110V: 1 byte; 15V: 1 byte; Capacitor Voltage: 1 byte
    if (data == NULL || length != 5)
        return;

    if (m_CurrentState == POWER_ON)
    {
        /// 在供电状态下，如果电压过低，则认为掉电
        if (m_ncDownCheckLimit > data[3])
        {
            m_CurrentState = POWER_OFF;
            DBG_LOG((DATAWATCHER, "-------------POWER OFF [%u]-------------!\n", data[3]));
        }
    }
    else
    {
        if (m_ncUpCheckLimit < data[3])
        {
            m_CurrentState = POWER_ON;
            DBG_LOG((DATAWATCHER, "+++++++++++++POWER ON [%u]+++++++++++++!\n", data[3]));
        }
    }
}

