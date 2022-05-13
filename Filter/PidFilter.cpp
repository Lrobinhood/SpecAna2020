/******************************************************************************************



    author: Robin
    date:   2020-10-12

******************************************************************************************/
//#include "stdafx.h"

#include <stdio.h>
#include <map>

#include "../AnaBase/AnaBase.h"
#include "../Misc/RecordFile.h"
#include "PidFilter.h"





CPidFilter::CPidFilter()
{
    m_pvPIDHandle = static_cast<void *>(this);
    //m_u64PIDHandle = reinterpret_cast<u64>(this);
    m_u16PID       = 0xFFFF;

    m_u32PidPackageCount = 0;

}



CPidFilter::~CPidFilter()
{
    if (NULL != m_fpRecord)
    {
        delete m_fpRecord;
    }   
}



void CPidFilter::SetPid(u16 u16PID)
{
    if (m_u16PID != u16PID)
    {
        m_u16PID = u16PID;
    
        char record_name[32];

        if (NULL != m_fpRecord)
        {
            delete m_fpRecord;
            m_fpRecord = NULL;
        }

        if (m_isRecord)
        {
            snprintf(record_name, 32, "PID_0x%x.log", m_u16PID);

            m_fpRecord = new CRecordFile(static_cast<const char *>(record_name));
        }
    }
}

          //void ReceiveTSPack(u8 *pu8Data, size_t szStreamOffset = 0)
void CPidFilter::ReceiveTSPack(u8 *pu8Data, size_t szStreamOffset)
{
    if (!IsEnable()) return;

    u16 u16PID = ((pu8Data[1] & 0x1F) << 8) + pu8Data[2];

    if (m_u16PID == u16PID)
    {
        m_u32PidPackageCount++;

        notify(pu8Data, szStreamOffset);

        if (m_isDebug)
        {
            printf("TS Package's Offset(0x%x), starting 4 bytes: %2x %2x %2x %2x \n", szStreamOffset, pu8Data[0], pu8Data[1], pu8Data[2], pu8Data[3]);
        }

        if (m_isRecord)
        {
            if (NULL != m_fpRecord)
            {
                char record_log[64];

                snprintf(record_log, 64, "%8x: %2x %2x %2x %2x", szStreamOffset, pu8Data[0], pu8Data[1], pu8Data[2], pu8Data[3]);

                m_fpRecord->RecordOneLog(record_log);
            }
        }
        
    }
    else
    {
        // PID not Match, do nothing
    }

    return;
}



void CPidFilter::RegisterNotify(void *Key,  CFilterBase &Filter)
{
     m_mapNotify.insert(pair<void *, CFilterBase &>(Key, Filter));  
}



void CPidFilter::UnRegisterNotify(void *Key)
{
    m_mapNotify.erase(Key);
}



void CPidFilter::notify(u8 *pBuf, size_t szOffset)
{
    map<void *, CFilterBase &>::iterator iter;  
  
    for(iter = m_mapNotify.begin(); iter != m_mapNotify.end(); iter++)  
       iter->second.ReceiveTSPack(pBuf, szOffset);
}

