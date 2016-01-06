/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TimeVal.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月18日 09时07分31秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _TIMEVAL_H
#define _TIMEVAL_H

#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#include <string>

namespace HELIUM
{
    /**
     * UNIX系统timeval的类包装
     *
     */
    class CTimeVal : public timeval
    {
        public:
            enum Zone
            {
                GMT,
                LOCAL
            };

            CTimeVal();

            /**
             * 将秒/微妙对转换成CTimeVal对象
             *
             * @param secs 秒
             * @param msecs 微秒
             *
             */
            CTimeVal(long secs, long msecs);

            /**
             * 将浮点数转换成CTimeVal对象
             *
             * @param secs 秒
             *
             */
            CTimeVal(double secs);

            /**
             * 将结构体timeval转换成CTimeVal对象
             *
             * @param tv 秒
             *
             */
            CTimeVal(const timeval& tv);

            /// 复制构造函数
            CTimeVal(const CTimeVal& tv);

            /// 转换成浮点数
            operator double() const;

            /// 设置秒数
            void setSec(long secs) { tv_sec = secs; }

            /// 获取秒数
            long getSec(void) const { return tv_sec; }

            /// 设置微妙
            void setMicroSec(long msecs) { tv_usec = msecs; }

            /// 获取微妙
            long getMicroSec(void) const { return tv_usec; }

            /// 将微妙转换为毫秒
            long milliSec(void) const;

            /// 赋值
            CTimeVal& operator=(const CTimeVal& tv);

            /// 加
            CTimeVal& operator+=(const CTimeVal& rhs);
            friend CTimeVal operator+(const CTimeVal& lhs, const CTimeVal& rhs);

            /// 减
            CTimeVal& operator-=(const CTimeVal& rhs);
            friend CTimeVal operator-(const CTimeVal& lhs, const CTimeVal& rhs);

            /// 小于
            bool operator<(const CTimeVal& rhs) const;

            /// 等于
            bool operator==(const CTimeVal& rhs) const;

            /// 大于
            friend bool operator>(const CTimeVal& lhs, const CTimeVal& rhs);

            /// 不等于
            friend bool operator!=(const CTimeVal& lhs, const CTimeVal& rhs);

            /// 小于等于
            friend bool operator<=(const CTimeVal& lhs, const CTimeVal& rhs);

            /// 大于等于
            friend bool operator>=(const CTimeVal& lhs, const CTimeVal& rhs);

            /**
             * 将时间格式化为字符串
             *
             * @param fmt 格式
             *
             */
            std::string fmtString(const char *fmt) const;

            /// 格式化为 时:分:秒
            std::string fmtHHMMSS(void) const;

            /// 格式化为 年-月-日
            std::string fmtYYMMDD(void) const;

            /// 格式化为 时:分:秒:毫秒
            std::string fmtHHMMSSMLS(void) const;

            /// 格式化为 分:秒
            std::string fmtMMSS(void) const;

            /// 格式化为 分:秒:毫秒
            std::string fmtMMSSMLS(void) const;

            /// 格式化为 秒:毫秒
            std::string fmtSSMLS(void) const;

            /// 获取0时刻: {0, 0}
            static CTimeVal zeroTime(void) { return m_Zero; }

            /// 获取系统当前挂钟时间
            static CTimeVal gettimeofday(void);

            /// 获取系统当前单调时间
            static CTimeVal getMonotonicTime(void);

            /// 获取系统当前的单调时间(毫秒)
            static uint64_t getMonotonicMs(void);

            /// 将本地时间转换成Delphi中TDateTime类型
            static double localTDateTime(void);

            /// 校时
            static int adjustTime(struct tm* calendar_time, int threshold, int rtt, int utc);

        protected:
            void init(long sec, long msec, int tz);

        private:
            /**
             * 在对CTimeVal进行数学运算后，将它规范化。具体地，确保秒和微妙都是非负数，
             * 且微妙换算成秒后要小于1秒(将多余的部分累加到秒数上)
             */
            void normalize(void);

        private:
            /// 时区
            int m_TimeZone;

            /// 0时刻
            static CTimeVal m_Zero;
    };

    inline CTimeVal operator+(const CTimeVal& lhs, const CTimeVal& rhs)
    {
        CTimeVal temp(lhs);
        temp += rhs;
        temp.normalize();
        return (temp);
    }

    inline CTimeVal operator-(const CTimeVal& lhs, const CTimeVal& rhs)
    {
        CTimeVal temp(lhs);
        temp -= rhs;
        temp.normalize();
        return (temp);
    }

    inline bool operator>(const CTimeVal& lhs, const CTimeVal& rhs)
    {
        return (rhs < lhs);
    }

    inline bool operator!=(const CTimeVal& lhs, const CTimeVal& rhs)
    {
        return ( !(lhs == rhs) );
    }

    inline bool operator<=(const CTimeVal& lhs, const CTimeVal& rhs)
    {
        return ( !(rhs < lhs) );
    }

    inline bool operator>=(const CTimeVal& lhs, const CTimeVal& rhs)
    {
        return ( !(lhs < rhs) );
    }
}

#endif //_TIMEVAL_H
