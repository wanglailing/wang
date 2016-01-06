/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: getArfcn.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2015年02月10日 08时58分29秒
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
#include <string.h>
#include <arpa/inet.h>

#include "../GNetFrame.h"

static char act_data[] = {
    0x02, 0x02, 0x00, 0x42, 0x73, 0x00, 0x00, 0x00, 0x51, 0x68, 0x03, 0x09, 0xF6, 0x02, 0x8A, 0x21, 
    0xFB, 0x2C, 0x40, 0x37, 0x99, 0x30, 0xC3, 0x00, 0x6E, 0x1F, 0x2A, 0x61, 0x37, 0x99, 0xCA, 0x02, 
    0x88, 0x19, 0x2C, 0x3F, 0x37, 0x99, 0xFB, 0x02, 0x92, 0x0F, 0x29, 0x4A, 0x37, 0x99, 0x80, 0x00, 
    0x73, 0x0A, 0xFF, 0xFF, 0xFF, 0xFF, 0xEC, 0x00, 0x72, 0x0A, 0x02, 0x94, 0x37, 0x99, 0xD3, 0x02, 
    0x90, 0x09, 0x29, 0xA9, 0x37, 0x99, 0x7F, 0x03
};

static char bcch2[] = {
    0x02, 0x02, 0x00, 0x42, 0x73, 0x00, 0x00, 0x00, 0x55, 0xC8, 0x03, 0x09, 0xFF, 0x03, 0xE8, 0x2E, 
    0xFB, 0x26, 0x64, 0x53, 0x04, 0x30, 0xFE, 0x03, 0xED, 0x0A, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
    0x7D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x7D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 
    0x7D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x7D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 
    0x7D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x27, 0x03
};

static char rf_trace[] = {
    0x02, 0x46, 0x00, 0x48, 0x53, 0x01, 0x00, 0x00, 0x55, 0xB1, 0x03, 0x00, 0x15, 0x03, 0xE7, 0x10, 
    0x03, 0xE8, 0x22, 0x03, 0xE9, 0x11, 0x03, 0xEA, 0x05, 0x03, 0xEB, 0x04, 0x03, 0xEC, 0x07, 0x03, 
    0xED, 0x0B, 0x03, 0xEE, 0x07, 0x03, 0xEF, 0x03, 0x03, 0xF0, 0x02, 0x03, 0xF1, 0x07, 0x03, 0xF2, 
    0x06, 0x03, 0xF3, 0x0A, 0x03, 0xF4, 0x09, 0x03, 0xF5, 0x08, 0x03, 0xF6, 0x03, 0x03, 0xF7, 0x0A, 
    0x03, 0xF8, 0x0D, 0x03, 0xF9, 0x09, 0x03, 0xFA, 0x09, 0x03, 0xFB, 0x08, 0x64, 0x03
};

void dumpBcch(const uint16_t* bcchs)
{
    for (int i = 0; i < 7; ++i)
    {
        printf("%d:%d\n", bcchs[i] & 0x7fff, bcchs[i] >> 15);
    }
}

template<class T>
void temptest(T pRealtimeDat)
{
    uint16_t bcch_arfcns[7];

    /// 收集频点号
    if (pRealtimeDat)
    {
        bcch_arfcns[0] = ::ntohs(pRealtimeDat->serving.bcch);
        if (pRealtimeDat->serving.bsic >> 7 == 1)
            bcch_arfcns[0] |= 0x8000;

        printf("cellid=%d, lac=%d, C/I=%d\n", 
                ::ntohs(pRealtimeDat->serving.cellid), ::ntohs(pRealtimeDat->serving.lac), pRealtimeDat->serving.carrtointer);

        for (int i = 0; i < 6; ++i)
        {
            bcch_arfcns[i + 1] = ::ntohs(pRealtimeDat->neighbours[i].bcch);
            if (pRealtimeDat->neighbours[i].bsic >> 7 == 1)
                bcch_arfcns[i + 1] |= 0x8000;
        }

        dumpBcch(bcch_arfcns);
    }
}

