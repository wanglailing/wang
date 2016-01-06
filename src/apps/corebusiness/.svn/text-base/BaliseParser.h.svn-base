/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: BaliseParser.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月28日 15时09分50秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _BALISE_PARSER_H
#define _BALISE_PARSER_H

namespace COREBUSINESS
{
    class CBaliseParser
    {
        public:
            /// 应答器头部各域的索引名, 这些枚举值必须是从0开始的连续值
            typedef enum emHeaderFieldIndex
            {
                HDRFIELD_QUPDOWN = 0,         /**< 信息传送方向 */
                HDRFIELD_MVERSION,            /**< 语言/代码版本编号 */
                HDRFIELD_QMEDIA,              /**< 信息传输媒介 */
                HDRFIELD_NPIG,                /**< 本应答器在应答器组中的位置 */
                HDRFIELD_NTOTAL,              /**< 应答器组所包含的应答器数 */
                HDRFIELD_MDUP,                /**< 本条信息与前后信息的关系 */
                HDRFIELD_MMCOUNT,             /**< 报文计数器 */
                HDRFIELD_NIDC,                /**< 地区编号 */
                HDRFIELD_NIDBG,               /**< 应答器标识号 */
                HDRFIELD_QLINK,               /**< 应答器组的链接关系 */
                HDRFIELD_TOTAL
            }HDRFIELD_INDEX;

            /// 定义应答器头部各字段的布局
            typedef struct stHdrFieldLayout
            {
                HDRFIELD_INDEX index;
                int            bitlen;
                unsigned short value;
            }HDR_LAYOUT;

        public:
            CBaliseParser() {}
            ~CBaliseParser() {}

            /**
             * 解析应答器编号
             *
             * @param [in]  balise 应答器数据
             * @param [in]  length 应答器数据长度
             *
             * @return 应答器编号
             *
             */
            static unsigned int getBaliseNo(const unsigned char* balise, int length);

            /**
             * 解析应答器编号
             *
             * @param [in]  baliseno 应答器编号
             * @param [out] dump_buf 应答器编号输出缓冲区  
             * @param [in]  buf_len  输出缓冲区长度
             *
             */
            static void dumpBaliseNo(unsigned int baliseno, char* dump_buf, int buf_len);

        private:
            CBaliseParser(const CBaliseParser&);
            CBaliseParser& operator=(const CBaliseParser&);

            /// 头部各域的布局
            static HDR_LAYOUT HdrFieldsLayout[];
    };
}

#endif //_BALISE_PARSER_H
