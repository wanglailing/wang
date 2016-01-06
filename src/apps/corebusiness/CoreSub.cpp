/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: CoreSub.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年09月24日 17时18分35秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "CoreSub.h"
#include "CoreApp.h"
#include "Logger.h"
#include "SlaveDevManager.h"
#include "CirNode.h"
#include "GpsNode.h"
#include "AtpPlugin.h"

using namespace HELIUM;
using namespace COREBUSINESS;

CCoreSub::CCoreSub() : m_CoreData(CExchangePacket((uint8_t)PROG_COREPROC)),
    m_SharedData(CExchangePacket((uint8_t)PROG_COREPROC)), m_Transmiter(NULL),
    m_CirId(DMS_CIR), m_GpsId(DMS_GPS), m_TcrId(DMS_TCR), m_AtpId(ATP_300T),
    m_TcasCommId(COMM_TCAS), m_GprsNetState((uint8_t)STATUS_OK),
    m_GsmrNetState((uint8_t)STATUS_OK), m_EffectiveStamp(0),
    m_KeepaliveStamp(0), m_TcasHeartStamp(0)
{
    m_KeepaliveInterval = 60.0f;
    ::memset(&m_DataStatus, 0, sizeof(m_DataStatus));

    DataId data_id;
    if ((data_id = APPCONFIG->getSlaveDevType("CIR")) != 0)
        m_CirId = data_id;
    m_BusinessNodes[m_CirId] = new CCirNode(120000);

    if ((data_id = APPCONFIG->getSlaveDevType("GPS")) != 0)
        m_GpsId = data_id;
    m_BusinessNodes[m_GpsId] = new CGpsNode(2000);

    /// TCR的解析放在StepRunner中
    if ((data_id = APPCONFIG->getSlaveDevType("TCR")) != 0)
        m_TcrId = data_id;

    /// 所有的ATP具备相同的设备类型
    if ((data_id = APPCONFIG->getSlaveDevType("ATP300T")) != 0)
        m_AtpId = data_id;

    if ((data_id = APPCONFIG->getSlaveDevType("COMMTCAS")) != 0)
        m_TcasCommId = data_id;

    m_GprsId = APPCONFIG->getSlaveDevType("GPRS");
    m_GsmrId = APPCONFIG->getSlaveDevType("GSMR");

    m_SubSet[0] = (unsigned char)m_CirId;
    m_SubSet[1] = (unsigned char)m_GpsId;
    m_SubSet[2] = (unsigned char)m_TcrId;
    m_SubSet[3] = (unsigned char)m_AtpId;
    m_SubSet[4] = (unsigned char)m_TcasCommId;

    m_AtpTypeStr = APPCONFIG->getAtpType();

    m_StepRunner = new CStepRunner(5);

    /// 启动走行距离计算线程
    m_StepRunner->start();
}

CCoreSub::~CCoreSub()
{
    std::map<DataId, CDataNode*>::iterator iter;
    for (iter = m_BusinessNodes.begin(); iter != m_BusinessNodes.end(); iter++)
    {
        delete iter->second;
        iter->second = NULL;
    }

    m_StepRunner->stop();
    delete m_StepRunner;
}

int CCoreSub::open(HELIUM::CReactor* r)
{
    m_Reactor = r;
    m_HeartbeatTid = m_Reactor->registerTimerHandler(this, m_HeartbeatInterval, m_TimerName);
    m_KeepaliveTid = m_Reactor->registerTimerHandler(this, m_KeepaliveInterval, "Keepalive with land");

    return 0;
}

int CCoreSub::handleTimeout(HELIUM::TimerId tid)
{
    if (tid == m_HeartbeatTid && m_HeartbeatInfo != NULL)
    {
        getStream().write((const char*)(m_HeartbeatInfo->begin()), m_HeartbeatInfo->readableBytes());
        return 0;
    }
    else if (tid == m_KeepaliveTid)
    {
        sendKeepaliveData();
        manageGsmrNet();
        return 1;
    }

    return 1;
}

void CCoreSub::sendKeepaliveData(void)
{
    m_KeepaliveStamp = CTimeVal::getMonotonicMs();
    if ((m_KeepaliveStamp - m_EffectiveStamp) >= 60000)
    {
        CBuffer keepalive_pack;
        pushMiscData(&keepalive_pack, m_KeepaliveStamp, true);
        CPacketManager::packData(&keepalive_pack, (uint8_t)DMSMSG_BASE,
                gAtpHandlers.getRealtimeInfo(NULL), sizeof(OperBasic));

        CExchangePacket base_packet((uint8_t)PROG_COREPROC);
        base_packet.reset();
        base_packet.append((uint8_t*)keepalive_pack.peek(), keepalive_pack.readableBytes());
        base_packet.finish();
        m_Transmiter->writeData((const char*)(base_packet.begin()), base_packet.readableBytes());
    }
}

