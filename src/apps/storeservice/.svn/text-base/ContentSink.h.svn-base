/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ContentSink.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年11月05日 10时33分03秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _CONTENT_SINK_H
#define _CONTENT_SINK_H

#include <algorithm>

#include "Reactor.h"
#include "TcpSocket.h"
#include "Acceptor.h"
#include "Buffer.h"

#include "ContentService.h"
#include "../DmsBusinessDat.h"

namespace STORESERVICE
{
    /**
     * 接受业务处理客户端的连接，收集并存储各种业务数据。
     *
     */
    class CContSink
    {
        friend class CContServ;

        public:
            typedef HELIUM::CAcceptor<CContServ, HELIUM::CTcpSocket> ACCEPTOR_TYPE;

        public:
            explicit CContSink(HELIUM::CReactor *r);
            ~CContSink();

            /**
             * 初始化数据交换服务。
             *
             * @return 0 -- 成功; -1 -- 失败
             */
            int init(void);

        private:
            CContSink(const CContSink&);
            CContSink& operator=(const CContSink&);

        private:
            /// 反应堆对象
            HELIUM::CReactor *m_Reactor;

            /// 业务数据发送者接受器
            ACCEPTOR_TYPE *m_DataAcceptor;

            /// UNIX域地址的绑定路径
            static const char *m_UnixBindPath;
    };
}

#endif //_CONTENT_SINK_H
