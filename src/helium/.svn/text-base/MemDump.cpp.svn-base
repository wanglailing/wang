/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: MemDump.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月27日 16时03分22秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "MemDump.h"

using namespace HELIUM;

CMemDump::CMemDump(const char *msg, int len) : m_Dump(NULL)
{
    register int i;		// ptr into source buffer
    register int j;		// pair elements counter
    register int k;		// pairs counter [0;16[

    const char *p;		// 源缓冲区的指针
    char *hex;		        // 指向目标缓冲区16进制部分的指针
    char *ascii;		// 指向目标缓冲区ASC-II部分的指针

    long final_len;

    if (len <= 0 || msg == (char*) NULL) 
    {
        DBG_LOG((FATALERR, "No data to process. Data length requested: %d <= 0!\n", len));
        return;
    }

    j = k = 1;

    /*
     * 每行包含16个字节的数据，最多需要74个字符
     * 示例如下：
     *
     * 0         1         2         3         4         5         6         7
     * 0123456789012345678901234567890123456789012345678901234567890123456789012
     * -------------------------------------------------------------------------
     * 3132 3037 3039 3039 3031 3130 3839 3033  1207090901108903
     * 3038 3132 3030 3331 3030 0d0a 3839 3033  0812003100\r\n8903
     * 0d0a 0d0a 0d0a 0d0a 0d0a 0d0a 0d0a 0d0a  \r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n
     *
     * 如果全部16个字节的数据都是控制字符，它们的ASCII表示将会扩展到72个字符，
     * 再加上换行符和字符串结束标记，刚好是74个字符。
     *
     * 如果长度不是16的倍数，则会另起一行存放剩余的数据。
     *
     */

    final_len = (int (len/16) + 1 + (len % 16 ? 1:0)) * 74;

    m_Dump = new char[final_len];
    memset (m_Dump, ' ', final_len);

    p     = msg;				
    hex   = m_Dump;		
    ascii = m_Dump + 41;

    for (i = 0; i < len; i++) 
    {
        sprintf(hex, "%01X%01X", p[i] >> 4 & 0x0f, p[i] & 0x0f); 
        hex += 2;

        if      (p[i] == '\n') { sprintf(ascii, "\\n"); ascii += 2; }
        else if (p[i] == '\t') { sprintf(ascii, "\\t"); ascii += 2; }
        else if (p[i] == '\v') { sprintf(ascii, "\\v"); ascii += 2; }
        else if (p[i] == '\b') { sprintf(ascii, "\\b"); ascii += 2; }
        else if (p[i] == '\r') { sprintf(ascii, "\\r"); ascii += 2; }
        else if (p[i] == '\f') { sprintf(ascii, "\\f"); ascii += 2; }
        else if (p[i] == '\a') { sprintf(ascii, "\\a"); ascii += 2; }
        else if (p[i] == '\0') { sprintf(ascii, "\\0"); ascii += 2; }
        else {
            sprintf(ascii++, "%c", ((p[i] < ' ' || p[i] > '~') ? '.' : p[i]));
        }

        if (!(j++ % 2)) {
            sprintf(hex++, " ");
        }

        k %= 16;

        if (!(k++)) {
            *hex = ' ';
            sprintf(ascii++,"\n");
            hex = ascii;
            ascii +=  41;
        }
    }
    *hex = ' ';
    m_Dump [final_len-1] = '\0';
}

void CMemDump::dumpToLog(unsigned long mask, const char *info, const char *msg, int len)
{
    if (LOGGER->groupEnabled(static_cast<LogGroup>(mask)) && len > 0) 
    {
        CMemDump temp(msg, len);
        DBG_LOG((mask, "(%d bytes) %s\n", len, info));
        DBG_LOG((mask, "\n\n%s\n\n", temp.getMemDump()));
    }
}
