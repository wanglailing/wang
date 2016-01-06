/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataRecorder.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月07日 15时21分07秒
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
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "DataRecorder.h"
#include "SharedBuffer.h"
#include "StoreApp.h"

#include "Logger.h"
#include "ZlibStream.h"

using namespace HELIUM;
using namespace STORESERVICE;

static const uint32_t FileFlag = 'Z' + ('L' << 8) + ('I' << 16) + ('B' << 24);
static const uint32_t FileVer = 0x00010000;

HELIUM::Crc<uint16_t> CDataRecorder::Crc16_CCITT(0x8408, 0x1021);

unsigned int CDataRecorder::maxRawLength = 1024 * 10;
unsigned int CDataRecorder::maxCompressedLen = 1024;

CDataRecorder::CDataRecorder() : m_bRunning(true)
{
    m_Worker.start(workerRoutine, this);
}

CDataRecorder::~CDataRecorder()
{
    m_Worker.stop();

    std::map<unsigned char, FileInfo*>::iterator iter;
    for (iter = m_FdBuf.begin(); iter != m_FdBuf.end(); ++iter)
    {
        ::close(iter->second->fd);
        delete iter->second->buf;
        delete iter->second;
    }

    m_FdBuf.clear();
}

int CDataRecorder::createFile(const char* name)
{
    if (name == NULL || STORE_MGR->isCFCardMounted() == false)
        return -1;

    char file_fullname[255];
    ::snprintf(file_fullname, sizeof(file_fullname), "%s/%s", STORE_MGR->DataDir, name);

    int fd = ::open(file_fullname, O_CREAT | O_RDWR);
    if (fd < 0)
    {
        ERR_LOG((DATAWATCHER, "Failed to open the file[%s]!\n", file_fullname));
        return -1;
    }

    bool clear_file = false;
    FileHeader file_hdr;

    struct stat st_buf;
    fstat(fd, &st_buf);
    if (st_buf.st_size >= (int)sizeof(FileHeader))
    {
        ::read(fd, &file_hdr, sizeof(FileHeader));
        if (file_hdr.flag != FileFlag || file_hdr.version != FileVer)
        {
            clear_file = true;
        }
        else
        {
            lseek(fd, 0, SEEK_END);
        }
    }
    else if (st_buf.st_size > 0)
    {
        clear_file = true;
    }

    /// 清空文件
    if (clear_file == true)
    {
        ::close(fd);
        fd = ::open(file_fullname, O_CREAT | O_WRONLY | O_TRUNC);
        if (fd < 0)
        {
            ERR_LOG((DATAWATCHER, "Failed to truncate the file[%s]!\n", file_fullname));
            return -1;
        }
    }

    /// 写入文件头
    if (st_buf.st_size == 0 || clear_file == true)
    {
        ::memset(&file_hdr, 0, sizeof(FileHeader));
        file_hdr.flag = FileFlag;
        file_hdr.version = FileVer;
        if (::write(fd, &file_hdr, sizeof(FileHeader)) < 0)
        {
            ERR_LOG((DATAWATCHER, "Failed to write the file header!\n"));
        }
    }

    ::fchmod(fd, 0644);
    return fd;
}

FileInfo* CDataRecorder::queryFdBuffer(unsigned char type, unsigned int timestamp, char compress)
{
    unsigned int curr_date = timestamp - timestamp % (24 * 60 * 60);

    FileInfo* result = NULL;

    std::map<unsigned char, FileInfo*>::iterator iter = m_FdBuf.find(type);
    if (iter != m_FdBuf.end())
    {
        /// 日期变化时，关闭当前文件，并创建一个新的文件
        if (iter->second->createDate != curr_date)
        {
            writeData(iter->second->fd, iter->second->buf, compress, true);
            ::close(iter->second->fd);

            iter->second->fd = generateFile((DATAREC_TYPE)type, timestamp);
            iter->second->createDate = curr_date;
        }

        result = iter->second;
    }
    else
    {
        FileInfo* file_info = new FileInfo;
        file_info->fd = generateFile((DATAREC_TYPE)type, timestamp);
        file_info->createDate = curr_date;
        file_info->buf = new CBuffer();
        m_FdBuf.insert(std::make_pair(type, file_info));

        result = file_info;
    }

    return result;
}

