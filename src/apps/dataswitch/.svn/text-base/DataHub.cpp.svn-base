/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: DataHub.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年08月20日 14时48分14秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "DataHub.h"
#include "UnixAddress.h"
#include "InetAddress.h"
#include "DataSwitchApp.h"
#include "SlaveDevManager.h"
#include "BoardBusOp.h"
#include "MemDump.h"
#include "Assure.h"

#include "../CommAddress.h"
#include "../JruProto.h"

using namespace HELIUM;
using namespace DMS;

const char* CDataHub::m_UnixBindPath = DATASWITCH_ADDRESS;

CDataHub::CDataHub(CReactor *r) : m_Reactor(r), m_bdcastPack(CExchangePacket((uint8_t)PROG_DATASWITCH)),
    m_GetDataTid(0), m_QueryStateTid(0), m_JruKeepaliveTid(0)
{
    m_SwitchAcceptor = new CAcceptor<CSwitchService, CTcpSocket>(r);

    if (APP_CONFIG->getAtpType().compare("300T") == 0)
    {
        m_JruReceiver = new CAcceptor<CJRUService, CTcpSocket>(r);
        m_MvbReceiver = new CAcceptor<CJRUService, CTcpSocket>(r);
    }
    else
    {
        m_JruReceiver = NULL;
        m_MvbReceiver = NULL;
    }
}

CDataHub::~CDataHub()
{
    delete m_SwitchAcceptor; m_SwitchAcceptor = NULL;
    delete m_JruReceiver; m_JruReceiver = NULL;
    delete m_MvbReceiver; m_MvbReceiver = NULL;
}

int CDataHub::init(void)
{
    if (BOARD_BUS->openBus() < 0)
    {
        ERR_LOG((DATASWITCH, "Failed to open the base board bus!\n"));
    }
    else
    {
        CTimeVal tv(0.005f);
        m_GetDataTid = m_Reactor->registerTimerHandler(this, tv, "Board Bus IO");

        tv.setSec(10);
        m_QueryStateTid = m_Reactor->registerTimerHandler(this, tv, "Query Device State");
    }

    if (m_JruReceiver != NULL && m_MvbReceiver != NULL)
    {
        CTimeVal tv(10.0f);
        m_JruKeepaliveTid = m_Reactor->registerTimerHandler(this, tv, "JRU KeepAlive");
    }

    m_SwitchAcceptor->setId("Data Hub Service");
    CUnixAddress listenAddr(m_UnixBindPath);
    ASSERT_EXIT(!listenAddr.bad());
    ASSERT_EXIT(m_SwitchAcceptor->open(listenAddr) == 0);

    if (m_JruReceiver)
    {
        m_JruReceiver->setId("JRU Receiver Service");
        CInetAddress jruAddr(JRU_LISTENPORT);
        ASSERT_EXIT(!jruAddr.bad());
        ASSERT_EXIT(m_JruReceiver->open(jruAddr) == 0);
    }

    if (m_MvbReceiver)
    {
        m_MvbReceiver->setId("MVB Receiver Service");
        CInetAddress mvbAddr(MVB_LISTENPORT);
        ASSERT_EXIT(!mvbAddr.bad());
        ASSERT_EXIT(m_MvbReceiver->open(mvbAddr) == 0);
    }

    return 0;
}

int CDataHub::handleTimeout(TimerId tid)
{
    if (tid == m_GetDataTid)
    {
        char buf[4096];
        ssize_t length = BOARD_BUS->recvFromBus(buf, sizeof(buf));
        if (length < 0)
        {
            ERR_LOG((DATASWITCH, "Board bus read() return [%d]\n", length));
        }
        else
        {
            parseBusProto((unsigned char *)(buf), length);
        }
    }
    else if (tid == m_QueryStateTid)
    {
        BOARD_BUS->queryDeviceState();
    }
    else if (tid == m_JruKeepaliveTid)
    {
        keepaliveToJru();
    }

    return 1;
}

int CDataHub::parseBusProto(unsigned char *buf, int len)
{
    if (buf == NULL || len < (int)(sizeof(BusDataHeader) + 2))
    {
        return -1;
    }

    BusDataHeader *pBusHdr = (BusDataHeader *)(buf);
    if (pBusHdr->headFlag != 0xF0F0)
    {
        ERR_LOG((DATASWITCH, "Invalid bus data header flag[%d]\n", pBusHdr->headFlag));
        return -1;
    }

    unsigned short serv_type;
    unsigned short packet_len;
    unsigned char* packlen_pos;

    BUS_SERVTYPE busToS = static_cast<BUS_SERVTYPE>(pBusHdr->ToS);
    switch (busToS)
    {
        case NORMAL_SERVICE:
            memcpy(&serv_type, buf + sizeof(BusDataHeader) + sizeof(DeviceId), sizeof(unsigned short));
            packlen_pos = (buf + sizeof(BusDataHeader) + sizeof(DeviceId));
            switch ((DATA_SERVTYPE)serv_type)
            {
                case DAT_EXCHANGE:
                    /// 设备标识符的低2字节为地址和端口号，将其从广播数据中去除
                    packet_len = pBusHdr->payloadLen - 2;   /// 将服务类型字段填充为报文长度
                    memcpy(packlen_pos, &packet_len, sizeof(unsigned short));
                    broadcastData(NULL, reinterpret_cast<const char *>(buf + sizeof(BusDataHeader) + 2),
                            pBusHdr->payloadLen - 2, false);
                    break;
                case DAT_SPECIAL:
                case DAT_UPGRADE_ACK:
                case DAT_UPGRADE_OVER:
                    break;
                default:
                    DBG_LOG((DATASWITCH, "Unknown board bus service type[%04X]\n", serv_type));
                    break;
            }
            break;
        case INTERNAL_SERVICE:
            parseBoardInternalData(reinterpret_cast<const char *>(buf + sizeof(BusDataHeader)),
                    pBusHdr->payloadLen);
            break;
        case EXTEND_SERVICE:
            DBG_LOG((DATASWITCH, "<%d> Extended service.\n", pBusHdr->seq));
            break;
        default:
            DBG_LOG((DATASWITCH, "<%d> Unknown service.\n", pBusHdr->seq));
            break;
    }

    return 0;
}

