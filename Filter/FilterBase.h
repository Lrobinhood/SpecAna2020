/******************************************************************************************



    author: Robin
    date:   2020-10-12

******************************************************************************************/


#ifndef __FILTERBASE_H__
#define __FILTERBASE_H__

class CRecordFile;

typedef enum tagFilterState
{
    EN_FILTER_DISABLE = 0,
    EN_FILTER_ENABLE
}EN_FILTER_STATE;

class CFilterBase{

public:
    CFilterBase(): m_enFilterState(EN_FILTER_DISABLE), m_isDebug(false), m_isRecord(false), m_fpRecord(NULL) {;}
    ~CFilterBase() {;}

    virtual void ReceiveTSPack(u8 *pu8Data, size_t szStreamOffset = 0) = 0;

    inline void DisableFilter() { m_enFilterState = EN_FILTER_DISABLE; }

    inline void EnableFilter() { m_enFilterState = EN_FILTER_ENABLE; }

    inline bool IsEnable() { return (EN_FILTER_ENABLE == m_enFilterState); }

    void SetDebug(bool debug) {m_isDebug = debug; }

    void SetRecord(bool record) {m_isRecord = record; }


protected:
        EN_FILTER_STATE m_enFilterState;

        bool m_isDebug;

        bool m_isRecord;

        CRecordFile *m_fpRecord;

};


#endif
