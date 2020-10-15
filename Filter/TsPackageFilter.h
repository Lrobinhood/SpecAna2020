/******************************************************************************************



	Author:	Robin
	Date:	2020-9-24

******************************************************************************************/
#include <map>

#include "FilterBase.h"

using namespace std;

#ifndef __TSPACKAGE_FILTER_H__
#define __TSPACKAGE_FILTER_H__


typedef enum tag_en_Filter_State{
	EN_TSPACKAGE_FILTER_STATE_NOT_SYNC,
	EN_TSPACKAGE_FILTER_STATE_SYNC
}EN_TSPACKAGE_FILTER_STATE;

class CTsPackageFilter{
public:
	CTsPackageFilter() {m_pRoundBuf = NULL; m_State = EN_TSPACKAGE_FILTER_STATE_NOT_SYNC; m_u32Progress = 0;}
	~CTsPackageFilter(){;}

	//EN_DRAG_DATA_RESULT GetOneTsPackage(u8 *pBuf, u32 u32PackagelLen = 188);
	void SetRoundBuf( CRoundBuf *pBuf) { m_pRoundBuf = pBuf; }

	u32 GetTsPackage(u8 *pBuf, u32 u32PackageLen = 188);
    u32 GetTsPackageEx(u8 *pBuf, size_t &szStreamOffset, u32 u32PackageLen = 188);

    void RegisterNotify(u32 Key, CFilterBase &Filter);
    void UnRegisterNotify(u32 Key);

protected:
	// 同步包头
	virtual bool syncpackage();

    void notify(u8 *pBuf, size_t szOffset);

private:
	CRoundBuf *m_pRoundBuf;
	EN_TSPACKAGE_FILTER_STATE m_State;

    map<u32, CFilterBase &> m_mapNotify;

    u32 m_u32Progress;
};

#endif // #ifndef __TSPACKAGE_FILTER_H__
