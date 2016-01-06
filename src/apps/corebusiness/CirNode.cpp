/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CirNode.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月26日 16时15分32秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <string.h>

#include "CirNode.h"

using namespace HELIUM;
using namespace COREBUSINESS;

CCirNode::CCirNode(int interval) : CDataNode(interval), m_RunSpeed(0)
{
    resetTrainNum(&m_TrainNum);
}

CCirNode::~CCirNode()
{
}

void CCirNode::resetTrainNum(CirTrainum *trainum)
{
    memset(trainum->parsed.flag, 0x20, sizeof(trainum->parsed.flag));
    trainum->parsed.num = 0;
    trainum->stamp = 0;
}

int CCirNode::parseData(const unsigned char *buf, int len, HELIUM::CBuffer* nodes_buf, uint64_t now)
{
    (void)nodes_buf;
    (void)checkTimeliness(now);

    if (len == sizeof(CIRTRANS_TAX))
    {
        CIRTRANS_TAX* tax_data = (CIRTRANS_TAX*)buf;
        memcpy(&m_RunSpeed, tax_data->runspeed, sizeof(uint16_t));
        m_RunSpeed &= 0x3ff;

        memcpy(&m_TrainNum.parsed.num, tax_data->trainumpart, sizeof(tax_data->trainumpart));
        memcpy(m_TrainNum.parsed.flag, tax_data->trainumflag, sizeof(tax_data->trainumflag));
    }
    else if (len == sizeof(CIR_TO_DMS))
    {
        CIR_TO_DMS* cir_data = (CIR_TO_DMS*)buf;
        memcpy(&m_TrainNum.parsed.num, cir_data->trainumpart, sizeof(cir_data->trainumpart));
        memcpy(m_TrainNum.parsed.flag, cir_data->trainumflag, sizeof(cir_data->trainumflag));
    }
    else
    {
        m_Status = (uint8_t)STATUS_ERROR;
        return -1;
    }

    m_TrainNum.stamp = now;
    m_Status = (uint8_t)STATUS_OK;

    return 0;
}

void CCirNode::dump(void)
{
    static int seq;
    DBG_LOG((DATAWATCHER, "-------------------- CIR:%d --------------------\n", seq++));
    char train_letters[16] = {'\0'};
    memcpy(train_letters, m_TrainNum.parsed.flag, sizeof(m_TrainNum.parsed.flag));
    DBG_LOG((DATAWATCHER, "Train Number: %s%d\n", train_letters, m_TrainNum.parsed.num));
    DBG_LOG((DATAWATCHER, "Run Speed   : %3u km/h\n", m_RunSpeed));
}

unsigned char* CCirNode::getParsedInfo(int* length)
{
    (void)length;
    return (unsigned char*)&m_TrainNum;
}

