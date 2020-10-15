/******************************************************************************************



	author:	Robin
	date:	2020-10-12

******************************************************************************************/

#include "stdafx.h"

#include "RecordFile.h"

CRecordFile::CRecordFile(char *pstrFileName)
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



void CRecordFile::RecordOneLog(char *psLog)
{
	fprintf(m_fp, "%s \n", psLog);
}



int CRecordFile::openfile(char *pstrFileName)
{
	fopen_s(&m_fp, pstrFileName, "w");
	if (NULL == m_fp)
	{
		printf("\n Open File:%s Failed! \n", pstrFileName);

		return -1;
	}

	printf("\n Open File:%s Success! \n", pstrFileName);

	return 0;
}





