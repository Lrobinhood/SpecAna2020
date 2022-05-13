/*
 * Copyright (C) 2020 Amlogic Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

// Ocean Blue Software header files
#include "techtype.h"
#include "dbgfuncs.h"
//#include "stbhwos.h"

// CERT Module header files
//#include "cert_log.h"
#ifdef __cplusplus
}
#endif

#include "dvb_bitparser.h"


BOOLEAN static inline fillReservoir(DVB_BITPARSER * bit_parser)
{
    if (bit_parser->data_size == 0) {
        bit_parser->b_over_read = true;
        return FALSE;
    }

    bit_parser->num_reservoir = 0;
    U8BIT i;
    for (i = 0; bit_parser->data_size > 0 && i < 4; ++i) {
        bit_parser->num_reservoir = (bit_parser->num_reservoir << 8) | *bit_parser->data_buf;

        ++bit_parser->data_buf;
        --bit_parser->data_size;
    }

    bit_parser->num_bitsleft = 8 * i;
    bit_parser->num_reservoir <<= 32 - bit_parser->num_bitsleft;
    return TRUE;
}

BOOLEAN static inline getBitsGraceful(DVB_BITPARSER * bit_parser, U32BIT n, U32BIT *out)
{
    if (n > 32) {
        return FALSE;
    }

    U32BIT result = 0;
    while (n > 0) {
        if (bit_parser->num_bitsleft == 0) {
            if (!fillReservoir(bit_parser)) {
                return FALSE;
            }
        }

        U32BIT m = n;
        if (m > bit_parser->num_bitsleft) {
            m = bit_parser->num_bitsleft;
        }

        result = (result << m) | (bit_parser->num_reservoir >> (32 - m));
        bit_parser->num_reservoir <<= m;
        bit_parser->num_bitsleft -= m;

        n -= m;
    }

    *out = result;
    return TRUE;
}



U32BIT DVB_BitParserInit(DVB_BITPARSER * bit_parser, const U8BIT *buf, U32BIT size)
{
    if (bit_parser == NULL ||
        buf == NULL ||
        size == 0) {
        //ALOGE("%s invalid parameter", __FUNCTION__);
        return -1;
    }

    bit_parser->data_buf       = buf;
    bit_parser->data_size      = size;
    bit_parser->num_reservoir  = 0;
    bit_parser->num_bitsleft   = 0;
    bit_parser->b_over_read   = FALSE;


    return 0;
}

U32BIT DVB_BitParserDeinit(DVB_BITPARSER * bit_parser)
{
    bit_parser->data_buf       = NULL;
    bit_parser->data_size      = 0;
    bit_parser->num_reservoir  = 0;
    bit_parser->num_bitsleft   = 0;
    bit_parser->b_over_read   = FALSE;

    return 0;
}

U32BIT DVB_BitParserGetBits(DVB_BITPARSER * bit_parser, U32BIT n)
{
    U32BIT ret = 0;
    getBitsGraceful(bit_parser, n, &ret);
    return ret;
}


BOOLEAN DVB_BitParserSkipBits(DVB_BITPARSER * bit_parser, U32BIT n)
{
    U32BIT dummy;
    while (n > 32) {
        if (!getBitsGraceful(bit_parser, 32, &dummy)) {
            return false;
        }
        n -= 32;
    }

    if (n > 0) {
        return getBitsGraceful(bit_parser, n, &dummy);
    }
    return TRUE;
}

void DVB_BitParserPutBits(DVB_BITPARSER * bit_parser, U32BIT x, U32BIT n)
{
    if (bit_parser->b_over_read) {
        return;
    }

    while (bit_parser->num_bitsleft + n > 32) {
        bit_parser->num_bitsleft -= 8;
        --bit_parser->data_buf;
        ++bit_parser->data_size;
    }

    bit_parser->num_reservoir = (bit_parser->num_reservoir >> n) | (x << (32 - n));
    bit_parser->num_bitsleft += n;
    return;
}


