/*******************************************************************************
 * Copyright (C) 2014 The DTVKit Open Software Foundation Ltd (www.dtvkit.org)
 * Copyright (C) 2004 Ocean Blue Software Ltd
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
 * @brief   Debug functions header file
 *
 * @file    dbgfuncs.h
 * @date    01/12/2004
 * @author  Ocean Blue
 */

// pre-processor mechanism so multiple inclusions don't cause compilation error
#ifndef _DBGFUNCS_H
#define _DBGFUNCS_H

// uncomment this for assert prints
//#define DEBUG_ASSERT

// uncomment this for standard ANSI C assertions
//#define DEBUG_ASSERT_STANDARD

// uncomment this for func start/end messages
//#define DEBUG_FUNCTION

#ifdef DEBUG_ASSERT_STANDARD
#undef assert
#include <assert.h>
#endif

// function profiling macros
#ifdef DEBUG_FUNCTIONS
#include "stbhwc.h"
#define FUNCTION_START(x) STB_SPDebugWrite("START: " # x)
#define FUNCTION_FINISH(x) STB_SPDebugWrite("END : " # x)
#else
#define FUNCTION_START(x)
#define FUNCTION_FINISH(x)
#endif

// assertion macros
#ifdef DEBUG_ASSERT
#include "stbhwc.h"
#define ASSERT(assertion) if (!(assertion)) { STB_SPDebugAssertFail(__FILE__, __LINE__, # assertion); }
#else // DEBUG_ASSERT
#ifdef DEBUG_ASSERT_STANDARD
#define ASSERT(assertion) assert(assertion)
#else // DEBUG_ASSERT_STANDARD
#define ASSERT(assertion)
#endif // DEBUG_ASSERT_STANDARD
#endif // DEBUG_ASSERT
//Add
//#define DEBUG_PRINTING_ENABLED
#if defined(DEBUG_PRINTING_ENABLED)
#include "stbhwc.h"
#if !defined(NO_FUNC_ID)
#define DBGPRINT(x, ...) STB_SPDebugWrite( "%s:%d " x, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#else // compiler does not support __FUNCTION__
#define DBGPRINT(x, ...) STB_SPDebugWrite( "%s:%d " x, __FILE__, __LINE__, ##__VA_ARGS__);
#endif
#else /*!DEBUG_PRINTING_ENABLED*/
#define DBGPRINT(...)
#endif /*DEBUG_PRINTING_ENABLED*/

#define DBG(x, ...) STB_SPDebugWrite("=>%s:%d " x, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif // _DBGFUNCS_H

