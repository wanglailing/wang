/*
 * ----------------------------------------------------------------------------------
 *     系统名称: 列控设备动态监测系统 
 *       文件名: Destroyer.h
 *
 *       版本号: 1.0
 *     创建日期: 2014年06月16日 16时25分04秒
 *
 *         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 *  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 *
 *
 *     修改历史:   none
 *
 * ----------------------------------------------------------------------------------
 */

#ifndef _DESTROYER_H
#define _DESTROYER_H

namespace HELIUM
{
    /**
     * 单件类的辅助类
     *
     * @see CSingleton
     *
     */
    template <class T>
    class CDestroyer
    {
        public:
            CDestroyer(T* d = 0) : m_ObjToGuard(d) {}

            /**
             * 删除守护的对象
             *
             */
            ~CDestroyer()
            {
                if (m_ObjToGuard)
                {
                    delete m_ObjToGuard;
                }
            }

            /**
             * 设置守护对象
             *
             * @param d 被守护的对象指针
             *
             */
            void setGuard(T* d) 
            {
                m_ObjToGuard = d;
            }

        private:
            CDestroyer(const CDestroyer<T>&);
            CDestroyer<T>& operator=(const CDestroyer<T>&);

        private:
            /// 要守护的对象
            T* m_ObjToGuard;
    };
}

#endif //_DESTROYER_H
