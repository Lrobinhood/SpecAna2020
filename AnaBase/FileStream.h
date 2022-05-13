/******************************************************************************************



	author:	Robin
	date:	2013-9-27

******************************************************************************************/

#ifndef __FILE_STREAM_H__
#define __FILE_STREAM_H__

class CFileStream{
public:
	CFileStream();
	CFileStream(const char *pstrFileName);

   	~CFileStream();

	int OpenFile(const char *pstrFileName);

	int ReadData(u8 *pBuf, u32 u32ReadLen);

	bool IsFileEnd();

	void ResetFile(); // file point return to the Beginning

	size_t GetFilePos();

    long GetFileLen() {return m_FileLen;}

protected:
	int openfile(const char *pstrFileName);

private:
	FILE *m_fp;

    long m_FileLen;
};


#endif