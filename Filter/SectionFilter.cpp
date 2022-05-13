/******************************************************************************************



    author: Robin
    date:   2013-10-25

******************************************************************************************/
//#include "stdafx.h"

#include <memory.h>
#include <stdio.h>

#include "../AnaBase/AnaBase.h"
#include "../Misc/DbgManager.h"

#include "SectionFilter.h"



// Need Refactor
u8 g_u8WaitGetDataFlag = 0;

CSectionFilter::CSectionFilter()
{
    m_u8SectionBufFlag = 0;
    m_u8HasAcrossData  = 0;

    memset(m_SectionBuf, 0, M_SECTION_BUF_LEN);

    m_u32SectionPackageCount    = 0;
    m_u32SectionCount           = 0;

    m_szFirstPackOffset = 0;
    m_enRcvState        = EN_SECTION_RCV_FIRST_PACK;
    m_u32CurSectionLen  = 0;
    m_u32RcvSectionLen  = 0;

    m_funcParser    = NULL;
    m_funcParserEx  = NULL;
    m_funcFilterCB  = NULL;

    m_pFatherFlt    = NULL;

}



CSectionFilter::~CSectionFilter()
{

}



void CSectionFilter::ReceiveTSPack(u8 *pu8Data, size_t szStreamOffset)
{
    if (!IsEnable()) return;

    u8 u8Offset = 0;
    u8 u8PointField = 0;

    u8Offset += TS_PACKAGE_HEAD_LEN;


    if (2 == adaptation_field_control(pu8Data))
    {
        // 10 Adaptation_field only, no payload
        return;
    }


    if (3 == adaptation_field_control(pu8Data))
    {
        printf("[%s, %d] adaptation_field_length:%u \n", __FUNCTION__, __LINE__, adaptation_field_length(pu8Data));

        if (adaptation_field_length(pu8Data) >= M_TS_PACKAGE_PAYLOAD_LEN)
        {
            // ERR no payload
            printf("\n [Func:%s, Line:%d] \n", __FUNCTION__, __LINE__);

            return;
        }

        u8Offset += adaptation_field_length(pu8Data) + 1;
    }

    if (1 == payload_unit_start_indicator(pu8Data) && EN_SECTION_RCV_FIRST_PACK == m_enRcvState)
    {
        u8PointField = pu8Data[u8Offset];
        if (m_isDebug)
        {
            printf("[%s, %d] point_field:%u \n", __FUNCTION__, __LINE__, u8PointField);
        }

        u8Offset = u8Offset + 1 + u8PointField; // Why ??? --- A: Skip the point_field domain
    }
    else if (1 == payload_unit_start_indicator(pu8Data) && EN_SECTION_RCV_OTHER_PACK == m_enRcvState)
    {
        // NOTE: The Current Package include the end date of the currenct section, and the start data of the next section
        u8PointField = pu8Data[u8Offset];
        if (m_isDebug)
        {
            printf("[%s, %d] point_field:%u \n", __FUNCTION__, __LINE__, u8PointField);
        }

        u8Offset = u8Offset + 1; // Why ???

        m_u8HasAcrossData = 1;

        printf("[Func:%s, Line:%d] payload_unit_start_indicator is 1, m_enRcvState:%d \n", __FUNCTION__, __LINE__, m_enRcvState);

    }
    else if (EN_SECTION_RCV_OTHER_PACK == m_enRcvState)
    {
        ;
    }
    else
    {
        if (m_isDebug)
        {
            printf("[%s, %d] ERROR - payload_unit_start_indicator is 0, m_enRcvStarte:%s \n", __FUNCTION__, __LINE__,
                    EN_SECTION_RCV_FIRST_PACK == m_enRcvState ? "EN_SECTION_RCV_FIRST_PACK" : "EN_SECTION_RCV_OTHER_PACK");
        }

        return;
    }


    u8 *pu8SectStart = pu8Data + u8Offset;

    if (EN_SECTION_RCV_FIRST_PACK == m_enRcvState)
    {
        // Receive Section the first Pacakge

        if (M_TS_PACKAGE_PAYLOAD_LEN - u8Offset <= DRV_TSPU_FILT_SECTION_PATTERN_LENGTH)
        {
            // ERR no table payload
            printf("\n [Func:%s, Line:%d] ERR no table payload \n", __FUNCTION__, __LINE__);

            return;
        }

        for (int i = 0; i < DRV_TSPU_FILT_SECTION_PATTERN_LENGTH; i++)
        {
            if ((pu8SectStart[i] | m_unFilter_Cfg.stSectionParams.au8Mask[i]) != (m_unFilter_Cfg.stSectionParams.au8Match[i] | m_unFilter_Cfg.stSectionParams.au8Mask[i]))
            {
                // Not Match
                if (m_isDebug)
                {
                    printf("\n [Func:%s, Line:%d] Not Match \n", __FUNCTION__, __LINE__);
                }

                return ;
            }
        }

        // Match
        m_u32CurSectionLen = ((pu8SectStart[1] & 0x0f) << 8) + pu8SectStart[2] + 3; // Table  Head Section 3Byte Head

        // Save to Section Filter Buf
        m_u32RcvSectionLen = 0;
        memcpy(m_SectionBuf, pu8SectStart, M_TS_PACKAGE_LEN - u8Offset);
        m_u32RcvSectionLen += M_TS_PACKAGE_LEN - u8Offset;

        if (m_isDebug)
        {
            printf("\n [Func:%s, Line:%d] new Rcv Data Len:%u, total rcvlen:%u, Cur Sec Len:%u \n", __FUNCTION__, __LINE__, M_TS_PACKAGE_LEN - u8Offset, m_u32RcvSectionLen, m_u32CurSectionLen);
        }

        m_szFirstPackOffset = szStreamOffset;
        m_enRcvState = EN_SECTION_RCV_OTHER_PACK;
    }
    else if (EN_SECTION_RCV_OTHER_PACK == m_enRcvState)
    {
        // Save to Section Filter Buf
        memcpy(m_SectionBuf + m_u32RcvSectionLen, pu8SectStart, M_TS_PACKAGE_LEN - u8Offset);
        m_u32RcvSectionLen += M_TS_PACKAGE_LEN - u8Offset;

        if (m_isDebug)
        {
            printf("\n [Func:%s, Line:%d] new Rcv Data Len:%u, total rcvlen:%u \n", __FUNCTION__, __LINE__, M_TS_PACKAGE_LEN - u8Offset, m_u32RcvSectionLen);
        }

        if (1 == m_u8HasAcrossData)
        {
            printf("[%s:%d] m_u32RcvSectionLen:%u, m_u32CurSectionLen:%u \n", __FUNCTION__, __LINE__, m_u32RcvSectionLen, m_u32CurSectionLen);

            if (m_u32RcvSectionLen < m_u32CurSectionLen)
            {
                printf("[%s:%d] ERROR! \n", __FUNCTION__, __LINE__);

                pu8SectStart += u8PointField;
                memcpy(m_SectionBuf, pu8SectStart + u8PointField, M_TS_PACKAGE_LEN - u8Offset - u8PointField);

                if (m_isDebug)
                {
                    printf("\n [Func:%s, Line:%d] new Rcv Data Len:%u, total rcvlen:%u \n", __FUNCTION__, __LINE__, M_TS_PACKAGE_LEN - u8Offset - u8PointField, m_u32RcvSectionLen);
                }

                for (int i = 0; i < DRV_TSPU_FILT_SECTION_PATTERN_LENGTH; i++)
                {
                    if ((pu8SectStart[i] | m_unFilter_Cfg.stSectionParams.au8Mask[i]) != (m_unFilter_Cfg.stSectionParams.au8Match[i] | m_unFilter_Cfg.stSectionParams.au8Mask[i]))
                    {
                        m_enRcvState        = EN_SECTION_RCV_FIRST_PACK;
                        m_u8HasAcrossData   = 0;

                        // Not Match
                        return ;
                    }
                }

                m_u32RcvSectionLen  = M_TS_PACKAGE_LEN - u8Offset - u8PointField;
                m_u32CurSectionLen  = (u32)((m_SectionBuf[1] & 0x0F)<<8)  + (u32)(m_SectionBuf[2]) + 3;
                m_enRcvState        = EN_SECTION_RCV_OTHER_PACK;
                m_u8HasAcrossData   = 0;
            }
        }
    }



    m_u32SectionPackageCount++;


    if (m_u32RcvSectionLen >= m_u32CurSectionLen)
    {
        m_u32SectionCount++;

        m_u8SectionBufFlag = 1;



        // Process Data

        // Temp code 2020.10.15
        u8 buf[M_SECTION_BUF_LEN];
        u32 datalen = 0;

        //datalen = GetSectionData(buf, M_SECTION_BUF_LEN);
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

        u32 crc32 = 0xffffffff;
        crc32 = xcrc32(buf, datalen, 0xffffffff);
        if (0 == crc32)
        {
            printf("crc32 is OK!\n");
        }
        else
        {
            printf("crc32 is ERROR:0x%x \n", crc32);
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


u32 CSectionFilter::PeekSectionDataLen()
{
    u32 u32DataLen = 0;

    u32DataLen  = (u32)((m_SectionBuf[1] & 0x0F) << 8)  + (u32)(m_SectionBuf[2]) + 3;

    return u32DataLen;
}



u32  CSectionFilter::GetSectionData(u8 *pBuf, u32 u32BufLen)
{
    u32 u32DataLen = 0;

    if (0 == m_u8SectionBufFlag)
        return 0;

    // B: DBG
#if 0
    memset(pBuf, 0, u32BufLen);
#endif
    // E: DBG

    u32DataLen  = (u32)((m_SectionBuf[1] & 0x0F)<<8)  + (u32)(m_SectionBuf[2]) + 3;

    if (u32BufLen < u32DataLen)
    {
        printf("\n [Func:%s, Line:%d] ERR! u32BufLen:%d, u32DataLen:%d \n", __FUNCTION__, __LINE__, u32BufLen, u32DataLen);

        return 0;
    }

    memcpy(pBuf, m_SectionBuf, u32DataLen);

    m_u8SectionBufFlag  = 0;

    if (1 == m_u8HasAcrossData)
    {
        // LB Notes: 2020-11-26, need refactor, here offset may refer to "pointer_field"
        memcpy(m_SectionBuf, m_SectionBuf + u32DataLen, m_u32RcvSectionLen - u32DataLen);

        m_u32RcvSectionLen  = m_u32RcvSectionLen - u32DataLen;
        m_u32CurSectionLen  = (u32)((m_SectionBuf[1] & 0x0F)<<8)  + (u32)(m_SectionBuf[2]) + 3;
        m_enRcvState        = EN_SECTION_RCV_OTHER_PACK;
        m_u8HasAcrossData   = 0;

        if (m_isDebug)
        {
            printf("\n [Func:%s, Line:%d] Begin new Section - Rcv Data Len:%u, Cur Sec len:%u \n", __FUNCTION__, __LINE__, m_u32RcvSectionLen, m_u32CurSectionLen);
        }

        for (int i = 0; i < DRV_TSPU_FILT_SECTION_PATTERN_LENGTH; i++)
        {
            if ((m_SectionBuf[i] | m_unFilter_Cfg.stSectionParams.au8Mask[i]) != (m_unFilter_Cfg.stSectionParams.au8Match[i] | m_unFilter_Cfg.stSectionParams.au8Mask[i]))
            {
                m_enRcvState        = EN_SECTION_RCV_FIRST_PACK;
                m_u32RcvSectionLen  = 0;
                m_u32CurSectionLen  = 0;

                // Not Match
                if (m_isDebug)
                {
                    printf("\n [Func:%s, Line:%d] Not Match \n", __FUNCTION__, __LINE__);
                }
            }
        }
    }
    else
    {
        m_u32RcvSectionLen  = 0;
        m_u32CurSectionLen  = 0;

        m_enRcvState        = EN_SECTION_RCV_FIRST_PACK;
    }


    // B: Dbg
    if (m_isDebug)
    {
        printf("\nFilter TableID:0x%x, SectionCount:%d, ver:%u, sec_num:%d, last_sec_num:%d Data: \n", pBuf[0], m_u32SectionCount,
               (pBuf[5] >> 1) & 0x1F, pBuf[6], pBuf[7]);
        dbg_show_mem(pBuf, u32DataLen);
    }

    // E: Dbg

    g_u8WaitGetDataFlag = 0;    // Need Refactor

    return u32DataLen;
}


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


