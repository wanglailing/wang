/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: AtpAPI.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月10日 11时16分31秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ATP_API_H
#define _ATP_API_H

#include <stdint.h>

/**
 * 初始化ATP解析对象
 *
 * @return 0 -- 成功；-1 -- 失败
 *
 */
extern "C" int atpInitPlugin(void);

/**
 * 释放ATP解析对象
 *
 * @return 0 -- 成功；-1 -- 失败
 *
 */
extern "C" int atpFreePlugin(void);

/**
 * 解析ATP数据
 *
 * @return 0 -- 成功；-1 -- 失败
 *
 */
extern "C" int atpParseData(const unsigned char* buf, int length, void* parsed_buf, uint64_t timenow);

/**
 * 用自身计算的走行距离更新ATP走行距离
 *
 * @param distance 自身计算的走行距离
 *
 */
extern "C" void atpUpdateRunDistance(unsigned int distance);

/**
 * 更新车次
 *
 * @param trainum   车次信息
 * @param timestamp 车次信息的接收时间
 *
 */
extern "C" void atpUpdateTrainNumber(const void* trainum, unsigned int timestamp);

/**
 * 获取ATP运行速度
 *
 * @param now 当前时刻(用于判断速度的时效性)
 *
 */
extern "C" unsigned short atpGetRunSpeed(uint64_t now);

/**
 * 获取ATP状态
 *
 * @param now 当前时刻(用于判断数据的时效性)
 *
 * @return 状态(高字为300S/300T在C2等级下的状态，低字为C2/C3等级下的状态)
 */
extern "C" unsigned int atpGetState(uint64_t now);

/**
 * 更新数据状态
 *
 * @param pstate 数据状态的地址
 *
 */
extern "C" void atpUpdateDataState(const void* pstate);

/**
 * 获取版本信息
 *
 */
extern "C" const char* atpGetVersion(void);

/**
 * 获取ATP实时信息
 *
 * @return ATP实时信息的地址
 *
 */
extern "C" unsigned char* atpGetRealtimeInfo(int* length);

#endif //_ATP_API_H
