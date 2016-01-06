/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Singleton.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月16日 15时38分58秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "Destroyer.h"

namespace HELIUM
{
    /**
     * C++会自动删除静态对象,但是，它不保证删除的顺序。换句话说，单件类的析构函数的调用顺序是
     * 不确定的。
     * 为了保证析构顺序，CSingleton类把对象T的拥有权转移给CDestroyer类，当程序退出时，CDestroyer
     * 及其相关对象将按正确的顺序销毁。
     *
     * @see CDestroyer
     *
     */
    template <class T>
    class CSingleton
    {
        public:
            /// 返回模板类T的实例
            static T* getInstance()
            {
                if (m_Instance == 0)
                {
                    m_Instance = new T; 
                    m_Destroyer.setGuard(m_Instance);
                }
                return m_Instance;
            }

        protected:
            CSingleton() {}
            virtual ~CSingleton() {}

            friend class CDestroyer<T>;

        private:
            /// 对象T的实例指针
            static T* m_Instance;

            /// 拥有对象T的销毁器
            static CDestroyer<T> m_Destroyer;
    };
}

/**
 * @def
 *
 * 初始化由CSingleton类托管的静态变量
 */
#define HELIUM_DECL_SINGLETON(K) \
template <> K* HELIUM::CSingleton<K>::m_Instance = NULL; \
template <class T> HELIUM::CDestroyer<T> HELIUM::CSingleton<T>::m_Destroyer; \
template HELIUM::CDestroyer<K> HELIUM::CSingleton<K>::m_Destroyer;

#endif //_SINGLETON_H_