int CDataHub::appendJruState(char* dst, int dst_len, const char* src, int src_len)
{
    int result;

    if (dst_len < (src_len + (int)sizeof(DeviceItem)))
    {
        return -1;
    }

    memcpy(dst, src, src_len);
    result = src_len;

    if (JRU_REPO->empty() == false)
    {
        /// 第1个字节为设备个数
        *dst += 1;

        DeviceItem dev_item;
        memset(&dev_item, 0, sizeof(dev_item));
        dev_item.devid.id = DATASWITCH_APP->get300tId();
        dev_item.devid.id <<= 16;
        dev_item.devid.id |= 0xB1FE;
        dev_item.devstatus.status = 0x0101;

        memcpy(dst + result, &dev_item, sizeof(dev_item));
        result += sizeof(dev_item);
    }

    return result;
}

void CDataHub::parseBoardInternalData(const char *buf, int len)
{
    if (!buf || len <= 1)
        return;

    char state_buf[1024];
    int state_len;

    SUBPACK_HEADER subpack_hdr;
    BUS_INTER_SERVT inter_serv_type = (BUS_INTER_SERVT)(*buf);
    switch (inter_serv_type)
    {
        case INTER_DEVSTATUS:
            SLAVEDEV_MGR->update(reinterpret_cast<const unsigned char *>(buf + 1),
                    len - 1);

            /// JRU(300T)数据可能会通过网络传输，需要为其模拟一个设备标识
            state_len = appendJruState(state_buf, sizeof(state_buf), buf + 1, len - 1);

            subpack_hdr.dataid = DMS_STATE;
            subpack_hdr.len = sizeof(SUBPACK_HEADER) + state_len;
            broadcastData(&subpack_hdr, state_buf, state_len);
            break;
        case INTER_GET_DEVSTATUS:
            break;
        case INTER_TO_SLAVE:
            break;
        case INTER_FROM_SLAVE:
            break;
        case INTER_EXTEND:
            break;
        default:
            break;
    }
}

int CDataHub::broadcastData(const HELIUM::SUBPACK_HEADER* subpack_hdr, const char *buf, int len, bool broadcast)
{
    if (SERV_REPO->empty() == true || buf == NULL || len <= 0)
    {
        return -1;
    }

    unsigned short data_id;

    m_bdcastPack.reset();
    if (subpack_hdr != NULL)
    {
        data_id = subpack_hdr->dataid;
        m_bdcastPack.append((uint8_t*)subpack_hdr, sizeof(SUBPACK_HEADER));
    }
    else
    {
        memcpy(&data_id, buf, sizeof(data_id));
    }
    m_bdcastPack.append((uint8_t*)buf, len);
    m_bdcastPack.finish();

    CRepository<CSwitchService>::const_iterator iter = SERV_REPO->begin();
    while (iter != SERV_REPO->end())
    {
        if ( *iter && 
           ((broadcast == false && (*iter)->isDataQueried((unsigned char)data_id) == true) || broadcast == true) )
        {
            (*iter)->getStream().write((const char*)(m_bdcastPack.begin()), m_bdcastPack.readableBytes());
        }

        iter++;
    }

    return 0;
}

void CDataHub::keepaliveToJru(void)
{
    if (JRU_REPO->empty() == true)
    {
        return;
    }

    time_t now;
    time(&now);

    struct tm ct;
    char heartbeat_buf[80];
    memset(heartbeat_buf, 0 , sizeof(heartbeat_buf));
    ct = *(::localtime((const time_t*)&now));

    heartbeat_buf[0] = '*';
    heartbeat_buf[1] = '*';
    ::strftime(heartbeat_buf + 2, sizeof(heartbeat_buf) - 5, "%F %T", &ct);
    ::strncat(heartbeat_buf, "##", 2);

    CRepository<CJRUService>::const_iterator iter = JRU_REPO->begin();
    while (iter != JRU_REPO->end())
    {
        if (*iter)
        {
            (*iter)->getStream().write(heartbeat_buf, strlen(heartbeat_buf));
        }

        iter++;
    }
}

