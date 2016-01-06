/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统
 *       文件名: AppConfig.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年10月21日 10时47分17秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H

#include <string>
#include <vector>
#include <map>

#include "tinyxml/tinyxml.h"

namespace HELIUM
{
    /**
     * 应用程序通用配置。
     *
     */
    class CAppConfig
    {
        public:
            explicit CAppConfig(void* params = NULL);
            virtual ~CAppConfig();

            /// 获取ATP类型
            std::string getAtpType(void) const { return m_AtpType; }

            /// 获取车号的字符串表示
            std::string getTrainIDStr(void) const { return m_TrainIDStr; }

            /// 获取车号的数字表示
            unsigned int getTrainIDNum(void) const { return m_TrainIDNum; }

            /// 获取从板设备类型
            unsigned short getSlaveDevType(const char *dev_name) const;

            /// GPRS数据是否使用字符填充协议
            bool isGprsStuffed(void) const { return m_bGprsStuff; }

            /**
             * @brief 将16进制字符串转换成数字形式
             *
             * @param value  16进制字符串
             * @param length 字符串长度
             *
             * @return 整数
             *
             */
            static unsigned int convertHexString(const char *value, int length);

            /**
             * @brief 获取某个叶节点的值
             *
             * @param doc          XML文档
             * @param nodes_chain  xml节点路径(从第二级节点到叶节点的路径，用'/'分割)
             *
             * @return 叶节点的值
             *
             */
            static const char* getXmlNodeValue(const TiXmlDocument& doc, const char* nodes_chain);

        protected:
            /**
             * @brief 解析配置文件
             *
             * @param file_name 配置文件名
             * @param params    自定义参数
             *
             */
            void parseConfigFile(const char* file_name, void* params);

            /**
             * @brief 解析自定义配置
             *
             * @param doc    XML文档
             * @param params 自定义参数
             *
             */
            virtual void parseCustomConfig(const TiXmlDocument& doc, void* params)
            {
                (void)doc;
                (void)params;
            }

            /**
             * @brief 加载设备类型列表
             *
             * @param file_name 配置文件名
             *
             */
            void loadDevTypeList(const char* file_name);

            /**
             * @brief 将xml节点树放入向量容器
             *
             * @param src      xml节点树(不包含根节点)
             * @param nodetree 向量容器(存放各xml节点)
             *
             */
            static void getXmlNodesTree(std::string& src, std::vector<std::string>& nodetree);

            /**
             * @brief 将车号字符串转换成数字形式
             *
             * @param train_id 车号
             *
             * @return 数字形式的车号
             *
             */
            unsigned int convertTrainIdToNum(const char *train_id);

        protected:
            /// DMS版本号
            std::string m_DmsVersion;

            /// ATP类型
            std::string m_AtpType;

            /// 车号
            std::string m_TrainIDStr;
            unsigned int m_TrainIDNum;

            /// GPRS数据采用字符填充协议
            bool m_bGprsStuff;

            /// 从板设备类型集
            std::map<std::string, unsigned short> m_SlaveDevTypes;

        private:
            CAppConfig(const CAppConfig&);
            CAppConfig& operator=(const CAppConfig&);
    };
}

#endif //_APP_CONFIG_H
