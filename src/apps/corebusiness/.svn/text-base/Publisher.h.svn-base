/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Publisher.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月11日 08时56分24秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _PUBLISHER_H
#define _PUBLISHER_H

#include "Reactor.h"
#include "TcpSocket.h"
#include "Acceptor.h"
#include "PubService.h"

namespace COREBUSINESS
{
    class CPublisher
    {
        public:
            typedef HELIUM::CAcceptor<CPubService, HELIUM::CTcpSocket> ACCEPTOR_TYPE;

        public:
            explicit CPublisher(HELIUM::CReactor *r);
            ~CPublisher();

            /**
             * 初始化数据交换服务。
             *
             * @return 0 -- 成功; -1 -- 失败
             */
            int init(void);

            /**
             * 向订阅者广播数据
             *
             * @param buf 数据地址
             * @param len 数据长度
             *
             * @return 发送的字节数; -1 -- 出错
             */
            int broadcastData(const char *buf, int len);

        private:
            CPublisher(const CPublisher&);
            CPublisher& operator=(const CPublisher&);

        private:
            /// 反应堆对象
            HELIUM::CReactor *m_Reactor;

            /// 核心数据订阅者接受器
            ACCEPTOR_TYPE *m_SubAcceptor;

            /// UNIX域地址的绑定路径
            static const char *m_UnixBindPath;
    };
}

#endif //_PUBLISHER_H
