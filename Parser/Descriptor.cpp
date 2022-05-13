/*********************************************************************




        File:       Descriptor.h

        Author:     Robin
        Date:       2021-09-07


**********************************************************************/



#include <iostream>

#include "Descriptor.h"


CDescriptor_0x48::CDescriptor_0x48():CFieldContainer("Service descriptor")
{
    addField(new CField("service_type", 8));

    CFieldLoopNContainer *pFieldLoopN1 = new CFieldLoopNContainer("service_provider_name_length", 8, EN_FIELDLOOPN_TYPE_NAME_STR);
    pFieldLoopN1->addField(new CField("char", 8));

    addField(pFieldLoopN1);

    CFieldLoopNContainer *pFieldLoopN2 = new CFieldLoopNContainer("service_name_length", 8, EN_FIELDLOOPN_TYPE_NAME_STR);
    pFieldLoopN2->addField(new CField("char", 8));

    addField(pFieldLoopN2);
}


void show_iso_language_code(u8 *buf, u32 buflen)
{
    if (3 != buflen)
    {
        printf("[%s] buflen(%u) is error!", __FUNCTION__, buflen);
    }
    else
    {
        u8 tmp[4];

        memcpy(tmp, buf, 3);
        tmp[3] = 0x00;
        printf("%s", tmp);
    }

}

CDescriptor_0x7F_0x08::CDescriptor_0x7F_0x08():CFieldContainer("Message descriptor")
{
    addField(new CField("descriptor_tag_extension", 8));
    addField(new CField("message_id", 8));
    addField(new CField("ISO_639_language_code", 24, show_iso_language_code));
    addField(new CLeftUnknownField());
}

CDescriptor_0x40::CDescriptor_0x40():CFieldContainer("Network name descriptor")
{
    addField(new CLeftUnknownField(EN_LEFTFIELD_TYPE_NAME_STR));
}

CDescriptor_0x41::CDescriptor_0x41():CFieldContainer("Service list descriptor")
{
    CFieldLoopContainer *pFieldLoop = new CFieldLoopContainer("service list", 0);

    pFieldLoop->addField(new CField("service_id", 16));
    pFieldLoop->addField(new CField("service_type", 8));

    addField(pFieldLoop);
}

void show_centre_frequency(u8 *buf, u32 buflen)
{
    u32 freq = U32TO_LITTLE_END(*((u32 *)buf));


    printf("%u Hz", freq*10);
}

CDescriptor_0x5A::CDescriptor_0x5A():CFieldContainer("Terrestrial delivery system descriptor")
{
    addField(new CField("centre_frequency", 32, show_centre_frequency));
    addField(new CField("bandwidth", 3));
    addField(new CField("priority", 1));
    addField(new CField("Time_Slicing_indicator", 1));
    addField(new CField("MPE-FEC_indicator", 1));
    addField(new CField("reserved_future_use", 2));
    addField(new CField("constellation", 2));
    addField(new CField("hierarchy_information", 3));
    addField(new CField("code_rate-HP_stream", 3));
    addField(new CField("code_rate-LP_stream", 3));
    addField(new CField("guard_interval", 2));
    addField(new CField("transmission_mode", 2));
    addField(new CField("other_frequency_flag", 1));
    addField(new CField("reserved_future_use", 32));
}


CDescriptor_0x62::CDescriptor_0x62():CFieldContainer("Frequency list descriptor")
{
    addField(new CField("reserved_future_use", 6));
    addField(new CField("coding_type", 2));

    CFieldLoopContainer *pFieldLoop = new CFieldLoopContainer("centre_frequency list", 0);

    pFieldLoop->addField(new CField("centre_frequency", 32, show_centre_frequency));
    addField(pFieldLoop);
}


void show_private_specifier_descriptor(u8 *buf, u32 buflen)
{
    u32 priv_sp_des = U32TO_LITTLE_END(*((u32 *)buf));

    if (0x0000233A == priv_sp_des)
    {
        printf("Independent Television Commission");
    }
    else
    {
        printf("known private sepcifier descriptor");
    }
}

CDescriptor_0x5F::CDescriptor_0x5F():CFieldContainer("Private data specifier descriptor")
{
    addField(new CField("private_data_specifier", 32, show_private_specifier_descriptor));
}

CDescriptor_0x83::CDescriptor_0x83():CFieldContainer("Logical channel descriptor V1")
{
    CFieldLoopContainer *pFieldLoop = new CFieldLoopContainer("logical_channel_descriptor v1", 0);

    pFieldLoop->addField(new CField("service_id", 16));
    pFieldLoop->addField(new CField("visible_service_flag", 1));
    pFieldLoop->addField(new CField("reserved", 1));
    pFieldLoop->addField(new CField("logical_channel_number", 14));

    addField(pFieldLoop);
}

CDescriptor_0x4D::CDescriptor_0x4D():CFieldContainer("short_event_descriptor")
{
    addField(new CField("ISO_639_language_code", 24, show_iso_language_code));

    CFieldLoopNContainer *pFieldLoopN1 = new CFieldLoopNContainer("event_name_length", 8, EN_FIELDLOOPN_TYPE_NAME_STR);
    pFieldLoopN1->addField(new CField("char", 8));

    addField(pFieldLoopN1);

    CFieldLoopNContainer *pFieldLoopN2 = new CFieldLoopNContainer("text_length", 8, EN_FIELDLOOPN_TYPE_NAME_STR);
    pFieldLoopN2->addField(new CField("char", 8));
    addField(pFieldLoopN2);
}

CDescriptor_0x50::CDescriptor_0x50():CFieldContainer("Component descriptor")
{
    addField(new CField("stream_content_ext", 4));
    addField(new CField("stream_content", 4));

    addField(new CField("component_type", 8));
    addField(new CField("component_tag", 8));

    addField(new CField("ISO_639_language_code", 24, show_iso_language_code));

    CFieldLoopContainer *pFieldLoop = new CFieldLoopContainer("text_char loop", 0, EN_FIELDCONTAINER_TYPE_NAME_STR);
    pFieldLoop->addField(new CField("text_char", 8));
    addField(pFieldLoop);
}

CDescriptor_0x54::CDescriptor_0x54():CFieldContainer("Content descriptor")
{
    CFieldLoopContainer *pFieldLoop = new CFieldLoopContainer("content_descriptor", 0);
    pFieldLoop->addField(new CField("content_nibble_level_1", 4));
    pFieldLoop->addField(new CField("content_nibble_level_2", 4));
    pFieldLoop->addField(new CField("user_byte", 8));

    addField(pFieldLoop);
}



