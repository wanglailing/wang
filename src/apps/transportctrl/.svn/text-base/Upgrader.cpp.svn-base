/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Upgrader.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年12月18日 17时42分26秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Upgrader.h"
#include "Logger.h"

using namespace HELIUM;
using namespace TRANSPORTCTRL;

HELIUM_DECL_SINGLETON(CUpgrader);

const char *CUpgrader::UnpackScript = "\
#!/bin/sh\n\
echo Start unpacking $1 ...\n\
cd ./tmp/unzips\n\
rm -rf *\n\
unzip ../$1\n\
if [ $? -eq 0 ]; then\n\
    echo Unpack file ok.^_^\n\
    if [ -f setup ]; then\n\
        chmod +x setup\n\
        echo Begin to execute setup script...\n\
        ./setup\n\
        exit $?\n\
    else\n\
        echo Can not find setup script!\n\
        exit 0\n\
    fi\n\
else\n\
    echo Unpack error!\n\
    exit 1\n\
fi\n\
exit 0\n";

CUpgrader::CUpgrader() : m_Fp(NULL), m_FileSize(0), m_RecvSize(0)
{
    memset(m_FileName, 0, sizeof(m_FileName));
    initFileStructs();
}

CUpgrader::~CUpgrader()
{
    if (m_Fp)
    {
        fclose(m_Fp);
        m_Fp = NULL;
    }
}

void CUpgrader::initFileStructs(void)
{
    struct stat stat_buf;
    if (stat("scripts", &stat_buf) < 0 || !S_ISDIR(stat_buf.st_mode))
    {
        mkdir("scripts", 0755);
    }

    if (stat("scripts/unpack", &stat_buf) < 0 || !S_ISREG(stat_buf.st_mode))
    {
        FILE *fp = fopen("scripts/unpack", "wt");
        fwrite(UnpackScript, 1, strlen(UnpackScript), fp);
        fclose(fp);
        chmod("scripts/unpack", 0755);
    }

    if (stat("tmp", &stat_buf) < 0 || !S_ISDIR(stat_buf.st_mode))
    {
        mkdir("tmp", 0755);
    }

    if (stat("tmp/unzips", &stat_buf) < 0 || !S_ISDIR(stat_buf.st_mode))
    {
        mkdir("tmp/unzips", 0755);
    }
}

void CUpgrader::workerRoutine(void* args)
{
    CUpgrader* self = (CUpgrader*)args;

    char cmd_buf[128];
    snprintf(cmd_buf, sizeof(cmd_buf), "./scripts/unpack %s", self->m_FileName);
    FILE* fp = popen(cmd_buf, "r");
    if (fp == NULL)
        return;

    char msg_buf[256];
    while (fgets(msg_buf, sizeof(msg_buf) - 1, fp) != NULL)
    {
        printf("====>%s", msg_buf);
    }

    pclose(fp);
    return;
}

int CUpgrader::createUpgradeFile(const FILE_DESCRIPTION* file_desc)
{
    char file_name[64];

    if (m_Fp)
    {
        fclose(m_Fp);
    }

    snprintf(m_FileName, sizeof(m_FileName), "%s", file_desc->name);
    snprintf(file_name, sizeof(file_name), "tmp/%s", file_desc->name);
    if ((m_Fp = fopen(file_name, "wb")) == NULL)
    {
        ERR_LOG((TRANSCTRL, "Failed to create the upgrading package<%s>!\n", file_desc->name));
        return -1;
    }

    DBG_LOG((TRANSCTRL, "Create the upgrading package<%s: %d>...\n", file_desc->name, file_desc->size));
    m_FileSize = file_desc->size;
    m_RecvSize = 0;

    return 0;
}

int CUpgrader::writeToFile(const char* data, int length)
{
    if (m_Fp == NULL)
        return -1;

    fwrite(data, 1, length, m_Fp);
    m_RecvSize += length;
    if (m_RecvSize >= m_FileSize)
    {
        fclose(m_Fp);
        m_Fp = NULL;

        /// 启动脚本执行线程
        DBG_LOG((TRANSCTRL, "Begin to execute the DMS upgrading thread ...\n"));
        m_Worker.start(workerRoutine, this);
    }

    return 0;
}

