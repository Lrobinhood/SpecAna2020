/*********************************************************************

        File:       FieldSimpleLoop.h

        Author:     Robin
        Date:       2021-09-07

----------------------------------------------------------------------
    service_provider_name_length                8 uimsbf
    for (i=0;i<N;i++){
        char                                    8 uimsbf
}

**********************************************************************/

#ifndef __FIELD_SIMPLE_LOOP_H__
#define __FIELD_SIMPLE_LOOP_H__

#include "FieldBase.h"



#include "typedef.h"



using namespace std;

class CFieldSimpleLoop : public CFieldBase
{
public:
    void setBuf(const u8 *pBuf, u32 BufLen);
    void getBuf(u8 *pBuf, u32 BufLen);
    u32 getBufLen() { return m_u32BufLen; }

    void showData();

    virtual u32 parserData(const u8 *pData, u32 const u32DataLen, const u32 u32BitOffset);

    string clsName() { return CField::clsname; }

    static string clsname;
    static CFieldBase* CreateItself(string name, u32 len) { return (CFieldBase*)new CField(name, len); }

    CFieldSimpleLoop(string name, u32 bitlen);

    ~CFieldSimpleLoop();

protected:
    u32 m_u32BufLen;
    u8 *m_pu8Buf;

private:
};

#endif  // #ifndef __FIELD_H__
