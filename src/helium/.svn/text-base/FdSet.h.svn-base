/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: FdSet.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月20日 09时49分41秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _FDSET_H
#define _FDSET_H

#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <list>

#include "Logger.h"

namespace HELIUM
{
    /**
     * 结构体fd_set的包裹类
     *
     */
    class CFdSet : public fd_set
    {
        public:
            CFdSet();

            /**
             * 设置fd的标志为打开
             *
             * @param fd 文件描述符
             *
             * @return false -- fd越界； true -- 其它
             */
            bool setFd(handler_t fd);

            /**
             * 设置fd的标志为关闭
             *
             * @param fd 文件描述符
             *
             * @return false -- fd越界； true -- 其它
             */
            bool clear(handler_t fd);

            /**
             * 测试fd的标志是否打开
             *
             * @param fd 文件描述符
             *
             * @return false -- 其它； true -- 打开
             */
            bool isSet(handler_t fd);

            /// 将没有标记为打开的描述符从列表中删除
            void sync(void);

            /// 重置描述符集中的所有位
            void reset(void);

            /**
             * 获取描述符集中的最大描述符
             *
             *
             * @return 最大文件描述符
             */
            int maxInSet(void);

            /**
             * 描述符集中已标记为打开的描述符个数
             *
             *
             * @return 打开的描述符个数
             */
            int numSet(void);

            /// 转储调试信息到日志中
            void dump(void);

            /// 调试信息
            std::string m_DumpInfo;

        private:
            typedef std::list<unsigned int>::iterator ActiveFdsIter;
            std::list<unsigned int> m_ActiveFds;
    };

    inline CFdSet::CFdSet() { reset(); }
    inline void CFdSet::dump(void) {}
    inline bool CFdSet::isSet(handler_t fd) { return FD_ISSET(fd, this); }
    inline int CFdSet::numSet(void) { return m_ActiveFds.size(); }
}

#endif //_FDSET_H
