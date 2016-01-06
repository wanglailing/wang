/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: SlaveDevManager.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月15日 09时51分42秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _SLAVE_DEV_MANAGER_H
#define _SLAVE_DEV_MANAGER_H

#include <stdint.h>
#include <vector>

#include "Singleton.h"
#include "../apps/BusProtocol.h"

namespace HELIUM
{
    /**
     * 从板设备管理
     *
     */
    class CSlaveDevManager : public CSingleton<CSlaveDevManager>
    {
        public:
            CSlaveDevManager();
            ~CSlaveDevManager();

            typedef std::vector<DeviceItem> ITEM_VEC;

            /// 更新设备列表
            void update(const unsigned char* buf, int len);

            /// 请求设备状态
            DeviceItem queryDeviceItem(uint16_t dat_id);

            /// 请求相同类型的设备状态
            void queryDeviceItems(uint8_t datype, ITEM_VEC& item_buf);

            /**
             * 将各个从板的状态数据放入缓冲区buf中
             *
             * @param buf          数据缓冲区地址
             * @param len          缓冲区长度
             *
             * @return 数据负载的长度; -1 -- 出错
             */
            int getSlaveDevState(char *buf, int len) const;

        private:
            CSlaveDevManager(const CSlaveDevManager&);
            CSlaveDevManager& operator=(const CSlaveDevManager&);

        private:
            /// 从板设备列表
            ITEM_VEC m_DevList;
    };

    #define SLAVEDEV_MGR CSlaveDevManager::getInstance()
}

#endif// _SLAVE_DEV_MANAGER_H
