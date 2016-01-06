/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: main.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月31日 16时46分52秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include "TransportApp.h"

using namespace TRANSPORTCTRL;

int main(int argc, char *argv[])
{
    TRANS_APP->init(&argc, argv, NULL);
    TRANS_APP->initService();
    TRANS_APP->processEvents();

    return 0;
}

