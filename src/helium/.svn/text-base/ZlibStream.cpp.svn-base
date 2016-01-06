/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: ZlibStream.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月31日 13时09分02秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "ZlibStream.h"

using namespace HELIUM;

CZlibCompress::CZlibCompress(CBuffer* output) : m_Output(output), m_ZError(Z_OK), m_BufferSize(1024)
{
    memset(&m_ZStream, 0, sizeof(m_ZStream));
    m_ZError = deflateInit(&m_ZStream, Z_DEFAULT_COMPRESSION);
}

CZlibCompress::~CZlibCompress()
{
    finish();
}

bool CZlibCompress::write(CBuffer* input)
{
    if (m_ZError != Z_OK)
        return false;

    void* in = const_cast<char*>(input->peek());
    m_ZStream.next_in = static_cast<Bytef*>(in);
    m_ZStream.avail_in = static_cast<int>(input->readableBytes());
    while (m_ZStream.avail_in > 0 && m_ZError == Z_OK)
    {
        m_ZError = compress(Z_NO_FLUSH);
    }

    if (m_ZStream.avail_in == 0)
    {
        m_ZStream.next_in = NULL;
    }

    return (m_ZError == Z_OK);
}

bool CZlibCompress::finish(void)
{
    if (m_ZError != Z_OK)
        return false;

    while (m_ZError == Z_OK)
    {
        m_ZError = compress(Z_FINISH);
    }

    m_ZError = deflateEnd(&m_ZStream);

    bool ok = (m_ZError == Z_OK);
    m_ZError = Z_STREAM_END;

    return ok;
}

int CZlibCompress::compress(int flush)
{
    m_Output->ensureWritableBytes(m_BufferSize);
    m_ZStream.next_out = reinterpret_cast<Bytef*>(m_Output->beginWrite());
    m_ZStream.avail_out = static_cast<int>(m_Output->writableBytes());

    int error = ::deflate(&m_ZStream, flush);
    m_Output->hasWritten(m_Output->writableBytes() - m_ZStream.avail_out);
    if (m_Output->writableBytes() == 0 && m_BufferSize < 65536)
    {
        m_BufferSize *= 2;
    }

    return error;
}

/////////////////////////////////////////////////////////////////////////////////////////

CZlibDecompress::CZlibDecompress(CBuffer* output) : m_Output(output), m_ZError(Z_OK), m_BufferSize(1024)
{
    memset(&m_ZStream, 0, sizeof(m_ZStream));
    m_ZError = inflateInit(&m_ZStream);
}

CZlibDecompress::~CZlibDecompress()
{
    finish();
}

bool CZlibDecompress::write(CBuffer* input)
{
    if (m_ZError != Z_OK)
        return false;

    void* in = const_cast<char*>(input->peek());
    m_ZStream.next_in = static_cast<Bytef*>(in);
    m_ZStream.avail_in = static_cast<int>(input->readableBytes());
    while (m_ZStream.avail_in > 0 && m_ZError == Z_OK)
    {
        m_ZError = decompress(Z_NO_FLUSH);
    }

    if (m_ZStream.avail_in == 0)
    {
        m_ZStream.next_in = NULL;
    }

    return (m_ZError == Z_OK);
}

bool CZlibDecompress::finish(void)
{
    if (m_ZError != Z_OK)
        return false;

    while (m_ZError == Z_OK)
    {
        m_ZError = decompress(Z_FINISH);
    }

    m_ZError = inflateEnd(&m_ZStream);

    bool ok = (m_ZError == Z_OK);
    m_ZError = Z_STREAM_END;

    return ok;
}

int CZlibDecompress::decompress(int flush)
{
    m_Output->ensureWritableBytes(m_BufferSize);
    m_ZStream.next_out = reinterpret_cast<Bytef*>(m_Output->beginWrite());
    m_ZStream.avail_out = static_cast<int>(m_Output->writableBytes());

    int error = ::inflate(&m_ZStream, flush);
    m_Output->hasWritten(m_Output->writableBytes() - m_ZStream.avail_out);
    if (m_Output->writableBytes() == 0 && m_BufferSize < 65536)
    {
        m_BufferSize *= 2;
    }

    return error;
}

