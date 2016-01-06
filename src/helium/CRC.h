/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CRC.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月23日 17时19分35秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _CRC_H
#define _CRC_H

namespace HELIUM
{
    /**
     * CRC32采用IEEE 802.3标准，其生成多项式为:
     * 1 + x + x^2 + x^4 + x^5 + x^7 + x^8 + x^10 + x^11 + x^12 + x^16 + x^22 + x^23 + x^26 + x^32,
     * 多项式对应的数字为：11101101101110001000001100100000(x^32在实际计算中隐含给出，因此不包含它的系数),
     * 将它转换为16进制数字即为0xEDB88320。多项式对应的数字可以颠倒，颠倒后即为0x04C11DB7。
     * 程序中两种方式都会用到。
     *
     * CRC16使用了两个标准，分别为:
     * CCITT版，多项式为 1 + x^5 + x^12 + x^16 (0x1021 或 0x8408); 以及
     * IBM版，  多项式为 1 + x^2 + x^15 + x^16 (0x8005 或 0xA001)
     *
     */
    template <class T>
    class Crc
    {
        public:
            /**
             * 构造函数
             *
             * @param loworder_polygon  低次幂优先的多项式
             * @param highorder_polygon 高次幂优先的多项式
             *
             */
            explicit Crc(T loworder_polygon, T highorder_polygon);
            ~Crc() {}

            /// 使用高次幂优先的多项式计算
            T calcHOPP(const void* data, int len, T init);

            /// 使用低次幂优先的多项式计算
            T calcLOPP(const void* data, int len, T init);

        private:
            void initLowOrderPPTable(T polygon);
            void initHighOrderPPTable(T polygon);

        private:
            T m_LowOrderPPTable[256];
            T m_HighOrderPPTable[256];
    };

    template <class T>
    Crc<T>::Crc(T loworder_polygon, T highorder_polygon)
    {
        initLowOrderPPTable(loworder_polygon);
        initHighOrderPPTable(highorder_polygon);
    }

    template <class T>
    void Crc<T>::initLowOrderPPTable(T polygon)
    {
        for (unsigned i = 0; i < 256; ++i)
        {
            T reg = i;

            for (int j = 0; j < 8; ++j)
            {
                bool lowBit = (reg & 1) != 0;
                reg >>= 1;
                if (lowBit)
                    reg ^= polygon;
            }
            m_LowOrderPPTable[i] = reg;
        }
    }

    template <class T>
    void Crc<T>::initHighOrderPPTable(T polygon)
    {
        for (unsigned i = 0; i < 256; ++i)
        {
            T reg = i << ((sizeof(T) << 3) - 8);

            for (int j = 0; j < 8; ++j)
            {
                bool topBit = (reg >> ((sizeof(T) << 3) - 1)) != 0;
                reg <<= 1;
                if (topBit)
                    reg ^= polygon;
            }
            m_HighOrderPPTable[i] = reg;
        }
    }

    template <class T>
    T Crc<T>::calcHOPP(const void* data, int len, T init)
    {
        const unsigned char* pbyte = (const unsigned char*)data;
        T cp;
        T crc = init;

        while (len-- > 0)
        {
            cp = (crc >> ((sizeof(T) << 3) - 8)) ^ *pbyte;
            crc = (crc << 8) ^ m_HighOrderPPTable[cp];
            pbyte++;
        }

        /// 32位校验和将结果取反
        if (sizeof(T) == 4)
            return ~crc;
        else
            return crc;
    }

    template <class T>
    T Crc<T>::calcLOPP(const void* data, int len, T init)
    {
        const unsigned char* pbyte = (const unsigned char*)data;
        T cp;
        T crc = init;

        while (len-- > 0)
        {
            cp = (crc & 0xff) ^ *pbyte;
            crc = (crc >> 8) ^ m_LowOrderPPTable[cp];
            pbyte++;
        }

        return ~crc;
    }
}

#endif //_CRC_H
