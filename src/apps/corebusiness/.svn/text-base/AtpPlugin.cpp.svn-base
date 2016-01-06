/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: AtpPlugin.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月10日 11时21分15秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <dlfcn.h>

#include "Logger.h"
#include "AtpPlugin.h"

using namespace HELIUM;

static std::string extension = ".so";
static std::string globalPluginDir = "./lib/";

AtpPluginHandlers gAtpHandlers;

/// 持有动态库的句柄，在卸载动态库时使用
static void* gPluginLib = NULL;

static std::string findPlugin(std::string plugin_name)
{
    std::string name = globalPluginDir + plugin_name + extension;
    FILE* fp = fopen(name.c_str(), "rb");
    if (fp)
    {
        fclose(fp);
        return name;
    }

    return std::string("");
}

bool loadPlugin(std::string plugin)
{
    memset(&gAtpHandlers, 0, sizeof(AtpPluginHandlers));

    std::string real_plugname = findPlugin(plugin);
    void* hLib = dlopen(real_plugname.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (hLib)
    {
        atpInitHandler initProc;
        *(void**)&initProc = dlsym(hLib, "atpInitPlugin");
        if (initProc == NULL)
        {
            DBG_LOG((COREPROC, "Plugin: [%s] does not contain method atpInitPlugin(). Error: %s\n", 
                    plugin.c_str(), dlerror()));
            dlclose(hLib);
            return false;
        }
        else
        {
            /// 初始化ATP解析对象
            if ((*initProc)() < 0)
            {
                DBG_LOG((COREPROC, "Plugin: [%s] initialize the Atp parser error.\n",
                            plugin.c_str()));
                dlclose(hLib);
                return false;
            }

            *(void**)&gAtpHandlers.getVersion        = dlsym(hLib, "atpGetVersion");
            *(void**)&gAtpHandlers.getRunSpeed       = dlsym(hLib, "atpGetRunSpeed");
            *(void**)&gAtpHandlers.getState          = dlsym(hLib, "atpGetState");
            *(void**)&gAtpHandlers.getRealtimeInfo   = dlsym(hLib, "atpGetRealtimeInfo");
            *(void**)&gAtpHandlers.parseData         = dlsym(hLib, "atpParseData");
            *(void**)&gAtpHandlers.updateRunDistance = dlsym(hLib, "atpUpdateRunDistance");
            *(void**)&gAtpHandlers.updateTrainNum    = dlsym(hLib, "atpUpdateTrainNumber");
            *(void**)&gAtpHandlers.updateDataState   = dlsym(hLib, "atpUpdateDataState");

            gPluginLib = hLib;
            return true;
        }
    }
    else
    {
        DBG_LOG((COREPROC, "Plugin: [%s] not found. Error: %s\n",
                    plugin.c_str(), dlerror()));
        return false;
    }

    return false;
}

bool unloadPlugin(void)
{
    if (gPluginLib)
    {
        atpFreeHandler freeProc;
        *(void**)&freeProc = dlsym(gPluginLib, "atpFreePlugin");
        if (freeProc == NULL)
        {
            DBG_LOG((COREPROC, "Plugin does not contain method atpFreePlugin(). Error: %s\n", 
                    dlerror()));
        }
        else
        {
            /// 释放ATP解析对象
            (*freeProc)();
        }

        dlclose(gPluginLib);
        gPluginLib = NULL;

        /// 清理ATP解析接口
        memset(&gAtpHandlers, 0, sizeof(AtpPluginHandlers));
    }

    return true;
}

