/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: AtpPlugin.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月29日 10时48分57秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ATP_PLUGIN_H
#define _ATP_PLUGIN_H

#include <stdint.h>
#include <string>

typedef int (*atpInitHandler)(void);
typedef int (*atpFreeHandler)(void);
typedef const char* (*getVerHandler)(void);
typedef unsigned short (*getRunSpeedHandler)(uint64_t now);
typedef unsigned int   (*getStateHandler)(uint64_t now);
typedef unsigned char* (*getRealtimeInfoHandler)(int* length);
typedef int (*parseDataHandler)(const unsigned char* buf, int length, void* parsed_buf, uint64_t timenow);
typedef void (*updateRunDistanceHandler)(unsigned int distance);
typedef void (*updateTrainumHandler)(const void* trainum, unsigned int timestamp);
typedef void (*updateDataStateHandler)(const void* pstate);

struct AtpPluginHandlers
{
    getVerHandler             getVersion;           /// 获取版本信息
    getRunSpeedHandler        getRunSpeed;          /// 获取ATP运行速度
    getStateHandler           getState;             /// 获取ATP状态
    getRealtimeInfoHandler    getRealtimeInfo;      /// 获取ATP实时信息
    parseDataHandler          parseData;            /// 数据解析
    updateRunDistanceHandler  updateRunDistance;    /// 更新走行距离
    updateTrainumHandler      updateTrainNum;       /// 更新车次信息
    updateDataStateHandler    updateDataState;      /// 更新数据状态
};

/**
 * @brief 加载插件
 *
 * @param plugin 插件名称
 *
 * @return true -- 成功；false -- 失败。
 */
bool loadPlugin(std::string plugin);

/**
 * @brief 卸载插件
 *
 * @param plugin 插件名称
 *
 * @return true -- 成功；false -- 失败。
 */
bool unloadPlugin(void);

/// ATP数据处理接口
extern AtpPluginHandlers gAtpHandlers;

#endif //_ATP_PLUGIN_H
