/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ConnService.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月22日 14时11分38秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _CONNECT_SERVICE_H
#define _CONNECT_SERVICE_H

#include "DataSubService.h"

namespace DMS
{
    class ConnService : public HELIUM::CDataSubscriber
    {
        public:
            ConnService();
            ~ConnService();

            void processData(const char* data, int length);
            void pushData(const char* data, int length);

        private:
            ConnService(const ConnService&);
            ConnService& operator=(const ConnService&);

        private:
            HELIUM::CBuffer m_ExchangeBuf;
    };
}

#endif //_CONNECT_SERVICE_H
