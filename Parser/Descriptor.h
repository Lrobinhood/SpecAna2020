/*********************************************************************




        File:       Descriptor.h

        Author:     Robin
        Date:       2021-09-07


**********************************************************************/

#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include "FieldContainer.h"


#define DESCRIPTOR_DECLARE(descriptor, clsname) class clsname : public CFieldContainer \
                                                { \
                                                    public: \
                                                    clsname(); \
                                                    static CFieldContainer* CreateItself() { return (CFieldContainer*)new clsname(); } \
                                                }

DESCRIPTOR_DECLARE("Service descriptor", CDescriptor_0x48);
DESCRIPTOR_DECLARE("Message descriptor", CDescriptor_0x7F_0x08);


DESCRIPTOR_DECLARE("Network name descriptor", CDescriptor_0x40);
DESCRIPTOR_DECLARE("Service list descriptor", CDescriptor_0x41);

DESCRIPTOR_DECLARE("Terrestrial delivery system descriptor", CDescriptor_0x5A);
DESCRIPTOR_DECLARE("Frequency list descriptor", CDescriptor_0x62);
DESCRIPTOR_DECLARE("Private data specifier descriptor", CDescriptor_0x5F);
DESCRIPTOR_DECLARE("Logical channel descriptor V1", CDescriptor_0x83);

DESCRIPTOR_DECLARE("Short event descriptor", CDescriptor_0x4D);
DESCRIPTOR_DECLARE("Component descriptor", CDescriptor_0x50);
DESCRIPTOR_DECLARE("Content descriptor", CDescriptor_0x54);







#endif


