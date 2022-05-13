/******************************************************************************************



	author:	Robin
	date:	2020-10-15

******************************************************************************************/

#ifndef __SECTIONFILTER_H__
#define __SECTIONFILTER_H__

#include "FilterBase.h"
#include "FilterParams.h"

#define M_SECTION_BUF_LEN   (1024*4 + 1024)     // max section length + 1024, maybe has m_u8HasAcrossData or stuffing data

typedef enum tagSectionRCvState
{
    EN_SECTION_RCV_FIRST_PACK = 0,
    EN_SECTION_RCV_OTHER_PACK
}EN_SECTION_RCV_STATE;

typedef void (*PARSER_FUNC)(u8 *pBuf, u32 u32DataLen);
typedef void (*PARSER_FUNC_EX)(u8 *pBuf, u32 u32DataLen, size_t szFirstPackOffset);

typedef void (*FILTER_CALLBACK)(u8 path, u16 bytes, u16 pfilt_id);

typedef void (*SEND_DMX_MSG)(u8 path, u16 bytes, u16 pfilt_id, u8 *buf, FILTER_CALLBACK cb_func);


class CSectionFilter:public CFilterBase{

public:
    CSectionFilter();
    ~CSectionFilter();

    void ReceiveTSPack(u8 *pu8Data, size_t szStreamOffset = 0);

    void ConfigFilter(UN_DRV_TSPU_FILTER_CFG *punFilterCfg);

    void SetFilterTableId(u8 u8TableID, u8 u8TidMask = 0x00, u16 u16TsId = 0xffff, u16 u16TsIdMask = 0xffff);

    u32 GetSectionData(u8 *pBuf, u32 u32BufLen);
    u32 PeekSectionDataLen();

    void SetParserFunc(PARSER_FUNC pfuncParser) { m_funcParser = pfuncParser; }
    void ResetParserFunc() { m_funcParser= NULL; }


    void SetParserFuncEx(PARSER_FUNC_EX pfuncParserEx) { m_funcParserEx = pfuncParserEx; }
    void ResetParserFuncEx() { m_funcParserEx= NULL; }

    void SetFilterCBFunc(FILTER_CALLBACK pfuncCB) { m_funcFilterCB = pfuncCB; }
    void ResetFilterCBFunc() { m_funcFilterCB = NULL; }

    void SetSendDmxMsgFunc(SEND_DMX_MSG func) { m_funcSendDmxMsg = func; }

    void SetSecFltId(u16 FltId) { m_SecFltId = FltId; }
    u16 GetSecFltId() { return m_SecFltId; }

    void SetFatherFlt(CFilterBase *pFatherFlt) { m_pFatherFlt = pFatherFlt; }
    CFilterBase *GetFatherFlt() { return m_pFatherFlt; }

protected:
	u8 m_SectionBuf[M_SECTION_BUF_LEN];
    u8 m_u8SectionBufFlag;      // 0: no data 1: data
    u8 m_u8HasAcrossData;       // 0: no across data 1: has across data



private:
    u32 m_u32SectionPackageCount;
    u32 m_u32SectionCount;

    EN_SECTION_RCV_STATE m_enRcvState;
    u32 m_u32CurSectionLen;
    u32 m_u32RcvSectionLen;

    UN_DRV_TSPU_FILTER_CFG m_unFilter_Cfg;

    PARSER_FUNC m_funcParser;
    PARSER_FUNC_EX m_funcParserEx;
    FILTER_CALLBACK m_funcFilterCB;
    SEND_DMX_MSG m_funcSendDmxMsg;

    u16 m_SecFltId;

    CFilterBase *m_pFatherFlt;

    size_t m_szFirstPackOffset;
};


#endif
