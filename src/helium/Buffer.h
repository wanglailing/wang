/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Buffer.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月25日 10时16分27秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _BUFFER_H
#define _BUFFER_H

#include <vector>
#include <algorithm>
#include <assert.h>
#include <string.h>

namespace HELIUM
{
    /**
     * 通道缓冲区操作
     *
     */
    class CBuffer
    {
        public:
            CBuffer();
            ~CBuffer() {}

            /**
             * 获取缓冲区的可读位置
             *
             * @return 可读缓冲区起始地址
             *
             */
            const char *peek(void) const;

            /// 可读字节数
            size_t readableBytes(void) const;

            /// 可写字节数
            size_t writableBytes(void) const;

            /// 已丢弃字节数
            size_t prependableBytes(void) const;

            /**
             * 向缓冲区中追加数据
             *
             * @param data 数据地址
             * @param len  数据长度
             *
             */
            void append(const char *data, size_t len);
            void append(const void *data, size_t len);

            /// 在可读缓冲区起始部分添加数据
            void prepend(const void *data, size_t len);

            /// 更新可读缓冲区的首地址
            void retrieve(size_t len);
            void retrieveAll(void);
            void retrieveUntil(const char *end);

            /**
             * 在缓冲区中查找字符串
             *
             * @param str 被查找的字符串
             * @param len 字符串长度
             *
             * @return 字符串在缓冲区中的位置
             */
            const char *findString(const char *str, size_t len);

            /**
             * 从描述符中读取数据
             *
             * @param fd         描述符
             * @param save_errno 出错编号
             *
             * @return 已读取数据的长度
             */
            ssize_t readFd(int fd, int *saved_errno);

            /// 确保有足够的可写入空间
            void ensureWritableBytes(size_t len);

            /// 更新可写缓冲区的首地址
            void hasWritten(size_t len);

            /**
             * 获取缓冲区的可写地址
             *
             */
            char *beginWrite(void);
            const char *beginWrite(void) const;

        private:
            /**
             * 分配新的空间
             *
             * @param len 待写入数据的长度
             *
             */
            void makeSpace(size_t len);

            /**
             * 获取缓冲区的起始地址
             *
             */
            char *begin(void);
            const char *begin(void) const;

        private:
            /// 缓冲区容器
            std::vector<char> m_Buff;

            /// 读缓冲区首地址
            size_t m_readerIndex;

            /// 写缓冲区首地址
            size_t m_writerIndex;

            static const size_t m_initialSize;
            static const size_t m_cheapPrepend;
    };

    inline CBuffer::CBuffer()
        : m_Buff(m_initialSize + m_cheapPrepend),
          m_readerIndex(m_cheapPrepend),
          m_writerIndex(m_cheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == m_initialSize);
        assert(prependableBytes() == m_cheapPrepend);
    }

    inline const char* CBuffer::peek(void) const
    {
        return (begin() + m_readerIndex);
    }

    inline size_t CBuffer::readableBytes(void) const
    {
        return (m_writerIndex - m_readerIndex);
    }

    inline size_t CBuffer::writableBytes(void) const
    {
        return (m_Buff.size() - m_writerIndex);
    }

    inline size_t CBuffer::prependableBytes(void) const
    {
        return m_readerIndex;
    }

    inline void CBuffer::hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        m_writerIndex += len;
    }

    inline void CBuffer::append(const void *data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    inline void CBuffer::retrieveAll(void)
    {
        m_readerIndex = m_cheapPrepend;
        m_writerIndex = m_cheapPrepend;
    }

    inline void CBuffer::retrieveUntil(const char *end)
    {
        assert(peek() <= end && end <= beginWrite());
        retrieve(end - peek());
    }

    inline char* CBuffer::begin(void)
    {
        return &*m_Buff.begin();
    }

    inline const char* CBuffer::begin(void) const
    {
        return &*m_Buff.begin();
    }

    inline char* CBuffer::beginWrite(void)
    {
        return (begin() + m_writerIndex);
    }

    inline const char* CBuffer::beginWrite(void) const
    {
        return (begin() + m_writerIndex);
    }
}

#endif //_BUFFER_H
