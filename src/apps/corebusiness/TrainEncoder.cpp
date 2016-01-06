/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: TrainEncoder.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月27日 11时39分15秒
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
#include <string.h>

#include "TrainEncoder.h"

using namespace COREBUSINESS;

static const char* code_table[CODE_COUNT]  = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
    "U", "V", "W", "X", "Y", "Z", "", "", "", "", "FA", "FB", "FC", "FD", "FE", "FF", "FG", "FH", "FI",
    "FJ", "FK", "FL", "FM", "FN", "FO", "FP", "FQ", "FR", "FS", "FT", "FU", "FV", "FW", "FX", "FY", "FZ",
    "DJ", "FDJ", "0A", "0B", "0C", "0D", "0E", "0F", "0G", "0H", "0I", "0J", "0K", "0L", "0M", "0N", "0O",
    "0P", "0Q", "0R", "0S", "0T", "0U", "0V", "0W", "0X", "0Y", "0Z", "0FA", "0FB", "0FC", "0FD", "0FE",
    "0FF", "0FG", "0FH", "0FI", "0FJ", "0FK", "0FL", "0FM", "0FN", "0FO", "0FP", "0FQ", "0FR", "0FS", "0FT",
    "0FU", "0FV", "0FW", "0FX", "0FY", "0FZ", "0DJ", "0FDJ"
};

CTrainEncoder* CTrainEncoder::m_Instance = NULL;

CTrainEncoder* CTrainEncoder::getInstance()
{
    if (m_Instance == NULL)
    {
        m_Instance = new CTrainEncoder();
    }

    return m_Instance;
}

CTrainEncoder::CTrainEncoder()
{
    for (int i = 0; i < CODE_COUNT; ++i)
    {
        m_CodeMap.insert(std::make_pair(std::string(code_table[i]), i));
    }
}

/**
 * @brief 将车次转换为数字
 *
 * @param train 车次号
 *
 * @return 车次编码
 */
uint32_t CTrainEncoder::strToNumber(std::string& train)
{
    uint32_t result = 0;
    std::string total("FFF");
    std::string letter_part;
    std::string num_part;

    letter_part.clear();
    num_part.clear();

    if (train.empty())
    {
        return 0;
    }

    /// 获取字母部分
    std::string::iterator iter;
    int seq;
    for (seq = 0, iter = train.begin(); iter != train.end(); ++seq, ++iter)
    {
        if (seq > 0 && (*iter >= '0' && *iter <= '9'))
            break;

        letter_part.append(1, *iter);
    }

    /// 获取数字部分
    for (; iter != train.end(); ++iter)
    {
        num_part.append(1, *iter);
    }

    /// 将数字部分补齐为5位
    int left = 5 - num_part.length();
    for (int i = 0; i < left; ++i)
    {
        num_part.append(1, 'F');
    }

    /// 将字母部分和数字部分组合为字符串
    if (letter_part.length() == 1 && (letter_part[0] < 'A' || letter_part[0] > 'Z'))   // 没有字母部分
    {
        letter_part.append(num_part);
        num_part = letter_part;
    }
    else
    {
        std::multimap<std::string, int>::iterator iter;
        iter = m_CodeMap.find(letter_part);
        if (iter != m_CodeMap.end())
        {
            char temp[4];
            snprintf(temp, sizeof(temp), "%03d", iter->second);
            total = std::string(temp);
        }
    }

    total.append(num_part);

    /// 转换为BCD码
    for (unsigned int i = 0; i < 8 && i < total.length(); ++i)
    {
        if (total[i] == 'F')
        {
            result |= 0xf;
        }
        else
        {
            result |= (total[i] - '0');
        }

        if (i < 7)
        {
            result <<= 4;
        }
    }

    return result;
}

/**
 * @brief 将数字转换为车次
 *
 * @param
 *
 * @return
 */
std::string CTrainEncoder::numberToStr(uint32_t train_num)
{
    std::string result("");
    int letter_index;

    letter_index = train_num >> 20;
    if (letter_index != 0xfff)  /// 字母区非空
    {
        letter_index = (letter_index >> 8) * 100 + ((letter_index >> 4) & 0x0f) * 10 + (letter_index & 0x0f);

        if (letter_index < CODE_COUNT)
        {
            result.append(code_table[letter_index]);
        }
    }

    train_num &= 0x000fffff;
    for (int i = 4; i >= 0; --i)
    {
        char numpart = (char)((train_num >> (i << 2)) & 0x0f);

        if (numpart != 0xf)
        {
            result.append(1, numpart + 0x30);
        }
    }

    return result;
}

void CTrainEncoder::decodeTrainNum(TrainNumber& train_struct, uint32_t train_num)
{
    if (train_num == 0xffffffff)
        return;

    memset(&train_struct, 0x0, sizeof(TrainNumber));
    int letter_index;

    letter_index = train_num >> 20;
    if (letter_index != 0xfff)  /// 字母区非空
    {
        letter_index = (letter_index >> 8) * 100 + ((letter_index >> 4) & 0x0f) * 10 + (letter_index & 0x0f);

        if (letter_index < CODE_COUNT)
        {
            int copy_len = strlen(code_table[letter_index]);
            if (copy_len > (int)sizeof(train_struct.flag))
                copy_len = sizeof(train_struct.flag);

            memcpy(train_struct.flag, code_table[letter_index], copy_len);
        }
    }

    train_num &= 0x000fffff;
    for (int i = 4; i >= 0; --i)
    {
        uint8_t numpart = (uint8_t)((train_num >> (i << 2)) & 0x0f);

        if (numpart != 0xf)
        {
            train_struct.num = train_struct.num * 10 + numpart;
        }
    }
}

uint32_t CTrainEncoder::encodeTrainNum(const TrainNumber& train_struct)
{
    std::string trainstr("");

    for (unsigned int i = 0; i < sizeof(train_struct.flag); ++i)
    {
        if (train_struct.flag[i] == '\0')
            break;

        trainstr.push_back(train_struct.flag[i]);
    }

    char numpart[16] = {'\0'};
    snprintf(numpart, sizeof(numpart), "%d", train_struct.num);
    trainstr += numpart;

    return strToNumber(trainstr);
}

