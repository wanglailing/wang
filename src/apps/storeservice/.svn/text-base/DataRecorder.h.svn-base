/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataRecorder.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月07日 14时51分36秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DATA_RECORDER_H
#define _DATA_RECORDER_H

#include <stdint.h>
#include <string>
#include <map>

#include "Buffer.h"
#include "CRC.h"
#include "Thread.h"

#include "../DmsDatatype.h"

namespace STORESERVICE
{
    /**
     * @def
     * 文件相关信息
     *
     */
    typedef struct stFileInfo
    {
        int      fd;              /// 文件描述符
        uint32_t createDate;      /// 文件创建日期
        HELIUM::CBuffer* buf;     /// 文件数据缓冲区
    }FileInfo;

    /**
     *  将数据记录到文件中
     *
     */
    class CDataRecorder
    {
        public:
#pragma pack(push, 1)
            /**
             * @def
             * 文件头部
             *
             */
            typedef struct stFileHeader
            {
                uint32_t flag;
                uint32_t version;
                uint8_t  reserved[6];
                uint8_t  atptype;
                uint8_t  atpver;
            }FileHeader;

            /**
             * @def
             * 数据块头部
             *
             */
            typedef struct stBulkHeader
            {
                uint16_t crc16;
                uint16_t complen;
                uint32_t rawlen;
            }BulkHeader;
#pragma pack(pop)

        public:
            CDataRecorder();
            ~CDataRecorder();

            /// 查询文件描述符所对应的缓冲区
            FileInfo* queryFdBuffer(unsigned char type, unsigned int timestamp, char compress);

            void stop(void) { m_bRunning = false; }

            /// 将时间戳转换成字符串
            static std::string timeStamp(time_t now);

            /// 16位校验和
            static HELIUM::Crc<uint16_t> Crc16_CCITT;

        private:
            CDataRecorder(const CDataRecorder&);
            const CDataRecorder& operator=(const CDataRecorder&);

        private:
            /**
             * 打开或创建文件
             *
             * @param name 文件名
             *
             * @return 文件描述符
             */
            int createFile(const char* name);

            /**
             * 根据数据类型创建文件
             *
             * @param data_type 数据类型
             * @param now       当前时刻
             *
             * @return 文件描述符
             *
             */
            int generateFile(DATAREC_TYPE data_type, unsigned int now);

            static void workerRoutine(void* args);

            /**
             * 将数据写入文件
             *
             * @param fd        文件描述符
             * @param buf       数据
             * @param compress  压缩标志
             * @param bflush    强制写入标志
             *
             * @return 
             */
            int writeData(int fd, HELIUM::CBuffer* buf, char compress, bool bflush = false);

        private:
            /// 文件描述符与缓冲区的映射
            std::map<unsigned char, FileInfo*> m_FdBuf;

            /// 设备管理线程
            HELIUM::CThread m_Worker;

            /// 线程运行标志
            volatile bool m_bRunning;

            /// 原始数据的最大长度
            static unsigned int maxRawLength;

            /// 压缩数据的最大长度
            static unsigned int maxCompressedLen;
    };
}

#endif //_DATA_RECORDER_H
