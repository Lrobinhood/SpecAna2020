/*********************************************************************




        File:       DescriptorFactory.h

        Author:     Robin
        Date:       2021-09-07


**********************************************************************/


#include <iostream>

#include "DescriptorFactory.h"
#include "Descriptor.h"


CDescriptorFactory* CDescriptorFactory::instance_ = NULL;


CDescriptorFactory& CDescriptorFactory::Instance()
{
    if (instance_ == NULL)
    {
        instance_ = new CDescriptorFactory;

        #define REG_CLASS_CREATOR_TAGEX(a, tag, tagex) do { \
                                    instance_->RegisterFieldClass(a::CreateItself, tag, tagex); \
                                } while(0)

        #define REG_CLASS_CREATOR_TAG(a, tag) REG_CLASS_CREATOR_TAGEX(a, tag, 0)

        REG_CLASS_CREATOR_TAG(CDescriptor_0x48, 0x48);
        REG_CLASS_CREATOR_TAGEX(CDescriptor_0x7F_0x08, 0x7F, 0x08);

        REG_CLASS_CREATOR_TAG(CDescriptor_0x40, 0x40);
        REG_CLASS_CREATOR_TAG(CDescriptor_0x41, 0x41);

        REG_CLASS_CREATOR_TAG(CDescriptor_0x5A, 0x5A);
        REG_CLASS_CREATOR_TAG(CDescriptor_0x62, 0x62);

        REG_CLASS_CREATOR_TAG(CDescriptor_0x5F, 0x5F);
        REG_CLASS_CREATOR_TAG(CDescriptor_0x83, 0x83);

        REG_CLASS_CREATOR_TAG(CDescriptor_0x4D, 0x4D);
        REG_CLASS_CREATOR_TAG(CDescriptor_0x50, 0x50);
        REG_CLASS_CREATOR_TAG(CDescriptor_0x54, 0x54);

    }

    return *instance_;
}



CFieldContainer* CDescriptorFactory::CreateFieldContainerInstance(u8 u8DescriptorTag, u8 u8DescriptorTagEx)
{
    DESCRIPTOR_CREATE_FUNC func = NULL;
    map<u16, DESCRIPTOR_CREATE_FUNC>::iterator iter;

    iter = m_mapCreator.find((u8DescriptorTag << 8) + u8DescriptorTagEx);

    if(iter != m_mapCreator.end())
    {
        func = iter->second;
    }
    else
    {
        printf("Cannot find Descriptor: Tag[0x%x], TagEx[0x%x] \n", u8DescriptorTag, u8DescriptorTagEx);
        //std::cout << "Cannot find Descriptor Tag: [" << (int)u8DescriptorTag <<"]" <<" TagEx: <" << (int)u8DescriptorTagEx << ">" << endl;
    }

    if (func)
    {
        return func();
    }
    else
    {
        return NULL;
    }
}



void CDescriptorFactory::RegisterFieldClass(DESCRIPTOR_CREATE_FUNC func, u8 u8DescriptorTag, u8 u8DescriptorTagEx)
{
    m_mapCreator.insert(pair<u16, DESCRIPTOR_CREATE_FUNC>((u8DescriptorTag << 8) + u8DescriptorTagEx, func));
}


