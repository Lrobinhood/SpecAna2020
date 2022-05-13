/*********************************************************************




        File:       FieldContainer.CPP

        Author:     Robin
        Date:       2017-06-07



**********************************************************************/

#include <stdio.h>

#include "FieldContainer.h"
#include "DescriptorBase.h"
#include "MultipleStringStructureBase.h"

#include "ParseAssit.h"
#include "FieldFactory.h"



CFieldContainer::CFieldContainer(string name): CFieldBase(name)
{
    m_u32CurrentOffset = 0;
}

CFieldContainer::~CFieldContainer()
{
    vector <CFieldBase *>::iterator iter1;

    for (iter1 = m_vecField.begin(); iter1 != m_vecField.end(); iter1++)
    {
        delete (*iter1);
    }
}

void CFieldContainer::addField(CFieldBase *pField)
{
    pField->setOffset(m_u32CurrentOffset);
    m_u32CurrentOffset += pField->bitlen();

    m_vecField.push_back(pField);
}

void CFieldContainer::showData()
{
    vector <CFieldBase *>::iterator iter1;

    printf("\n");
    printf("%s Table Begin >>> \n\n", m_sName.data());

    for (iter1 = m_vecField.begin(); iter1 != m_vecField.end(); iter1++)
    {
        (*iter1)->showData();
    }

    printf("\n");
    printf("%s Table End <<< \n\n", m_sName.data());

}


u32 CFieldContainer::parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset)
{
    u32 u32CurBitOffset = u32BitOffset;
    u32 u32CurFieldLen  = 0;


    vector <CFieldBase *>::iterator iter1;

    for (iter1 = m_vecField.begin(); iter1 != m_vecField.end(); iter1++)
    {
        string name = (*iter1)->name();

        u32CurFieldLen = (*iter1)->bitlen();

        // check len
        if (u32CurBitOffset + u32CurFieldLen > 8 * u32DataLen)
        {
            #ifdef _DBG
            printf("\n [Func:%s, Line;%d] ERR len (%u, %u, %u) \n", __FUNCTION__, __LINE__, u32CurBitOffset, u32CurFieldLen, u32DataLen);
            #endif

            // TO be done ...
            // throw exception
        }

        #ifdef _DBG
        printf("\n [Func:%s, Line;%d][Johnson] [%u:%u]\n", __FUNCTION__, __LINE__, u32DataLen, u32CurBitOffset);
        #endif

        u32CurBitOffset += (*iter1)->parserData(pData, u32DataLen, u32CurBitOffset);
    }

    if (u32CurBitOffset < (8 * u32DataLen))
    {
        // To be done ...

    }

    return (u32CurBitOffset - u32BitOffset);
}



CFieldLoopContainer::CFieldLoopContainer(string name) : CFieldContainer(name)
{
    m_u32LoopCount = 0;
    m_u32LeftBit   = 32;

    m_enType = EN_FIELDCONTAINER_TYPE_COMMON;
}

CFieldLoopContainer::CFieldLoopContainer(string name, u32 u32LeftBit): CFieldContainer(name)
{
    m_u32LoopCount = 0;
    m_u32LeftBit   = u32LeftBit;

    m_enType = EN_FIELDCONTAINER_TYPE_COMMON;
}

CFieldLoopContainer::CFieldLoopContainer(string name, u32 u32LeftBit, EN_FIELDCONTAINER_TYPE enType): CFieldContainer(name)
{
    m_u32LoopCount = 0;
    m_u32LeftBit   = u32LeftBit;

    m_enType = enType;
}


CFieldLoopContainer::~CFieldLoopContainer()
{
    vector <CFieldBase *>::iterator iter1;

    for (iter1 = m_vecLoopField.begin(); iter1 != m_vecLoopField.end(); iter1++)
    {
        delete (*iter1);
    }
}

u32 CFieldLoopContainer::bitlen()
{
    return getOneLoopBits() * m_u32LoopCount;
}

u32 CFieldLoopContainer::getOneLoopBits()
{
    u32 u32LoopBits = 0;

    vector <CFieldBase *>::iterator iter1;

    for (iter1 = m_vecField.begin(); iter1 != m_vecField.end(); iter1++)
    {
        u32LoopBits += (*iter1)->bitlen();
    }

    return u32LoopBits;
}

