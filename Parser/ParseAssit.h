/*********************************************************************
 
 
 
 
        File:       ParseAssit.h
        
        Author:     Robin
        Date:       2017-06-07
 
 
**********************************************************************/

#ifndef __PARSE_ASSIT_H__
#define __PARSE_ASSIT_H__


// Return Value : ValidBufLen in "pBuf"
u32 ParseAssit_getBitsFromBuf(const u8 *pData, const u32 u32DataLen, const u32 u32CurBitOffset, u32 u32CurFieldLen, u8 *pBuf, u32 u32BufLen);

u32 Buf_2_u32(u8 *pBuf, u32 u32BufLen);


#endif
  
