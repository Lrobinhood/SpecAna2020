/******************************************************************************************



    author: Robin
    date:   2021-09-01

******************************************************************************************/
//#include "stdafx.h"

#include <memory.h>
#include <stdio.h>

#include "../AnaBase/AnaBase.h"
#include "../Misc/DbgManager.h"

#include "PesFilter.h"



// Need Refactor
//u8 g_u8WaitGetDataFlag = 0;

CPesFilter::CPesFilter()
{
    m_pPesBuf       = NULL;
    m_u32PefBufSize = 0;
    m_u32RcvPesLen  = 0;
    m_u32PesCount   = 0;

    m_enRcvState    = EN_PES_RCV_FIND_START_CODE;
    m_u8StreamId    = 0;
}



CPesFilter::~CPesFilter()
{

}



void CPesFilter::ReceiveTSPack(u8 *pu8Data, size_t szStreamOffset)
{
    if (!IsEnable()) return;

    u8 u8Offset = 0;

    u8Offset += TS_PACKAGE_HEAD_LEN;

    if (0 == adaptation_field_control(pu8Data))
    {
        // 10 Adaptation_field only, no payload
        printf("\n [Func:%s, Line:%d] adaptation field is '0'. \n", __FUNCTION__, __LINE__);

        return;
    }

    if (2 == adaptation_field_control(pu8Data))
    {
        // 10 Adaptation_field only, no payload
        return;
    }


    if (3 == adaptation_field_control(pu8Data))
    {
        if (adaptation_field_length(pu8Data) >= M_TS_PACKAGE_PAYLOAD_LEN)
        {
            // ERR no payload
            printf("\n [Func:%s, Line:%d] \n", __FUNCTION__, __LINE__);

            return;
        }

        u8Offset += adaptation_field_length(pu8Data) + 1;
    }

    u8 *pu8TsPayLoadStart = pu8Data + u8Offset;

    if (EN_PES_RCV_FIND_START_CODE == m_enRcvState)
    {
        // Receive Section the first Pacakge
        if (0 == payload_unit_start_indicator(pu8Data))
        {
            printf("\n [Func:%s, Line:%d] Not payload start! \n", __FUNCTION__, __LINE__);
            return;
        }

        if (M_TS_PACKAGE_PAYLOAD_LEN - u8Offset < DRV_TSPU_FILT_PES_PATTERN_LENGTH)
        {
            // ERR no table payload
            printf("\n [Func:%s, Line:%d] ERR no table payload \n", __FUNCTION__, __LINE__);

            return;
        }

        #if 0
        for (int i = 0; i < DRV_TSPU_FILT_PES_PATTERN_LENGTH; i++)
        {
            if ((pu8SectStart[i] | m_unFilter_Cfg.stSectionParams.au8Mask[i]) != (m_unFilter_Cfg.stSectionParams.au8Match[i] | m_unFilter_Cfg.stSectionParams.au8Mask[i]))
            {
                // Not Match
                return ;
            }
        }
        #endif

        // Match

        // Check packet_start_code_prefix
        if (0x000001 == ((pu8TsPayLoadStart[0] << 16) + (pu8TsPayLoadStart[1] << 8) + pu8TsPayLoadStart[2]))
        {
            m_u8StreamId = pu8TsPayLoadStart[3];

            m_u32CurPesLen = ((pu8TsPayLoadStart[4] << 8) + pu8TsPayLoadStart[5]);

            printf("[%s, %d] Find packet_start_code_prefix, streamid:0x%x, PES Len:%u \n", __FUNCTION__, __LINE__, m_u8StreamId, m_u32CurPesLen);

            if (m_u32PefBufSize > m_u32CurPesLen)
            {
                ; // do nothing
            }
            else if (NULL != m_pPesBuf)
            {
                delete []m_pPesBuf;
                m_pPesBuf = NULL;
            }

            if (NULL == m_pPesBuf)
            {
                m_pPesBuf = new u8[m_u32CurPesLen];
                m_u32PefBufSize = m_u32CurPesLen;
            }

            // Save to Section Filter Buf
            u32 cplen = (m_u32CurPesLen - m_u32RcvPesLen) > (u32)(M_TS_PACKAGE_LEN - u8Offset) ? (M_TS_PACKAGE_LEN - u8Offset) : (m_u32CurPesLen - m_u32RcvPesLen);
            memcpy(m_pPesBuf + m_u32RcvPesLen, pu8TsPayLoadStart, cplen);
            m_u32RcvPesLen += cplen;

            m_szFirstPackOffset = szStreamOffset;
            m_enRcvState = EN_SECTION_RCV_PES_DATA;
        }
        else
        {
            if (m_isDebug)
            {
                printf("[%s, %d] not find packet_start_code_prefix \n", __FUNCTION__, __LINE__);
            }

            return;
        }
    }
    else if (EN_SECTION_RCV_PES_DATA == m_enRcvState)
    {
        // Save to Section Filter Buf
        u32 cplen = (m_u32CurPesLen - m_u32RcvPesLen) > (u32)(M_TS_PACKAGE_LEN - u8Offset) ? (M_TS_PACKAGE_LEN - u8Offset) : (m_u32CurPesLen - m_u32RcvPesLen);
        memcpy(m_pPesBuf + m_u32RcvPesLen, pu8TsPayLoadStart, cplen);
        m_u32RcvPesLen += cplen;
    }



    //m_u32SectionPackageCount++;


    if (m_u32RcvPesLen >= m_u32CurPesLen)
    {
        m_u32PesCount++;

        //m_u8SectionBufFlag = 1;



        // Process Data

        // Temp code 2020.10.15
        u8 buf[1<<16];
        u32 datalen = 0;


        datalen = GetPesData(buf, 1<<16);

        #if 0
        if (NULL != m_funcFilterCB)
        {
            datalen = GetSectionData(buf, M_SECTION_BUF_LEN);

            if (NULL != m_funcSendDmxMsg)
            {
                m_funcSendDmxMsg(0, datalen, m_SecFltId, buf, m_funcFilterCB);
            }
        }
        else if (NULL != m_funcParserEx)
        {
            datalen = GetSectionData(buf, M_SECTION_BUF_LEN);
            m_funcParserEx(buf, datalen, m_szFirstPackOffset);
        }
        else if (NULL != m_funcParser)
        {
            datalen = GetSectionData(buf, M_SECTION_BUF_LEN);

            m_funcParser(buf, datalen);

            //DisableFilter();
        }
        else
        {
            datalen = GetSectionData(buf, M_SECTION_BUF_LEN);
        }
        #endif

        if (NULL != m_funcParser)
        {
            m_funcParser(buf, datalen);

            DisableFilter();
        }

        /*
        if (NULL != m_pfnDataNotify)
        {
            #if 0
            m_pfnDataNotify(m_u32ConnectedPidFilterHandle);
            #else
            TSPU_DATA_MSG_STRU stMsg;

            stMsg.u32CallBackHandle  = (u32)m_pfnDataNotify;
            stMsg.u32PIDFilterHandle = (u32)m_u32ConnectedPidFilterHandle;

            stMsg.u32DataLen    = 0;
            stMsg.pu8Data       = NULL;

            #endif

        }
        */

    }

#if 0

    // B: Dbg
    if (m_isDebug)
    {
        printf("\nFilter TableID:0x%x Package:%d, Data: \n", m_SectionBuf[0], m_u32SectionPackageCount);

        dbg_show_mem(pu8Data, M_TS_PACKAGE_LEN);
    }

    // E: Dbg
#endif

}