u32 CFieldLoopContainer::getOneLoopFieldNum()
{
    return (u32)m_vecField.size();
}

u32 CFieldLoopContainer::parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset)
{
    u32 u32CurBitOffset = u32BitOffset;
    u32 u32CurFieldLen  = 0;

    u32 u32OneLoopBits  = getOneLoopBits();


    vector <CFieldBase *>::iterator iter1;

    while (u32CurBitOffset + u32OneLoopBits + m_u32LeftBit <= 8 * u32DataLen) // 32 CRC_32
    {
        for (iter1 = m_vecField.begin(); iter1 != m_vecField.end(); iter1++)
        {
            string name = (*iter1)->name();

            u32CurFieldLen = (*iter1)->bitlen();

            CFieldBase *pNewField = NULL;

            {
                // Need Refactor !!!

#if 0
                string className("CDescriptorLoopContainer");

                if (string("CDescriptorLoopContainer") == (*iter1)->className())
                    pNewField = new CDescriptorLoopContainer(name, u32CurFieldLen);
                else if (string("CDescriptorBase") == (*iter1)->className())
                    pNewField = new CDescriptorBase(name);
                else if (string("CMultipleStringStructureBase") == (*iter1)->className())
                    pNewField = new CMultipleStringStructureBase(name);
                else if (string("CMultipleStringStructureLoopContainer") == (*iter1)->className())
                    pNewField = new CMultipleStringStructureLoopContainer(name, u32CurFieldLen);
                else
                    pNewField = new CField(name, u32CurFieldLen);
#endif
                pNewField = CFieldFactory::Instance().CreateFieldInstance((*iter1)->clsName(), name, u32CurFieldLen, (*iter1));
            }

            if (NULL == pNewField)
            {
                // To be done ...
                // Thrown excption
            }

            pNewField->setOffset(u32CurBitOffset);

            m_vecLoopField.push_back(pNewField);

            u32CurBitOffset += pNewField->parserData(pData, u32DataLen, u32CurBitOffset);
        }

        m_u32LoopCount++;
    }

    return (u32CurBitOffset - u32BitOffset);
}

void CFieldLoopContainer::showData()
{
    u32 u32LoopCount = 0;
    const u32 u32FieldNum = getOneLoopFieldNum();

    vector <CFieldBase *>::iterator iter1 = m_vecLoopField.begin();

    switch (m_enType)
    {
        case EN_FIELDCONTAINER_TYPE_NAME_STR:
        {
            u32 len     = m_vecLoopField.size();
            u8 *pBuf    = new u8[len + 1];

            u32 count = 0;
            while (iter1 != m_vecLoopField.end())
            {
                 (*iter1)->getBuf(pBuf + count, 1);
                count++;
                iter1++;
            }

            pBuf[count] = 0x00;
            printf("TXT[%s] \n", pBuf);

            delete []pBuf;

            break;
        }

        case EN_FIELDCONTAINER_TYPE_COMMON:
        default:
        {
            printf("\n[%s] Total Loop %u Begin >>> \n", m_sName.data(), m_u32LoopCount);

            while (iter1 != m_vecLoopField.end())
            {
                if (0 == u32LoopCount % u32FieldNum)
                {
                    printf("\nThe %u Loop > \n", 1 + u32LoopCount / u32FieldNum);
                }

                (*iter1)->showData();

                u32LoopCount++;
                iter1++;
            }

            printf("\n[%s] Total Loop %u End >>> \n\n", m_sName.data(), m_u32LoopCount);

            break;
        }
    }
}



CFieldLoopNContainer::CFieldLoopNContainer(string name, u32 u32BitsLen) :  CFieldContainer(name)
{
    m_u32BitLen = u32BitsLen;

    m_pLoopNField = new CField(name, u32BitsLen);
    if (NULL == m_pLoopNField)
    {
        // To be done ...
        // Thrown excption
    }

    m_enFieldLoopNType = EN_FIELDLOOPN_TYPE_COMMON;
}

CFieldLoopNContainer::CFieldLoopNContainer(string name, u32 u32BitsLen, EN_FIELDLOOPN_TYPE type) :  CFieldContainer(name)
{
    m_u32BitLen = u32BitsLen;

    m_pLoopNField = new CField(name, u32BitsLen);
    if (NULL == m_pLoopNField)
    {
        // To be done ...
        // Thrown excption
    }

    m_enFieldLoopNType = type;
}


