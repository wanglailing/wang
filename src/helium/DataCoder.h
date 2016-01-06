/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataCoder.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月04日 08时55分48秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DATA_CODER_H
#define _DATA_CODER_H

#include <string>
#include <stdint.h>

#include "CRC.h"
#include "Buffer.h"
#include "Base64Coder.h"
#include "ZlibStream.h"

namespace HELIUM 
{
    /**
     * 数据的压缩编码及解码解压处理
     *
     */
    class CDataCoder
    {
        public:
            CDataCoder() {}
            ~CDataCoder() {}

        public:
            /**
             * 压缩并编码缓冲区数据
             *
             * @param data 缓冲区地址
             *
             * @return 编码后的字符串
             */ 
            static std::string* encodeData(CBuffer* data);

            /**
             * 解码并解压缩字符串
             *
             * @param output  输出缓冲区地址
             * @param enc_str 编码字符串
             *
             */ 
            static void decodeData(CBuffer* output, std::string* enc_str);

            /// 为数据帧添加校验和与帧尾
            static void wrapData(char* dst, int buf_len, int head_len, const char* tag_tail, const char* tag_crc = "");
            static void wrapData(std::string& packet, int head_len, const char* tag_tail, const char* tag_crc = "");

            /// 计算32位校验和
            static std::string calcCRC32(const char* data, int length);

        public:
            /// DMS帧头标志
            static const char* DmsTagHead;

            /// DMS帧尾标志
            static const char* DmsTagTail;

            /// TCAS帧头标志
            static const char* TcasTagHead;

            /// TCAS帧校验标志
            static const char* TcasTagCrc;

            /// TCAS帧尾标志
            static const char* TcasTagTail;

            /// 32位校验和
            static Crc<uint32_t> Crc32;

        private:
            CDataCoder(const CDataCoder&);
            const CDataCoder& operator=(const CDataCoder&);
    };
}

#endif //_DATA_CODER_H
