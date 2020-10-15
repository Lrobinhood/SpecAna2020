/******************************************************************************************



	author:	Robin
	date:	2020-10-15

******************************************************************************************/

#ifndef __SECTIONFILTER_H__
#define __SECTIONFILTER_H__

#include "FilterBase.h"
#include "FilterParams.h"

#define M_SECTION_BUF_LEN   (1024*4)

typedef enum tagSectionRCvState
{
    EN_SECTION_RCV_FIRST_PACK = 0,
    EN_SECTION_RCV_OTHER_PACK        
}EN_SECTION_RCV_STATE;

typedef void (*PARSER_FUNC)(u8 *pBuf, u32 u32DataLen);

class CSectionFilter:public CFilterBase{

public:
    CSectionFilter();
    ~CSectionFilter();
        
    void ReceiveTSPack(u8 *pu8Data, size_t szStreamOffset = 0);

    void ConfigFilter(UN_DRV_TSPU_FILTER_CFG *punFilterCfg);

    void SetFilterTableId(u8 u8TableID);

    u32 GetSectionData(u8 *pBuf, u32 u32BufLen);

	void SetParserFunc(PARSER_FUNC pfuncParser) { m_funcParser = pfuncParser; }
	void ResetParserFunc() { m_funcParser= NULL; }
    
        
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
};


#endif