void processData(const char* data, int length)
{
    /**
     * 1. 判断数据类型:
     * 1.1 通话数据
     * 1.2 待机数据
     *         收集BCCH频点号及其有效性
     * 1.3 RF扫描结果
     *         收集BCCH频点号及其信号强度
     *
     */
    if (data == NULL)
        return;

    /**
     * 验证数据帧的格式
     */
    const FrameHeader* pFrmHdr = reinterpret_cast<const FrameHeader*>(data);
    unsigned char appid = pFrmHdr->modeid & 0x03;
    if (pFrmHdr->stx != 0x02 || appid != 0x02)
    {
        printf("header error: stx=%02X, appid=%02X\n", pFrmHdr->stx, appid);
        return;
    }

    int msglen = ::ntohs(pFrmHdr->msglen);
    if ((msglen + 6) > length)
    {
        printf("length error: msglen=%d\n", msglen);
        return;
    }

    const FrameTail* pFrmTail = reinterpret_cast<const FrameTail*>(data + msglen + 4);
    if (pFrmTail->etx != 0x03)
    {
        printf("etx error: etx=%02X, msglen=%d\n", pFrmTail->etx, msglen);
        return;
    }


    /**
     * 解析数据帧的内容
     */
    if (pFrmHdr->msgid == 0x0073) /// 实时数据
    {
        /// 待机数据
        if (pFrmHdr->resvmode == 0x03)
        {
            const IdleData* pRealtimeDat = reinterpret_cast<const IdleData*>(data + sizeof(FrameHeader));
            temptest(pRealtimeDat);
        }
        /// 通话数据
        else if (pFrmHdr->resvmode == 0x04)
        {
            const DedicatedData* pRealtimeDat = reinterpret_cast<const DedicatedData*>(data + sizeof(FrameHeader));
            temptest(pRealtimeDat);
        }
    }
    /// RF扫描的结果
    else if (pFrmHdr->msgid == 0x0153 && pFrmHdr->resvmode == 0x03)
    {
        uint16_t arfcn_num;
        ::memcpy(&arfcn_num, data + sizeof(FrameHeader), sizeof(uint16_t));
        arfcn_num = ::ntohs(arfcn_num);

        for (unsigned int i = 0; i < arfcn_num; ++i)
        {
            RFTrace rf_trace;
            ::memcpy(&rf_trace, data + sizeof(FrameHeader) + 2 + i * sizeof(rf_trace), sizeof(rf_trace));
            printf("<%d>Arfcn=%d, rx_lev=%d\n", i, ::ntohs(rf_trace.arfcn), rf_trace.rx_lev);
        }
    }
}

int main(int argc, char *argv[])
{
    FrameHeader hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.stx = 0x02;
    hdr.modeid = 0x02;

    FrameTail tail;
    memset(&tail, 0, sizeof(tail));
    tail.etx = 0x03;

    DedicatedData calldat;
    memset(&calldat, 0x55, sizeof(calldat));

    IdleData idledat;
    memset(&idledat, 0x66, sizeof(idledat));

    char buf[512];
    hdr.msglen = 7 + sizeof(idledat);
    hdr.msglen = ::htons(hdr.msglen);
    hdr.msgid = 0x0073;
    hdr.resvmode = 0x03;

    memcpy(buf, &hdr, sizeof(hdr));
    memcpy(buf + sizeof(hdr), &idledat, sizeof(idledat));
    memcpy(buf + sizeof(hdr) + sizeof(idledat), &tail, sizeof(tail));

    //processData(buf, sizeof(hdr) + sizeof(idledat) + sizeof(tail));

    /////////////////////////////////////////////
    hdr.msglen = 7 + sizeof(calldat);
    hdr.msglen = ::htons(hdr.msglen);
    hdr.msgid = 0x0073;
    hdr.resvmode = 0x04;

    memcpy(buf, &hdr, sizeof(hdr));
    memcpy(buf + sizeof(hdr), &calldat, sizeof(calldat));
    memcpy(buf + sizeof(hdr) + sizeof(calldat), &tail, sizeof(tail));

    //processData(buf, sizeof(hdr) + sizeof(calldat) + sizeof(tail));

    processData(act_data, sizeof(act_data));

    processData(rf_trace, sizeof(rf_trace));
    processData(bcch2, sizeof(bcch2));

    return 0;
}

