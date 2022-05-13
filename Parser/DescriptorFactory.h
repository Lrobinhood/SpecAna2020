/*********************************************************************




        File:       DescriptorFactory.h

        Author:     Robin
        Date:       2021-09-07


**********************************************************************/

#ifndef __DESCRIPTOR_FACTORY_H__
#define __DESCRIPTOR_FACTORY_H__

#include <map>
#include "FieldContainer.h"


typedef CFieldContainer* (* DESCRIPTOR_CREATE_FUNC)();


class CDescriptorFactory
{
public:
    static CDescriptorFactory& Instance();

    CFieldContainer* CreateFieldContainerInstance(u8 u8DescriptorTag, u8 u8DescriptorTagEx = 0);


private:
    CDescriptorFactory() { ; }
    ~CDescriptorFactory() { ; }

    CDescriptorFactory(const CDescriptorFactory&) { ; }
    CDescriptorFactory& operator =(const CDescriptorFactory&) { ; }

    void RegisterFieldClass(DESCRIPTOR_CREATE_FUNC func, u8 u8DescriptorTag, u8 u8DescriptorTagEx = 0);


private:
    static CDescriptorFactory* instance_;
    map <u16, DESCRIPTOR_CREATE_FUNC> m_mapCreator;


};


#endif


