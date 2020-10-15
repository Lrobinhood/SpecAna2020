/******************************************************************************************



	Author:	Robin
	Date:	2020-10-12

******************************************************************************************/
#include "stdafx.h"

#include "../Misc/RecordFile.h"

//---B: RecordFile ---

void UT_RecordFile_ReordOneLog()
{
    CRecordFile recordFile("UT_RecordFile.log");

    recordFile.RecordOneLog("Begin Test");
    recordFile.RecordOneLog("Hello World");
    recordFile.RecordOneLog("End Test");
}

//---E: RecordFile ---