/*******************************************************************************
 * Copyright ?2014 The DTVKit Open Software Foundation Ltd (www.dtvkit.org)
 * Copyright ?2006 Ocean Blue Software Ltd
 *
 * This file is part of a DTVKit Software Component
 * You are permitted to copy, modify or distribute this file subject to the terms
 * of the DTVKit 1.0 Licence which can be found in licence.txt or at www.dtvkit.org
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * If you or your organisation is not a member of DTVKit then you have access
 * to this source code outside of the terms of the licence agreement
 * and you are expected to delete this and any associated files immediately.
 * Further information on DTVKit, membership and terms can be found at www.dtvkit.org
 *******************************************************************************/
/**
 * @brief   System Wide Global Technical Data Type Definitions.
 * @file    techtype.h
 * @date    August 2006
 * @author  Paul Marshall
 */

#ifndef _TECHTYPE_H
#define _TECHTYPE_H

#ifndef NO_STDINT_H
#include <stddef.h>
#include <stdint.h>
#endif

/****************************************************************************
 *  System MACRO Definitions
 ****************************************************************************/

#ifndef USE_UNWANTED_PARAM
/* MACRO TO PREVENT "parameter not used" WARNING                           */
/* In some cases, function parameter lists are pre-defined and cannot be   */
/* changed, even though the parameters are not used.  Such cases produce   */
/* numerous unnecessary warnings which make it difficult to spot important */
/* warnings.  This macro can be used in such circumstances to fool the     */
/* compiler into thinking the function parameter is used without creating  */
/* unwanted code.                                                          */
#ifdef NDEBUG
#define USE_UNWANTED_PARAM(param)
#else
#define USE_UNWANTED_PARAM(param) param = param
#endif
#endif

/****************************************************************************
 *  Remove CONSTANT Definitions
 ****************************************************************************/

#undef FALSE
#undef TRUE
#undef loop
#ifdef NO_STDINT_H
#undef NULL
#undef NULL_PTR
#endif

/****************************************************************************
 *  System CONSTANT Definitions
 ****************************************************************************/

#define  FALSE         0
#define  TRUE          1

/* Generic NULL Definition */
#ifdef NO_STDINT_H
#define  NULL          0
#define  NULL_PTR      ((void *)NULL)
#endif

/****************************************************************************
 *  System DATA TYPE SIZE Definitions
 ****************************************************************************/

#ifndef NO_STDINT_H
typedef uint8_t U8BIT;
typedef int8_t S8BIT;
typedef uint16_t U16BIT;
typedef int16_t S16BIT;
typedef uint32_t U32BIT;
typedef int32_t S32BIT;
typedef uint64_t U64BIT;
#else
typedef unsigned char U8BIT;
typedef unsigned short U16BIT;
typedef signed char S8BIT;
typedef signed short S16BIT;
typedef unsigned long U32BIT;
typedef signed long S32BIT;
typedef unsigned long long U64BIT;
#endif

typedef void *PVOID;
typedef U8BIT *PU8BIT;
typedef U8BIT BOOLEAN;           /* BOOLEAN as 1 byte */


#ifdef _WIN32
#define M_PI 3.1415926535
#define true 1
#define false 0

#define STB_FreeMemory free
#define STB_GetMemory malloc
#define TIMEOUT_NEVER 0
#endif
/* The correct language pointer definitions should always match  */
/* the type of data being used.  For example, 'char*' for text   */
/* strings and 'unsigned char*' for data byte arrays.            */

#endif  /* _TECHTYPE_H */
