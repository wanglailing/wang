/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Base64Coder.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月14日 16时58分49秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _BASE64_CODER_H
#define _BASE64_CODER_H

#include <string>

#include "Buffer.h"

namespace HELIUM 
{
    /**
     * BASE64编解码器
     *
     */
    class CBase64Coder
    {
        public:
            CBase64Coder() {}
            ~CBase64Coder() {}

        public:
            /**
             * 编码缓冲区数据
             *
             * @param buf 缓冲区地址
             * @param len 缓冲区长度
             *
             * @return 编码后的字符串
             */ 
            static std::string* encode(const unsigned char* buf, unsigned int len);

            /**
             * Base64解码
             *
             * @param msg 编码的报文
             * @param output_buf 解码输出缓冲区
             *
             * @return 解码后的数据长度
             */ 
            static int decode(std::string* msg, CBuffer* output_buf);

        private:
            /// Base64字符集映射
            static const std::string m_Base64Map;
    };
}

#endif //_BASE64_CODER_H
