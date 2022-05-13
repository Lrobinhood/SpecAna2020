/*********************************************************************




        File:       FieldFactory.h

        Author:     Robin
        Date:       2021-09-06


**********************************************************************/

#include <iostream>

#include "field.h"
#include "FieldFactory.h"
#include "DescriptorBase.h"
#include "MultipleStringStructureBase.h"


CFieldFactory* CFieldFactory::instance_ = NULL;


CFieldFactory& CFieldFactory::Instance()
{
    if (instance_ == NULL)
    {
        instance_ = new CFieldFactory;

        #if 0
        instance_->RegisterFieldClass(CDescriptorBase::clsname, CDescriptorBase::CreateItself);
        instance_->RegisterFieldClass(CDescriptorLoopContainer::clsname, CDescriptorLoopContainer::CreateItself);
        instance_->RegisterFieldClass(CField::clsname, CField::CreateItself);
        #endif

        #define REG_CLASS_CREATOR(a) do { \
                                    instance_->RegisterFieldClass(a::clsname, a::CreateItself); \
                                } while(0)

        REG_CLASS_CREATOR(CDescriptorBase);
        REG_CLASS_CREATOR(CDescriptorLoopContainer);
        REG_CLASS_CREATOR(CMultipleStringStructureBase);
        REG_CLASS_CREATOR(CMultipleStringStructureLoopContainer);
        REG_CLASS_CREATOR(CField);


    }

    return *instance_;
}



CFieldBase* CFieldFactory::CreateFieldInstance(string clsname, string fieldname, u32 fieldlen, CFieldBase *pFieldTemplet)
{
    FIELD_CREATE_FUNC func = NULL;
    map<string, FIELD_CREATE_FUNC>::iterator iter;

    iter = m_mapCreator.find(clsname);

    if(iter != m_mapCreator.end())
    {
        func = iter->second;
    }
    else
    {
        printf("Cannot find class:: clsname[%s], fieldname[%s] \n", clsname.c_str(), fieldname.c_str());
        //std::cout << "Cannot find class: [" << clsname <<"]" <<" filed name: <" << fieldname << ">" << endl;
    }

    if (func)
    {
        return func(fieldname, fieldlen, pFieldTemplet);
    }
    else
    {
        return (CFieldBase *)new CField(fieldname, fieldlen);
    }
}



void CFieldFactory::RegisterFieldClass(string clsname, FIELD_CREATE_FUNC func)
{
    m_mapCreator.insert(pair<string, FIELD_CREATE_FUNC>(clsname, func));
}


