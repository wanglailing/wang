/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: App.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月12日 13时02分49秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _APP_H_
#define _APP_H_

#include <string>

#include "EventHandler.h"
#include "Logger.h"
#include "Reactor.h"

namespace HELIUM
{
    /**
     * 应用程序框架。
     *
     */
    class CApp : public virtual CEventHandler
    {
        public:
            enum LogFlag
            {
                KEEPLOG,    /**< 向当前日志中追加内容 */
                RMLOG       /**< 先将已存在的日志删除，然后再写入新的内容 */
            };

        public:
            CApp();
            virtual ~CApp();

            /**
             * 打开日志,处理标准命令行参数
             *
             * @param argc 命令行参数个数的指针
             * @param argv 命令行参数指针
             * @param help_msg 帮助信息
             *
             */
            virtual void init(int *argc, char *argv[], const char *help_msg);

            /**
             * 将帮助信息打印到标准输出
             */
            virtual void displayHelp(void);

            /**
             * 服务初始化接口
             */
            virtual void initService(void) = 0;

            /**
             * 事件处理接口
             */
            virtual void processEvents(void) = 0;

            /// 严重出错信号的处理。由派生类根据自身的应用实现
            virtual void fatalSignalHook(void) {}

            /// 信号处理
            int handleSignal(int signum);

            /// 服务是否处于活动状态
            bool serviceActive(void) const { return !m_GracefulQuit; }

            /// 停止服务
            void stopService(void);

            /// 获取应用程序名
            std::string getProcName(void) const { return m_ProcName; }

            /// 获取版本号
            std::string getVersion(void) const { return m_Version; }

            /**
             * 获取反应堆对象
             */
            CReactor* getReactor(void) { return &m_Reactor; }

            /**
             * 获取程序退出值
             */
            int getExitValue(void) const { return m_ExitValue; }

        protected:
            /// 应用程序名
            std::string m_ProcName;

            /// 程序停止并退出的标志
            bool m_GracefulQuit;

            /// 反应堆对象
            CReactor m_Reactor;

            /// 程序版本号
            std::string m_Version;

            /// 应用程序帮助信息
            const char *m_HelpMsg;

            /// 日志创建方式
            LogFlag m_LogFlag;

            /// 日志掩码
            unsigned long m_LogMask;

            /// 将程序打印到标准输出
            std::string m_LogStdout;

            /// 程序退出后的返回值
            int m_ExitValue;

        private:
            CApp(const CApp&);
            CApp& operator=(const CApp&);

            /// 应用程序内部初始化
            void initInternals(void);
    };
}

#endif // _APP_H_
