/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: LogMask.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月16日 18时18分44秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _LOG_MASK_H
#define _LOG_MASK_H

namespace HELIUM
{
    enum LogGroup
    {
        TRACE       = 0x00000001,    /**< */
        APP         = 0x00000002,    /**< */
        ALL_APPS    = 0x00000004,    /**< */
        FATALERR    = 0x00000008,    /**< */
        /*-----------------------------------------------------------------------------*/
        REACTOR     = 0x00000010,    /**< */
        SOCKET      = 0x00000020,    /**< */
        DATASWITCH  = 0x00000040,    /**< 数据交换模块 */
        MTSERVICE   = 0x00000080,    /**< 维护服务模块 */
        COREPROC    = 0x00000100,    /**< 核心业务处理模块 */
        TRANSCTRL   = 0x00000200,    /**< 传输控制模块 */
        DATAWATCHER = 0x00000400,    /**< 数据打印输出 */
        ALL_LIB     = 0x7FFFFFE0,    /**< */
        ALL         = 0x7FFFFFFF,    /**< */
        NONE        = 0x00000000     /**< */
    };

    enum MarkType
    {
        FUNC_MSG,
        FUNC_ENTRY,
        FUNC_EXIT
    };
}

#endif //_LOG_MASK_H
