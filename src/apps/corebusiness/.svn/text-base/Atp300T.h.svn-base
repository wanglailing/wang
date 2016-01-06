/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Atp300T.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月27日 16时34分07秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ATP_300T_H
#define _ATP_300T_H

#include "AtpNode.h"
#include "StmToJru.h"
#include "Sub027JuridicalRec.h"
#include "../DmsBusinessDat.h"

namespace COREBUSINESS
{
    /**
     * 300T型ATP数据
     *
     */
    class CAtp300T : public CAtpNode
    {
        public:
            CAtp300T(int interval);
            ~CAtp300T();

            int parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now);

            unsigned int getState(uint64_t now);

        private:
            /// 综合C3和C2数据生成ATP基础信息
            bool makeBasicInfo(bool more_infos, uint64_t now);

            /// 复位基础信息
            void resetDataInfo(uint8_t reset_area = 0);

        private:
            CAtp300T(const CAtp300T&);
            CAtp300T& operator=(const CAtp300T&);

            /// STM数据解析
            CSTMToJRU* m_STMParser;

            /// JRU数据解析
            CJuridicalRecParser* m_JRUParser;
    };
}

#endif //_ATP_300T_H
