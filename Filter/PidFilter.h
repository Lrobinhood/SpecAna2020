/******************************************************************************************



	author:	Robin
	date:	2020-10-12

******************************************************************************************/
#include <map>
#include "FilterBase.h"

#ifndef __PIDFILTER_H__
#define __PIDFILTER_H__

using namespace std;

class CPidFilter:public CFilterBase
{

public:
    CPidFilter();
    ~CPidFilter();
        
	void SetPid(u16 u16PID);

    void ReceiveTSPack(u8 *pu8Data, size_t szStreamOffset = 0);

    void RegisterNotify(u32 Key, CFilterBase &Filter);
    void UnRegisterNotify(u32 Key);

	/*
    void ConnectSectionFilter(u32 u32SectionFilterHandle);
    void DisconnectSectionFilter(u32 u32SectionFilterHandle);
    u32 GetConnectSectionFilter();
	*/

	/*
    void SetDataNotifyFunc(TSPU_DATA_NOTIFY_FUNC pfnNotify, void *pvTspuDataQueue);
    TSPU_DATA_NOTIFY_FUNC GetDataNotifyFunc();
    */
    
protected:
    //CSectionFilter* TransformSectionFilter2Pointer(u32 u32SectionFilterHandle);
     
    void notify(u8 *pBuf, size_t szOffset);
    
private:
    u32 m_u32PIDHandle;

    u16 m_u16PID;
    u32 m_u32PidPackageCount;

    map<u32, CFilterBase &> m_mapNotify;


	/*
    TSPU_DATA_NOTIFY_FUNC m_pfnDataNotify;
    void *m_pvTspuDataQueue;
    */
};

#endif