CFieldLoopNContainer::~CFieldLoopNContainer()
{
    vector <CFieldBase *>::iterator iter1;

    for (iter1 = m_vecLoopField.begin(); iter1 != m_vecLoopField.end(); iter1++)
    {
        delete (*iter1);
    }

    if (NULL != m_pLoopNField)
        delete m_pLoopNField;
}

u32 CFieldLoopNContainer::parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset)
{
    u32 u32CurBitOffset = u32BitOffset;
    u32 u32CurFieldLen  = 0;

    u32CurBitOffset += m_pLoopNField->parserData(pData, u32DataLen, u32CurBitOffset);

    // get Loop N
    u32 len     = m_pLoopNField->bitlen();
    u32 bytes   = (len + 7) / 8;
    u8 *pBuf    = new u8[bytes];

    m_pLoopNField->getBuf(pBuf, bytes);

    m_u32LoopN = Buf_2_u32(pBuf, bytes);

    delete[]pBuf;

    vector <CFieldBase *>::iterator iter1;

    for (u32 loop = 0; loop < m_u32LoopN; loop++)
    {
        for (iter1 = m_vecField.begin(); iter1 != m_vecField.end(); iter1++)
        {
            string name = (*iter1)->name();

            u32CurFieldLen = (*iter1)->bitlen();

            CFieldBase *pNewField = NULL;

            {
                // Need Refactor !!!
#if 0
                string className("CDescriptorLoopContainer");

                if (string("CDescriptorLoopContainer") == (*iter1)->className())
                    pNewField = new CDescriptorLoopContainer(name, u32CurFieldLen);
                else if (string("CDescriptorBase") == (*iter1)->className())
                    pNewField = new CDescriptorBase(name);
                else if (string("CMultipleStringStructureBase") == (*iter1)->className())
                    pNewField = new CMultipleStringStructureBase(name);
                else if (string("CMultipleStringStructureLoopContainer") == (*iter1)->className())
                    pNewField = new CMultipleStringStructureLoopContainer(name, u32CurFieldLen);
                else
                    pNewField = new CField(name, u32CurFieldLen);
#endif
                pNewField = CFieldFactory::Instance().CreateFieldInstance((*iter1)->clsName(), name, u32CurFieldLen, (*iter1));
            }

            if (NULL == pNewField)
            {
                // To be done ...
                // Thrown excption
            }

            pNewField->setOffset(u32CurBitOffset);

            m_vecLoopField.push_back(pNewField);

            u32CurBitOffset += pNewField->parserData(pData, u32DataLen, u32CurBitOffset);
        }
    }

    return (u32CurBitOffset - u32BitOffset);
}

void CFieldLoopNContainer::showData()
{
    u32 u32LoopCount = 0;

    vector <CFieldBase *>::iterator iter1 = m_vecLoopField.begin();

    m_pLoopNField->showData();

    switch (m_enFieldLoopNType)
    {
        case EN_FIELDLOOPN_TYPE_NAME_STR:
        {
            u32 len     = m_pLoopNField->bitlen();
            u32 bytes   = (len + 7) / 8;
            u8 *pBuf    = new u8[bytes];

            m_pLoopNField->getBuf(pBuf, bytes);

            u32LoopCount = Buf_2_u32(pBuf, bytes);

            delete []pBuf;

            pBuf = new u8[u32LoopCount + 1];

            u32 count = 0;
            while (iter1 != m_vecLoopField.end())
            {
                 (*iter1)->getBuf(pBuf + count, 1);
                count++;
                iter1++;
            }

            pBuf[count] = 0x00;
            printf("name[%s] \n", pBuf);

            delete []pBuf;

            break;
        }

        case EN_FIELDLOOPN_TYPE_COMMON:
        default:
        {
            printf("\n[%s] Total Loop %u Begin >>> \n", m_sName.data(), m_u32LoopN);

            while (iter1 != m_vecLoopField.end())
            {
                if (0 == u32LoopCount % m_vecField.size())
                {
                    printf("\nThe %u Loop > \n", 1 + u32LoopCount / m_vecField.size());
                }

                (*iter1)->showData();

                u32LoopCount++;
                iter1++;
            }

            printf("\n[%s] Total Loop %u End >>> \n\n", m_sName.data(), m_u32LoopN);

            break;
        }
    }
}

