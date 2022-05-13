/******************************************************************************************



	author:	Robin
	date:	2020-10-12

******************************************************************************************/

//#include "stdafx.h"
#include <stdio.h>

#include "RecordFile.h"

CRecordFile::CRecordFile(const char *pstrFileName)
{
    int ret = 0;

    m_fp = NULL;

    ret = openfile(pstrFileName);
}



CRecordFile::~CRecordFile()
{
    if (NULL != m_fp)
        fclose(m_fp);
}



void CRecordFile::RecordOneLog(const char *psLog)
{
    fprintf(m_fp, "%s \n", psLog);
}



int CRecordFile::openfile(const char *pstrFileName)
{
    m_fp = fopen(pstrFileName, "w");

    if (NULL == m_fp)
    {
        printf("\n Open File:%s Failed! \n", pstrFileName);

        return -1;
    }

    printf("\n Open File:%s Success! \n", pstrFileName);

    return 0;
}





