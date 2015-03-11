/** 
* @brief	数据更新接口类
* @author	 
* @version	1.0
* @date		2010-2011
* @bug		N/A
* @warning	N/A
* @note		N/A
*/

#ifndef _IDATAUPDATE_H
#define _IDATAUPDATE_H

class IDataUpdate
{
public:

	/**
	* @brief: 数据更新接口函数
    * @description:	数据更新接口函数
	* @param: pBuffer 
	* @return: TRUE 成功，FALSE 失败。
	*/
	virtual void DataUpdate(char *pBuffer, int size) = 0;

private:

};
 

#endif