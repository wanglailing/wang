/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: RbcTriggerSets.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年12月22日 10时18分30秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <algorithm>
#include <string.h>

#include "AppConfig.h"
#include "RbcTriggerSets.h"

using namespace HELIUM;
using namespace COREBUSINESS;

HELIUM_DECL_SINGLETON(CRbcTrigSets);

uint8_t CRbcTrigSets::FromRbcDefaultSets[] = {
    2, 3, 6, 9, 15, 16, 18, 27, 28, 33, 37, 38, 39, 40, 41, 43
};

uint8_t CRbcTrigSets::ToRbcDefaultSets[] = {
    130, 132, 137, 138, 146, 147, 149, 150, 153, 154, 156, 157, 159
};

CRbcTrigSets::CRbcTrigSets()
{
    init();
}

CRbcTrigSets::~CRbcTrigSets()
{}

void CRbcTrigSets::init(void)
{
    const char *fromrbc_value = NULL;
    const char *torbc_value   = NULL;

    TiXmlDocument doc("rbcmsg.xml");
    if (doc.LoadFile() == true)
    {
        fromrbc_value = CAppConfig::getXmlNodeValue(doc, "fromrbc/");
        torbc_value   = CAppConfig::getXmlNodeValue(doc, "torbc/");
    }

    pushValueToContainer(m_FromRbcTriggerSets, fromrbc_value, FromRbcDefaultSets, sizeof(FromRbcDefaultSets));
    pushValueToContainer(m_ToRbcTriggerSets, torbc_value, ToRbcDefaultSets, sizeof(ToRbcDefaultSets));
}

void CRbcTrigSets::pushValueToContainer(std::vector<uint8_t> &container,
                                        const char *values,
                                        const uint8_t *default_values,
                                        unsigned int default_count)
{
    if (values != NULL)
    {
        char *pvalue;
        char *token;
        char *saveptr;
        for (pvalue = (char *)values; ; pvalue = NULL)
        {
            token = strtok_r(pvalue, ",", &saveptr);
            if (token == NULL)
            {
                break;
            }

            container.push_back((uint8_t)atoi(token));
        }
    }
    else
    {
        for (unsigned int i = 0; i < default_count; ++i)
        {
            container.push_back(default_values[i]);
        }
    }
}

bool CRbcTrigSets::isRbcTriggered(uint8_t type, uint8_t id) const
{
    std::vector<uint8_t>::const_iterator begin_iter;
    std::vector<uint8_t>::const_iterator end_iter;
    std::vector<uint8_t>::const_iterator iter;

    /// 来自RBC
    if (type == 9)
    {
        begin_iter = m_FromRbcTriggerSets.begin();
        end_iter   = m_FromRbcTriggerSets.end();
    }
    /// 发往RBC
    else if (type == 10)
    {
        begin_iter = m_ToRbcTriggerSets.begin();
        end_iter   = m_ToRbcTriggerSets.end();
    }
    else
    {
        return false;
    }

    iter = std::find(begin_iter, end_iter, id);
    if (iter != end_iter)
    {
        return true;
    }

    return false;
}
