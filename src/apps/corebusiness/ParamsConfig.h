/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: ParamsConfig.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年12月02日 14时37分22秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _PARAMS_CONFIG_H
#define _PARAMS_CONFIG_H

#include "AppConfig.h"

namespace COREBUSINESS
{
    struct CustomParams
    {
        CustomParams() : jruVer("230"), sendTrackVoltage(false), dataToCir(false), dataToEB(false) {}

        std::string jruVer;       /// JRU版本号
        bool sendTrackVoltage;    /// 回传轨道电压幅值
        bool dataToCir;           /// 向CIR设备发数据
        bool dataToEB;            /// 向EB设备发数据
    };

    /**
     * 核心业务处理程序参数配置
     *
     */
    class CParamsConfig : public HELIUM::CAppConfig
    {
        public:
            explicit CParamsConfig(void* params);
            ~CParamsConfig();

        protected:
            void parseCustomConfig(const TiXmlDocument& doc, void* params);

        private:
            CParamsConfig(const CParamsConfig&);
            CParamsConfig& operator=(const CParamsConfig&);

        private:
    };
}

#endif  //_PARAMS_CONFIG_H
