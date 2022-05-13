/*********************************************************************




        File:       FieldFactory.h

        Author:     Robin
        Date:       2021-09-06


**********************************************************************/

#ifndef __FIELD_FACTORY_H__
#define __FIELD_FACTORY_H__

#include <map>
#include "FieldBase.h"

typedef CFieldBase* (* FIELD_CREATE_FUNC)(string fieldname, u32 fieldlen, CFieldBase *pFieldTemplet);

class CFieldFactory
{
public:
    static CFieldFactory& Instance();

    CFieldBase* CreateFieldInstance(string clsname, string fieldname, u32 fieldlen, CFieldBase *pFieldTemplet);

private:
    CFieldFactory() { ; }
    ~CFieldFactory() { ; }

    CFieldFactory(const CFieldFactory&) { ; }
    CFieldFactory& operator =(const CFieldFactory&) { ; }

    void RegisterFieldClass(string clsname, FIELD_CREATE_FUNC func);


private:
    static CFieldFactory* instance_;
    map <string, FIELD_CREATE_FUNC> m_mapCreator;


};


#endif


