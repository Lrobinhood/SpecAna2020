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

class CField : public CFieldBase
{
public:
	
	//string setFatherName();

	//u32 bitlen();

    //u32 offset();
    //void setOffset(u32 u32Offset);
	
	void setBuf(const u8 *pBuf, u32 BufLen);
	void getBuf(u8 *pBuf, u32 BufLen);
	void showData();

    virtual u32 parserData(const u8 *pData, u32 const u32DataLen, const u32 u32BitOffset); 


	CField(string name, u32 bitlen);

	~CField();

protected:
	u32		m_u32BufLen;
	u8		*m_pu8Buf;

private:
	static const u32 MAX_NAME_LEN = 48; 
};


class CLeftUnknownField : public CField
{
public:

    virtual u32 parserData(const u8 *pData, u32 const u32DataLen, const u32 u32BitOffset); 
    
    CLeftUnknownField();

    ~CLeftUnknownField();        
        
};



#endif  // #ifndef __FIELD_H__
