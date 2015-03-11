// DataContainer.h: interface for the CDataContainer class.
//
//////////////////////////////////////////////////////////////////////
/**
* @file DataContainer.h
* @brief ��������ģ����
* @description ��������ģ���࣬������̴߳���
* @author 
* @version 1.0
* @history �޸ļ�¼��
  <�޸���> <�޸�����> <�汾> <�޸����ݼ��>
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
	* @brief: �������в�������
    * @description:	�������в�������
	* @param: data ����������
	* @return: N/A
	*/
	void PushData(const T &data);

	/**
	* @brief: ��ȡ��������������
    * @description:	��ȡ�����е���������
	* @param: data �������
	* @return: TRUE �ɹ��� FALSE ʧ�ܡ�
	*/
	bool GetData(T &data);

	/**
	* @brief: �ж������Ƿ�Ϊ��
    * @description:	�ж������Ƿ�Ϊ��
	* @return: TRUE �ɹ��� FALSE ʧ�ܡ�
	*/
	bool IsEmpty();

	/**
	* @brief: �ͷ�����
    * @description:	�ͷ�����
	* @return: N/A
	*/
	void FreeContainer();

private:

	//��������
	list<T> m_lstData;

	//�ٽ��� �ڶ��̲߳�������ʱ���б���
	CRITICAL_SECTION m_rwLock;

};

//���캯��
template <typename T>
CDataContainer<T>::CDataContainer()
{
	m_lstData.clear();
}

//��������
template <typename T>
CDataContainer<T>::~CDataContainer()
{
	m_lstData.clear();
}

//�ͷ�����
template <typename T>
void CDataContainer<T>::FreeContainer()
{
	delete this;
}

//��������
template <typename T>
void CDataContainer<T>::PushData(const T &data)
{
	EnterCriticalSection(&m_rwLock);

	m_lstData.push_back(data);

	LeaveCriticalSection(&m_rwLock);
}

//��ȡ����
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

//�ж������Ƿ�Ϊ��
template <typename T>
bool CDataContainer<T>::IsEmpty()
{
	return m_lstData.empty();
}


#endif // !defined(AFX_DATACONTAINER_H__BE7EC340_B13B_4992_BAB7_5CFFA896DFD8__INCLUDED_)
