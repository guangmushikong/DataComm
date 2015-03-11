// DataContainer.h: interface for the CDataContainer class.
//
//////////////////////////////////////////////////////////////////////
/**
* @file DataContainer.h
* @brief 数据容器模版类
* @description 数据容器模版类，加入多线程处理
* @author 
* @version 1.0
* @history 修改记录表
  <修改者> <修改日期> <版本> <修改内容简介>
*/

#if !defined(AFX_DATACONTAINER_H__BE7EC340_B13B_4992_BAB7_5CFFA896DFD8__INCLUDED_)
#define AFX_DATACONTAINER_H__BE7EC340_B13B_4992_BAB7_5CFFA896DFD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"


template <typename T>
class CDataContainer   
{
public:
	CDataContainer();
	virtual ~CDataContainer();

	/**
	* @brief: 在容器中插入数据
    * @description:	在容器中插入数据
	* @param: data 待插入数据
	* @return: N/A
	*/
	void PushData(const T &data);

	/**
	* @brief: 获取容器的最新数据
    * @description:	获取容器中的最新数据
	* @param: data 输出数据
	* @return: TRUE 成功， FALSE 失败。
	*/
	bool GetData(T &data);

	/**
	* @brief: 判断容器是否为空
    * @description:	判断容器是否为空
	* @return: TRUE 成功， FALSE 失败。
	*/
	bool IsEmpty();

	/**
	* @brief: 释放容器
    * @description:	释放容器
	* @return: N/A
	*/
	void FreeContainer();

private:

	//数据容器
	list<T> m_lstData;

	//临界区 在多线程操作容器时进行保护
	CRITICAL_SECTION m_rwLock;

};

//构造函数
template <typename T>
CDataContainer<T>::CDataContainer()
{
	m_lstData.clear();
}

//析构函数
template <typename T>
CDataContainer<T>::~CDataContainer()
{
	m_lstData.clear();
}

//释放容器
template <typename T>
void CDataContainer<T>::FreeContainer()
{
	delete this;
}

//插入数据
template <typename T>
void CDataContainer<T>::PushData(const T &data)
{
	EnterCriticalSection(&m_rwLock);

	m_lstData.push_back(data);

	LeaveCriticalSection(&m_rwLock);
}

//获取数据
template <typename T>
bool CDataContainer<T>::GetData(T &data)
{
	if (!m_lstData.empty())
	{
		EnterCriticalSection(&m_rwLock);
		data = m_lstData.front();

		m_lstData.pop_front();
		
		LeaveCriticalSection(&m_rwLock);
		
		return TRUE;
	}

	return FALSE;
}

//判断容器是否为空
template <typename T>
bool CDataContainer<T>::IsEmpty()
{
	return m_lstData.empty();
}


#endif // !defined(AFX_DATACONTAINER_H__BE7EC340_B13B_4992_BAB7_5CFFA896DFD8__INCLUDED_)
