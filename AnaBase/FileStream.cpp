/******************************************************************************************



    author: Robin
    date:   2013-9-27

******************************************************************************************/
//#include "stdafx.h"

#include <stdio.h>
#include <io.h>

#include "AnaBaseType.h"
#include "FileStream.h"

CFileStream::CFileStream():m_FileLen(0)
{
    m_fp = NULL;
}



CFileStream::CFileStream(const char *pstrFileName):m_FileLen(0)
{
    int ret = 0;

    m_fp = NULL;

    ret = openfile(pstrFileName);
}



CFileStream::~CFileStream()
{
    if (NULL != m_fp)
        fclose(m_fp);
}



int CFileStream::OpenFile(const char *pstrFileName)
{
    if (NULL != m_fp)
    {
        fclose(m_fp);

        m_fp = NULL;
    }

    return openfile(pstrFileName);
}



int total = 0;
int CFileStream::ReadData(u8 *pBuf, u32 u32ReadLen)
{
    int iReadLen = 0;

    if (NULL == m_fp)
        return -1;

    iReadLen = fread(pBuf, 1, u32ReadLen,  m_fp);

    // B: Dbg
    #if 1

    //printf("\n [LB Test] file pos:%ld, iReadLen:%d, u32ReadLen:%d \n", ftell(m_fp), iReadLen, u32ReadLen);

    if (iReadLen > 0)
    {
        total += iReadLen;
    }
    else
    {

        printf("\n [Func:%s] u32ReadLen:%d, iReadLen:%d, read total:%d, err:%d \n", __FUNCTION__, u32ReadLen, iReadLen, total, ferror(m_fp));
    }

    #endif

    // E: Dbg

    return iReadLen;
}


bool CFileStream::IsFileEnd()
{
    if (NULL == m_fp)
        return true;

    return !(0 == feof(m_fp));
}



int CFileStream::openfile(const char *pstrFileName)
{
    m_fp = fopen(pstrFileName, "rb");
    if (NULL == m_fp)
    {
        printf("\n Open File:%s Failed! \n", pstrFileName);

        return -1;
    }


    fseek(m_fp, 0L, SEEK_END);

    m_FileLen = ftell(m_fp);

    fseek(m_fp, 0L,SEEK_SET);

    printf("\n Open File:%s Success! filelen:%ld, pos:%ld \n",
            pstrFileName, m_FileLen, ftell(m_fp));

    return 0;
}


void CFileStream::ResetFile()
{
     fseek(m_fp, 0, SEEK_SET);
}


size_t CFileStream::GetFilePos()
{
    return ftell(m_fp);
}


