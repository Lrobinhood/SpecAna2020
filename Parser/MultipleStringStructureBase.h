/*********************************************************************
 
File:		MultipleStringStructureBase.h

Author:	Johnson
Date:	2017-06-26
 
**********************************************************************/

 
#ifndef __MULTIPLESTRINGSTRUCTUREBASE_H__
#define __MULTIPLESTRINGSTRUCTUREBASE_H__

#include "FieldContainer.h"


#include <vector> 

#include "typedef.h"
#include "field.h"



using namespace std;



class CMultipleStringStructureBase : public CFieldContainer
{
public:
        CMultipleStringStructureBase(string name);
        ~CMultipleStringStructureBase();


        virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset); 

        virtual void showData();

        virtual string className() { return string("CMultipleStringStructureBase"); }

        
protected:
    u32 m_u32LoopN;
    CFieldBase *m_MultipleStringStructure;
    vector<CFieldBase *> m_MultipleStringStructureLoopField;

private:
            
};



class CMultipleStringStructureLoopContainer : public CFieldContainer
{
public:
    CMultipleStringStructureLoopContainer(string name, u32 u32LenFieldBits);
    ~CMultipleStringStructureLoopContainer();


virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset); 

virtual void showData();

virtual string className() { return string("CMultipleStringStructureLoopContainer"); }


protected:
    u32 m_u32LoopN;
    CFieldBase *m_MultipleStringStructure;
    vector<CFieldBase *> m_MultipleStringStructureLoopField;
    
private:
    
};

#endif  // #ifndef __MULTIPLESTRINGSTRUCTURE_H__
 

