/******************************************************************************************



	author:	Robin
	date:	2020-09-23

******************************************************************************************/

#ifndef __ANA_BUF_H__
#define __ANA_BUF_H__

#include "AnaBaseType.h"
#include "FileStream.h"

#define M_ANABUF_PROTECT_SIZE (M_TS_PACKAGE_LEN * 1)

typedef enum{

	EN_SUCCESS = 0,
	EN_GET_DATA_FAIL,
	EN_GET_DATA_END,
	EN_DATA_SOURCE_NOT_EXIST,
	EN_DATA_BUF_FULL
}EN_DRAG_DATA_RESULT;

typedef enum{

	EN_PACKAGE_UNSYNC = 0,
	EN_PACKAGE_SYNC
}EN_PACKAGE_SYNC_STATE;

class CAnaBuf {

public:
	CAnaBuf();
	CAnaBuf(u32 u32BufSize);

	~CAnaBuf();

	int ResetBufSize(u32 u32BufSize = 1024);

	void ConnectFileSource(CFileStream *pFileSource);

	EN_DRAG_DATA_RESULT GetOneTsPackage(u8 *pBuf, u32 u32PackagelLen = 188);

protected:
	// ���� round buffer size
	int resetbuf(u32 u32BufSize);

	// �� round buffer ��u32Offsetƫ��(���round buffer��headposd)����peekһ���ֽ����ݹ��۲�
	//u8 peekonebyte(u32 u32Offset);

	// �� round buffer ��u32Offsetƫ��(���round buffer��headposd)����peekһ�����ݹ��۲�
	//void peekdata(u8 *pBuf, u32 u32PeekLen, u32 u32Offset);

	// ͬ����ͷ
	//virtual void syncpackage();

	// �ӹ�����File�л�ȡ���ݣ����� round buffer
	EN_DRAG_DATA_RESULT dragdata();

	// �� round buffer ��ȡ�� u32Package
	void getdata(u8 *pBuf, u32 u32PackageLen);


private:
	u32 m_u32BufSize;
	u32 m_u32HeadPos;
	u32 m_u32TailPos;
	u32 m_u32AvaData;

	EN_PACKAGE_SYNC_STATE  m_enSyncState;
	
	u8 *m_pBuf;

	CFileStream *m_pFileSource;

};

#endif // end "#ifndef __ANA_BUF_H__"