/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataCoder.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年01月04日 09时07分09秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2015, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "DataCoder.h"

using namespace HELIUM;

const char* CDataCoder::DmsTagHead = "$1$";
const char* CDataCoder::DmsTagTail = "$2$";

const char* CDataCoder::TcasTagHead = "@1@";
const char* CDataCoder::TcasTagCrc  = "@2@";
const char* CDataCoder::TcasTagTail = "@3@";

Crc<uint32_t> CDataCoder::Crc32(0xEDB88320, 0x04C11DB7);

std::string* CDataCoder::encodeData(CBuffer* data)
{
    if (data == NULL)
        return NULL;

    unsigned short raw_len = data->readableBytes();

    /// ZLIB压缩
    CBuffer compressed;
    CZlibCompress zlib_compress(&compressed);
    zlib_compress.write(data);
    zlib_compress.finish();

    /// 在压缩数据前面附上原始数据的长度
    compressed.prepend(&raw_len, sizeof(unsigned short));

    /// BASE64编码
    std::string* result = CBase64Coder::encode((const unsigned char*)compressed.peek(), 
            compressed.readableBytes());

    return result;
}

void CDataCoder::decodeData(CBuffer* output, std::string* enc_str)
{
    if (output == NULL || enc_str == NULL)
        return;

    /// BASE64解码
    CBuffer decode_buf;
    CBase64Coder::decode(enc_str, &decode_buf);

    /// 起始两个字节为原始数据的长度，将其删除
    decode_buf.retrieve(2);

    /// ZLIB解压
    output->retrieveAll();
    CZlibDecompress zlib_decompress(output);
    zlib_decompress.write(&decode_buf);
    zlib_decompress.finish();
}

void CDataCoder::wrapData(char* dst, int buf_len, int head_len, const char* tag_tail, const char* tag_crc)
{
    int payload_len = ::strlen(dst);
    if (dst == NULL || payload_len <= head_len)
        return;

    std::string crc32 = calcCRC32(dst + head_len, payload_len - head_len);

    int left_space = buf_len - payload_len - 1;
    ::strncat(dst, tag_crc, left_space);
    left_space -= ::strlen(tag_crc);

    ::strncat(dst, crc32.c_str(), left_space);
    left_space -= 8;

    ::strncat(dst, tag_tail, left_space);
}

void CDataCoder::wrapData(std::string& packet, int head_len, const char* tag_tail, const char* tag_crc)
{
    std::string crc32 = calcCRC32(packet.c_str() + head_len, 
            packet.length() - head_len);

    packet += std::string(tag_crc);
    packet += crc32;
    packet += std::string(tag_tail);
}

std::string CDataCoder::calcCRC32(const char* data, int length)
{
    char crc_str[9] = {0};

    ::snprintf(crc_str, sizeof(crc_str), "%08X", Crc32.calcLOPP(data, length, 0xFFFFFFFF));

    return std::string(crc_str);
}


