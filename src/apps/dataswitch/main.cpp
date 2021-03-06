/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: main.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月12日 13时19分09秒
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

#include "DataSwitchApp.h"

using namespace DMS;

int main(int argc, char *argv[])
{
    DATASWITCH_APP->init(&argc, argv, NULL);
    DATASWITCH_APP->initService();
    DATASWITCH_APP->processEvents();

    return 0;
}
