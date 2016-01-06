/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: RbcTriggerSets.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年12月22日 10时11分14秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _RBC_TRIGGER_SETS_H
#define _RBC_TRIGGER_SETS_H

#include <vector>
#include <stdint.h>

#include "Singleton.h"

namespace COREBUSINESS
{
    /**
     * RBC消息触发集
     */
    class CRbcTrigSets : public HELIUM::CSingleton<CRbcTrigSets>
    {
        public:
            CRbcTrigSets();
            ~CRbcTrigSets();

            bool isRbcTriggered(uint8_t type, uint8_t id) const;

        private:
            CRbcTrigSets(const CRbcTrigSets&);
            CRbcTrigSets& operator=(const CRbcTrigSets&);

            /// 初始化RBC消息集
            void init(void);

            void pushValueToContainer(std::vector<uint8_t> &container,
                                      const char *values,
                                      const uint8_t *default_values,
                                      unsigned int default_count);

        private:
            /// 来自RBC的触发集
            std::vector<uint8_t> m_FromRbcTriggerSets;

            /// 发往RBC的触发集
            std::vector<uint8_t> m_ToRbcTriggerSets;

            /// 来自RBC消息的默认触发标识集
            static uint8_t FromRbcDefaultSets[];

            /// 发往RBC消息的默认触发标识集
            static uint8_t ToRbcDefaultSets[];
    };

    #define RBC_SETS CRbcTrigSets::getInstance()
}

#endif //_RBC_TRIGGER_SETS_H
