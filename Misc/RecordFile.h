/******************************************************************************************



	author:	Robin
	date:	2020-10-12

******************************************************************************************/

#ifndef __RECORD_FILE_H__
#define __RECORD_FILE_H__

class CRecordFile{
public:
	CRecordFile(char *pstrFileName);

   	~CRecordFile();

	void RecordOneLog(char *psLog);



protected:
	int openfile(char *pstrFileName);

private:
	FILE *m_fp;
};


#endif


