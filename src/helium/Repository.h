/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Repository.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年07月30日 15时33分39秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _REPOSITORY_H
#define _REPOSITORY_H

#include <vector>

namespace HELIUM
{
    /**
     * 资源容器类。保存有资源的引用，不负责资源的回收。
     *
     */
    template<typename T>
    class CRepository
    {
        public:
            typedef T* value_type;
            typedef size_t size_type;

            typedef typename std::vector<value_type> list_t;
            typedef typename std::vector<value_type>::iterator iterator;
            typedef typename std::vector<value_type>::const_iterator const_iterator;

        public:
            CRepository() { m_Collection = new list_t; }
            virtual ~CRepository()
            {
                if (m_Collection)
                {
                    clear();
                    delete m_Collection;
                }
            }

            iterator begin() { return m_Collection->begin(); }
            const_iterator begin() const { return m_Collection->begin(); }

            iterator end() { return m_Collection->end(); }
            const_iterator end() const { return m_Collection->end(); }

            bool empty() const { return m_Collection->empty(); }
            size_type size() const { return m_Collection->size(); }

            void push_back(const value_type& x) 
            {
                m_Collection->push_back(x);
            }

            iterator erase(iterator pos) { return m_Collection->erase(pos); }

            bool erase(const value_type& x) 
            {
                iterator iter = m_Collection->begin();
                while (iter != m_Collection->end())
                {
                    if ((*iter) == x)
                    {
                        erase(iter);
                        return true;
                    }

                    iter++;
                }

                return false;
            }

            void clear() 
            {
                m_Collection->erase(m_Collection->begin(), m_Collection->end());
            }

        private:
            list_t *m_Collection;
    };
}

#endif //_REPOSITORY_H
