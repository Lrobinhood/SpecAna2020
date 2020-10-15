/*********************************************************************
 
 
 
 
        File:       FieldBase.cpp
        
        Author:     Robin
        Date:       2017-06-07
 
 
**********************************************************************/

#include "stdafx.h"

#include "FieldBase.h"




u32 CFieldBase::bitlen()
{
    return m_u32BitLen;
}


void CFieldBase::setOffset(u32 u32Offset)
{
    m_u32Offset = u32Offset;

}   

u32 CFieldBase::offset()
{
    return m_u32Offset;
} 


void CFieldBase::getBuf(u8 *pBuf, u32 BufLen)
{
    // To be done ...

    printf("\n [Func:%s, Line;%d] ERR should not be here, not support !!! \n", __FUNCTION__, __LINE__);

}
  