void CCoreSub::manageGsmrNet(void)
{
    bool open_gsmr = false;

    /// 根据GPS速度和ATP速度决定网络操作类型
    const GpsInfo *gpsinfo = (const GpsInfo *)getGpsInfo(NULL);
    if ( ((m_DataStatus.atp == (uint8_t)STATUS_OK || m_DataStatus.atpc2 == (uint8_t)STATUS_OK) &&
          gAtpHandlers.getRunSpeed(m_KeepaliveStamp) >= 5) ||
         (m_DataStatus.gps == (uint8_t)STATUS_OK && gpsinfo->speed > 10) )
    {
        open_gsmr = true;
    }

    /// 通过G网板卡的设备状态获取G网通信状态
    if (open_gsmr == false || (open_gsmr == true && m_GsmrNetState == (uint8_t)STATUS_OK))
    {
        return;
    }

    /// 发送G网打开命令
    const char *open_cmd  = "**OPENGSMR##";
    int cmdlen = strlen(open_cmd);

    HELIUM::CExchangePacket container((uint8_t)PROG_COREPROC);
    SUBPACK_HEADER subpack_hdr;
    subpack_hdr.dataid = REMOTE_CMD;
    subpack_hdr.len = sizeof(SUBPACK_HEADER) + sizeof(m_GsmrId) + cmdlen;

    container.reset();
    container.append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));
    container.append((uint8_t*)&m_GsmrId, sizeof(m_GsmrId));
    container.append((uint8_t*)(open_cmd), cmdlen);
    container.finish();
    getStream().write((const char*)(container.begin()), container.readableBytes());
}

void CCoreSub::pushMiscData(HELIUM::CBuffer* pbuf, uint64_t mono_now, bool need_state)
{
    /// 封装GPS数据
    CPacketManager::packData(pbuf, (uint8_t)DMSMSG_GPS,
                             getGpsInfo(NULL), sizeof(GpsInfo));

    /// 收集并封装DMS状态数据
    if (need_state == true)
    {
        collectDataState(mono_now);
        CPacketManager::packData(pbuf, (uint8_t)DMSMSG_STATUS,
                                 (unsigned char*)&m_DataStatus, sizeof(m_DataStatus));
    }

    /// 封装本地时间
    unsigned char date_time[6];
    struct tm local_tm;
    time_t now = time(NULL);
    localtime_r(&now, &local_tm);
    date_time[0] = local_tm.tm_year % 100;
    date_time[1] = local_tm.tm_mon + 1;
    date_time[2] = local_tm.tm_mday;
    date_time[3] = local_tm.tm_hour;
    date_time[4] = local_tm.tm_min;
    date_time[5] = local_tm.tm_sec;
    CPacketManager::packData(pbuf, (uint8_t)DMSMSG_DATETIME,
            date_time, sizeof(date_time), 1);
}

void CCoreSub::processData(const char* data, int length)
{
    if (data == NULL || length <= 0)
        return;

    uint64_t timenow = CTimeVal::getMonotonicMs();

    bool recv_atp = false;
    bool gps_valid = false;
    bool core_transmit = false;

    int offset = sizeof(CAPSUL_HEADER);    /// 跳过报文头部
    while (offset < length)
    {
        SUBPACK_HEADER* subpack_hdr = (SUBPACK_HEADER*)(data + offset);
        const unsigned char* payload_ptr = (const unsigned char*)(data + offset + 4);
        const unsigned char* packet_ptr  = (const unsigned char*)(data + offset);
        int payload_len = subpack_hdr->len - 4;

        /// ATP处理逻辑从动态库中显式加载
        if ((uint8_t)(subpack_hdr->dataid) == (uint8_t)m_AtpId && gAtpHandlers.parseData)
        {
            /// 将数据状态传递给ATP解析库
            collectDataState(timenow);
            gAtpHandlers.updateDataState((unsigned char *)&m_DataStatus);

            /// 将CIR车次传递给ATP解析库
            gAtpHandlers.updateTrainNum(m_BusinessNodes[m_CirId]->getParsedInfo(NULL), timenow);

            int bgp_passed = gAtpHandlers.parseData(packet_ptr, subpack_hdr->len, &m_NodesBuffer, timenow);
            if (bgp_passed == 2)   /// 应答器组变化时将过应答器距离清零
            {
                m_StepRunner->resetPassBaliseDist();
            }
            recv_atp = true;
        }
        else if (subpack_hdr->dataid == m_TcrId)
        {
            m_StepRunner->parseData(payload_ptr, payload_len, NULL, timenow);
        }
        else if (subpack_hdr->dataid == m_TcasCommId)
        {
            m_TcasHeartStamp = timenow;
        }
        else if (subpack_hdr->dataid == DMS_STATE)
        {
            HELIUM::SLAVEDEV_MGR->update(reinterpret_cast<const unsigned char *>(payload_ptr),
                    payload_len);
            m_GprsNetState = parseDeviceState(m_GprsId);
            m_GsmrNetState = parseDeviceState(m_GsmrId);
        }
        else
        {
            int result = 0;
            std::map<DataId, CDataNode*>::const_iterator iter = m_BusinessNodes.find((DataId)subpack_hdr->dataid);
            if (iter != m_BusinessNodes.end() && iter->second)
            {
                result = iter->second->parseData(payload_ptr, payload_len, NULL, timenow);
            }

            if (subpack_hdr->dataid == m_GpsId && result > 0)
            {
                gps_valid = true;
            }
        }

        offset += subpack_hdr->len;
    }

    int parsed_len = m_NodesBuffer.readableBytes();
    if (parsed_len > 0)
    {
        CBuffer prepend_buf;
        pushMiscData(&prepend_buf, timenow, false);

        m_CoreData.reset();
        if (recv_atp == true)
        {
            /// 将TCR电压幅值放到ATP数据后面(有些车型不需要回传TCR电压幅值)
            if (m_AtpTypeStr == "200H" || m_AtpTypeStr == "200C")
            {
                uint8_t tcr_buf[1024];
                int tcr_len = m_StepRunner->pushVoltages(tcr_buf, sizeof(tcr_buf));
                if (tcr_len > 0)
                {
                    prepend_buf.append(tcr_buf, tcr_len);
                }
            }
        }

        /**
         * BASE包还具有报文边界的作用，总是将它放在报文的末尾。
         * 传输控制模块在收到BASE包后，会将该包连同之前收到的报文一并封装。
         * prepend_buf的作用就是将其它包放在BASE包的前面。
         */
        m_CoreData.append((uint8_t*)prepend_buf.peek(), prepend_buf.readableBytes());
        m_CoreData.append((uint8_t*)m_NodesBuffer.peek(), m_NodesBuffer.readableBytes());
        if (recv_atp == true || gps_valid == true)
        {
            core_transmit = true;
            prepareCoreData(&m_CoreData);
        }
        m_CoreData.finish();
        m_Transmiter->writeData((const char*)(m_CoreData.begin()), m_CoreData.readableBytes());
        m_EffectiveStamp = timenow;

        m_NodesBuffer.retrieveAll();
    }

    /**
     * ATP和GPS数据中含有速度信息，扩展模块需要用速度自行计算走行距离，
     * 因此这些数据要及时广播出去
     */
    if (recv_atp == true || gps_valid == true)
    {
        pushShareData(core_transmit);
    }
}