int CDataRecorder::generateFile(DATAREC_TYPE data_type, unsigned int now)
{
    std::string file_type;
    switch (data_type)
    {
        case DATAREC_CIR:
            file_type = "CIR";
            break;
        case DATAREC_ATP:
        case DATAREC_ATPC2:
            file_type = "ATP";
            break;
        case DATAREC_GNET:
            file_type = "GNC";
            break;
        default:
            return -1;
    }

    std::string extinfo("");
    if (data_type == DATAREC_ATPC2)
    {
        extinfo = "C2";
    }

    /// 格式化日期
    struct tm ct;
    char time_buf[80];
    memset(time_buf, 0 , sizeof(time_buf));
    ct = *(::localtime((const time_t*)&now));
    ::strftime(time_buf, sizeof(time_buf), "%Y_%m_%d", &ct);

    char file_name[255];
    ::snprintf(file_name, sizeof(file_name), "%s%s_%s_%s%s.dat", file_type.c_str(), time_buf,
            APPCONFIG->getTrainIDStr().c_str(), APPCONFIG->getAtpType().c_str(), extinfo.c_str());

    int fd = createFile(file_name);
    return fd;
}

std::string CDataRecorder::timeStamp(time_t now)
{
    static unsigned int record_count;
    char buf[64];

    struct tm* time_now = ::localtime(&now);
    if ((record_count++ % 20) == 0)
    {
        ::strftime(buf, sizeof(buf), "\n%Y_%m_%d %H:%M:%S ", time_now);
    }
    else
    {
        ::strftime(buf, sizeof(buf), "\n%H:%M:%S ", time_now);
    }

    return (std::string(buf));
}

void CDataRecorder::workerRoutine(void* args)
{
    CDataRecorder* self = (CDataRecorder*)args;
    while (self->m_bRunning)
    {
        char compress_flag = 0;
        FileInfo* pinfo = SHARED_BUFFER->getData(self, &compress_flag);
        if (pinfo != NULL)
        {
            self->writeData(pinfo->fd, pinfo->buf, compress_flag);
        }
    }

    return;
}

int CDataRecorder::writeData(int fd, HELIUM::CBuffer* buf, char compress, bool bflush)
{
    if (fd < 0 || buf == NULL || buf->readableBytes() <= 0)
        return -1;

    if (compress == 1)
    {
        BulkHeader bulk_hdr;
        bulk_hdr.rawlen = buf->readableBytes();

        CBuffer compressed;
        CZlibCompress zlib_compress(&compressed);
        zlib_compress.write(buf);
        zlib_compress.finish();

        bulk_hdr.complen = compressed.readableBytes();

        if (bulk_hdr.rawlen >= maxRawLength || bulk_hdr.complen >= maxCompressedLen || bflush == true)
        {
            bulk_hdr.crc16 = Crc16_CCITT.calcHOPP(&bulk_hdr.complen, sizeof(bulk_hdr.complen), 0xFFFF);
            bulk_hdr.crc16 = Crc16_CCITT.calcHOPP(&bulk_hdr.rawlen, sizeof(bulk_hdr.rawlen), bulk_hdr.crc16);
            bulk_hdr.crc16 = Crc16_CCITT.calcHOPP(compressed.peek(), compressed.readableBytes(), bulk_hdr.crc16);

            compressed.prepend(&bulk_hdr, sizeof(BulkHeader));
            if (::write(fd, compressed.peek(), compressed.readableBytes()) < 0)
            {
                ERR_LOG((DATAWATCHER, "Failed to write the compressed data!\n"));
            }

            buf->retrieveAll();
        }
    }
    else
    {
        if (::write(fd, buf->peek(), buf->readableBytes()) < 0)
        {
            ERR_LOG((DATAWATCHER, "Failed to write the original data!\n"));
        }

        buf->retrieveAll();
    }

    return 0;
}

