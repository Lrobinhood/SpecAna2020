/*********************************************************************




        File:       field.h

        Author:     Robin
        Date:       2017-06-06


**********************************************************************/

#ifndef __FIELD_H__
#define __FIELD_H__

#include "FieldBase.h"


#include <vector>

#include "typedef.h"



using namespace std;

typedef void (* FIELD_EX_SHOW_FUNC)(u8 *buf, u32 buflen);

class CField : public CFieldBase
{
public:

    //string setFatherName();

    //u32 bitlen();

    //u32 offset();
    //void setOffset(u32 u32Offset);

    void setBuf(const u8 *pBuf, u32 BufLen);
    void getBuf(u8 *pBuf, u32 BufLen);
    u32 getBufLen() { return m_u32BufLen; }

    void showData();

    virtual u32 parserData(const u8 *pData, u32 const u32DataLen, const u32 u32BitOffset);

    string clsName() { return CField::clsname; }

    static string clsname;
    static CFieldBase* CreateItself(string name, u32 len, CFieldBase *pFieldTemplet = NULL) { return (CFieldBase*)new CField(name, len, pFieldTemplet == NULL  ? NULL : ((CField *)pFieldTemplet)->m_ExShowFunc); }

    CField(string name, u32 bitlen);
    CField(string name, u32 bitlen, FIELD_EX_SHOW_FUNC func);

    ~CField();

protected:
    u32     m_u32BufLen;
    u8      *m_pu8Buf;
    static const u32 MAX_NAME_LEN = 48;

    FIELD_EX_SHOW_FUNC m_ExShowFunc;

protected:
    void showAssisant();

};


typedef enum {
    EN_LEFTFIELD_TYPE_COMMON,
    EN_LEFTFIELD_TYPE_NAME_STR
}EN_LEFTFIELD_TYPE;


class CLeftUnknownField : public CField
{
public:

    virtual u32 parserData(const u8 *pData, u32 const u32DataLen, const u32 u32BitOffset);

    void showData();

    CLeftUnknownField();

    CLeftUnknownField(EN_LEFTFIELD_TYPE enType);

    ~CLeftUnknownField();


private:
    EN_LEFTFIELD_TYPE m_enFieldType;

};



#endif  // #ifndef __FIELD_H__
