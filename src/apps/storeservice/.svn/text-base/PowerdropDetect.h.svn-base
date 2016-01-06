/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: PowerdropDetect.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年10月20日 15时11分04秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _POWERDROP_DETECT_H
#define _POWERDROP_DETECT_H

#include <cstddef>

#include "Singleton.h"

namespace STORESERVICE
{
    class CPowerdropDetecter : public HELIUM::CSingleton<CPowerdropDetecter>
    {
        public:
            enum EPowerState {
                POWER_ON  = 0x00,   /**< 上电 */
                POWER_OFF = 0x01    /**< 掉电 */
            };

        public:
            CPowerdropDetecter();
            ~CPowerdropDetecter();

            /**
             * 解析掉电信息
             *
             * @param data   掉电数据
             * @param length 数据长度
             *
             */
            void decode(const unsigned char* data, int length);

            /**
             * 获取电源状态
             *
             */
            EPowerState getPowerState(void) { return m_CurrentState; }

        private:
            CPowerdropDetecter(const CPowerdropDetecter&);
            CPowerdropDetecter& operator=(const CPowerdropDetecter&);

        private:
            static const int m_ncUpCheckLimit = 120;
            static const int m_ncDownCheckLimit = 50;

            EPowerState m_CurrentState;
    };

    #define POWERDROP_DETECTER CPowerdropDetecter::getInstance()
}

#endif //_POWERDROP_DETECT_H
