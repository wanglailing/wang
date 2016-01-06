/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Buffer.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月25日 13时32分48秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <errno.h>
#include <sys/uio.h>

#include "Buffer.h"

using namespace HELIUM;

const size_t CBuffer::m_initialSize = 1024;
const size_t CBuffer::m_cheapPrepend = 32;

void CBuffer::ensureWritableBytes(size_t len)
{
    if (writableBytes() < len)
    {
        makeSpace(len);
    }
    assert(writableBytes() >= len);
}

void CBuffer::append(const char *data, size_t len)
{
    if (data == NULL || len <= 0)
        return;

    ensureWritableBytes(len);
    std::copy(data, data + len, beginWrite());
    hasWritten(len);
}

void CBuffer::prepend(const void *data, size_t len)
{
    assert(len <= prependableBytes());
    m_readerIndex -= len;
    const char *d = static_cast<const char *>(data);
    std::copy(d, d + len, begin() + m_readerIndex);
}

void CBuffer::retrieve(size_t len)
{
    assert(len <= readableBytes());
    if (len < readableBytes())
    {
        m_readerIndex += len;
    }
    else
    {
        retrieveAll();
    }
}

const char* CBuffer::findString(const char *str, size_t len)
{
    const char *end = beginWrite();
    const char *dest = std::search(peek(), end, str, str + len);
    return (dest == end ? NULL : dest);
}

ssize_t CBuffer::readFd(int fd, int *saved_errno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + m_writerIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    const int iov_cnt = (writable < sizeof(extrabuf) ? 2 : 1);
    const ssize_t n = ::readv(fd, vec, iov_cnt);
    if (n < 0)
    {
        *saved_errno = errno;
    }
    else if ((size_t)(n) <= writable)
    {
        m_writerIndex += n;
    }
    else
    {
        m_writerIndex = m_Buff.size();
        append(extrabuf, n - writable);
    }

    return n;
}

void CBuffer::makeSpace(size_t len)
{
    if (writableBytes() + prependableBytes() < len + m_cheapPrepend)
    {
        m_Buff.resize(m_writerIndex + len);
    }
    else
    {
        assert(m_cheapPrepend < m_readerIndex);
        size_t readable = readableBytes();
        std::copy(begin() + m_readerIndex,
                  begin() + m_writerIndex,
                  begin() + m_cheapPrepend);
        m_readerIndex = m_cheapPrepend;
        m_writerIndex = m_readerIndex + readable;
        assert(readable == readableBytes());
    }
}

