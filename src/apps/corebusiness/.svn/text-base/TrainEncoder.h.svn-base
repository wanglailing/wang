/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TrainEncoder.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月27日 11时34分55秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef TRAINENCODER_H
#define TRAINENCODER_H

#include <stdint.h>
#include <string>
#include <map>

#include "../DmsBusinessDat.h"

#define CODE_COUNT 112

namespace COREBUSINESS
{
    class CTrainEncoder
    {
        public:
            static CTrainEncoder* getInstance();

            ~CTrainEncoder() {}

            /// 将车次字符串编码为数字
            uint32_t strToNumber(std::string&);

            /// 将编码车次转换为字符串
            std::string numberToStr(uint32_t);

            /// 将编码车次转换为字符和数字两部分
            void decodeTrainNum(TrainNumber& train_struct, uint32_t train_num);

            /// 将车次结构体编码为数字
            uint32_t encodeTrainNum(const TrainNumber& train_struct);

        private:
            CTrainEncoder(); 

        private:
            static CTrainEncoder* m_Instance;

            /// 车次编码表
            std::multimap<std::string, int> m_CodeMap;
    };

    #define TRAIN_ENCODER CTrainEncoder::getInstance()
}

#endif // TRAINENCODER_H
