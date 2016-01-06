/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: AppConfig.cpp
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月21日 11时18分57秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#include "AppConfig.h"
#include "Logger.h"

using namespace HELIUM;

CAppConfig::CAppConfig(void* params) : m_DmsVersion("DMS-II"), m_AtpType("300T"),
                                       m_TrainIDStr("9-999-1"), m_bGprsStuff(false)
{
    parseConfigFile("config.xml", params);
    loadDevTypeList("devlist.xml");
    m_TrainIDNum = convertTrainIdToNum(m_TrainIDStr.c_str());
}

CAppConfig::~CAppConfig()
{
}

void CAppConfig::parseConfigFile(const char* file_name, void* params)
{
    TiXmlDocument doc(file_name);
    if (doc.LoadFile() == false)
    {
        return;
    }

    const char* value;

    /**
     * 所有配置项的查询字符串都要按照如下格式：
     *     二级节点/三级节点/.../叶节点/
     *
     */
    if ((value = getXmlNodeValue(doc, "system/version/")) != NULL)
    {
        m_DmsVersion = std::string(value);
    }

    if ((value = getXmlNodeValue(doc, "train/atp/type/")) != NULL)
    {
        m_AtpType = std::string(value);
    }

    const char* train_type   = getXmlNodeValue(doc, "train/train_id/type/");
    const char* train_num    = getXmlNodeValue(doc, "train/train_id/number/");
    const char* train_endian = getXmlNodeValue(doc, "train/train_id/fringe/");
    if (train_type && train_num && train_endian)
    {
        m_TrainIDStr = std::string(train_type) + "_" + std::string(train_num) + "_" + std::string(train_endian);
    }

    if ((value = getXmlNodeValue(doc, "save_config/gsmrFramT/")) != NULL)
    {
        m_bGprsStuff = (bool)(atoi(value));
    }

    parseCustomConfig(doc, params);
}

void CAppConfig::loadDevTypeList(const char* file_name)
{
    TiXmlDocument doc(file_name);
    if (doc.LoadFile() == false)
    {
        return;
    }

    TiXmlElement *rootElement = doc.RootElement();
    TiXmlNode *pSlaveBoard = rootElement->FirstChild("slaveboard");
    if (pSlaveBoard == NULL)
    {
        return;
    }

    TiXmlNode *pChild;
    for (pChild = pSlaveBoard->FirstChild(); pChild != NULL; pChild = pChild->NextSibling())
    {
        TiXmlElement *pChildElement = pChild->ToElement();
        if (pChildElement == NULL)
            continue;

        unsigned short devtype = (unsigned short)convertHexString(pChildElement->GetText(), 4);
        m_SlaveDevTypes[pChild->Value()] = devtype;
    }
}

unsigned int CAppConfig::convertHexString(const char *value, int length)
{
    if (value == NULL || length <= 0)
        return 0;

    unsigned int result = 0;
    for (int i = 0; i < length; ++i)
    {
        if (value[i] >= '0' && value[i] <= '9')
        {
            result += (value[i] - '0');
        }
        else if (value[i] >= 'A' && value[i] <= 'F')
        {
            result += (value[i] - '7');
        }

        if (i < (length - 1))
        {
            result <<= 4;
        }
    }

    return result;
}

void CAppConfig::getXmlNodesTree(std::string& src, std::vector<std::string>& nodetree)
{
    char delimiter = '/';
    size_t node_pos = 0;
    size_t delimit_pos;
    do {
        delimit_pos = src.find(delimiter, node_pos);
        nodetree.push_back(src.substr(node_pos, delimit_pos - node_pos));
        node_pos = delimit_pos + 1;
    } while( delimit_pos != std::string::npos);
}

const char* CAppConfig::getXmlNodeValue(const TiXmlDocument& doc, const char* nodes_chain)
{
    if (nodes_chain == NULL)
        return NULL;

    std::string tree(nodes_chain);
    std::vector<std::string> nodes;
    nodes.clear();
    getXmlNodesTree(tree, nodes);

    const TiXmlNode* child = doc.RootElement();
    for (unsigned int i = 0; i < nodes.size(); ++i)
    {
        if (child == NULL)
            break;

        if (nodes[i].empty())
            child = child->FirstChild();
        else
            child = child->FirstChild(nodes[i].c_str());
    }

    if (child)
    {
        const TiXmlText* childText = child->ToText();
        if (childText)
        {
            return childText->Value();
        }
    }

    return NULL;
}

unsigned int CAppConfig::convertTrainIdToNum(const char *train_id)
{
    unsigned int result;
    unsigned int num_part, type, endian;
    sscanf(train_id, "%u_%u_%u", &type, &num_part, &endian);

    result = ((type << 24) | (num_part << 1) | endian);

    return result;
}

unsigned short CAppConfig::getSlaveDevType(const char *dev_name) const
{
    std::map<std::string, unsigned short>::const_iterator iter;
    if ((iter = m_SlaveDevTypes.find(std::string(dev_name))) != m_SlaveDevTypes.end())
        return iter->second;

    return 0;
}

