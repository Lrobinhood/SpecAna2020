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

#ifndef _DVB_BITPARSER_H
#define _DVB_BITPARSER_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    const U8BIT *data_buf;
    U32BIT data_size;
    U32BIT num_reservoir;  // left-aligned bits
    U32BIT num_bitsleft;
    BOOLEAN b_over_read;
}DVB_BITPARSER;

U32BIT DVB_BitParserInit(DVB_BITPARSER * bit_parser, const uint8_t *buf, U32BIT size);

U32BIT DVB_BitParserDeinit(DVB_BITPARSER * bit_parser);

U32BIT DVB_BitParserGetBits(DVB_BITPARSER * bit_parser, U32BIT n);

BOOLEAN DVB_BitParserSkipBits(DVB_BITPARSER * bit_parser, U32BIT n);

#ifdef __cplusplus
}
#endif


#endif
