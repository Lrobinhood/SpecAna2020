/******************************************************************************************



	Author:	Robin
	Date:	2020-10-15

******************************************************************************************/

#ifndef __PARSER_FUNC_H__
#define __PARSER_FUNC_H__

typedef void (*TABLE_PARSER_FUNC)(u8 *, u32);

void SDT_Parser(u8 *pBuf, u32 u32DataLen);
void NIT_Parser(u8 *pBuf, u32 u32DataLen);
void EIT_Parser(u8 *pBuf, u32 u32DataLen);

void PAT_Parser(u8 *pBuf, u32 u32DataLen);
void PMT_Parser(u8 *pBuf, u32 u32DataLen);

void BAT_Parser(u8 *pBuf, u32 u32DataLen);




void Common_Set_TsRate(double tsrate);
double Common_Get_TsRate();

void Common_Reinit_Max_Interval();
void Common_Get_Max_Interval(u32 &interva_idx, double &interval);

void Common_Set_Tbl_Parser(TABLE_PARSER_FUNC func);

void Common_Table_ParserEx_SetMatchVersion(BOOLEAN bMatchVersion);
void Common_Table_ParserEx(u8 *pBuf, u32 u32DataLen, size_t szOffset);
#endif
