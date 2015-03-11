/** 
* @brief	数据通知接口类
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
	* @brief: 增加数据观察者
    * @description:	增加数据观察者
	* @param: pObserver 实现了IDataUpdate接口的对象指针 
	* @return: N/A
	*/
	virtual void AddObserver(IDataUpdate *pObserver) = 0;

	/**
	* @brief: 删除数据观察者
    * @description:	删除数据观察者
	* @param: pObserver 实现了IDataUpdate接口的对象指针 
	* @return: N/A
	*/
	virtual void RemoveObserver(IDataUpdate *pObserver) = 0;

	/**
	* @brief: 通知所有数据观察者
    * @description:	通知所有数据观察者
	* @param: pBuffer 数据缓存
	* @param: pBuffer 数据长度
	* @return: N/A
	*/
	virtual void Notify(char *pBuffer, int size) = 0;

};

#endif