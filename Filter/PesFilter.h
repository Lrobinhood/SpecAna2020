/******************************************************************************************



	author:	Robin
	date:	2021-09-01

******************************************************************************************/

#ifndef __PESFILTER_H__
#define __PESFILTER_H__

#include "FilterBase.h"
#include "FilterParams.h"


typedef enum tagPESRcvState
{
    EN_PES_RCV_FIND_START_CODE = 0,
    EN_SECTION_RCV_PES_DATA
}EN_PES_RCV_STATE;



typedef void (*PARSER_FUNC)(u8 *pBuf, u32 u32DataLen);

/*
typedef void (*PARSER_FUNC_EX)(u8 *pBuf, u32 u32DataLen, size_t szFirstPackOffset);

typedef void (*FILTER_CALLBACK)(u8 path, u16 bytes, u16 pfilt_id);

typedef void (*SEND_DMX_MSG)(u8 path, u16 bytes, u16 pfilt_id, u8 *buf, FILTER_CALLBACK cb_func);
*/

class CPesFilter:public CFilterBase{

public:
    CPesFilter();
    ~CPesFilter();

    void ReceiveTSPack(u8 *pu8Data, size_t szStreamOffset = 0);

    void ConfigFilter(UN_DRV_TSPU_FILTER_CFG *punFilterCfg);

    u32 GetPesData(u8 *pBuf, u32 u32BufLen);

    void SetParserFunc(PARSER_FUNC pfuncParser) { m_funcParser = pfuncParser; }
    void ResetParserFunc() { m_funcParser= NULL; }

/*
    void SetFilterTableId(u8 u8TableID, u8 u8TidMask = 0x00, u16 u16TsId = 0xffff, u16 u16TsIdMask = 0xffff);


    u32 PeekSectionDataLen();

   


    void SetParserFuncEx(PARSER_FUNC_EX pfuncParserEx) { m_funcParserEx = pfuncParserEx; }
    void ResetParserFuncEx() { m_funcParserEx= NULL; }

    void SetFilterCBFunc(FILTER_CALLBACK pfuncCB) { m_funcFilterCB = pfuncCB; }
    void ResetFilterCBFunc() { m_funcFilterCB = NULL; }

    void SetSendDmxMsgFunc(SEND_DMX_MSG func) { m_funcSendDmxMsg = func; }

    void SetSecFltId(u16 FltId) { m_SecFltId = FltId; }
    u16 GetSecFltId() { return m_SecFltId; }

    void SetFatherFlt(CFilterBase *pFatherFlt) { m_pFatherFlt = pFatherFlt; }
    CFilterBase *GetFatherFlt() { return m_pFatherFlt; }
*/

protected:
    u8 *m_pPesBuf;
    u32 m_u32PefBufSize;
    u8 m_u8PesBufFlag;      // 0: no data 1: data
    u8 m_u8HasAcrossData;   // 0: no across data 1: has across data



private:
    EN_PES_RCV_STATE m_enRcvState;

    u32 m_u32CurPesLen;
    u32 m_u32RcvPesLen;

    u8 m_u8StreamId;
    UN_DRV_TSPU_FILTER_CFG m_unFilter_Cfg;

    // Stastic
    u32 m_u32PesCount;
    PARSER_FUNC m_funcParser;

    /*
    
    PARSER_FUNC_EX m_funcParserEx;
    FILTER_CALLBACK m_funcFilterCB;
    SEND_DMX_MSG m_funcSendDmxMsg;

    u16 m_SecFltId;

    CFilterBase *m_pFatherFlt;
*/
    size_t m_szFirstPackOffset;
};


#endif
