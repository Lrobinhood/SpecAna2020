/******************************************************************************************



	author:	Robin
	date:	2020-10-12

******************************************************************************************/

#ifndef __RECORD_FILE_H__
#define __RECORD_FILE_H__

#include <stdio.h>

class CRecordFile{
public:
	CRecordFile(const char *pstrFileName);

   	~CRecordFile();

	void RecordOneLog(const char *psLog);



protected:
	int openfile(const char *pstrFileName);

private:
	FILE *m_fp;
};


#endif


