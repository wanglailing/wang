/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ZlibStream.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月15日 09时35分25秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ZLIB_STREAM_H
#define _ZLIB_STREAM_H

#include <zlib.h>

#include "Buffer.h"

namespace HELIUM
{
    /**
     * 使用zlib库压缩数据
     *
     */
    class CZlibCompress
    {
        public:
            explicit CZlibCompress(CBuffer* output);
            ~CZlibCompress();

            const char* zlibErrorMessage(void) const { return m_ZStream.msg; }
            int zlibErrorCode(void) const { return m_ZError; }
            int64_t inputBytes(void) const { return m_ZStream.total_in; }
            int64_t outputBytes(void) const { return m_ZStream.total_out; }
            int internalOutputBufSize(void) const { return m_BufferSize; }

            bool write(CBuffer* input);
            bool finish(void);

        private:
            CZlibCompress(const CZlibCompress&);
            CZlibCompress& operator=(const CZlibCompress&);

            /// 压缩数据
            int compress(int flush);

        private:
            /// 输出缓冲区
            CBuffer* m_Output;

            z_stream m_ZStream;

            /// zlib错误码
            int m_ZError;

            int m_BufferSize;
    };

    /**
     * 使用zlib库解压数据
     *
     */
    class CZlibDecompress
    {
        public:
            explicit CZlibDecompress(CBuffer* output);
            ~CZlibDecompress();

            const char* zlibErrorMessage(void) const { return m_ZStream.msg; }
            int zlibErrorCode(void) const { return m_ZError; }
            int64_t inputBytes(void) const { return m_ZStream.total_in; }
            int64_t outputBytes(void) const { return m_ZStream.total_out; }
            int internalOutputBufSize(void) const { return m_BufferSize; }

            bool write(CBuffer* input);
            bool finish(void);

        private:
            CZlibDecompress(const CZlibDecompress&);
            CZlibDecompress& operator=(const CZlibDecompress&);

            /// 解压数据
            int decompress(int flush);

        private:
            /// 输出缓冲区
            CBuffer* m_Output;

            z_stream m_ZStream;

            /// zlib错误码
            int m_ZError;

            int m_BufferSize;
    };
}

#endif //_ZLIB_STREAM_H
