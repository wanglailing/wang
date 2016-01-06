/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: EventHandler.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月18日 10时08分22秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <string>

namespace HELIUM
{
    /**
     * @file
     * 响应各种事件的抽象接口(如IO事件、定时器事件和信号事件等)
     *
     */

    /// 定时器ID在定时器事件响应函数中被使用
    typedef unsigned long TimerId;

    /**
     * @enum
     * 定义由反应堆使用的各种事件
     *
     */
    enum EventType
    {
        READ_EVENT    = 0x01,    /**< 读事件 */
        WRITE_EVENT   = 0x02,    /**< 写事件 */
        EXCEPT_EVENT  = 0x04,    /**< 异常事件 */
        TIMEOUT_EVENT = 0x10,    /**< 超时事件 */
        SIGNAL_EVENT  = 0x20,    /**< 信号事件 */
        RWE_EVENT     = 0x07,    /**< 读、写和异常事件 */
        ALL_EVENTS    = 0x37,    /**< 所有的事件 */
    };

    inline bool isReadEvent(EventType et) 
    {
        return ( (et & READ_EVENT) == READ_EVENT );
    }

    inline bool isWriteEvent(EventType et) 
    {
        return ( (et & WRITE_EVENT) == WRITE_EVENT );
    }

    inline bool isExceptEvent(EventType et) 
    {
        return ( (et & EXCEPT_EVENT) == EXCEPT_EVENT );
    }

    inline bool isTimeoutEvent(EventType et) 
    {
        return ( (et & TIMEOUT_EVENT) == TIMEOUT_EVENT );
    }

    inline bool isSignalEvent(EventType et) 
    {
        return ( (et & SIGNAL_EVENT) == SIGNAL_EVENT );
    }

    inline bool isRWEEvent(EventType et) 
    {
        return ( isReadEvent(et) && isWriteEvent(et) && isExceptEvent(et) );
    }

    /**
     * CEventHandler是一个抽象类，它是各种事件响应的抽象接口。
     * 目前支持的事件有信号事件、定时器事件和IO事件。
     *
     * @see EventType
     */ 
    class CEventHandler
    {
        public:
            CEventHandler();
            virtual ~CEventHandler() {}

            /**
             * 读事件的回调函数
             *
             * @return 0 -- 成功; -1 -- 失败。
             */ 
            virtual int handleRead(int fd);

            /**
             * 写事件的回调函数
             *
             * @return 0 -- 成功; -1 -- 失败。
             */ 
            virtual int handleWrite(int fd);

            /**
             * 异常事件的回调函数
             *
             * @return 0 -- 成功; -1 -- 失败。
             */ 
            virtual int handleException(int fd);

            /**
             * 超时事件的回调函数
             *
             * @return 0 -- 其它; 1 -- 重新定时。
             */ 
            virtual int handleTimeout(TimerId tid);

            /**
             * 信号事件的回调函数
             *
             * @return 0 -- 成功; -1 -- 失败。
             */ 
            virtual int handleSignal(int signum);

            /**
             * 对端结束的回调函数。没有与之对应的事件，一般情况下，它将做为
             * handleRead()处理过程的一部分。
             *
             * @return 0 -- 成功; -1 -- 失败。
             */ 
            virtual int handleClose(int fd);

            /// 设置事件响应器的ID
            void setId(const std::string &id) { m_Id = id; }

            /// 获取事件响应器的ID
            std::string getId(void) const { return m_Id; }

        protected:
            /// 事件响应器的ID
            std::string m_Id;
    };

    inline CEventHandler::CEventHandler() : m_Id("EventHandler") {}

    inline int CEventHandler::handleRead(int /*fd*/) { return -1; }
    inline int CEventHandler::handleWrite(int /*fd*/) { return -1; }
    inline int CEventHandler::handleException(int /*fd*/) { return -1; }
    inline int CEventHandler::handleClose(int /*fd*/) { return -1; }
    inline int CEventHandler::handleTimeout(TimerId /*tid*/) { return -1; }
    inline int CEventHandler::handleSignal(int /*signum*/) { return -1; }

    /**
     * CEventHandler类中I/O相关的回调函数的函数指针
     *
     * @see CEventHandler
     */
    typedef int (CEventHandler::*EH_IO_CALLBACK)(int);
}

#endif //_EVENT_HANDLER_H
