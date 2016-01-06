/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Assure.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月24日 13时39分01秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _ASSURE_H
#define _ASSURE_H

#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "Logger.h"

/**
 * assert()包裹函数的集合
 *
 */

/**
 * @def
 * 触发断言时，退出程序
 *
 */
#define ASSERT_EXIT(exp) \
    do {\
        if ( !(exp) ) {\
            DBG_LOG((HELIUM::FATALERR, "Assert aborted false expression!\n")); \
            DBG_LOG((HELIUM::FATALERR, "Error on line %d in file %s\n", __LINE__, __FILE__)); \
            raise(SIGTERM); \
        } \
    } while (0)

/**
 * @def
 * 触发断言时，退出函数
 *
 * @param exp 表达式
 *
 * @return false -- 表达式的值为false; 空 -- 表达式的值为true(继续函数的执行)
 *
 */
#define ASSERT_RETURN(exp) \
    do {\
        if ( !(exp) ) {\
            DBG_LOG((HELIUM::FATALERR, "Assert return false expression!\n")); \
            DBG_LOG((HELIUM::FATALERR, "Error on line %d in file %s\n", __LINE__, __FILE__)); \
            return (false); \
        } \
    } while (0)

/**
 * @def
 * 触发断言时，退出函数
 *
 * @param exp 表达式
 *
 */
#define ASSERT_RETURN_VOID(exp) \
    do {\
        if ( !(exp) ) {\
            DBG_LOG((HELIUM::FATALERR, "Assert return false expression!\n")); \
            DBG_LOG((HELIUM::FATALERR, "Error on line %d in file %s\n", __LINE__, __FILE__)); \
            return; \
        } \
    } while (0)

#endif //_ASSURE_H
