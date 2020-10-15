 /*********************************************************************
 
 
 
 
        File:       field.cpp
        
        Author:     Robin
        Date:       2017-06-06
 
 
**********************************************************************/

#include "stdafx.h"

#include <stdio.h>

#include "field.h"

#include "ParseAssit.h"




CField::CField(string name, u32 bitlen): CFieldBase(name)
{
	m_u32BitLen = bitlen;
	m_u32BufLen = (m_u32BitLen + 7)/8;
	m_pu8Buf	= new u8[m_u32BufLen];

	if (NULL != m_pu8Buf)
		memset(m_pu8Buf, 0, m_u32BufLen);
	
	m_u32Offset = 0;
}

CField::~CField()
{
	if (NULL != m_pu8Buf)
		delete []m_pu8Buf;
}

#if 0
u32 CField::bitlen()
{
	return m_u32BitLen;
}
#endif

void CField::setBuf(const u8 *pBuf, u32 BufLen)
{
	u32 len = 0;

	memset(m_pu8Buf, 0, m_u32BufLen);

	if (m_u32BufLen >= BufLen)
	{
		len = BufLen;
	}
	else
	{
		len = m_u32BufLen;

		printf("\n Err BufLen:%d, Actually Len is:%d \n", m_u32BufLen, BufLen);
	}

	memcpy(m_pu8Buf, pBuf, len);
}

void CField::getBuf(u8 *pBuf, u32 BufLen)
{
	u32 len = 0;

	if (BufLen >= m_u32BufLen)
	{
		len = m_u32BufLen;
	}
	else
	{
		len = BufLen;

		printf("\n Err BufLen:%d, Actually Len is:%d \n", BufLen, m_u32BufLen);
	}

	memcpy(pBuf, m_pu8Buf, len);
}

void CField::showData()
{
	u32 spacenum = 0;
    char fieldtitle[128];

    if (NULL == m_pu8Buf || 0 == m_u32BufLen)
        return;
    
	sprintf_s(fieldtitle, "%s (len:%u, off:%u):", m_sName.data(), m_u32BitLen, m_u32Offset);

	if (MAX_NAME_LEN > strlen(fieldtitle))
		spacenum = MAX_NAME_LEN - strlen(fieldtitle);
	else 
		spacenum = 0;

    printf("%s", fieldtitle);
	
	for (u32 i = 0; i < spacenum; i++)
		printf(" ");

	for (u32 i = 0; i < m_u32BufLen; i++)
	{
		if (0x0F < m_pu8Buf[i])
			printf("0x%x ", m_pu8Buf[i]);
		else
			printf("0x0%x ", m_pu8Buf[i]);

        if (15 == (i % 16))
        {
            printf("\n");  

            for (u32 j = 0; j < MAX_NAME_LEN; j++)
                printf(" ");
        }
	}

	printf("\n");
}


// return value: consumed bits
u32 CField::parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset)
{
    // check offset
	if (u32BitOffset != m_u32Offset)
	{
	    // Waring
		printf("\n [Func:%s, Line;%d] ERR offset Filed Name:%s, (%u, %u), Auto Adjust this field offset \n\n", __FUNCTION__, __LINE__, m_sName.data() , u32BitOffset, m_u32Offset);
		
        // Adjust Offset
        m_u32Offset = u32BitOffset;
	}

    u32 u32ValidBufLen = ParseAssit_getBitsFromBuf(pData, u32DataLen, u32BitOffset, m_u32BitLen, m_pu8Buf, m_u32BufLen);
	printf("\n [Func:%s, Line;%d][Johnson] u32ValidBufLen = %u , m_u32BitLen = %u [%u:%u]\n", __FUNCTION__, __LINE__, u32ValidBufLen, m_u32BitLen, u32BitOffset, m_u32Offset);

    return m_u32BitLen;
}

CLeftUnknownField::CLeftUnknownField() : CField("LefUnknownFiled", 0)
{

    m_pu8Buf = NULL;
}

CLeftUnknownField::~CLeftUnknownField()
{
    if (NULL != m_pu8Buf)
		delete []m_pu8Buf;
}

// return value: consumed bits
u32 CLeftUnknownField::parserData(const u8 *pData, u32 const u32DataLen, const u32 u32BitOffset)
{
    if (u32BitOffset + 32 >= 8 * u32DataLen)    // Usually, 32bit CRC at the end of Table section
    {
        return 0;
    }

    //init data buffer
    m_u32Offset = u32BitOffset;
    
    m_u32BitLen = 8 * u32DataLen - 32 - u32BitOffset;
    m_u32BufLen = (m_u32BitLen + 7)/8;
	m_pu8Buf	= new u8[m_u32BufLen];

	if (NULL != m_pu8Buf)
		memset(m_pu8Buf, 0, m_u32BufLen);

    return CField::parserData(pData, u32DataLen, u32BitOffset);
}
    
