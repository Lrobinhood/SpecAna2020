/******************************************************************************************



	Author:	Robin
	Date:	2020-10-10

******************************************************************************************/
//#include "stdafx.h"


#include "../AnaBase/AnaBase.h"

#include "TsPackageFilter.h"



void CTsPackageFilter::RegisterNotify(u32 Key,  CFilterBase *pFilter)
{
    m_mapNotify.insert(pair<u32, CFilterBase *>(Key, pFilter));
}



void CTsPackageFilter::UnRegisterNotify(u32 Key)
{
    m_mapNotify.erase(Key);
}

CFilterBase* CTsPackageFilter::FindFilter(u32 key)
{
    CFilterBase *pFilter = NULL;

    map <u32, CFilterBase *>::iterator iter;

    iter = m_mapNotify.find(key);

    if (iter != m_mapNotify.end())
    {
        pFilter = iter->second;
    }

    return pFilter;
}



u32 CTsPackageFilter::GetTsPackage(u8 *pBuf, u32 u32PackageLen)
{
    u32 ret = 0;

    if (EN_TSPACKAGE_FILTER_STATE_NOT_SYNC == m_State)
    {
        if (syncpackage())
        {
            m_State = EN_TSPACKAGE_FILTER_STATE_SYNC;
        }
        else
        {
            ret = 0;
        }
    }

    if (EN_TSPACKAGE_FILTER_STATE_SYNC == m_State)
    {
        ret = m_pRoundBuf->GetData(pBuf, u32PackageLen);

        if (M_TS_PACKAGE_SYNC != m_pRoundBuf->PeekOneByte(0))
        {

            printf("\n [LB Test]  Lost TS Sync at Offset: %d ! \n", m_pRoundBuf->GetFileOffset());

            m_State = EN_TSPACKAGE_FILTER_STATE_NOT_SYNC;
        }

    }

    return ret;
}



u32 CTsPackageFilter::GetTsPackageEx(u8 *pBuf, size_t &szStreamOffset, u32 u32PackageLen)
{
    u32 ret = 0;

    if (EN_TSPACKAGE_FILTER_STATE_NOT_SYNC == m_State)
    {
        if (syncpackage())
        {
            m_State = EN_TSPACKAGE_FILTER_STATE_SYNC;
        }
        else
        {
            ret = 0;
        }
    }

    if (EN_TSPACKAGE_FILTER_STATE_SYNC == m_State)
    {
        szStreamOffset  = m_pRoundBuf->GetFileOffset();
        ret             = m_pRoundBuf->GetData(pBuf, u32PackageLen);

        // Send the TS Package to downstream filters
        notify(pBuf, szStreamOffset);

        if ((((double)szStreamOffset / (double)m_pRoundBuf->GetConnectFileLen()) * 100) > (m_Progress + 5))
        {
            m_Progress = (u32)(((double)szStreamOffset / (double)m_pRoundBuf->GetConnectFileLen()) * 100);

            printf("\n [LB Test] Process File %4f \n", m_Progress);
        }

        if (M_TS_PACKAGE_SYNC != m_pRoundBuf->PeekOneByte(0))
        {

            printf("\n [LB Test]  Lost TS Sync at Offset: %u ! \n", m_pRoundBuf->GetFileOffset());

            m_State = EN_TSPACKAGE_FILTER_STATE_NOT_SYNC;
        }

    }

    return ret;
}

bool CTsPackageFilter::syncpackage()
{
    u8 temp;
    u32 i;

    do
    {
        for (i = 0; i < m_pRoundBuf->GetAvaDataLen(); i++)
        {
            temp = m_pRoundBuf->PeekOneByte(i);

            if (M_TS_PACKAGE_SYNC == temp)
            {
                // Discard the previous data
                m_pRoundBuf->DiscardData(i);

                if (M_TS_PACKAGE_SYNC == m_pRoundBuf->PeekOneByte(0) &&
                        M_TS_PACKAGE_SYNC == m_pRoundBuf->PeekOneByte(M_TS_PACKAGE_LEN) &&
                        M_TS_PACKAGE_SYNC == m_pRoundBuf->PeekOneByte(M_TS_PACKAGE_LEN * 2))
                {
                    return true;
                }
                else
                {
                    m_pRoundBuf->DiscardData(1);

                    break;
                }
            }
        }

        if (i == m_pRoundBuf->GetAvaDataLen())
            m_pRoundBuf->DiscardData(m_pRoundBuf->GetAvaDataLen());

    }
    while(1);

    return false;
}



void CTsPackageFilter::notify(u8 *pBuf, size_t szOffset)
{
    map<u32, CFilterBase *>::iterator iter;

    for(iter = m_mapNotify.begin(); iter != m_mapNotify.end(); iter++)
        iter->second->ReceiveTSPack(pBuf, szOffset);
}
