/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: WindowTransServer.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月14日 11时10分18秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _WINDOWTRANS_SERVER_H
#define _WINDOWTRANS_SERVER_H

#include <map>

#include "WindowTransport.h"
#include "Buffer.h"

namespace TRANSPORTCTRL
{
    /**
     * 窗口传输服务端。
     *
     */
    class CWTransServer: public CWindowTrans
    {
        public:
            typedef void (*OpFunc)(const char* data, int length);

        public:
            CWTransServer();
            ~CWTransServer();

            /// 插入一包数据
            std::string insertPacket(std::string* packet);

            /// 处理已排序的数据
            void processPackets(void);

        protected:
            void reset(void) {}

        private:
            CWTransServer(const CWTransServer&);
            CWTransServer& operator=(const CWTransServer&);

            /// 处理原始数据
            void processRawData(const char *data, int length);

            /// 处理DMS升级文件描述信息
            static void procDmsUpgradeDesc(const char *data, int length);

            /// 处理DMS升级文件内容
            static void procDmsUpgradeContent(const char *data, int length);

            /// 将EOAS升级文件描述信息转发给EOAS扩展模块
            static void forwardEoasUpgradeDesc(const char *data, int length);

            /// 将EOAS升级文件内容转发给EOAS扩展模块
            static void forwardEoasUpgradeContent(const char *data, int length);

        private:
            /// 请求的报文ID
            unsigned int m_NeededID;

            /// Zlib解压缓冲区
            HELIUM::CBuffer m_DecompressBuf;

            /// 数据处理集
            std::map<unsigned short, OpFunc> m_DataProcs;
    };
}

#endif //_WINDOWTRANS_SERVER_H
