/*********************************************************************
 
 
 
 
        File:       DescriptorBase.cpp
        
        Author:     Robin
        Date:       2017-06-08
 
 
 
**********************************************************************/

#include "stdafx.h"

#include <stdio.h>

#include "Field.h"
#include "DescriptorBase.h"

#include "ParseAssit.h"



CDescriptorBase::CDescriptorBase(string name) : CFieldContainer(name)
{
    addField(new CField("descriptor_tag", 8));
    addField(new CField("descriptor_length", 8));

    m_u32BitLen = 8 * 2; // Minimum number of bits
}

CDescriptorBase::~CDescriptorBase()
{

}

u32 CDescriptorBase::parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset)
{
    u32 u32CurBitOffset = u32BitOffset;

	vector <CFieldBase *>::iterator iter1; 

    // Get Descriptor "Tag" and "Length"
	for (iter1 = m_vecField.begin(); iter1 != m_vecField.end(); iter1++)
	{
		string name = (*iter1)->name();

		u32CurBitOffset += (*iter1)->parserData(pData, u32DataLen, u32CurBitOffset);
	}

    // Get Descriptor "Data"
    iter1  = m_vecField.end() - 1;
    
    u8 u8DescriptorLen = 0;

    (* iter1)->getBuf(&u8DescriptorLen, 1); // bytes

    
    CField *pNewField = new CField("descriptor_data", u8DescriptorLen * 8);
    if (NULL == pNewField)
    {
        // To be done ... 
        // Thrown excption
    }

    addField(pNewField);

    pNewField->setOffset(u32CurBitOffset);

    u32CurBitOffset += pNewField->parserData(pData, u32DataLen, u32CurBitOffset);

    return (u32CurBitOffset - u32BitOffset);
}



void CDescriptorBase::showData()
{
    vector <CFieldBase *>::iterator iter1 = m_vecField.begin(); 

    printf("\n[CDescriptorBase][%s] Begin >>> \n", m_sName.data());

    while (iter1 != m_vecField.end())
    {

        (*iter1)->showData();

        iter1++;
    }    

    printf("\n[CDescriptorBase][%s] End >>> \n\n", m_sName.data());
}



CDescriptorLoopContainer::CDescriptorLoopContainer(string name, u32 u32LenFieldBits) : CFieldContainer(name)
{
    m_u32BitLen = u32LenFieldBits;
    
    addField(new CField(name, u32LenFieldBits));
}

CDescriptorLoopContainer::~CDescriptorLoopContainer()
{

}

u32 CDescriptorLoopContainer::parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset)
{
    u32 u32CurBitOffset = u32BitOffset;
    u32 u32LoopBytes    = 0;
    
       	
	vector <CFieldBase *>::iterator iter1; 

    // get Descriptor Loop Length, unit: bytes
    iter1 = m_vecField.begin();
    u32CurBitOffset += (*iter1)->parserData(pData, u32DataLen, u32CurBitOffset);

    u32 len     = (*iter1)->bitlen();
    u32 bytes   = (len + 7) / 8;
    u8 *pBuf    = new u8[bytes];
    (*iter1)->getBuf(pBuf, bytes);
    
    u32LoopBytes = Buf_2_u32(pBuf, bytes);

    delete pBuf;
    
    
    // Get Descriptor
    u32 u32LoopBits             = 8 * u32LoopBytes;
    u32 u32TotalConsumedBits    = 0;
    
    while (u32TotalConsumedBits < u32LoopBits) 
    {
        CDescriptorBase *pNewDescriptor = new CDescriptorBase("Descriptor");

        if (NULL == pNewDescriptor)
        {
            // To be done ... 
            // Thrown excption
        }

        pNewDescriptor->setOffset(u32CurBitOffset);

	    m_vecField.push_back(pNewDescriptor);

        u32 u32CurComsumedBits = 0;
        
        u32CurComsumedBits = pNewDescriptor->parserData(pData, u32DataLen, u32CurBitOffset);

    	u32CurBitOffset         += u32CurComsumedBits;

        u32TotalConsumedBits    += u32CurComsumedBits;
    }

    return (u32CurBitOffset - u32BitOffset);
}

void CDescriptorLoopContainer::showData()
{
    vector <CFieldBase *>::iterator iter1 = m_vecField.begin(); 

    printf("\n[CDescriptorLoopContainer][%s] Begin >>> \n", m_sName.data());

    while (iter1 != m_vecField.end())
    {

        (*iter1)->showData();

        iter1++;
    }    

    printf("\n[CDescriptorLoopContainer][%s] End >>> \n\n", m_sName.data());
}    

