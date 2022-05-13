/*********************************************************************




        File:       DescriptorBase.h

        Author:     Robin
        Date:       2017-06-08


**********************************************************************/

#ifndef __DESCRIPTORBASE_H__
#define __DESCRIPTORBASE_H__

#include "FieldContainer.h"


#include <vector>

#include "typedef.h"
#include "field.h"



using namespace std;



class CDescriptorBase : public CFieldContainer
{
public:
    CDescriptorBase(string name);
    ~CDescriptorBase();


    virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset);

    virtual void showData();

    string clsName() { return CDescriptorBase::clsname; }

    static string clsname;
    static CFieldBase* CreateItself(string name, u32 len, CFieldBase *pFieldTemplet = NULL) { return (CFieldBase*)new CDescriptorBase(name); }

protected:
    CFieldContainer *m_DescriptorDetailContainer;

private:

};



class CDescriptorLoopContainer : public CFieldContainer
{
public:
    CDescriptorLoopContainer(string name, u32 u32LenFieldBits);
    ~CDescriptorLoopContainer();


    virtual u32 parserData(const u8* pData, const u32 u32DataLen, const u32 u32BitOffset);

    virtual void showData();

    string clsName() { return CDescriptorLoopContainer::clsname; }

    static string clsname;
    static CFieldBase* CreateItself(string name, u32 len, CFieldBase *pFieldTemplet = NULL) { return (CFieldBase*)new CDescriptorLoopContainer(name, len); }

protected:

private:
};

#endif  // #ifndef __DESCRIPTORBASE_H__


