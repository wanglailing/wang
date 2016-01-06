/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: StepRunner.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月15日 09时06分41秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _STEP_RUNNER_H
#define _STEP_RUNNER_H

#include <stdint.h>

#include "Thread.h"
#include "Mutex.h"
#include "Packet.h"
#include "TcrNode.h"

#define MAX_VOLTAGE_COUNT (800)
#define LOW_PRECISION  0
#define HIGH_PRECISION 1

namespace COREBUSINESS
{
#pragma pack(push, 1)
    typedef struct stTcrSharedHeader
    {
        HELIUM::SUBPACK_HEADER subhdr;    /// 子包头部
        uint8_t                id;        /// 报文标识
        uint16_t               count;     /// 电压幅值计数
    }TCR_HEADER;
#pragma pack(pop)

    /**
     * 实时计算走行距离
     *
     */
    class CStepRunner
    {
        public:
            explicit CStepRunner(int run_scale);
            ~CStepRunner();

            void start();
            void stop();

            int parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
            {
                return m_TcrNode->parseData(buf, len, nodes_buf, now);
            }

            unsigned int getTcrState(uint64_t now)
            {
                return m_TcrNode->getState(now);
            }

            int pushVoltages(unsigned char* buf, int length);

            /// 复位过应答器距离
            void resetPassBaliseDist() { m_PassBaliseDist = 0; }

            /// 获取过应答器距离
            uint16_t getPassBaliseDist() const { return m_PassBaliseDist; }

            /// 获取高精度走行距离
            unsigned int getPreciseRunDist() const { return m_RunDistance[HIGH_PRECISION]; }

        private:
            CStepRunner(const CStepRunner&);
            CStepRunner& operator=(const CStepRunner&);

            /// 线程执行体
            static void workerRoutine(void* args);

            /// 复位TCR数据包头部
            void resetTcrHeader(void);

        private:
            CTcrNode* m_TcrNode;

            bool m_bStopping;

            /// 走行距离计算线程
            HELIUM::CThread m_Worker;

            /// 保护TCR电压幅值数据
            HELIUM::CMutex  m_TcrMutex;

            /// TCR报文头部
            TCR_HEADER m_TcrHeader;

            /// TCR电压幅值
            unsigned char m_TcrVoltages[MAX_VOLTAGE_COUNT];

            /// 累积走行距离
            double m_CumulativeDist[2];

            /// 标准走行距离
            volatile unsigned int m_RunDistance[2];

            /// 走行刻度(当距离增量不小于此值时，才会更新走行距离)
            int m_StepRunScale[2];

            /// 过应答器距离
            volatile uint16_t m_PassBaliseDist;

            /// 上一次走行的时间戳
            uint64_t m_LastStepStamp;
    };
}

#endif //_STEP_RUNNER_H
