/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: MaskSet.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月20日 10时12分52秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _MASK_SET_H
#define _MASK_SET_H

#include "FdSet.h"

namespace HELIUM
{
    /**
     * 被select()使用的描述符集掩码
     *
     */
    class CMaskSet
    {
        public:
            /// 读描述符集
            CFdSet m_ReadSet;

            /// 写描述符集
            CFdSet m_WriteSet;

            /// 异常描述符集
            CFdSet m_ExceptSet;

        public:
            /// 重置描述符集
            void reset(void);

            void sync(void);

            /// 获取描述符集中的最大描述符
            int maxFd(void);

            /// 将调试信息转储到日志
            void dump(void);
    };

    inline void CMaskSet::reset(void)
    {
        m_ReadSet.reset();
        m_WriteSet.reset();
        m_ExceptSet.reset();
    }

    inline void CMaskSet::sync(void)
    {
        m_ReadSet.sync();
        m_WriteSet.sync();
        m_ExceptSet.sync();
    }

    inline int CMaskSet::maxFd(void)
    {
        return std::max( m_ReadSet.maxInSet(), 
                std::max(m_WriteSet.maxInSet(), m_ExceptSet.maxInSet()) );
    }

    inline void CMaskSet::dump(void) {}
}

#endif //_MASK_SET_H
