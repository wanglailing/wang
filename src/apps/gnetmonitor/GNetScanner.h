/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GNetScanner.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月09日 13时03分36秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _GNET_SCANNER_H
#define _GNET_SCANNER_H

#include <string.h>
#include <arpa/inet.h>

#include "GNetFrame.h"

namespace EXTSERVICE
{
    class CGNetScan
    {
        public:
            CGNetScan();
            ~CGNetScan();

            /**
             * 解析G网检测模块的数据
             *
             * @param module_id 模块标识
             * @param data      数据地址
             * @param length    数据长度
             *
             */
            void parseFrame(uint16_t module_id, const char* data, int length);

            /// 设置模块的标识
            void setModulesId(uint16_t moda, uint16_t modb) { m_ModuleAid = moda; m_ModuleBid = modb; }

        private:
            CGNetScan(const CGNetScan&);
            CGNetScan& operator=(const CGNetScan&);

            /**
             * 收集BCCH频点号
             *
             * @param pRealtimeDat 实时数据
             *
             */
            template<class T>
            void gatherArfcns(T pRealtimeDat)
            {
                uint16_t bcch_arfcns[7];

                if (pRealtimeDat)
                {
                    bcch_arfcns[0] = decodeBcch(pRealtimeDat->serving.bcch, pRealtimeDat->serving.bsic);

                    for (int i = 0; i < 6; ++i)
                    {
                        bcch_arfcns[i + 1] = decodeBcch(pRealtimeDat->neighbours[i].bcch, pRealtimeDat->neighbours[i].bsic);
                    }

                    if (isArfcnsChanged(bcch_arfcns))
                    {
                        ::memcpy(m_BcchArfcns, bcch_arfcns, sizeof(bcch_arfcns));
                        dumpArfcns();
                    }
                }
            }

            uint16_t decodeBcch(uint16_t bcch, uint8_t vbcch)
            {
                uint16_t result = ntohs(bcch);
                /// 最高位表示BCCH的有效性
                if (vbcch >> 7 == 1)
                    result |= 0x8000;

                return result;
            }

            /// 检查频点号是否变化
            bool isArfcnsChanged(const uint16_t* arfcns);

            /// 检查频点号是否存在
            bool findArfcn(uint16_t bcch, const uint16_t* arfcns);

            /// 打印频点号
            void dumpArfcns(void) const;

        private:
            /**
             * 当前服务小区及其6个邻区的频点号。
             * 第一个为当前服务小区；
             * 频点号的最高位表示有效性(1--有效；0--无效)
             *
             */
            uint16_t m_BcchArfcns[7];

            /// 接收信号质量
            uint8_t m_RxQual;

            /// G网检测模块A和模块B的标识
            uint16_t m_ModuleAid;
            uint16_t m_ModuleBid;

            /// 模块A(B)的工作模式(低字节为模式，高字节为子模式)
            uint16_t m_ModuleAmode;
            uint16_t m_ModuleBmode;

            /// 最小数据帧长度
            int m_SmallestFrmLen;
    };
}

#endif //_GNET_SCANNER_H