uint8_t CCoreSub::getTcasState(uint64_t now)
{
    if (m_TcasHeartStamp != 0 && (now - m_TcasHeartStamp) < 120000)
        return 1;

    return 0;
}

void CCoreSub::collectDataState(uint64_t now)
{
    unsigned int atp_state = gAtpHandlers.getState(now);

    m_DataStatus.atp   = (uint8_t)atp_state;
    m_DataStatus.atpc2 = (uint8_t)(atp_state >> 16);
    m_DataStatus.cir   = m_BusinessNodes[m_CirId]->getState(now);
    m_DataStatus.gps   = m_BusinessNodes[m_GpsId]->getState(now);
    m_DataStatus.tcr   = m_StepRunner->getTcrState(now);
    m_DataStatus.gprs  = m_GprsNetState;
    m_DataStatus.gsmr  = m_GsmrNetState;
    m_DataStatus.tcas  = getTcasState(now);
}

uint8_t CCoreSub::parseDeviceState(DataId dataid)
{
    uint8_t result = (uint8_t)STATUS_TIMEOUT;

    DeviceItem dev_item;
    dev_item = HELIUM::SLAVEDEV_MGR->queryDeviceItem(dataid);
    if ((uint16_t)(dev_item.devid.id >> 16) != dataid)
    {
        return (uint8_t)STATUS_ERROR;
    }

    if ((dev_item.devstatus.fields.state & 0x01) == 1)
    {
        result = (uint8_t)STATUS_OK;
    }

    return result;
}

void CCoreSub::prepareCoreData(HELIUM::CExchangePacket* container)
{
    int atp_len;
    int gps_len;
    SUBPACK_HEADER subpack_hdr;

    AtpRTInfo* atp_info = (AtpRTInfo*)gAtpHandlers.getRealtimeInfo(&atp_len);

    /// 更新过应答器距离和走行距离
    atp_info->balise.passdist = m_StepRunner->getPassBaliseDist();
    atp_info->opbase.basic.runDistance = m_StepRunner->getPreciseRunDist();

    unsigned char* gps_info = getGpsInfo(&gps_len);
    subpack_hdr.dataid = CORE_DATASETS;
    subpack_hdr.len = sizeof(subpack_hdr) + atp_len + gps_len + sizeof(m_DataStatus);

    container->append((uint8_t*)&subpack_hdr, sizeof(subpack_hdr));
    container->append((uint8_t*)atp_info, atp_len);
    container->append(gps_info, gps_len);
    container->append((uint8_t*)&m_DataStatus, sizeof(m_DataStatus));
}

void CCoreSub::pushShareData(bool has_transmited)
{
    m_SharedData.reset();
    prepareCoreData(&m_SharedData);
    m_SharedData.finish();

    if (has_transmited == false)
    {
        m_Transmiter->writeData((const char*)(m_SharedData.begin()), m_SharedData.readableBytes());
    }
    PUBLISHER->broadcastData((const char*)m_SharedData.begin(), m_SharedData.readableBytes());
}

unsigned char* CCoreSub::getGpsInfo(int* length)
{
    return m_BusinessNodes[m_GpsId]->getParsedInfo(length);
}

