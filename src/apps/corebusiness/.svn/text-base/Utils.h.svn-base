/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Utils.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月17日 13时24分11秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <stddef.h>
#include <stdint.h>

#include "../DmsBusinessDat.h"

#define COUNTOF(X) sizeof(X)/sizeof(X[0])

/**
 * @brief 将一定长度的比特位转换为无符号32位整型数
 *
 * @param buf          原始数据
 * @param total_bitlen 原始数据总比特位长度
 * @param bitpos       比特位的起始位置
 * @param bitlen       比特位的长度
 *
 * @return 无符号32位整型数
 */
uint32_t ConvertBitsToUInt32(const unsigned char* buf, int total_bitlen, int bitpos, int bitlen);

/**
 * @brief 将8位BCD码转换为十进制数字
 *
 * @param bcd 8位BCD码
 *
 * @return 十进制数
 */
uint8_t ConvertBCDToDecimal(uint8_t bcd);

/**
 * @brief 将16位BCD码转换为十进制数字
 *
 * @param bcd 16位BCD码
 *
 * @return 十进制数
 */
uint16_t ConvertBCDToDecimal(uint16_t bcd);

/**
 * @brief 将20位BCD码转换为十进制数字
 *
 * @param bcd 20位BCD码(用32位整数表示)
 *
 * @return 十进制数
 */
uint32_t ConvertBCDToDecimal(uint32_t bcd);

/**
 * @brief 更新应答器信息
 *
 * @param [in]  balise  应答器报文
 * @param [in]  length  报文长度
 * @param [in]  now     当前时刻
 * @param [out] atpinfo ATP实时信息(包含应答器信息)
 *
 * @return 应答器组变化标志(1 -- 未变化；2 -- 变化)
 */
int updateBaliseInfo(const unsigned char* balise, int length, uint64_t now, AtpRTInfo* atpinfo);

#endif //_UTILS_H

