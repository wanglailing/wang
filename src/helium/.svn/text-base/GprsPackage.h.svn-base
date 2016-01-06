/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: GprsPackage.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月16日 11时32分55秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _GPRS_PACKAGE_H
#define _GPRS_PACKAGE_H

const char CHANNEL_TOCPU_CONFIG    = 0x25;    /// GPRS板发给CPU板的配置数据
const char CHANNEL_TOCPU_SERVDAT   = 0x29;    /// GPRS板发给CPU板的服务器数据
const char CHANNEL_FROMCPU_CONFIG  = 0x26;    /// CPU板发给GPRS板的配置数据
const char CHANNEL_FROMCPU_SERVDAT = 0x2A;    /// CPU板发给GPRS板的服务器数据
const char CHANNEL_GPRS_STATE      = 0x0D;    /// GPRS板向外发送的状态数据包

const unsigned short GPRS_OPDATA   = 0x0000;   /// 业务数据
const unsigned short GPRS_COMMAND  = 0x0100;   /// 命令消息
const unsigned short GPRS_DMSDESC  = 0x0201;   /// DMS升级文件描述信息
const unsigned short GPRS_DMSCONT  = 0x0301;   /// DMS升级文件内容
const unsigned short GPRS_EOASDESC = 0x0202;   /// EOAS升级文件描述信息
const unsigned short GPRS_EOASCONT = 0x0302;   /// EOAS升级文件内容

namespace HELIUM
{
    /**
     * 处理GPRS/GSM-R报文
     *
     */
    class CGprsPackage
    {
        public:
            explicit CGprsPackage(bool stuff_data = false);
            virtual ~CGprsPackage();

            /// 解析GPRS/GSM-R数据
            void parseData(unsigned short source, const char *buf, int length, void* response);

            /// 字符填充原始数据
            char* stuffData(char *src, int srclen, int* stuffed_len, char channel_type = CHANNEL_FROMCPU_SERVDAT);

            /// 处理业务数据
            virtual void procBusinessData(unsigned short source, const char *buf, int length, void* response) {}

            /// 数据是否被字符填充
            bool isDataStuffed(void) { return m_bStuffGprs; }

        private:
            CGprsPackage(const CGprsPackage&);
            const CGprsPackage& operator=(const CGprsPackage&);

        private:
            /// 字符填充缓冲区
            char *m_StuffedBuf;

            /// 字符还原缓冲区
            char *m_DestuffedBuf;

            /// GPRS/GSM-R数据是否经过字符填充
            bool m_bStuffGprs;

            static const int BuffLength;
    };
}

#endif //_GPRS_PACKAGE_H