#if 0
u32 CSectionFilter::PeekSectionDataLen()
{
    u32 u32DataLen = 0;

    u32DataLen  = (u32)((m_SectionBuf[1] & 0x0F) << 8)  + (u32)(m_SectionBuf[2]) + 3;

    return u32DataLen;
}
#endif

u32  CPesFilter::GetPesData(u8 *pBuf, u32 u32BufLen)
{
    u32 u32DataLen = 0;

#if 0
    if (0 == m_u8SectionBufFlag)
        return 0;
#endif

    // B: DBG
#if 0
    memset(pBuf, 0, u32BufLen);
#endif
    // E: DBG

    u32DataLen  = (u32)(m_pPesBuf[4] << 8)  + (u32)(m_pPesBuf[5]);

    if (u32BufLen < u32DataLen)
    {
        printf("\n [Func:%s, Line:%d] ERR! u32BufLen:%d, u32DataLen:%d \n", __FUNCTION__, __LINE__, u32BufLen, u32DataLen);

        return 0;
    }

    memcpy(pBuf, m_pPesBuf, u32DataLen);

#if 0
    m_u8SectionBufFlag  = 0;
#endif

#if 0
    if (1 == m_u8HasAcrossData)
    {
        // LB Notes: 2020-11-26, need refactor, here offset may refer to "pointer_field"
        memcpy(m_SectionBuf, m_SectionBuf + u32DataLen, m_u32RcvSectionLen - u32DataLen);

        m_u32RcvSectionLen  = m_u32RcvSectionLen - u32DataLen;
        m_u32CurSectionLen  = (u32)((m_SectionBuf[1] & 0x0F)<<8)  + (u32)(m_SectionBuf[2]) + 3;
        m_enRcvState        = EN_SECTION_RCV_OTHER_PACK;
        m_u8HasAcrossData   = 0;
    }
    else
#endif
    {
        m_u32RcvPesLen  = 0;
        m_u32CurPesLen  = 0;

        m_enRcvState    = EN_PES_RCV_FIND_START_CODE;
    }


    // B: Dbg
    if (m_isDebug)
    {
        printf("\n Filter PES stream id:0x%x, Len:%u, PesCount:%d Data: \n", pBuf[3], u32DataLen, m_u32PesCount);

        dbg_show_mem(pBuf, u32DataLen);
    }

    // E: Dbg

    //g_u8WaitGetDataFlag = 0;    // Need Refactor

    return u32DataLen;
}


