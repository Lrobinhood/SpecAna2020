/******************************************************************************************



	Author:	Robin
	Date:	2020-10-10

******************************************************************************************/


#ifndef __UT_MAIN_H__
#define __UT_MAIN_H__


void UT_main(const char *TsFile);


//---B: UTFilter ---/
void UT_TsPackageFilter_GetTsPackage();

void UT_PidFilter_ReceiveTSPack();

void UT_TsPackageFilter_2_PidFilter();

void UT_PidFilter_2_SectionFilter();

void UT_PidFilter_2_SectionFilter_2_Parser();
//---E: UTFilter---/

void UT_FindPCR();
void UT_GetTsRate();

void UT_GetNitTime();
void UT_PidFilter_2_SectionFilter_2_ParserEx(const char *TsFile);

void UT_SDT_Actual_Table_Statistics(const char *TsFile);

void UT_NIT_Actual_Table_Statistics(const char *TsFile);

void UT_EIT_Actual_Table_Statistics(const char *TsFile);

void UT_PAT_Actual_Table_Statistics(const char *TsFile);

void UT_PMT_Actual_Table_Statistics(const char *TsFile);

void UT_BAT_Actual_Table_Statistics(const char *TsFile);

//---B: UTMisc ---/
void UT_RecordFile_ReordOneLog();
//---E: UTMisc ---/


//---B: UTPES ---/
void UT_RAW_PES_Filter(const char* TsFile);
//---B: UTPES ---/

//---B: UTParser ---/
void UT_FieldFactory();
//---E: UTParser ---/
#endif