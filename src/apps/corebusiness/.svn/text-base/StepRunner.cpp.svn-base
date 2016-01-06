/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: StepRunner.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月15日 09时23分12秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <string.h>

#include "StepRunner.h"
#include "AtpPlugin.h"
#include "TimeVal.h"
#include "Logger.h"

#include "../DmsDatatype.h"

using namespace HELIUM;
using namespace COREBUSINESS;

CStepRunner::CStepRunner(int run_scale) : m_bStopping(false)
{
    m_TcrNode = new CTcrNode(1000);

    memset(&m_TcrHeader, 0, sizeof(m_TcrHeader));
    m_TcrHeader.id = (uint8_t)DMSMSG_TRACKVOLTAGE;
    m_TcrHeader.subhdr.len = sizeof(TCR_HEADER);

    memset(m_TcrVoltages, 0, sizeof(m_TcrVoltages));
    memset(m_CumulativeDist, 0, sizeof(m_CumulativeDist));

    m_RunDistance[LOW_PRECISION]   = 0;
    m_RunDistance[HIGH_PRECISION]  = 0;

    m_StepRunScale[LOW_PRECISION]  = run_scale;
    m_StepRunScale[HIGH_PRECISION] = 2;

    m_PassBaliseDist = 0;

    m_LastStepStamp = CTimeVal::getMonotonicMs();
}

CStepRunner::~CStepRunner()
{
    m_Worker.stop();

    delete m_TcrNode; m_TcrNode = NULL;
}

void CStepRunner::start()
{
    m_Worker.start(workerRoutine, this);
}

void CStepRunner::stop()
{
    m_bStopping = true;
}

int CStepRunner::pushVoltages(unsigned char* buf, int length)
{
    int result = 0;

    m_TcrMutex.lock();
    if (m_TcrHeader.count > 0 && m_TcrHeader.subhdr.len < length)
    {
        ::memcpy(buf, &m_TcrHeader, sizeof(m_TcrHeader));
        ::memcpy(buf + sizeof(m_TcrHeader), m_TcrVoltages, m_TcrHeader.count);

        result = m_TcrHeader.subhdr.len;

        resetTcrHeader();
    }
    m_TcrMutex.unlock();

    return result;
}

void CStepRunner::workerRoutine(void* args)
{
    CStepRunner* self = (CStepRunner*)args;

    while (!self->m_bStopping)
    {
        uint64_t timenow = CTimeVal::getMonotonicMs();
        int elapsed_ms = timenow - self->m_LastStepStamp;
        self->m_LastStepStamp = timenow;

        unsigned short atp_speed = gAtpHandlers.getRunSpeed(timenow);
        if (atp_speed >= 400)
        {
            DBG_LOG((DATAWATCHER, "The ATP speed is too large:%u\n", atp_speed));
        }

        double ds = (double)atp_speed * elapsed_ms / 3600.0f;
        self->m_CumulativeDist[LOW_PRECISION]  += ds;
        self->m_CumulativeDist[HIGH_PRECISION] += ds;

        /**
         * 在将浮点型走行距离转换为整型走行距离时，需要消除累积误差
         */
        if (self->m_CumulativeDist[HIGH_PRECISION] >= self->m_StepRunScale[HIGH_PRECISION])
        {
            int passed_mile = self->m_StepRunScale[HIGH_PRECISION] * \
                ((int)self->m_CumulativeDist[HIGH_PRECISION] / self->m_StepRunScale[HIGH_PRECISION]);
            self->m_RunDistance[HIGH_PRECISION] += passed_mile;
            self->m_CumulativeDist[HIGH_PRECISION] -= passed_mile;

            /// 更新过应答器距离时需防止溢出
            unsigned int passbalise_dist = self->m_PassBaliseDist + passed_mile;
            if (passbalise_dist > 60000)
                self->m_PassBaliseDist = 65535;
            else
                self->m_PassBaliseDist = (uint16_t)passbalise_dist;
        }

        if (self->m_CumulativeDist[LOW_PRECISION] >= self->m_StepRunScale[LOW_PRECISION])
        {
            int passed_mile = self->m_StepRunScale[LOW_PRECISION] * \
                ((int)self->m_CumulativeDist[LOW_PRECISION] / self->m_StepRunScale[LOW_PRECISION]);
            self->m_RunDistance[LOW_PRECISION] += passed_mile;

            /**
             * 每隔5米需要放置一个轨道电压, 所以轨道电压的封包逻辑就与走行距离的更新
             * 耦合在一起了
             */
            self->m_TcrMutex.lock();
            while (self->m_CumulativeDist[LOW_PRECISION] >= self->m_StepRunScale[LOW_PRECISION])
            {
                if ((self->m_TcrHeader.count + 1) >= MAX_VOLTAGE_COUNT)
                {
                    self->resetTcrHeader();
                }
                self->m_TcrHeader.subhdr.len++;
                self->m_TcrVoltages[self->m_TcrHeader.count++] = self->m_TcrNode->getVoltage(timenow);

                self->m_CumulativeDist[LOW_PRECISION] -= self->m_StepRunScale[LOW_PRECISION];
            }
            self->m_TcrMutex.unlock();
        }

        if (elapsed_ms > 200)
        {
            DBG_LOG((DATAWATCHER, "Step interval out of range:%d\n", elapsed_ms));
        }

        ::usleep(20 * 1000);
    }
}

void CStepRunner::resetTcrHeader(void)
{
    m_TcrHeader.count = 0;
    m_TcrHeader.subhdr.len = sizeof(TCR_HEADER);
}

