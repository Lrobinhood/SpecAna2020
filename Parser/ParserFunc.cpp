/******************************************************************************************



	Author:	Robin
	Date:	2020-10-15

******************************************************************************************/

#include "stdafx.h"


#include "field.h"
#include "FieldContainer.h"
#include "DescriptorBase.h"
#include "MultipleStringStructureBase.h"

#include "ParserFunc.h"

void SDT_Parser(u8 *pBuf, u32 u32DataLen)
{
	CFieldContainer fieldContainer("SDT");
    
    // Table Head
	fieldContainer.addField(new CField("table_id", 8));
	fieldContainer.addField(new CField("section_syntax_indicator", 1));
	fieldContainer.addField(new CField("0", 1));
	fieldContainer.addField(new CField("Reserved", 2));
	fieldContainer.addField(new CField("section_length", 12));
	fieldContainer.addField(new CField("transport_stream_id", 16));
	fieldContainer.addField(new CField("Reserved", 2));
	fieldContainer.addField(new CField("vesion_number", 5));
	fieldContainer.addField(new CField("current_next_indicator", 1));
	fieldContainer.addField(new CField("section_number", 8));
	fieldContainer.addField(new CField("last_section_number", 8));

    fieldContainer.addField(new CField("original_network_id", 16));
    fieldContainer.addField(new CField("reserved_future_use", 8));


    // loop1: stream info loop
    CFieldLoopContainer *pfieldLoopContainer = new CFieldLoopContainer("sdt loop1 service info");
    
	pfieldLoopContainer->addField(new CField("service_id", 16));

	pfieldLoopContainer->addField(new CField("reserved_future_use", 6));
    pfieldLoopContainer->addField(new CField("EIT_schedule_flag", 1));
	pfieldLoopContainer->addField(new CField("EIT_present_following_flag", 1));
    
    pfieldLoopContainer->addField(new CField("runing_status", 3));
	pfieldLoopContainer->addField(new CField("free_CA_mode", 1));
    
    
    pfieldLoopContainer->addField(new CDescriptorLoopContainer("service descriptor", 12));
    
	fieldContainer.addField(pfieldLoopContainer);

    // CRC
	fieldContainer.addField(new CField("CRC_32", 32));

	fieldContainer.parserData(pBuf, u32DataLen, 0);
	
	fieldContainer.showData();
}



