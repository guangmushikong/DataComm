/** 
* @brief	���ݸ��½ӿ���
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
	* @brief: ���ݸ��½ӿں���
    * @description:	���ݸ��½ӿں���
	* @param: pBuffer 
	* @return: TRUE �ɹ���FALSE ʧ�ܡ�
	*/
	virtual void DataUpdate(char *pBuffer, int size) = 0;

private:

};
 

#endif