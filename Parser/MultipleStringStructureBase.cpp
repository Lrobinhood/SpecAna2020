/*********************************************************************
 
File:		MultipleStringStructureBase.cpp

Author:	Johnson
Date:	2017-06-26
 
**********************************************************************/

#include "stdafx.h"

#include <stdio.h>

#include "Field.h"
#include "MultipleStringStructureBase.h"

#include "ParseAssit.h"



CMultipleStringStructureBase::CMultipleStringStructureBase(string name) : CFieldContainer(name)
{
	m_u32BitLen = 8;
	m_MultipleStringStructure = new CField("number_strings", 8);
	addField(m_MultipleStringStructure);
	if (NULL == m_MultipleStringStructure)
	{
		// To be done ... 
		// Thrown excption
	}
}


CMultipleStringStructureBase::~CMultipleStringStructureBase()
{

}

u32 CMultipleStringStructureBase::parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset)
{
	u32 u32CurBitOffset = u32BitOffset;
	u32 u32CurFieldLen	= 0;
	
	u32CurBitOffset += m_MultipleStringStructure->parserData(pData, u32DataLen, u32CurBitOffset);

	u8 *pBuf	= new u8[1];
	m_MultipleStringStructure->getBuf(pBuf, 1);
	m_u32LoopN = Buf_2_u32(pBuf, 1);
	delete pBuf;	
	printf("\n[Johnson] m_u32LoopN =%u\n", m_u32LoopN);
	
	for (u32 loop = 0; loop < m_u32LoopN; loop++)
	{
		CField *pISO_639_language_code = new CField("ISO_639_language_code", 24);
		addField(pISO_639_language_code);
		pISO_639_language_code->setOffset(u32CurBitOffset);
		u32CurBitOffset += pISO_639_language_code->parserData(pData, u32DataLen, u32CurBitOffset);			

		CField *pNumber_segments = new CField("number_segments", 8);
		addField(pNumber_segments);
		pNumber_segments->setOffset(u32CurBitOffset);
		u32CurBitOffset += pNumber_segments->parserData(pData, u32DataLen, u32CurBitOffset);
		
		u32 NumOfNumber_segments;
		u8 *pBufNumber_segments = new u8[1];
		pNumber_segments->getBuf(pBufNumber_segments, 1);
		NumOfNumber_segments = Buf_2_u32(pBufNumber_segments, 1);
		delete pBufNumber_segments;
		printf("\n[Johnson] NumOfNumber_segments =%u\n", NumOfNumber_segments);
		
		for (u32 loop = 0; loop < NumOfNumber_segments; loop++)
		{			
			CField *pCompression_type = new CField("compression_type", 8);
			addField(pCompression_type);
			pCompression_type->setOffset(u32CurBitOffset);
			u32CurBitOffset += pCompression_type->parserData(pData, u32DataLen, u32CurBitOffset);			
			
			CField *pMode = new CField("mode", 8);
			addField(pMode);
			pMode->setOffset(u32CurBitOffset);
			u32CurBitOffset += pMode->parserData(pData, u32DataLen, u32CurBitOffset);
			
			CField *pNumber_bytes = new CField("number_bytes", 8);
			addField(pNumber_bytes);
			pNumber_bytes->setOffset(u32CurBitOffset);
			u32CurBitOffset += pNumber_bytes->parserData(pData, u32DataLen, u32CurBitOffset);
			
			u32 NumOfNumber_bytes;
			u8 *pBufNumber_bytes	= new u8[1];
			pNumber_bytes->getBuf(pBufNumber_bytes, 1);
			NumOfNumber_bytes = Buf_2_u32(pBufNumber_bytes, 1);
			delete pBufNumber_bytes;
			printf("\n[Johnson] NumOfNumber_bytes =%u \n", NumOfNumber_bytes);
			
			CField *pCompression_string_byte = new CField("[compression_string_byte", NumOfNumber_bytes * 8);
			addField(pCompression_string_byte);
			pCompression_string_byte->setOffset(u32CurBitOffset);
			
			u32CurBitOffset += pCompression_string_byte->parserData(pData, u32DataLen, u32CurBitOffset);

		}
			
	}

	return (u32CurBitOffset - u32BitOffset);
}


void CMultipleStringStructureBase::showData()
{
    vector <CFieldBase *>::iterator iter1 = m_vecField.begin(); 

    printf("\n[CMultipleStringStructureBase][%s] Begin >>> \n", m_sName.data());

    while (iter1 != m_vecField.end())
    {

        (*iter1)->showData();

        iter1++;
    }    

    printf("\n[CMultipleStringStructureBase][%s] End >>> \n\n", m_sName.data());
}



CMultipleStringStructureLoopContainer::CMultipleStringStructureLoopContainer(string name, u32 u32BitsLen) : CFieldContainer(name)
{
	m_u32BitLen = u32BitsLen;
	
	addField(new CField(name, u32BitsLen));
}



CMultipleStringStructureLoopContainer::~CMultipleStringStructureLoopContainer()
{

}


u32 CMultipleStringStructureLoopContainer::parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset)
{
	u32 u32CurBitOffset = u32BitOffset;
	u32 u32LoopBytes	= 0;
	
		
	vector <CFieldBase *>::iterator iter1; 

	// get Descriptor Loop Length, unit: bytes
	iter1 = m_vecField.begin();
	u32CurBitOffset += (*iter1)->parserData(pData, u32DataLen, u32CurBitOffset);

	u32 len 	= (*iter1)->bitlen();
	u32 bytes	= (len + 7) / 8;
	u8 *pBuf	= new u8[bytes];
	(*iter1)->getBuf(pBuf, bytes);
	
	u32LoopBytes = Buf_2_u32(pBuf, bytes);

	delete pBuf;
	
	
	// Get Descriptor
	u32 u32LoopBits 			= 8 * u32LoopBytes;
	u32 u32TotalConsumedBits	= 0;
	
	while (u32TotalConsumedBits < u32LoopBits) 
	{
		CMultipleStringStructureBase *pNewDescriptor = new CMultipleStringStructureBase("CMultipleStringStructureBase");

		if (NULL == pNewDescriptor)
		{
			// To be done ... 
			// Thrown excption
		}

		pNewDescriptor->setOffset(u32CurBitOffset);

		m_vecField.push_back(pNewDescriptor);

		u32 u32CurComsumedBits = 0;
		
		u32CurComsumedBits = pNewDescriptor->parserData(pData, u32DataLen, u32CurBitOffset);

		u32CurBitOffset 		+= u32CurComsumedBits;

		u32TotalConsumedBits	+= u32CurComsumedBits;
	}

	return (u32CurBitOffset - u32BitOffset);
}



void CMultipleStringStructureLoopContainer::showData()
{
    vector <CFieldBase *>::iterator iter1 = m_vecField.begin(); 

    printf("\n[CMultipleStringStructureLoopContainer][%s] Begin >>> \n", m_sName.data());

    while (iter1 != m_vecField.end())
    {

        (*iter1)->showData();

        iter1++;
    }    

    printf("\n[CMultipleStringStructureLoopContainer][%s] End >>> \n\n", m_sName.data());
}

