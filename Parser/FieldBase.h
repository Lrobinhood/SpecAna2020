/*********************************************************************
 
 
 
 
        File:       FieldBase.h
        
        Author:     Robin
        Date:       2017-06-07
 
 
**********************************************************************/
 
#ifndef __FIELD_BASE_H__
#define __FIELD_BASE_H__ 

#include <string>

#include "typedef.h"

using namespace std;

class CFieldBase
{
public:
    CFieldBase(string& name) : m_u32Offset(0), m_u32BitLen(0) {m_sName = name; }
    
	virtual string& name() { return m_sName; }

    virtual string className() { return string("CFieldBase"); }


    virtual u32 bitlen();

    virtual void setOffset(u32 u32Offset);

    virtual u32 offset();

	virtual void getBuf(u8 *pBuf, u32 BufLen);

		
    // return value: consumed bits
    virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset) = 0; 

    virtual void showData() = 0;
	

protected:
	string m_sName;
    //string m_sClassName;

    u32 m_u32Offset;

    u32 m_u32BitLen;
};

typedef struct DB_TVCTServiceInfo 
{
    u8 u8ServiceIndex;
    void* pu8ShortName;
    u16 u16MajorChNum;
    u16 u16MinorChNum;
    u16 u16ChannelTSID;
   
    u8 u8ModulationMode;
    u8 u8ETM_location;
    u8 u8Hidden;
    u8 u8ServiceType;
    u16 u16SourceId;
    
    u8 u8DescriptorTag;
    u8 u8StreamType;
    u16 u16Elementary_PID;
    
}ST_TVCT_SERVICE_INFO;

#endif
