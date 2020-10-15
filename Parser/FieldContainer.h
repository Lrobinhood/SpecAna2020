/*********************************************************************
 
 
 
 
        File:       FieldContainer.h
        
        Author:     Robin
        Date:       2017-06-07
 
 
**********************************************************************/
 
#ifndef __FIELDCONTAINER_H__
#define __FIELDCONTAINER_H__

#include "FieldBase.h"


#include <vector> 

#include "typedef.h"
#include "field.h"



using namespace std;

class CFieldContainer : public CFieldBase
{
public:
       
	virtual void addField(CFieldBase *pField);

   

	CFieldContainer(string name);

	~CFieldContainer();

	virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset); 

    virtual void showData();
    
    
private:
    
protected:
	vector<CFieldBase *> m_vecField;

	u32 m_u32CurrentOffset;

};



class CFieldLoopContainer : public CFieldContainer
{
public:
        CFieldLoopContainer(string name);
        ~CFieldLoopContainer();


        virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset); 

        virtual void showData();


        u32 bitlen();
  
protected:
        u32 m_u32LoopCount;

        u32 m_u32BitLen;

        vector<CFieldBase *> m_vecLoopField;

private:
        u32 getOneLoopBits();
        u32 getOneLoopFieldNum();
            
};

class CFieldLoopNContainer : public CFieldContainer
{
public:
        CFieldLoopNContainer(string name, u32 u32BitsLen);
        ~CFieldLoopNContainer();


        virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset); 

        virtual void showData();
  
protected:
        u32 m_u32LoopN;

        CFieldBase *m_pLoopNField;

        vector<CFieldBase *> m_vecLoopField;

private:
            
};


#endif  // #ifndef __FIELDCONTAINER_H__
 
