/** 
* @brief	����֪ͨ�ӿ���
* @author	 
* @version	1.0
* @date		2010-2011
* @bug		N/A
* @warning	N/A
* @note		N/A
*/

#ifndef _INOTIFY_H
#define _INOTIFY_H

#include "IDataUpdate.h"

class INotify
{
public:
	/**
	* @brief: �������ݹ۲���
    * @description:	�������ݹ۲���
	* @param: pObserver ʵ����IDataUpdate�ӿڵĶ���ָ�� 
	* @return: N/A
	*/
	virtual void AddObserver(IDataUpdate *pObserver) = 0;

	/**
	* @brief: ɾ�����ݹ۲���
    * @description:	ɾ�����ݹ۲���
	* @param: pObserver ʵ����IDataUpdate�ӿڵĶ���ָ�� 
	* @return: N/A
	*/
	virtual void RemoveObserver(IDataUpdate *pObserver) = 0;

	/**
	* @brief: ֪ͨ�������ݹ۲���
    * @description:	֪ͨ�������ݹ۲���
	* @param: pBuffer ���ݻ���
	* @param: pBuffer ���ݳ���
	* @return: N/A
	*/
	virtual void Notify(char *pBuffer, int size) = 0;

};

#endif