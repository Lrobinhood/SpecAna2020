/******************************************************************************************



	author:	Robin
	date:	2020-09-23

******************************************************************************************/
#include "stdafx.h"

#include <stdio.h>
#include <memory.h>

#include "AnaBaseType.h"
#include "AnaBuf.h"


CAnaBuf::CAnaBuf()
{
	m_u32BufSize = 0;
	m_u32HeadPos = 0;
	m_u32TailPos = 0;
	m_u32AvaData = 0;

	m_pBuf = NULL;

	m_pFileSource = NULL;

}

CAnaBuf::CAnaBuf(u32 u32BufSize)
{
	int iRet = 0;

	m_u32BufSize = 0;
	m_u32HeadPos = 0;
	m_u32TailPos = 0;
	m_u32AvaData = 0;

	m_enSyncState = EN_PACKAGE_UNSYNC;

	m_pBuf = NULL;

	m_pFileSource = NULL;

	iRet = resetbuf(u32BufSize);

}

CAnaBuf::~CAnaBuf()
{
	if (NULL != m_pBuf)
		delete []m_pBuf;

}

int CAnaBuf::ResetBufSize(u32 u32BufSize)
{
	return resetbuf(u32BufSize);

}

void CAnaBuf::ConnectFileSource(CFileStream *pFileSource)
{
	m_pFileSource = pFileSource;
}

int CAnaBuf::resetbuf(u32 u32BufSize)
{
	int iRet = 0;

	if (NULL != m_pBuf)
	{
		delete []m_pBuf;
		
		m_pBuf = NULL;
	}

	m_u32BufSize = u32BufSize + M_ANABUF_PROTECT_SIZE;

	m_u32HeadPos = 0;
	m_u32TailPos = 0;
	m_u32AvaData = 0;

	m_pBuf = new u8[m_u32BufSize];
	if (NULL == m_pBuf)
	{
		printf("\n Malloc datalen:%d Faild! \n", m_u32BufSize);

		iRet = -1;
	}

	return iRet;
}



EN_DRAG_DATA_RESULT CAnaBuf::GetOneTsPackage(u8 *pBuf, u32 u32PackagelLen)
{
	EN_DRAG_DATA_RESULT enRet = EN_SUCCESS;
		
	do
	{
		enRet = dragdata();

		if (EN_GET_DATA_END == enRet && m_u32AvaData < u32PackagelLen)
			return EN_GET_DATA_END;

					
		while (m_u32AvaData > 0)
		{
			if (M_TS_PACKAGE_SYNC == m_pBuf[m_u32HeadPos])
			{
				if (m_u32AvaData < M_TS_PACKAGE_LEN)
				{
					enRet = dragdata();
				}

				if (m_u32AvaData < M_TS_PACKAGE_LEN)
				{
					m_u32HeadPos = (m_u32HeadPos + 1) % m_u32BufSize;
					m_u32AvaData--;
				}
				else
				{
					getdata(pBuf, M_TS_PACKAGE_LEN);
					
					return EN_SUCCESS;
				}
			}
			else
			{
				m_u32HeadPos = (m_u32HeadPos + 1) % m_u32BufSize;
				m_u32AvaData--;
			}
		}
	} while (1);
}

void CAnaBuf::getdata(u8 *pBuf, u32 u32PackageLen)
{
	if (m_u32BufSize - m_u32HeadPos >= u32PackageLen)
	{
		memcpy(pBuf, &m_pBuf[m_u32HeadPos], u32PackageLen);
		
		m_u32HeadPos = (m_u32HeadPos + u32PackageLen) % m_u32BufSize;
	}
	else
	{
		// 1.
		u32 u32BufEndLen = m_u32BufSize - m_u32HeadPos;

		memcpy(pBuf, &m_pBuf[m_u32HeadPos], u32BufEndLen);
		
		// 2.
		u32 u32BufFrondLen = u32PackageLen - u32BufEndLen;
		memcpy(pBuf + u32BufEndLen, &m_pBuf[0], u32BufFrondLen);

		m_u32HeadPos = u32BufFrondLen;
	}

	m_u32AvaData -= u32PackageLen;
}

