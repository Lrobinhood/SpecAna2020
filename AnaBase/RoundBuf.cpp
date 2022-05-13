/******************************************************************************************



    Author: Robin
    Date:   2020-9-24

******************************************************************************************/
//#include "stdafx.h"

#include <stdio.h>
#include <memory.h>

#include "AnaBaseType.h"
#include "FileStream.h"

#include "RoundBuf.h"


CRoundBuf::CRoundBuf()
{
    int iRet = 0;

    m_u32BufSize = 0;

    m_u32HeadPos = 0;
    m_u32TailPos = 0;
    m_u32AvaData = 0;

    m_ConnectedFileOffset = 0;

    m_pBuf = NULL;

    m_pFileSource = NULL;

    iRet = resetbuf(1024);

}


CRoundBuf::CRoundBuf(u32 u32BufSize)
{
    int iRet = 0;

    m_u32BufSize = 0;

    m_u32HeadPos = 0;
    m_u32TailPos = 0;
    m_u32AvaData = 0;

    m_ConnectedFileOffset = 0;

    m_pBuf = NULL;

    m_pFileSource = NULL;

    iRet = resetbuf(u32BufSize);

}

CRoundBuf::~CRoundBuf()
{
    if (NULL != m_pBuf)
        delete []m_pBuf;

}

int CRoundBuf::ResetBufSize(u32 u32BufSize)
{
    return resetbuf(u32BufSize);

}

void CRoundBuf::ConnectFileSource(CFileStream *pFileSource)
{
    m_pFileSource = pFileSource;

    dragdata();
}

long CRoundBuf::GetConnectFileLen()
{
    if (NULL != m_pFileSource)
        return m_pFileSource->GetFileLen();

    return 0;
}

int CRoundBuf::resetbuf(u32 u32BufSize)
{
    int iRet = 0;

    if (NULL != m_pBuf)
    {
        delete []m_pBuf;

        m_pBuf = NULL;
    }

    m_u32BufSize = u32BufSize;

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


#if 0
EN_DRAG_DATA_RESULT CRoundBuf::GetOneTsPackage(u8 *pBuf, u32 u32PackagelLen)
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



bool CRoundBuf::syncpackage()
{

}
#endif

u32 CRoundBuf::GetData(u8 *pBuf, u32 u32PackageLen)
{
    // Not Enought Data to get
    if (u32PackageLen > m_u32AvaData)
        return 0;

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

    EN_DRAG_DATA_RESULT ret = dragdata();
    if (EN_SUCCESS != ret)
    {
        printf("\n [LB Test] [%s, %d] call dragdata faild! \n", __FUNCTION__, __LINE__);

        return 0;
    }

    return u32PackageLen;

}



u32 CRoundBuf::DiscardData(u32 u32Len)
{
    // Not Enought Data to Discard
    if (u32Len > m_u32AvaData)
        return 0;

    if (m_u32BufSize - m_u32HeadPos >= u32Len)
    {
        m_u32HeadPos = (m_u32HeadPos + u32Len) % m_u32BufSize;
    }
    else
    {
        // 1.
        u32 u32BufEndLen = m_u32BufSize - m_u32HeadPos;


        // 2.
        u32 u32BufFrondLen = u32Len - u32BufEndLen;

        m_u32HeadPos = u32BufFrondLen;
    }

    m_u32AvaData -= u32Len;

    EN_DRAG_DATA_RESULT ret = dragdata();
    if (EN_SUCCESS != ret && EN_GET_DATA_END != ret)
    {
        printf("\n [LB Test] [%s, %d] call dragdata faild! \n", __FUNCTION__, __LINE__);

        return 0;
    }

    return u32Len;

}


// 从 round buffer 的u32Offset偏移(相对round buffer的headpos)处，peek一个字节数据供观察
u8 CRoundBuf::PeekOneByte(u32 u32Offset)
{
    if (u32Offset > m_u32BufSize)
        return 0;

    return m_pBuf[(m_u32HeadPos + u32Offset) % m_u32BufSize];
}

// 从 round buffer 的u32Offset偏移(相对round buffer的headpos)处，peek一段数据供观察
//void PeekData(u8 *pBuf, u32 u32PeekLen, u32 u32Offset);




// Try best to get the more data, fill the buffer full
EN_DRAG_DATA_RESULT CRoundBuf::dragdata()
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
            printf("\n [LB Test] [%s:%d] Maybe File End! u32CanGetDataLen:%d, Read Byte:%d, totallen:%d \n", __FILE__, __LINE__,
                u32CanGetDataLen, iReadLen, totallen);
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

    if (NULL != m_pFileSource)
        m_ConnectedFileOffset = m_pFileSource->GetFilePos() - m_u32AvaData;

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








