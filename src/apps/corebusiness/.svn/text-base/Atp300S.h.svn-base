/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Atp300S.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月29日 14时37分13秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ATP_300S_H
#define _ATP_300S_H

#include <map>

#include "AtpNode.h"
#include "CommonData.h"
#include "Sub027JuridicalRec.h"
#include "Cpu2ToJru.h"

namespace COREBUSINESS
{
    class CAtp300S : public CAtpNode
    {
        public:
            CAtp300S(int interval);
            ~CAtp300S();

            int parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            unsigned int getState(uint64_t now);

        private:
            /// 解析MVB(C3)数据
            int parseMVBData(const unsigned char* buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            /// 综合C3和C2数据生成ATP基础信息
            bool makeBasicInfo(bool more_infos, uint64_t now);

        private:
            CAtp300S(const CAtp300S&);
            CAtp300S& operator=(const CAtp300S&);

            /// C2数据解析
            CCpu2ToJru* m_Cpu2Parser;

            /// C3模式下JRU数据解析
            CJuridicalRecParser* m_JRUParser;
    };
}

#endif //_ATP_300S_H