// Try best to get the more data, fill the buffer full
EN_DRAG_DATA_RESULT CAnaBuf::dragdata()
{
	EN_DRAG_DATA_RESULT enRet = EN_SUCCESS;
	int iReadLen = 0;
	int iTotalReadLenInOneCircle = 0;

	static u32 totallen = 0;

	u32 u32CanGetDataLen = m_u32BufSize - m_u32AvaData;

	if (0 == u32CanGetDataLen)
	{
		enRet = EN_DATA_BUF_FULL;
		goto EXIT;
	}

	if (NULL == m_pFileSource)
	{
		enRet = EN_DATA_SOURCE_NOT_EXIST;
		goto EXIT;
	}

	if (m_pFileSource->IsFileEnd())
	{
		enRet = EN_GET_DATA_END;
		goto EXIT;
	}
	
	if (m_u32TailPos < m_u32HeadPos)
	{
		iReadLen = m_pFileSource->ReadData(&m_pBuf[m_u32TailPos], u32CanGetDataLen);
		iTotalReadLenInOneCircle += iReadLen;

		if (iReadLen != u32CanGetDataLen)
		{
			printf("\n [LB Test]  Maybe File End! u32CanGetDataLen:%d, Read Byte:%d, totallen:%d \n", u32CanGetDataLen, iReadLen, totallen);
		}
		
		if (iReadLen > 0)
		{
			m_u32TailPos = (m_u32TailPos + iReadLen) % m_u32BufSize;
			m_u32AvaData += iReadLen;
		}
		else
		{
			enRet = EN_GET_DATA_FAIL;
			goto EXIT;
		}

	}
	else
	{
		// 1. Fill buf End area
		u32 u32EndAreaLen = m_u32BufSize - m_u32TailPos;

		if (u32EndAreaLen > 0)
		{

			iReadLen = m_pFileSource->ReadData(&m_pBuf[m_u32TailPos], u32EndAreaLen);
			iTotalReadLenInOneCircle += iReadLen;

			if (iReadLen != u32EndAreaLen)
			{
				printf("\n [LB Test]  FAILD! u32EndAreaLen:%d, Read Byte:%d, totallen:%d \n", u32EndAreaLen, iReadLen, totallen);
			}
			
			if (iReadLen == u32EndAreaLen)
			{
				m_u32TailPos = 0;
				m_u32AvaData += iReadLen;
			}
			else if (iReadLen >= 0)
			{
				m_u32TailPos = (m_u32TailPos + iReadLen) % m_u32BufSize;
				m_u32AvaData += iReadLen;
				
				enRet = EN_SUCCESS;
				goto EXIT;
			}
			else
			{
				enRet = EN_GET_DATA_FAIL;
				goto EXIT;
			}
		}
		
		// 2. Fill buf Frontarea
		u32 u32FrontAreaLen = u32CanGetDataLen - u32EndAreaLen;

		if (u32FrontAreaLen > 0)
		{
			iReadLen = m_pFileSource->ReadData(&m_pBuf[m_u32TailPos], u32FrontAreaLen);
			iTotalReadLenInOneCircle += iReadLen;

			if (iReadLen != u32FrontAreaLen)
			{
				printf("\n [LB Test]  FAILD! u32FrontAreaLen:%d, Read Byte:%d, totallen:%d \n", u32FrontAreaLen, iReadLen, totallen);
			}
			
			if (iReadLen > 0)
			{
				m_u32TailPos = (m_u32TailPos + iReadLen) % m_u32BufSize;
				m_u32AvaData += iReadLen;
			}
			else
			{
				enRet = EN_GET_DATA_FAIL;
				goto EXIT;
			}
		}

	}

EXIT:	
	extern int total;

	totallen += iTotalReadLenInOneCircle;

	if (total != totallen)
	{
		printf("\n [LB Test]  [Lind:%d] total:%d, Read Byte:%d, totallen:%d \n", __LINE__, total, iReadLen, totallen);		
	}

	if (EN_GET_DATA_END == enRet)
	{
		printf("\n [LB Test]  enRet:%d, Read Byte:%d, totallen:%d \n", enRet, iReadLen, totallen);
	}

	return enRet;
}








