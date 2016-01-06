/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Packet.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月23日 13时41分47秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _PACKET_H
#define _PACKET_H

#include <stdint.h>
#include <assert.h>
#include <vector>
#include <algorithm>

namespace HELIUM
{
#pragma pack(push, 1)
    typedef struct stCapsulHeader
    {
        uint16_t hdrflag;      /// 报头标记
        uint16_t length;       /// 报文长度
        uint16_t seqnum;       /// 报文编号
        uint8_t  senderid;     /// 发送者标识
    }CAPSUL_HEADER;

    typedef struct stSubPackHeader
    {
        uint16_t dataid;       /// 数据标识
        uint16_t len;          /// 子报文长度
    }SUBPACK_HEADER;
#pragma pack(pop)

    /**
     * 各模块之间用于数据交换的报文封装。
     *
     */
    class CExchangePacket
    {
        public:
            CExchangePacket(uint8_t sender);

            /**
             * 将内容封装成一个完整的报文
             *
             * @param sender  发送者标识
             * @param dataid  数据标识
             * @param content 数据内容
             * @param len     数据长度
             *
             */
            CExchangePacket(uint8_t sender, uint16_t dataid, const char*content, size_t len);

            ~CExchangePacket() {}

            /// 可读字节数
            size_t readableBytes(void) const;

            /// 可写字节数
            size_t writableBytes(void) const;

            /**
             * 获取缓冲区的起始地址
             *
             */
            uint8_t* begin(void);
            const uint8_t* begin(void) const;

            /**
             * 向缓冲区中追加数据
             *
             * @param data 数据地址
             * @param len  数据长度
             *
             */
            void append(const uint8_t* data, size_t len);

            /// 确保有足够的可写入空间
            void ensureWritableBytes(size_t len);

            /// 更新可写缓冲区的首地址
            void hasWritten(size_t len);

            /// 重设缓冲区的状态
            void reset(void);

            /// 完成报文的封装
            void finish(void);

            /**
             * 查找一帧数据
             *
             * @param [in]  begin      搜索缓冲区的起始位置
             * @param [in]  end        搜索缓冲区的结束位置
             * @param [out] frame_len  数据帧长度
             *
             * @return 数据帧的起始地址
             */
            static const char* findFrame(const char* begin, const char* const end, uint16_t* frame_len);

        private:
            /// 初始化报文头部
            void initPacketHeader(uint8_t sender);

            /**
             * 分配新的空间
             *
             * @param len 待写入数据的长度
             *
             */
            void makeSpace(size_t len);

            /**
             * 获取缓冲区的可写地址
             *
             */
            uint8_t* beginWrite(void);

            /// 报文缓冲区
            std::vector<uint8_t> m_Buffer;

            /// 缓冲区可写地址
            size_t m_writerIndex;

            /// 报文标识
            static const uint16_t m_PacketFlag;

            /// 缓冲区初始大小
            static const size_t m_initialSize;
    };

    inline size_t CExchangePacket::readableBytes(void) const
    {
        return m_writerIndex;
    }

    inline size_t CExchangePacket::writableBytes(void) const
    {
        return (m_Buffer.size() - m_writerIndex);
    }

    inline void CExchangePacket::hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        m_writerIndex += len;
    }

    inline uint8_t* CExchangePacket::begin(void)
    {
        return &*m_Buffer.begin();
    }

    inline const uint8_t* CExchangePacket::begin(void) const
    {
        return &*m_Buffer.begin();
    }

    inline uint8_t* CExchangePacket::beginWrite(void)
    {
        return (begin() + m_writerIndex);
    }
}

#endif //_PACKET_H
