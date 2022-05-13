/*********************************************************************




        File:       ParseAssit.cpp

        Author:     Robin
        Date:       2017-06-07


**********************************************************************/

#include <stdio.h>

#include "typedef.h"

#include "ParseAssit.h"

const u8 const_bit_mask[8] = {0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};


u32 ParseAssit_getBitsFromBuf(const u8 *pData, const u32 u32DataLen, const u32 u32CurBitOffset, u32 u32CurFieldLen, u8 *pBuf, u32 u32BufLen)
{

    u32 u32CurOffsetEnd     = u32CurBitOffset + u32CurFieldLen;
    u32 u32CurValidBufLen   = 0;

    if (u32CurFieldLen > 8 * u32BufLen)
    {
        // To be done ...
        // Thorwn excption

        return 0;
    }

    while (u32CurBitOffset + 8 < u32CurOffsetEnd)
    {
        u32CurValidBufLen++;

        u32 u32CurBitPos = u32CurOffsetEnd - 8;

        if (0 == (u32CurBitPos % 8))
        {
            u32 FirstByteIdx    = u32CurBitPos / 8;

            pBuf[u32BufLen - u32CurValidBufLen] = pData[FirstByteIdx];
        }
        else
        {
            u32 FirstByteIdx    = u32CurBitPos / 8;
            u32 SecondByteIdx   = u32CurOffsetEnd / 8;

            u32 FirstByteBits   = 8 - (u32CurBitPos % 8);

            pBuf[u32BufLen - u32CurValidBufLen] = ((pData[FirstByteIdx] & const_bit_mask[FirstByteBits - 1]) << (8 - FirstByteBits)) +
                                                  (pData[SecondByteIdx] >> FirstByteBits);
        }

        u32CurOffsetEnd -= 8;
    }

    if (u32CurBitOffset < u32CurOffsetEnd)
    {
        u32CurValidBufLen++;

        if ((u32CurBitOffset / 8) < (u32CurOffsetEnd / 8))
        {
            u32 FirstByteIdx    = u32CurBitOffset / 8;
            u32 SecondByteIdx   = u32CurOffsetEnd / 8;

            u32 FirstByteBits   = 8 - (u32CurBitOffset % 8);
            u32 SecondByteBits  = u32CurOffsetEnd % 8;

            pBuf[u32BufLen - u32CurValidBufLen] = ((pData[FirstByteIdx] & const_bit_mask[FirstByteBits - 1]) << SecondByteBits) +
                                                   (pData[SecondByteIdx] >> (8 - SecondByteBits));
        }
        else
        {
            u32 FirstByteIdx    = u32CurBitOffset / 8;

            u32 PreInvalidBitLen        = u32CurBitOffset % 8;
            u32 PostInvalidBitLen       = 8 - (u32CurOffsetEnd % 8);

            pBuf[u32BufLen - u32CurValidBufLen] = (pData[FirstByteIdx] & const_bit_mask[7 - PreInvalidBitLen]) >> PostInvalidBitLen;
        }
    }

    return u32CurValidBufLen;
}



u32 Buf_2_u32(u8 *pBuf, u32 u32BufLen)
{
    u32 u32Len = 0;

    if (u32BufLen > 4)
    {
        printf("\n [Func:%s, Line:%d] Err Len:%u \n", __FUNCTION__, __LINE__, u32BufLen);

        return 0;
    }

    for (u32 i = 0; i < u32BufLen; i++)
    {
        u32Len = (u32Len << 8) + pBuf[i];
    }

    return u32Len;
}

