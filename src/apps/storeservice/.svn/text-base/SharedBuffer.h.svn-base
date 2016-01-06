/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: SharedBuffer.h
 *
 *       版本号: 1.0
 *     创建日期: 2015年03月25日 09时04分46秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef __SHARED_BUFFER_H
#define __SHARED_BUFFER_H

#include <pthread.h>
#include <semaphore.h>

#include "Buffer.h"
#include "Singleton.h"

#include "DataRecorder.h"

#define BUFFER_COUNT 32
#define ITEM_CAPTION 1024

namespace STORESERVICE
{
    /**
     * 存放业务数据的共享缓冲区
     */
    class CSharedBuffer : public HELIUM::CSingleton<CSharedBuffer>
    {
        public:
            typedef struct stDataItem
            {
                unsigned char type;           /// 数据类型
                short len;                    /// 数据长度
                unsigned int timestamp;       /// 时间戳
                char  compress;               /// 数据压缩标志(1 -- 压缩；0 -- 不压缩)
                char  data[ITEM_CAPTION];     /// 数据缓冲区
            }DataItem;

        public:
            CSharedBuffer();
            ~CSharedBuffer();

            /**
             * 将数据放入缓冲区
             *
             * @param type     数据类型  
             * @param buf      数据地址
             * @param len      数据长度
             * @param now      当前时刻
             * @param compress 数据压缩标志
             *
             */
            void pushData(unsigned char type, const char* buf, int len, unsigned int now, char compress);

            /**
             * 从缓冲区获取数据
             *
             * @param [in]  data_rec      数据记录器
             * @param [out] compress_flag 存放数据压缩标志
             *
             * @return 数据地址
             *
             */
            FileInfo* getData(CDataRecorder* data_rec, char* compress_flag);

        private:
            CSharedBuffer(const CSharedBuffer&);
            CSharedBuffer& operator=(const CSharedBuffer&);

        private:
            DataItem* m_DataSlots[BUFFER_COUNT];

            unsigned char m_ReadPos;
            unsigned char m_WritePos;

            /// 标示空闲的数据槽
            sem_t m_IdleSlot;

            /// 标示被占用的数据槽
            sem_t m_StoredSlot;
    };

    #define SHARED_BUFFER CSharedBuffer::getInstance()
}

#endif //__SHARED_BUFFER_H
