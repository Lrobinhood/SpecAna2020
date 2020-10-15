/******************************************************************************************



	Author:	Robin
	Date:	2020-9-24

******************************************************************************************/

#ifndef __ROUND_BUF_H__
#define __ROUND_BUF_H__


typedef enum tag_en_Drag_Data_Result{

	EN_SUCCESS = 0,
	EN_GET_DATA_FAIL,
	EN_GET_DATA_END,
	EN_DATA_SOURCE_NOT_EXIST,
	EN_DATA_BUF_FULL
}EN_DRAG_DATA_RESULT;

class CRoundBuf {

public:
	CRoundBuf();
	CRoundBuf(u32 u32BufSize);

	~CRoundBuf();

	int ResetBufSize(u32 u32BufSize = 1024);

	void ConnectFileSource(CFileStream *pFileSource);

    long GetConnectFileLen();

	
	// 从 round buffer 的u32Offset偏移(相对round buffer的headpos)处，peek一个字节数据供观察
	u8 PeekOneByte(u32 u32Offset);
	
	// 从 round buffer 的u32Offset偏移(相对round buffer的headpos)处，peek一段数据供观察
	void PeekData(u8 *pBuf, u32 u32PeekLen, u32 u32Offset);

	
	// 从 round buffer 中取出 u32Package
	u32 GetData(u8 *pBuf, u32 u32PackageLen);

	u32 DiscardData(u32 u32Len);

	u32 GetAvaDataLen() { return m_u32AvaData; }
	u32 GetBufSize() { return m_u32BufSize; }

	// Buffer HeadPos数据对应关联文件的偏移
	size_t GetFileOffset() { return m_ConnectedFileOffset; }

	
protected:
	// 重置 round buffer size
	int resetbuf(u32 u32BufSize);
	
	// 从关联的File中获取数据，填满 round buffer
	EN_DRAG_DATA_RESULT dragdata();


private:
	u32 m_u32BufSize;
	u32 m_u32HeadPos;
	u32 m_u32TailPos;
	u32 m_u32AvaData;

	size_t m_ConnectedFileOffset;
	
	u8 *m_pBuf;

	CFileStream *m_pFileSource;

};

#endif // #ifndef __ROUND_BUF_H__