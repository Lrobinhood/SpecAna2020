/******************************************************************************************



    Author: Robin
    Date:   2021-09-06

******************************************************************************************/
#include "../Parser/DescriptorBase.h"
#include "../Parser/FieldFactory.h"


void UT_FieldFactory()
{
    CFieldFactory::Instance().CreateFieldInstance(CDescriptorBase::clsname, "test", 10, NULL);
    CFieldFactory::Instance().CreateFieldInstance("unknown field cls", "unknown field", 12, NULL);
}