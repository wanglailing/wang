/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: ParamsConfig.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年12月02日 14时38分16秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "ParamsConfig.h"

using namespace HELIUM;
using namespace COREBUSINESS;

CParamsConfig::CParamsConfig(void* params) : CAppConfig(params)
{}

CParamsConfig::~CParamsConfig()
{}

void CParamsConfig::parseCustomConfig(const TiXmlDocument& doc, void* params)
{
    CustomParams *pParameters = (CustomParams *)params;
    const char* value;

    if ((value = getXmlNodeValue(doc, "train/atp/typeVer/")) != NULL)
    {
        pParameters->jruVer = std::string(value);
    }

    if ((value = getXmlNodeValue(doc, "send_data_config/ToCIR/")) != NULL)
    {
        pParameters->dataToCir = (bool)(atoi(value));
    }

    if ((value = getXmlNodeValue(doc, "send_data_config/ToEB/")) != NULL)
    {
        pParameters->dataToEB = (bool)(atoi(value));
    }
}
