/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: main.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月26日 13时34分46秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include "TcaservApp.h"

using namespace EXTSERVICE;

int main(int argc, char *argv[])
{
    TCASERV_APP->init(&argc, argv, NULL);
    TCASERV_APP->initService();
    TCASERV_APP->processEvents();

    return 0;
}