#if 0
void CSectionFilter::ConfigFilter(UN_DRV_TSPU_FILTER_CFG *punFilterCfg)
{
    memcpy(&m_unFilter_Cfg, punFilterCfg, sizeof(UN_DRV_TSPU_FILTER_CFG));

    //printf("\n [LB Test] ConfigFilter Filter:0x%x SerId;0x%x%x \n", m_unFilter_Cfg.stSectionParams.au8Match[0], m_unFilter_Cfg.stSectionParams.au8Match[3],  m_unFilter_Cfg.stSectionParams.au8Match[4]);
}


void CSectionFilter::SetFilterTableId(u8 u8TableID, u8 u8TidMask, u16 u16TsId, u16 u16TsIdMask)
{
    ST_DRV_TSPU_FILT_SECTION_PARAMS stSectionFilterParas;


    memset(&stSectionFilterParas, 0xff, sizeof(stSectionFilterParas));

    stSectionFilterParas.au8Match[0]    = u8TableID;
    stSectionFilterParas.au8Mask[0]     = u8TidMask;

    stSectionFilterParas.au8Match[3]    = (u16TsId & 0xff00) >> 8;
    stSectionFilterParas.au8Mask[3]     =  (u16TsIdMask & 0xff00) >> 8;

    stSectionFilterParas.au8Match[4]    = (u8)(u16TsId & 0xff);
    stSectionFilterParas.au8Mask[4]     = (u8)(u16TsIdMask & 0xff);

    ConfigFilter((UN_DRV_TSPU_FILTER_CFG *)&stSectionFilterParas);
}
#endif

