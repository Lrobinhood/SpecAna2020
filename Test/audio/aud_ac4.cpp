/*******************************************************************************
 *  Copyright @ 2020
 *
 *  audio ac-4 related functions.
 *
 *******************************************************************************/
/**
 * @brief   audio ac-4 related functions.
 * @file
 * @date    2021-08-25
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef __cplusplus
extern "C" {
#endif

// Ocean Blue Software header files
#include "techtype.h"
#include "dbgfuncs.h"
//#include "stbhwos.h"
//#include "stberc.h"
//#include "stbpes.h"
//#include "stbheap.h"

// CERT Module header files
#include "cert_log.h"
#ifdef __cplusplus
}
#endif

//#include "aud_ac4.h"
#include <memory.h>

#include "dvb_bitparser.h"

/*-----------------------------------------------------------------------------*/
/* Local Constant Definitions                                                  */
/*-----------------------------------------------------------------------------*/
#define AUD_AC4_TASK_QUEUE_NUM            50
#define AUD_AC4_TASK_PRIORITY             10
#define AUD_AC4_TASK_STACK_SIZE           4096

#define NUM_TRIM_CONFIGS                  9
#define NUM_SUBSTREAM                     16

#define AUD_AC4_INFO(...) CERT_LOG_INFO("AUD_AC4", __VA_ARGS__)

/*-----------------------------------------------------------------------------*/
/* Local Types Declarations                                                    */
/*-----------------------------------------------------------------------------*/
typedef struct
{
    U8BIT bitstream_version;
    U8BIT fs_index;
    U8BIT frame_rate_index;
    U8BIT frame_rate_factor;
    U8BIT payload_base;
    U8BIT n_substreams;
    U16BIT substream_size[NUM_SUBSTREAM];
    U32BIT size;
}TOCINFO;

typedef struct
{
    U8BIT presentation_config;
} PRESENTATION_INFO;

/*-----------------------------------------------------------------------------*/
/* Extern Global Variables                                                     */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Extern Function Prototype                                                   */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Local Function Protype                                                      */
/*-----------------------------------------------------------------------------*/
static void AUD_AC4_Task(void* pParam);
static void AC4PesCallback(U32BIT handle, U8BIT data_identifier, void *buffer_ptr, U32BIT buffer_size);
static U32BIT ReadVariableBits(DVB_BITPARSER * pstBitParser, U32BIT nbits);

BOOLEAN AC4ParsePesHeader(U8BIT *pes_ptr);
static BOOLEAN AC4ParseSyncFrame(DVB_BITPARSER * pstBitParser);
static BOOLEAN AC4ParseRawFrame(DVB_BITPARSER * pstBitParser, U32BIT u32frame_size);
static BOOLEAN AC4ParseTOC(DVB_BITPARSER * pstBitParser, TOCINFO * pstTocInfo);
static BOOLEAN AC4ParsePresentationInfo(DVB_BITPARSER * pstBitParser);
static BOOLEAN AC4ParsePresentationV1Info(DVB_BITPARSER * pstBitParser);
static BOOLEAN AC4ParseSubstreamGroupInfo(DVB_BITPARSER * pstBitParser);
static BOOLEAN AC4ParsePresentationV1Info(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo);
static BOOLEAN AC4ParseSubstreamGroupInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo);
static BOOLEAN AC4ParsePresentationV1Info(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo);
static BOOLEAN AC4ParsePresentationInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo);

/*-----------------------------------------------------------------------------*/
/* Local Global Variables                                                      */
/*-----------------------------------------------------------------------------*/
static void* gpAC4EventQueue = NULL;
static void* gpAC4TaskHandle = NULL;
static void* gpAC4MutexHandle = NULL;

/*-----------------------------------------------------------------------------*/
/* Debug Variables & Functions                                                 */
/*-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
/* Interface Functions                                                         */
/*-----------------------------------------------------------------------------*/

#if 0
BOOLEAN AUD_AC4_Init(void)
{
    gpAC4TaskHandle = STB_OSCreateTask(AUD_AC4_Task, NULL, AUD_AC4_TASK_STACK_SIZE, AUD_AC4_TASK_PRIORITY, (U8BIT *)"aud_ac4_task");
    gpAC4EventQueue = STB_OSCreateQueue(sizeof(U8BIT *), AUD_AC4_TASK_QUEUE_NUM);
    gpAC4MutexHandle = STB_OSCreateMutex();
    STB_RegisterPesCollectionCallback(AC4PesCallback, 0xAC, 0xAC);

    return TRUE;
}
#endif

/*-----------------------------------------------------------------------------*/
/* Local Functions                                                             */
/*-----------------------------------------------------------------------------*/
#if 0
static void AC4PesCallback(U32BIT handle, U8BIT data_identifier, void *buffer_ptr, U32BIT buffer_size)
{
    void *buffer;
    BOOLEAN success;

    AUD_AC4_INFO("[%s, %d] handle, id, buff_ptr, buff_size: 0x%x, 0x%x, %p, %d.", __FUNCTION__, __LINE__, handle, data_identifier, buffer_ptr, buffer_size);

    if (9 >= buffer_size) {
        AUD_AC4_INFO("[%s, %d] invalid PES.", __FUNCTION__, __LINE__);
        return;
    }

    buffer = STB_GetMemory(buffer_size);

    if (buffer != NULL)
    {
        memcpy(buffer, buffer_ptr, buffer_size);

        success = STB_OSWriteQueue(gpAC4EventQueue, (void *)&buffer, sizeof(U8BIT *), TIMEOUT_NOW);

        if (!success)
        {
            AUD_AC4_INFO("Failed to add pes buffer to AC4 queue");
            STB_FreeMemory(buffer);
        }
    }
}
#endif

static void printPresentationInfo(U8BIT *pes_ptr, U16BIT length)
{
    U16BIT index = 0;

    while ((index < 560) && (index < length)) {
        AUD_AC4_INFO("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
            pes_ptr[index + 0], pes_ptr[index + 1], pes_ptr[index + 2], pes_ptr[index + 3],
            pes_ptr[index + 4], pes_ptr[index + 5], pes_ptr[index + 6], pes_ptr[index + 7],
            pes_ptr[index + 8], pes_ptr[index + 9], pes_ptr[index +10], pes_ptr[index +11],
            pes_ptr[index +12], pes_ptr[index +13], pes_ptr[index +14], pes_ptr[index +15]);
        index += 16;
    }
}

/*
13818-1
2.4.3.7 Semantic definition of fields in PES packet
*/
BOOLEAN AC4ParsePesHeader(U8BIT *pes_ptr)
{
    U16BIT u16offset = 0;
    U16BIT u16PES_packet_length = 0;
    U8BIT u8PTS_DTS_flags = 0;
    U8BIT u8ESCR_flag = 0;
    U8BIT u8ES_rate_flag = 0;
    U8BIT u8DSM_trick_mode_flag = 0;
    U8BIT u8additional_copy_info_flag = 0;
    U8BIT u8PES_CRC_flag = 0;
    U8BIT u8PES_extension_flag = 0;
    U8BIT u8PES_header_data_length = 0;
    DVB_BITPARSER stBitParser;

    memset(&stBitParser, 0x00, sizeof(DVB_BITPARSER));

    AUD_AC4_INFO("[%s, %d] prefix: 0x%x:%x:%x.", __FUNCTION__, __LINE__, pes_ptr[0], pes_ptr[1], pes_ptr[2]);
    u16offset += 3; //packet_start_code_prefix

    AUD_AC4_INFO("[%s, %d] stream_id: 0x%x", __FUNCTION__, __LINE__, pes_ptr[u16offset]);
    u16offset += 1; //stream_id: 0xBD

    u16PES_packet_length = pes_ptr[u16offset]<<8 | pes_ptr[u16offset+1];
    AUD_AC4_INFO("[%s, %d] u16PES_packet_length: 0x%x", __FUNCTION__, __LINE__, u16PES_packet_length);
    u16offset += 2; //PES_packet_length

    if (u16PES_packet_length == 0x768) {
        printPresentationInfo(pes_ptr, u16PES_packet_length + 6);
    }

    DVB_BitParserInit(&stBitParser, pes_ptr + u16offset, u16PES_packet_length);

    AUD_AC4_INFO("[%s, %d] 1st byte: 0x%x", __FUNCTION__, __LINE__, DVB_BitParserGetBits(&stBitParser, 8));
    u8PTS_DTS_flags = DVB_BitParserGetBits(&stBitParser, 2);
    u8ESCR_flag = DVB_BitParserGetBits(&stBitParser, 1);
    u8ES_rate_flag = DVB_BitParserGetBits(&stBitParser, 1);
    u8DSM_trick_mode_flag = DVB_BitParserGetBits(&stBitParser, 1);
    u8additional_copy_info_flag = DVB_BitParserGetBits(&stBitParser, 1);
    u8PES_CRC_flag = DVB_BitParserGetBits(&stBitParser, 1);
    u8PES_extension_flag = DVB_BitParserGetBits(&stBitParser, 1);

    AUD_AC4_INFO("[%s, %d] : %d, %d, %d, %d, %d, %d, %d.", __FUNCTION__, __LINE__,
        u8PTS_DTS_flags, u8ESCR_flag, u8ES_rate_flag, u8DSM_trick_mode_flag, u8additional_copy_info_flag, u8PES_CRC_flag, u8PES_extension_flag);

    u8PES_header_data_length = DVB_BitParserGetBits(&stBitParser, 8);
    AUD_AC4_INFO("[%s, %d] u8PES_header_data_length: 0x%x", __FUNCTION__, __LINE__, u8PES_header_data_length);

    if (2 == u8PTS_DTS_flags) {
        DVB_BitParserSkipBits(&stBitParser, 5 * 8);
    }
    else if (3 == u8PTS_DTS_flags){
        DVB_BitParserSkipBits(&stBitParser, 10 * 8);
    }

    if (1 == u8ESCR_flag) {
        DVB_BitParserSkipBits(&stBitParser, 6 * 8);
    }

    if (1 == u8ES_rate_flag) {
        DVB_BitParserSkipBits(&stBitParser, 3 * 8);
    }

    if (1 == u8DSM_trick_mode_flag) {
        DVB_BitParserSkipBits(&stBitParser, 8);
    }

    if (1 == u8additional_copy_info_flag) {
        DVB_BitParserSkipBits(&stBitParser, 8);
    }

    if (1 == u8PES_CRC_flag) {
        DVB_BitParserSkipBits(&stBitParser, 2 * 8);
    }

    if (1 == u8PES_extension_flag) {
        U8BIT u8PES_private_data_flag = DVB_BitParserGetBits(&stBitParser, 1);
        U8BIT u8pack_header_field_flag = DVB_BitParserGetBits(&stBitParser, 1);
        U8BIT u8program_packet_sequence_counter_flag = DVB_BitParserGetBits(&stBitParser, 1);
        U8BIT u8PSTD_buffer_flag = DVB_BitParserGetBits(&stBitParser, 1);
        DVB_BitParserSkipBits(&stBitParser, 3);
        U8BIT u8PES_extension_flag_2 = DVB_BitParserGetBits(&stBitParser, 1);

        AUD_AC4_INFO("[%s, %d] : %d, %d, %d, %d, %d.", __FUNCTION__, __LINE__,
            u8PES_private_data_flag, u8pack_header_field_flag, u8program_packet_sequence_counter_flag, u8PSTD_buffer_flag, u8PES_extension_flag_2);

        if (1 == u8PES_private_data_flag) {
            DVB_BitParserSkipBits(&stBitParser, 128 * 8);
        }

        if (1 == u8pack_header_field_flag) {
            U8BIT u8pack_field_length = DVB_BitParserGetBits(&stBitParser, 8);
            DVB_BitParserSkipBits(&stBitParser, u8pack_field_length * 8);
        }

        if (1 == u8program_packet_sequence_counter_flag) {
            DVB_BitParserSkipBits(&stBitParser, 2 * 8);
        }

        if (1 == u8PSTD_buffer_flag) {
            DVB_BitParserSkipBits(&stBitParser, 2 * 8);
        }

        if (1 == u8PES_extension_flag_2) {
            DVB_BitParserSkipBits(&stBitParser, 1);
            U8BIT u8PES_extension_field_length = DVB_BitParserGetBits(&stBitParser, 7);
            DVB_BitParserSkipBits(&stBitParser, u8PES_extension_field_length * 8);
        }
    }

    AC4ParseSyncFrame(&stBitParser);

    return TRUE;
}

/*
ETSI TS 103 190-1 V1.3.1
Annex G (normative): AC-4 Sync Frame
*/
static BOOLEAN AC4ParseSyncFrame(DVB_BITPARSER * pstBitParser)
{
    U16BIT u16sync_word = 0;
    U32BIT u32frame_size = 0;
    U32BIT crc_word = 0;

    u16sync_word = DVB_BitParserGetBits(pstBitParser, 2 * 8);
    AUD_AC4_INFO("[%s, %d] u16sync_word: 0x%x", __FUNCTION__, __LINE__, u16sync_word);

    u32frame_size = DVB_BitParserGetBits(pstBitParser, 2 * 8);
    AUD_AC4_INFO("[%s, %d] u32frame_size: 0x%x", __FUNCTION__, __LINE__, u32frame_size);

    if (0xFFFF == u32frame_size) {
        u32frame_size = DVB_BitParserGetBits(pstBitParser, 3 * 8);
    }

    AUD_AC4_INFO("[%s, %d] u32frame_size: 0x%x", __FUNCTION__, __LINE__, u32frame_size);

    AC4ParseRawFrame(pstBitParser, u32frame_size);

    if (0xAC41 == u16sync_word) {
        crc_word = DVB_BitParserGetBits(pstBitParser, 2 * 8);
    }

    AUD_AC4_INFO("[%s, %d] crc_word: 0x%x", __FUNCTION__, __LINE__, crc_word);

    return TRUE;
}

/*
ETSI TS 103 190-1 V1.3.1
4.2.1 raw_ac4_frame - Raw AC-4 frame
*/
static BOOLEAN AC4ParseRawFrame(DVB_BITPARSER * pstBitParser, U32BIT u32frame_size)
{
    TOCINFO stTocInfo;
    U32BIT frameLeft = 0;

    memset(&stTocInfo, 0x00, sizeof(TOCINFO));

    AC4ParseTOC(pstBitParser, &stTocInfo);

    for (U8BIT i = 0; i < stTocInfo.n_substreams; i++) {
        //ac4_substream_data(); /* sub stream data stays byte aligned */
    }

    frameLeft = u32frame_size - stTocInfo.size;
    DVB_BitParserSkipBits(pstBitParser, frameLeft * 8);
    AUD_AC4_INFO("[%s, %d] u32frame_size: 0x%x", __FUNCTION__, __LINE__, u32frame_size);
    AUD_AC4_INFO("[%s, %d] stTocInfo.size: 0x%x", __FUNCTION__, __LINE__, stTocInfo.size);

    return TRUE;
}

/*
ETSI TS 103 190-1 V1.3.1
4.2.2 variable_bits - Variable bits
*/
static U32BIT ReadVariableBits(DVB_BITPARSER * pstBitParser, U32BIT nbits)
{
    BOOLEAN b_read_more = TRUE;
    U32BIT u32Value = 0;

    while(b_read_more) {
        u32Value += DVB_BitParserGetBits(pstBitParser, nbits);
        b_read_more = DVB_BitParserGetBits(pstBitParser, 1);
        if (!b_read_more) {
            break;
        }
        u32Value++;
        u32Value <<= nbits;
    }

    return u32Value;
}


static BOOLEAN substreamIndexTable(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    U8BIT n_substreams = DVB_BitParserGetBits(pstBitParser, 2);
    BOOLEAN b_size_present = FALSE;
    BOOLEAN b_more_bits = FALSE;

    if (0 == n_substreams) {
        n_substreams = ReadVariableBits(pstBitParser, 2) + 4;
    }

    AUD_AC4_INFO("[%s, %d] n_substreams: %d", __FUNCTION__, __LINE__, n_substreams);

    if (n_substreams > NUM_SUBSTREAM) {
        n_substreams = NUM_SUBSTREAM;
    }

    ptocInfo->n_substreams = n_substreams;

    if (1 == n_substreams) {
        b_size_present = DVB_BitParserGetBits(pstBitParser, 1);
    }
    else {
        b_size_present = 1;
    }

    if (b_size_present) {
        for (U8BIT s = 0; s < n_substreams; s++) {
            b_more_bits = DVB_BitParserGetBits(pstBitParser, 1);
            ptocInfo->substream_size[s] = DVB_BitParserGetBits(pstBitParser, 10);
            if (b_more_bits) {
                ptocInfo->substream_size[s] += (ReadVariableBits(pstBitParser, 2) << 10);
            }
        }
    }

    return TRUE;
}

/*
ETSI TS 103 190-1 V1.3.1
4.2.3.1 ac4_toc - AC-4 table of contents

ETSI TS 103 190-2 V1.2.1
6.2.1.1 ac4_toc
*/
static BOOLEAN AC4ParseTOC(DVB_BITPARSER * pstBitParser, TOCINFO * pstTocInfo)
{
    U8BIT u8bitstream_version = 0;
    BOOLEAN b_wait_frames = FALSE;
    BOOLEAN b_single_presentation = FALSE;
    U8BIT u8n_presentations = 0;
    U8BIT group_index = 0;
    U16BIT u16payload_base = 0;
    BOOLEAN b_payload_base = FALSE;
    //TOCINFO stTocInfo;
    U32BIT dataSize = pstBitParser->data_size;
    U8BIT byte_align = 0;

    AUD_AC4_INFO("[%s, %d] pstBitParser->data_size: %d", __FUNCTION__, __LINE__, dataSize);

    //memset(&stTocInfo, 0x00, sizeof(TOCINFO));
    u8bitstream_version = DVB_BitParserGetBits(pstBitParser, 2);
    AUD_AC4_INFO("[%s, %d] u8bitstream_version: %d", __FUNCTION__, __LINE__, u8bitstream_version);

    if (3 == u8bitstream_version) {
        u8bitstream_version += ReadVariableBits(pstBitParser, 2);
    }

    AUD_AC4_INFO("[%s, %d] u8bitstream_version: %d", __FUNCTION__, __LINE__, u8bitstream_version);
    pstTocInfo->bitstream_version = u8bitstream_version;

    DVB_BitParserSkipBits(pstBitParser, 10);//sequence_counter
    b_wait_frames = DVB_BitParserGetBits(pstBitParser, 1);//b_wait_frames
    AUD_AC4_INFO("[%s, %d] b_wait_frames: %d", __FUNCTION__, __LINE__, b_wait_frames);
    if (b_wait_frames) {
        U8BIT u8wait_frames = DVB_BitParserGetBits(pstBitParser, 3);//wait_frames
        AUD_AC4_INFO("[%s, %d] u8wait_frames: %d", __FUNCTION__, __LINE__, u8wait_frames);
        if (u8wait_frames > 0) {
            DVB_BitParserSkipBits(pstBitParser, 2);//reserved, br_code
        }
    }

    U8BIT fs_index = DVB_BitParserGetBits(pstBitParser, 1);//fs_index
    U8BIT frame_rate_index = DVB_BitParserGetBits(pstBitParser, 4);//frame_rate_index
    DVB_BitParserSkipBits(pstBitParser, 1);//b_iframe_global
    b_single_presentation = DVB_BitParserGetBits(pstBitParser, 1);//b_single_presentation
    pstTocInfo->frame_rate_index = frame_rate_index;
    pstTocInfo->fs_index = fs_index;

    AUD_AC4_INFO("[%s, %d] b_single_presentation: %d", __FUNCTION__, __LINE__, b_single_presentation);
    if (b_single_presentation) {
        u8n_presentations = 1;
    }
    else {
        BOOLEAN b_more_presentations = DVB_BitParserGetBits(pstBitParser, 1);//b_more_presentations
        AUD_AC4_INFO("[%s, %d] b_more_presentations: %d", __FUNCTION__, __LINE__, b_more_presentations);
        if (b_more_presentations) {
            u8n_presentations = ReadVariableBits(pstBitParser, 2) + 2;
        }
        else {
            u8n_presentations = 0;
        }
    }
    AUD_AC4_INFO("[%s, %d] u8n_presentations: %d", __FUNCTION__, __LINE__, u8n_presentations);

    b_payload_base = DVB_BitParserGetBits(pstBitParser, 1);//b_payload_base
    AUD_AC4_INFO("[%s, %d] b_payload_base: %d", __FUNCTION__, __LINE__, b_payload_base);
    if (b_payload_base) {
        U8BIT u8payload_base_minus1 = DVB_BitParserGetBits(pstBitParser, 5);//payload_base_minus1
        u16payload_base = u8payload_base_minus1 + 1;
        AUD_AC4_INFO("[%s, %d] u8payload_base_minus1: %d", __FUNCTION__, __LINE__, u8payload_base_minus1);
        if (0x20 == u16payload_base) {
            u16payload_base += ReadVariableBits(pstBitParser, 3);
        }
    }

    pstTocInfo->payload_base = u16payload_base;

    AUD_AC4_INFO("[%s, %d] u16payload_base: %d", __FUNCTION__, __LINE__, u16payload_base);

    if (u8bitstream_version <= 1) {
        for (U8BIT i = 0; i < u8n_presentations; ++i) {
            AC4ParsePresentationInfo(pstBitParser, pstTocInfo);
        }
    }
    else {
        BOOLEAN b_program_id = DVB_BitParserGetBits(pstBitParser, 1);//b_program_id
        U8BIT u8total_n_substream_groups = 0;
        AUD_AC4_INFO("[%s, %d] b_program_id: %d", __FUNCTION__, __LINE__, b_program_id);
        if (b_program_id) {
            U16BIT u16short_program_id = DVB_BitParserGetBits(pstBitParser, 2 * 8);//short_program_id
            BOOLEAN b_program_uuid_present = DVB_BitParserGetBits(pstBitParser, 1);//b_program_uuid_present
            AUD_AC4_INFO("[%s, %d] u16short_program_id: %d", __FUNCTION__, __LINE__, u16short_program_id);
            AUD_AC4_INFO("[%s, %d] b_program_uuid_present: %d", __FUNCTION__, __LINE__, b_program_uuid_present);
            if (b_program_uuid_present) {
                DVB_BitParserSkipBits(pstBitParser, 16 * 8);//program_uuid
            }
        }

        for (U8BIT i = 0; i < u8n_presentations; ++i) {
            group_index = AC4ParsePresentationV1Info(pstBitParser, pstTocInfo);
            if (u8total_n_substream_groups < group_index) {
                u8total_n_substream_groups = group_index;
            }
        }
        u8total_n_substream_groups++;

        AUD_AC4_INFO("[%s, %d] u8total_n_substream_groups: %d", __FUNCTION__, __LINE__, u8total_n_substream_groups);

        for (U8BIT i = 0; i < u8total_n_substream_groups; ++i) {
            AC4ParseSubstreamGroupInfo(pstBitParser, pstTocInfo);
        }
    }

    substreamIndexTable(pstBitParser, pstTocInfo);

    AUD_AC4_INFO("[%s, %d] pstBitParser->data_size: %d", __FUNCTION__, __LINE__, pstBitParser->data_size);
    AUD_AC4_INFO("[%s, %d] TOC size: %d", __FUNCTION__, __LINE__, dataSize - pstBitParser->data_size);

    byte_align = (dataSize - pstBitParser->data_size) % 8;

    pstTocInfo->size = (dataSize - pstBitParser->data_size) + byte_align;

    AUD_AC4_INFO("[%s, %d] byte_align: %d", __FUNCTION__, __LINE__, byte_align);
    AUD_AC4_INFO("[%s, %d] pstTocInfo->size: %d", __FUNCTION__, __LINE__, pstTocInfo->size);

    DVB_BitParserSkipBits(pstBitParser, byte_align * 8);

    return TRUE;
}

/*
ETSI TS 103 190-1 V1.3.1
4.2.3.3 presentation_version - Presentation version information
*/
static U8BIT PresentationVersion(DVB_BITPARSER * pstBitParser)
{
    U8BIT value = 0;
    BOOLEAN b_tmp = DVB_BitParserGetBits(pstBitParser, 1);//b_tmp

    while (b_tmp) {
        value++;
        b_tmp = DVB_BitParserGetBits(pstBitParser, 1);//b_tmp
    }

    AUD_AC4_INFO("[%s, %d] PresentationVersion: %d", __FUNCTION__, __LINE__, value);

    return value;
}

/*
ETSI TS 103 190-1 V1.3.1
4.2.3.4 frame_rate_multiply_info - Frame rate multiplier information
*/
static U8BIT frameRateMultiplyInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    BOOLEAN b_multiplier = FALSE;
    BOOLEAN multiplier_bit = FALSE;
    U8BIT frame_rate_factor = 1;

    switch (ptocInfo->frame_rate_index) {
        case 2:
        case 3:
        case 4:
            b_multiplier = DVB_BitParserGetBits(pstBitParser, 1);//b_multiplier;
            if (b_multiplier) {
                multiplier_bit = DVB_BitParserGetBits(pstBitParser, 1);//multiplier_bit;
                if (multiplier_bit) {
                    frame_rate_factor = 4;
                }
                else {
                    frame_rate_factor = 2;
                }
            }
            break;

        case 0:
        case 1:
        case 7:
        case 8:
        case 9:
            b_multiplier = DVB_BitParserGetBits(pstBitParser, 1);//b_multiplier;
            if (b_multiplier) {
                frame_rate_factor = 2;
            }
            break;

        default:
            break;
    }

    return frame_rate_factor;
}

/*
ETSI TS 103 190-1 V1.3.1
4.2.3.10 emdf_payloads_substream_info - EMDF payloads substream information
*/
static U8BIT emdfPayloadsSubstreamInfo(DVB_BITPARSER * pstBitParser)
{
    U8BIT substream_index = DVB_BitParserGetBits(pstBitParser, 2);//substream_index;
    if (3 == substream_index) {
        substream_index += ReadVariableBits(pstBitParser, 2);
    }

    AUD_AC4_INFO("[%s, %d] substream_index: %d", __FUNCTION__, __LINE__, substream_index);

    return substream_index;
}

/*
ETSI TS 103 190-1 V1.3.1
4.2.14.15 emdf_protection - EMDF protection data
*/
static U8BIT emdfProtection(DVB_BITPARSER * pstBitParser)
{
    U8BIT protection_length_primary = DVB_BitParserGetBits(pstBitParser, 2);//protection_length_primary;
    U8BIT protection_length_secondary = DVB_BitParserGetBits(pstBitParser, 2);//protection_length_secondary;

    AUD_AC4_INFO("[%s, %d] protection_length_primary: %d", __FUNCTION__, __LINE__, protection_length_primary);
    AUD_AC4_INFO("[%s, %d] protection_length_secondary: %d", __FUNCTION__, __LINE__, protection_length_secondary);
    switch (protection_length_primary) {
        case 1:
            DVB_BitParserSkipBits(pstBitParser, 8);
            break;
        case 2:
            DVB_BitParserSkipBits(pstBitParser, 32);
            break;
        case 3:
            DVB_BitParserSkipBits(pstBitParser, 128);
            break;
        default:
            break;
    }

    switch (protection_length_secondary) {
        case 1:
            DVB_BitParserSkipBits(pstBitParser, 8);
            break;
        case 2:
            DVB_BitParserSkipBits(pstBitParser, 32);
            break;
        case 3:
            DVB_BitParserSkipBits(pstBitParser, 128);
            break;
        default:
            break;
    }

    return protection_length_primary;
}


/*
ETSI TS 103 190-1 V1.3.1
4.2.3.5 emdf_info - EMDF information
*/
static U8BIT emdfInfo(DVB_BITPARSER * pstBitParser)
{
    U8BIT emdf_version = DVB_BitParserGetBits(pstBitParser, 2);//emdf_version;
    U8BIT key_id = 0;
    BOOLEAN b_emdf_payloads_substream_info = FALSE;
    AUD_AC4_INFO("[%s, %d] emdf_version: %d", __FUNCTION__, __LINE__, emdf_version);

    if (3 == emdf_version) {
        emdf_version += ReadVariableBits(pstBitParser, 2);
    }

    key_id = DVB_BitParserGetBits(pstBitParser, 3);//key_id;
    AUD_AC4_INFO("[%s, %d] key_id: %d", __FUNCTION__, __LINE__, key_id);
    if (7 == key_id) {
        key_id += ReadVariableBits(pstBitParser, 3);
    }

    b_emdf_payloads_substream_info = DVB_BitParserGetBits(pstBitParser, 1);//b_emdf_payloads_substream_info
    AUD_AC4_INFO("[%s, %d] b_emdf_payloads_substream_info: %d", __FUNCTION__, __LINE__, b_emdf_payloads_substream_info);
    if (b_emdf_payloads_substream_info) {
        emdfPayloadsSubstreamInfo(pstBitParser);
    }

    emdfProtection(pstBitParser);

    return TRUE;
}

static BOOLEAN AC4ParsePresentationInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    BOOLEAN b_single_substream = DVB_BitParserGetBits(pstBitParser, 1);//b_single_substream
    U8BIT u8presentation_config = 0;
    BOOLEAN b_add_emdf_substreams = FALSE;

    AUD_AC4_INFO("[%s, %d] b_single_substream: %d", __FUNCTION__, __LINE__, b_single_substream);
    if (TRUE != b_single_substream) {
        u8presentation_config = DVB_BitParserGetBits(pstBitParser, 3);//presentation_config
        AUD_AC4_INFO("[%s, %d] u8presentation_config: %d", __FUNCTION__, __LINE__, u8presentation_config);
        if (7 == u8presentation_config) {
            u8presentation_config += ReadVariableBits(pstBitParser, 2);
        }
        AUD_AC4_INFO("[%s, %d] u8presentation_config: %d", __FUNCTION__, __LINE__, u8presentation_config);
    }

    U8BIT audioPresentationVersion = PresentationVersion(pstBitParser);
    if (0 != audioPresentationVersion) {
        return FALSE;
    }

    if ((FALSE == b_single_substream) && (6 == u8presentation_config)) {
        b_add_emdf_substreams = 1;
    }
    else {
        U8BIT mdcompat = DVB_BitParserGetBits(pstBitParser, 3);//mdcompat
        BOOLEAN b_presentation_id = DVB_BitParserGetBits(pstBitParser, 1);//b_presentation_id
        U8BIT u8presentation_id = 0;

        if (b_presentation_id) {
            u8presentation_id = ReadVariableBits(pstBitParser, 2);
        }
        AUD_AC4_INFO("[%s, %d] u8presentation_id: %d", __FUNCTION__, __LINE__, u8presentation_id);

        frameRateMultiplyInfo(pstBitParser, ptocInfo);
        emdfInfo(pstBitParser);

        if (TRUE == b_single_substream) {
            //ac4_substream_info()
        }
    }

    return TRUE;
}

static U8BIT frameRateFractionsInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    U8BIT frame_rate_fraction = 1;
    BOOLEAN b_frame_rate_fraction = FALSE;
    BOOLEAN b_frame_rate_fraction_is_4 = FALSE;

    AUD_AC4_INFO("[%s, %d] ptocInfo->frame_rate_index: %d", __FUNCTION__, __LINE__, ptocInfo->frame_rate_index);

    if ((5 <= ptocInfo->frame_rate_index) && (9 >= ptocInfo->frame_rate_index)) {
        if (1 == ptocInfo->frame_rate_factor) {
            b_frame_rate_fraction = DVB_BitParserGetBits(pstBitParser, 1);//b_frame_rate_fraction
            if (TRUE == b_frame_rate_fraction) {
                frame_rate_fraction = 2;
            }
        }
    }

    if ((10 <= ptocInfo->frame_rate_index) && (12 >= ptocInfo->frame_rate_index)) {
        b_frame_rate_fraction = DVB_BitParserGetBits(pstBitParser, 1);//b_frame_rate_fraction
        if (1 == b_frame_rate_fraction) {
            b_frame_rate_fraction_is_4 = DVB_BitParserGetBits(pstBitParser, 1);//b_frame_rate_fraction_is_4
            if (TRUE == b_frame_rate_fraction_is_4) {
                frame_rate_fraction = 4;
            }
            else {
                frame_rate_fraction = 2;
            }
        }
    }

    AUD_AC4_INFO("[%s, %d] frame_rate_fraction: %d", __FUNCTION__, __LINE__, frame_rate_fraction);

    return frame_rate_fraction;
}

static U8BIT AC4SgiSpecifier(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    U8BIT group_index = 0;
    AUD_AC4_INFO("[%s, %d] bitstream_version: %d", __FUNCTION__, __LINE__, ptocInfo->bitstream_version);

    if (1 == ptocInfo->bitstream_version) {
        //ac4_substream_group_info
    }
    else {
        group_index = DVB_BitParserGetBits(pstBitParser, 3);//group_index
        if (7 == group_index) {
            group_index += ReadVariableBits(pstBitParser, 2);
        }
    }

    AUD_AC4_INFO("[%s, %d] group_index: %d", __FUNCTION__, __LINE__, group_index);

    return group_index;
}

static BOOLEAN PresentationConfigExtInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo, PRESENTATION_INFO * pstPreInfo)
{
    U16BIT n_skip_bytes = DVB_BitParserGetBits(pstBitParser, 5);//n_skip_bytes
    U16BIT n_bits_read = 0;
    U8BIT n_skip_bits = 0;
    BOOLEAN b_more_skip_bytes = DVB_BitParserGetBits(pstBitParser, 1);//b_more_skip_bytes

    AUD_AC4_INFO("[%s, %d] b_more_skip_bytes: %d", __FUNCTION__, __LINE__, b_more_skip_bytes);

    if (TRUE == b_more_skip_bytes) {
        n_skip_bytes += ReadVariableBits(pstBitParser, 2)<<5;
    }

    if ((1 == ptocInfo->bitstream_version) && (7 == pstPreInfo->presentation_config)) {
        n_bits_read = AC4ParsePresentationV1Info(pstBitParser, ptocInfo);
        if (n_bits_read % 8) {
            n_skip_bits = 8 - (n_bits_read % 8);
            DVB_BitParserSkipBits(pstBitParser, n_skip_bits);
            n_bits_read += n_skip_bits;
        }
        n_skip_bytes = n_skip_bytes - (n_bits_read / 8);
    }

    for (U16BIT i = 0; i < n_skip_bytes; i++) {
        DVB_BitParserSkipBits(pstBitParser, 8);
    }

    return TRUE;
}

static BOOLEAN AC4PresentationSubstreamInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    BOOLEAN b_alternative = DVB_BitParserGetBits(pstBitParser, 1);//b_alternative
    BOOLEAN b_pres_ndot = DVB_BitParserGetBits(pstBitParser, 1);//b_pres_ndot
    U8BIT substream_index = DVB_BitParserGetBits(pstBitParser, 2);//substream_index

    if (3 == substream_index) {
        substream_index += ReadVariableBits(pstBitParser, 2);
    }

    AUD_AC4_INFO("[%s, %d] substream_index: %d", __FUNCTION__, __LINE__, substream_index);

    return TRUE;
}

static U8BIT AC4ParsePresentationV1Info(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    BOOLEAN b_single_substream_group = DVB_BitParserGetBits(pstBitParser, 1);//b_single_substream_group
    U8BIT presentation_config = 0x1F;
    U8BIT n_substream_groups = 0;
    U8BIT n_substream_groups_minus2 = 0;
    U8BIT n_add_emdf_substreams = 0;
    U8BIT group_index = 0;
    BOOLEAN b_add_emdf_substreams = FALSE;
    BOOLEAN b_presentation_filter = FALSE;
    BOOLEAN b_enable_presentation = FALSE;
    BOOLEAN b_multi_pid = FALSE;
    BOOLEAN b_pre_virtualized = FALSE;
    PRESENTATION_INFO stPreInfo;

    AUD_AC4_INFO("[%s, %d] b_single_substream_group: %d", __FUNCTION__, __LINE__, b_single_substream_group);

    memset(&stPreInfo, 0x00, sizeof(PRESENTATION_INFO));
    if (FALSE == b_single_substream_group) {
        presentation_config = DVB_BitParserGetBits(pstBitParser, 3);
        AUD_AC4_INFO("[%s, %d] presentation_config: %d", __FUNCTION__, __LINE__, presentation_config);
        if (7 == presentation_config) {
            presentation_config += ReadVariableBits(pstBitParser, 2);
        }
    }
    AUD_AC4_INFO("[%s, %d] presentation_config: %d", __FUNCTION__, __LINE__, presentation_config);

    stPreInfo.presentation_config = presentation_config;
    if (1 != ptocInfo->bitstream_version) {
        U8BIT audPresentationVersion = PresentationVersion(pstBitParser);
        if (1 != audPresentationVersion) {
            return 0;
        }
    }

    if ((FALSE == b_single_substream_group) && (6 == presentation_config)) {
        b_add_emdf_substreams = 1;
    }
    else {
        if (1 != ptocInfo->bitstream_version) {
            U8BIT mdcompat = DVB_BitParserGetBits(pstBitParser, 3);//mdcompat
        }
        BOOLEAN b_presentation_id = DVB_BitParserGetBits(pstBitParser, 1);//b_presentation_id
        U8BIT u8presentation_id = 0;
        if (b_presentation_id) {
            u8presentation_id = ReadVariableBits(pstBitParser, 2);
        }

        AUD_AC4_INFO("[%s, %d] u8presentation_id: %d", __FUNCTION__, __LINE__, u8presentation_id);

        ptocInfo->frame_rate_factor = frameRateMultiplyInfo(pstBitParser, ptocInfo);
        frameRateFractionsInfo(pstBitParser, ptocInfo);
        emdfInfo(pstBitParser);

        b_presentation_filter = DVB_BitParserGetBits(pstBitParser, 1);//b_presentation_filter
        if (b_presentation_filter) {
            b_enable_presentation = DVB_BitParserGetBits(pstBitParser, 1);//b_enable_presentation
        }

        AUD_AC4_INFO("[%s, %d] b_single_substream_group: %d", __FUNCTION__, __LINE__, b_single_substream_group);

        if (TRUE == b_single_substream_group) {
            group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
            n_substream_groups = 1;
        }
        else {
            b_multi_pid = DVB_BitParserGetBits(pstBitParser, 1);//b_multi_pid
            switch (presentation_config) {
                case 0:
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    n_substream_groups = 2;
                    break;

                case 1:
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    n_substream_groups = 1;
                    break;

                case 2:
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    n_substream_groups = 2;
                    break;

                case 3:
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    n_substream_groups = 3;
                    break;

                case 4:
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    n_substream_groups = 2;
                    break;

                case 5:
                    n_substream_groups_minus2 = DVB_BitParserGetBits(pstBitParser, 2);//n_substream_groups_minus2
                    n_substream_groups = n_substream_groups_minus2 + 2;
                    if (5 == n_substream_groups) {
                        n_substream_groups += ReadVariableBits(pstBitParser, 2);
                    }

                    for (U8BIT sg = 0; sg < n_substream_groups; ++sg) {
                        group_index = AC4SgiSpecifier(pstBitParser, ptocInfo);
                    }
                    break;

                default:
                    PresentationConfigExtInfo(pstBitParser, ptocInfo, &stPreInfo);
                    break;
            }
        }

        b_pre_virtualized = DVB_BitParserGetBits(pstBitParser, 1);//b_pre_virtualized
        b_add_emdf_substreams = DVB_BitParserGetBits(pstBitParser, 1);//b_add_emdf_substreams
        AC4PresentationSubstreamInfo(pstBitParser, ptocInfo);
    }

    AUD_AC4_INFO("[%s, %d] b_add_emdf_substreams: %d", __FUNCTION__, __LINE__, b_add_emdf_substreams);

    if (b_add_emdf_substreams) {
        n_add_emdf_substreams = DVB_BitParserGetBits(pstBitParser, 2);//n_add_emdf_substreams
        if (n_add_emdf_substreams == 0) {
            n_add_emdf_substreams = ReadVariableBits(pstBitParser, 2) + 4;
        }

        AUD_AC4_INFO("[%s, %d] n_add_emdf_substreams: %d", __FUNCTION__, __LINE__, n_add_emdf_substreams);

        for (U8BIT i = 0; i < n_add_emdf_substreams; i++) {
            emdfInfo(pstBitParser);
        }
    }

    return group_index;
}

static BOOLEAN AC4SubstreamInfoChan(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo, BOOLEAN b_substreams_present)
{
    U16BIT channel_mode = 0;
    BOOLEAN b_4_back_channels_present = FALSE;
    BOOLEAN b_centre_present = FALSE;
    BOOLEAN b_sf_multiplier = FALSE;
    BOOLEAN sf_multiplier = FALSE;
    BOOLEAN b_bitrate_info = FALSE;
    BOOLEAN add_ch_base = FALSE;
    U8BIT top_channels_present = 0;
    U8BIT bitrate_indicator = 0;
    U16BIT substream_index = 0;

    channel_mode = DVB_BitParserGetBits(pstBitParser, 1);//channel_mode
    AUD_AC4_INFO("[%s, %d] channel_mode: 0x%x", __FUNCTION__, __LINE__, channel_mode);
    if (0 == channel_mode) {
        //mono
    }
    else {
        channel_mode <<= 1;
        channel_mode += DVB_BitParserGetBits(pstBitParser, 1);//channel_mode
        AUD_AC4_INFO("[%s, %d] channel_mode: 0x%x", __FUNCTION__, __LINE__, channel_mode);
        if (2 == channel_mode) {
            //stereo
        }
        else {
            channel_mode <<= 2;
            channel_mode += DVB_BitParserGetBits(pstBitParser, 2);//channel_mode
            AUD_AC4_INFO("[%s, %d] channel_mode: 0x%x", __FUNCTION__, __LINE__, channel_mode);
            if (0xC == channel_mode) {
                //3.0
            }
            else if (0xD == channel_mode) {
                //5.0
            }
            else if (0xE == channel_mode) {
                //5.1
            }
            else {
                channel_mode <<= 3;
                channel_mode += DVB_BitParserGetBits(pstBitParser, 3);//channel_mode
                AUD_AC4_INFO("[%s, %d] channel_mode: 0x%x", __FUNCTION__, __LINE__, channel_mode);
                if (0x78 == channel_mode) {
                    //7.0: 3/4/0 (L, C, R, Ls, Rs, Lrs, Rrs)
                }
                else if (0x79 == channel_mode) {
                    //7.1: 3/4/0.1 (L, C, R, Ls, Rs, Lrs, Rrs, LFE)
                }
                else if (0x7A == channel_mode) {
                    //7.0: 5/2/0 (L, C, R, Lw, Rw, Ls, Rs)
                }
                else if (0x7B == channel_mode) {
                    //7.1: 5/2/0.1 (L, C, R, Lw, Rw, Ls, Rs, LFE)
                }
                else if (0x7C == channel_mode) {
                    //7.0: 3/2/2 (L, C, R, Ls, Rs, Vhl, Vhr)
                }
                else if (0x7D == channel_mode) {
                    //7.1: 3/2/2.1 (L, C, R, Ls, Rs, Vhl, Vhr, LFE)
                }
                else {
                    channel_mode <<= 1;
                    channel_mode += DVB_BitParserGetBits(pstBitParser, 1);//channel_mode
                    AUD_AC4_INFO("[%s, %d] channel_mode: 0x%x", __FUNCTION__, __LINE__, channel_mode);
                    if (0xFC == channel_mode) {
                        //7.0.4
                    }
                    else if (0xFD == channel_mode) {
                        //7.1.4
                    }
                    else {
                        channel_mode <<= 1;
                        channel_mode += DVB_BitParserGetBits(pstBitParser, 1);//channel_mode
                        AUD_AC4_INFO("[%s, %d] channel_mode: 0x%x", __FUNCTION__, __LINE__, channel_mode);
                        if (0x1FC == channel_mode) {
                            //9.0.4
                        }
                        else if (0x1FD == channel_mode) {
                            //9.1.4
                        }
                        else if (0x1FE == channel_mode) {
                            //22.2
                        }
                        else if (0x1FF == channel_mode) {
                            //Reserved
                        }
                    }
                }
            }
        }
    }

    AUD_AC4_INFO("[%s, %d] channel_mode: 0x%x", __FUNCTION__, __LINE__, channel_mode);

    if (0x1FF == channel_mode) {
        channel_mode += ReadVariableBits(pstBitParser, 2);
    }

    if ((0xFC == channel_mode) ||
        (0xFD == channel_mode) ||
        (0x1FC == channel_mode) ||
        (0x1FD == channel_mode)) {
        b_4_back_channels_present = DVB_BitParserGetBits(pstBitParser, 1);//b_4_back_channels_present
        b_centre_present = DVB_BitParserGetBits(pstBitParser, 1);//b_centre_present
        top_channels_present = DVB_BitParserGetBits(pstBitParser, 2);//top_channels_present
    }

    if (1 == ptocInfo->fs_index) {
        b_sf_multiplier = DVB_BitParserGetBits(pstBitParser, 1);//b_sf_multiplier
        if (b_sf_multiplier) {
            sf_multiplier = DVB_BitParserGetBits(pstBitParser, 1);//sf_multiplier
        }
    }

    b_bitrate_info = DVB_BitParserGetBits(pstBitParser, 1);//b_bitrate_info
    if (b_bitrate_info) {
        bitrate_indicator = DVB_BitParserGetBits(pstBitParser, 3);//bitrate_indicator
        if ((0 == bitrate_indicator) ||
            (2 == bitrate_indicator) ||
            (4 == bitrate_indicator) ||
            (6 == bitrate_indicator)){
            //over
        }
        else {
            bitrate_indicator <<= 2;
            bitrate_indicator = DVB_BitParserGetBits(pstBitParser, 2);//bitrate_indicator
        }
    }

    if ((0x7A == channel_mode) ||
        (0x7B == channel_mode) ||
        (0x7C == channel_mode) ||
        (0x7D == channel_mode)) {
        add_ch_base = DVB_BitParserGetBits(pstBitParser, 1);//add_ch_base
    }

    for (U8BIT i = 0; i < ptocInfo->frame_rate_factor; i++) {
        DVB_BitParserSkipBits(pstBitParser, 1);//b_audio_ndot;
    }
    if (1 == b_substreams_present) {
        substream_index = DVB_BitParserGetBits(pstBitParser, 1);//substream_index
        if (3 == substream_index) {
            substream_index += ReadVariableBits(pstBitParser, 2);
        }
    }

    return TRUE;
}

static U16BIT AC4HsfExtSubstreamInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo, BOOLEAN b_substreams_present)
{
    U16BIT substream_index = 0;

    if (1 == b_substreams_present) {
        substream_index = DVB_BitParserGetBits(pstBitParser, 2);//substream_index
        if (3 == substream_index) {
            substream_index += ReadVariableBits(pstBitParser, 2);
        }
    }

    return substream_index;
}

static U16BIT OamdSubstreamInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo, BOOLEAN b_substreams_present)
{
    BOOLEAN b_oamd_ndot = DVB_BitParserGetBits(pstBitParser, 1);//b_oamd_ndot
    U16BIT substream_index = 0;

    if (b_substreams_present == 1) {
        substream_index = DVB_BitParserGetBits(pstBitParser, 2);//substream_index
        if (3 == substream_index) {
            substream_index += ReadVariableBits(pstBitParser, 2);
        }
    }

    return substream_index;
}

static BOOLEAN BedDynObjAssignment(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo, U8BIT n_signals)
{
    BOOLEAN b_dyn_objects_only = DVB_BitParserGetBits(pstBitParser, 1);//b_dyn_objects_only
    BOOLEAN b_isf = FALSE;
    BOOLEAN b_ch_assign_code = FALSE;
    BOOLEAN b_chan_assign_mask = FALSE;
    BOOLEAN b_nonstd_bed_channel_assignment = FALSE;
    U8BIT isf_config = 0;
    U8BIT bed_chan_assign_code = 0;
    U8BIT bed_ch_bits = 0;
    U32BIT nonstd_bed_channel_assignment_mask = 0;
    U16BIT std_bed_channel_assignment_mask = 0;
    U16BIT n_bed_signals_minus1 = 0;
    U16BIT n_bed_signals = 0;

    if (b_dyn_objects_only == 0) {
        b_isf = DVB_BitParserGetBits(pstBitParser, 1);//b_isf
        if (b_isf) {
            isf_config = DVB_BitParserGetBits(pstBitParser, 3);//isf_config
        }
        else {
            b_ch_assign_code = DVB_BitParserGetBits(pstBitParser, 1);//b_ch_assign_code
            if (b_ch_assign_code) {
                bed_chan_assign_code = DVB_BitParserGetBits(pstBitParser, 3);//bed_chan_assign_code
            }
            else {
                b_chan_assign_mask = DVB_BitParserGetBits(pstBitParser, 1);//b_chan_assign_mask
                if (b_chan_assign_mask) {
                    b_nonstd_bed_channel_assignment = DVB_BitParserGetBits(pstBitParser, 1);//b_nonstd_bed_channel_assignment
                    if (b_nonstd_bed_channel_assignment) {
                        nonstd_bed_channel_assignment_mask = DVB_BitParserGetBits(pstBitParser, 17);//nonstd_bed_channel_assignment_mask
                    }
                    else {
                        std_bed_channel_assignment_mask = DVB_BitParserGetBits(pstBitParser, 10);//std_bed_channel_assignment_mask
                    }
                }
                else {
                    if (n_signals > 1) {
                        bed_ch_bits = ceil(log2(n_signals));
                        n_bed_signals_minus1 = DVB_BitParserGetBits(pstBitParser, bed_ch_bits);//n_bed_signals_minus1
                        n_bed_signals = n_bed_signals_minus1 + 1;
                    }
                    else {
                        n_bed_signals = 1;
                    }
                    for (U16BIT b = 0; b < n_bed_signals; b++) {
                        DVB_BitParserSkipBits(pstBitParser, 4);//nonstd_bed_channel_assignment
                    }
                }
            }
        }
    }

    return TRUE;
}

static BOOLEAN trim(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    BOOLEAN b_trim_present = DVB_BitParserGetBits(pstBitParser, 1);//b_trim_present
    BOOLEAN b_default_trim = FALSE;
    BOOLEAN b_disable_trim = FALSE;
    U8BIT warp_mode = 0;
    U8BIT global_trim_mode = 0;
    U8BIT trim_balance_presence = 0;

    if (b_trim_present) {
        warp_mode = DVB_BitParserGetBits(pstBitParser, 2);//warp_mode
        DVB_BitParserSkipBits(pstBitParser, 2);//reserved
        global_trim_mode = DVB_BitParserGetBits(pstBitParser, 2);//global_trim_mode
        if (global_trim_mode == 2) {
            for (U8BIT cfg = 0; cfg < NUM_TRIM_CONFIGS; cfg++) {
                b_default_trim = DVB_BitParserGetBits(pstBitParser, 1);//b_default_trim
                if (b_default_trim == 0) {
                    b_disable_trim = DVB_BitParserGetBits(pstBitParser, 1);//b_disable_trim
                    if (b_disable_trim == 0) {
                        trim_balance_presence = DVB_BitParserGetBits(pstBitParser, 5);//trim_balance_presence
                        if ((trim_balance_presence>>4)&0x01) {
                            DVB_BitParserSkipBits(pstBitParser, 4);//trim_centre
                        }
                        if ((trim_balance_presence>>3)&0x01) {
                            DVB_BitParserSkipBits(pstBitParser, 4);//trim_surround
                        }
                        if ((trim_balance_presence>>2)&0x01) {
                            DVB_BitParserSkipBits(pstBitParser, 4);//trim_height
                        }
                        if ((trim_balance_presence>>1)&0x01) {
                            DVB_BitParserSkipBits(pstBitParser, 1);//bal3D_Y_sign_tb_code
                            DVB_BitParserSkipBits(pstBitParser, 4);//bal3D_Y_amount_tb
                        }
                        if (trim_balance_presence&0x01) {
                            DVB_BitParserSkipBits(pstBitParser, 1);//bal3D_Y_sign_lis_code
                            DVB_BitParserSkipBits(pstBitParser, 4);//bal3D_Y_amount_lis
                        }
                    }
                }
            }
        }
    }

    return TRUE;
}

static BOOLEAN stereo_dmx_coeff(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    AUD_AC4_INFO("[%s, %d] ERROR: without the function implementation.", __FUNCTION__, __LINE__);
    return TRUE;
}

static BOOLEAN tool_t2_to_f_s_b(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    AUD_AC4_INFO("[%s, %d] ERROR: without the function implementation.", __FUNCTION__, __LINE__);
    return TRUE;
/*
b_top_to_front; .............................................................................. 1
if (b_top_to_front == 1) {
gain_t2a_code; ............................................................................. 3
gain_t2b_code = 7;
}
else {
b_top_to_side; ............................................................................. 1
if (b_top_to_side == 1) {
gain_t2b_code; ........................................................................... 3
}
else {
gain_t2c_code; ........................................................................... 3
gain_t2b_code = 7;
}
}
*/
}

static BOOLEAN tool_t2_to_f_s(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    AUD_AC4_INFO("[%s, %d] ERROR: without the function implementation.", __FUNCTION__, __LINE__);
    return TRUE;
}

static BOOLEAN tool_tb_to_f_s_b(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    AUD_AC4_INFO("[%s, %d] ERROR: without the function implementation.", __FUNCTION__, __LINE__);
    return TRUE;
}

static BOOLEAN tool_tb_to_f_s(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    AUD_AC4_INFO("[%s, %d] ERROR: without the function implementation.", __FUNCTION__, __LINE__);
    return TRUE;
}

static BOOLEAN tool_tf_to_f_s_b(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    AUD_AC4_INFO("[%s, %d] ERROR: without the function implementation.", __FUNCTION__, __LINE__);
    return TRUE;
}

static BOOLEAN tool_tf_to_f_s(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    AUD_AC4_INFO("[%s, %d] ERROR: without the function implementation.", __FUNCTION__, __LINE__);
    return TRUE;
}

static BOOLEAN BedRenderInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    BOOLEAN b_bed_render_info = DVB_BitParserGetBits(pstBitParser, 1);//b_bed_render_info
    BOOLEAN b_stereo_dmx_coeff = FALSE;
    BOOLEAN b_cdmx_data_present = FALSE;
    BOOLEAN b_cdmx_w_to_f = FALSE;
    BOOLEAN b_cdmx_b4_to_b2 = FALSE;
    BOOLEAN b_tm_ch_present = FALSE;
    BOOLEAN b_cdmx_t2_to_f_s_b = FALSE;
    BOOLEAN b_cdmx_t2_to_f_s = FALSE;
    BOOLEAN b_tb_ch_present = FALSE;
    BOOLEAN b_cdmx_tb_to_f_s_b = FALSE;
    BOOLEAN b_cdmx_tb_to_f_s = FALSE;
    BOOLEAN b_tf_ch_present = FALSE;
    BOOLEAN b_cdmx_tf_to_f_s_b = FALSE;
    BOOLEAN b_cdmx_tf_to_f_s = FALSE;
    BOOLEAN b_cdmx_tfb_to_tm = FALSE;
    U8BIT gain_w_to_f_code = 0;
    U8BIT gain_b4_to_b2_code = 0;
    U8BIT gain_tfb_to_tm_code = 0;

    if (b_bed_render_info) {
        b_stereo_dmx_coeff = DVB_BitParserGetBits(pstBitParser, 1);//b_stereo_dmx_coeff
        if (b_stereo_dmx_coeff) {
            stereo_dmx_coeff(pstBitParser, ptocInfo);
        }
        b_cdmx_data_present = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_data_present
        if (b_cdmx_data_present) {
            b_cdmx_w_to_f = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_w_to_f
            if (b_cdmx_w_to_f) {
                gain_w_to_f_code = DVB_BitParserGetBits(pstBitParser, 3);//gain_w_to_f_code
            }
            b_cdmx_b4_to_b2 = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_b4_to_b2
            if (b_cdmx_b4_to_b2) {
                gain_b4_to_b2_code = DVB_BitParserGetBits(pstBitParser, 3);//gain_b4_to_b2_code
            }
            b_tm_ch_present = DVB_BitParserGetBits(pstBitParser, 1);//b_tm_ch_present
            if (b_tm_ch_present) {
                b_cdmx_t2_to_f_s_b = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_t2_to_f_s_b
                if (b_cdmx_t2_to_f_s_b) {
                    tool_t2_to_f_s_b(pstBitParser, ptocInfo);
                }
                b_cdmx_t2_to_f_s = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_t2_to_f_s
                if (b_cdmx_t2_to_f_s) {
                    tool_t2_to_f_s(pstBitParser, ptocInfo);
                }
            }
            b_tb_ch_present = DVB_BitParserGetBits(pstBitParser, 1);//b_tb_ch_present
            if (b_tb_ch_present) {
                b_cdmx_tb_to_f_s_b = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_tb_to_f_s_b
                if (b_cdmx_tb_to_f_s_b) {
                    tool_tb_to_f_s_b(pstBitParser, ptocInfo);
                }
                b_cdmx_tb_to_f_s = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_tb_to_f_s
                if (b_cdmx_tb_to_f_s) {
                    tool_tb_to_f_s(pstBitParser, ptocInfo);
                }
            }
            b_tf_ch_present = DVB_BitParserGetBits(pstBitParser, 1);//b_tf_ch_present
            if (b_tf_ch_present) {
                b_cdmx_tf_to_f_s_b = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_tf_to_f_s_b
                if (b_cdmx_tf_to_f_s_b) {
                    tool_tf_to_f_s_b(pstBitParser, ptocInfo);
                }
                b_cdmx_tf_to_f_s = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_tf_to_f_s
                if (b_cdmx_tf_to_f_s) {
                    tool_tf_to_f_s(pstBitParser, ptocInfo);
                }
            }
            if (b_tb_ch_present || b_tf_ch_present) {
                b_cdmx_tfb_to_tm = DVB_BitParserGetBits(pstBitParser, 1);//b_cdmx_tfb_to_tm
                if (b_cdmx_tfb_to_tm) {
                    gain_tfb_to_tm_code = DVB_BitParserGetBits(pstBitParser, 3);//gain_tfb_to_tm_code
                }
            }
        }
    }

    return TRUE;
}


static BOOLEAN OamdCommonData(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    BOOLEAN b_default_screen_size_ratio = DVB_BitParserGetBits(pstBitParser, 1);//b_default_screen_size_ratio
    BOOLEAN b_bed_object_chan_distribute = FALSE;
    BOOLEAN b_additional_data = FALSE;
    U8BIT add_data_bytes_minus1 = 0;
    U8BIT master_screen_size_ratio_code = 0;
    U16BIT add_data_bytes = 0;
    U16BIT add_data_bits = 0;
    U16BIT bits_used = 0;
    U16BIT add_data = 0;

    if (FALSE == b_default_screen_size_ratio) {
        master_screen_size_ratio_code = DVB_BitParserGetBits(pstBitParser, 5);//master_screen_size_ratio_code
    }
    b_bed_object_chan_distribute = DVB_BitParserGetBits(pstBitParser, 1);//b_bed_object_chan_distribute
    b_additional_data = DVB_BitParserGetBits(pstBitParser, 1);//b_additional_data
    if (b_additional_data) {
        add_data_bytes_minus1 = DVB_BitParserGetBits(pstBitParser, 1);//add_data_bytes_minus1
        add_data_bytes = add_data_bytes_minus1 + 1;
        if (add_data_bytes == 2) {
            add_data_bytes += ReadVariableBits(pstBitParser, 2);
        }
        add_data_bits = add_data_bytes * 8;
        bits_used = trim(pstBitParser, ptocInfo);
        bits_used += BedRenderInfo(pstBitParser, ptocInfo);
        add_data_bits = add_data_bits - bits_used;
        add_data = DVB_BitParserGetBits(pstBitParser, add_data_bits);
    }

    return TRUE;
}

static U16BIT AC4SubstreamInfoAjoc(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo, BOOLEAN b_substreams_present)
{
    BOOLEAN b_lfe = DVB_BitParserGetBits(pstBitParser, 1);//b_lfe
    BOOLEAN b_static_dmx = DVB_BitParserGetBits(pstBitParser, 1);//b_static_dmx
    BOOLEAN b_oamd_common_data_present = FALSE;
    BOOLEAN b_sf_multiplier = FALSE;
    BOOLEAN sf_multiplier = FALSE;
    BOOLEAN b_bitrate_info = FALSE;
    U8BIT n_fullband_dmx_signals = 0;
    U8BIT n_fullband_dmx_signals_minus1 = 0;
    U8BIT n_fullband_upmix_signals_minus1 = 0;
    U8BIT n_fullband_upmix_signals = 0;
    U8BIT bitrate_indicator = 0;
    U8BIT substream_index = 0;

    AUD_AC4_INFO("[%s, %d] b_static_dmx: %d", __FUNCTION__, __LINE__, b_static_dmx);

    if (b_static_dmx) {
        n_fullband_dmx_signals = 5;
    }
    else {
        n_fullband_dmx_signals_minus1 = DVB_BitParserGetBits(pstBitParser, 4);//n_fullband_dmx_signals_minus1
        n_fullband_dmx_signals = n_fullband_dmx_signals_minus1 + 1;
        BedDynObjAssignment(pstBitParser, ptocInfo, n_fullband_dmx_signals);
    }
    b_oamd_common_data_present = DVB_BitParserGetBits(pstBitParser, 1);//b_oamd_common_data_present
    if (b_oamd_common_data_present) {
        OamdCommonData(pstBitParser, ptocInfo);
    }
    n_fullband_upmix_signals_minus1 = DVB_BitParserGetBits(pstBitParser, 4);//n_fullband_upmix_signals_minus1
    n_fullband_upmix_signals = n_fullband_upmix_signals_minus1 + 1;
    if (16 == n_fullband_upmix_signals) {
        n_fullband_upmix_signals += ReadVariableBits(pstBitParser, 3);
    }
    BedDynObjAssignment(pstBitParser, ptocInfo, n_fullband_upmix_signals);
    if (1 == ptocInfo->fs_index) {
        b_sf_multiplier = DVB_BitParserGetBits(pstBitParser, 1);//b_sf_multiplier
        if (b_sf_multiplier) {
            sf_multiplier = DVB_BitParserGetBits(pstBitParser, 1);//sf_multiplier
        }
    }

    b_bitrate_info = DVB_BitParserGetBits(pstBitParser, 1);//b_bitrate_info
    if (b_bitrate_info) {
        bitrate_indicator = DVB_BitParserGetBits(pstBitParser, 3);//bitrate_indicator
        if ((0 == bitrate_indicator) ||
            (2 == bitrate_indicator) ||
            (4 == bitrate_indicator) ||
            (6 == bitrate_indicator)){
            //over
        }
        else {
            bitrate_indicator <<= 2;
            bitrate_indicator = DVB_BitParserGetBits(pstBitParser, 2);//bitrate_indicator
        }
    }

    for (U8BIT i = 0; i < ptocInfo->frame_rate_factor; i++) {//frame_rate_factor
        DVB_BitParserSkipBits(pstBitParser, 1);//b_audio_ndot
    }
    if (1 == b_substreams_present) {
        substream_index = DVB_BitParserGetBits(pstBitParser, 2);//substream_index
        if (substream_index == 3) {
            substream_index += ReadVariableBits(pstBitParser, 2);
        }
    }

    //sus_ver = 1;
    return 0;
}

static BOOLEAN AC4SubstreamInfoObj(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo, BOOLEAN b_substreams_present)
{
    U8BIT n_objects_code = DVB_BitParserGetBits(pstBitParser, 3);
    BOOLEAN b_dynamic_objects = DVB_BitParserGetBits(pstBitParser, 1);
    if (b_dynamic_objects) {
        BOOLEAN b_lfe = DVB_BitParserGetBits(pstBitParser, 1);
    }
    else {
        BOOLEAN b_bed_objects = DVB_BitParserGetBits(pstBitParser, 1);
        if (b_bed_objects) {
            BOOLEAN b_bed_start = DVB_BitParserGetBits(pstBitParser, 1);
            if (b_bed_start) {
                BOOLEAN b_ch_assign_code = DVB_BitParserGetBits(pstBitParser, 1);
                if (b_ch_assign_code) {
                    U8BIT bed_chan_assign_code = DVB_BitParserGetBits(pstBitParser, 3);
                }
                else {
                    BOOLEAN b_nonstd_bed_channel_assignment = DVB_BitParserGetBits(pstBitParser, 1);
                    if (b_nonstd_bed_channel_assignment) {
                        U32BIT nonstd_bed_channel_assignment_mask = DVB_BitParserGetBits(pstBitParser, 17);
                    }
                    else {
                        U32BIT std_bed_channel_assignment_mask = DVB_BitParserGetBits(pstBitParser, 10);
                    }
                }
            }
        }
        else {
            BOOLEAN b_isf = DVB_BitParserGetBits(pstBitParser, 1);
            if (b_isf) {
                BOOLEAN b_isf_start = DVB_BitParserGetBits(pstBitParser, 1);
                if (b_isf_start) {
                    U8BIT isf_config = DVB_BitParserGetBits(pstBitParser, 3);
                }
            }
            else {
                U8BIT res_bytes = DVB_BitParserGetBits(pstBitParser, 4);
                DVB_BitParserSkipBits(pstBitParser, res_bytes * 8);//reserved_data  8 * res_bytes
            }
        }
    }
    if (ptocInfo->fs_index == 1) {
        BOOLEAN b_sf_multiplier = DVB_BitParserGetBits(pstBitParser, 1);
        if (b_sf_multiplier) {
            BOOLEAN sf_multiplier = DVB_BitParserGetBits(pstBitParser, 1);
        }
    }
    BOOLEAN b_bitrate_info = DVB_BitParserGetBits(pstBitParser, 1);
    if (b_bitrate_info) {
        U8BIT tmp = DVB_BitParserGetBits(pstBitParser, 3);
        switch(tmp) {
            case 1:
            case 3:
            case 5:
            case 7:
                tmp = DVB_BitParserGetBits(pstBitParser, 2);
                break;

            default:
                break;
        }
    }
    for (U8BIT i = 0; i < ptocInfo->frame_rate_factor; i++) {
        BOOLEAN b_audio_ndot = DVB_BitParserGetBits(pstBitParser, 1);
    }
    if (b_substreams_present == 1) {
        U8BIT substream_index = DVB_BitParserGetBits(pstBitParser, 2);
        if (substream_index == 3) {
            substream_index += ReadVariableBits(pstBitParser, 2);
        }
    }
    //sus_ver = 1;

    return TRUE;
}

static BOOLEAN ContentType(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    U8BIT content_classifier = DVB_BitParserGetBits(pstBitParser, 3);//content_classifier
    BOOLEAN b_language_indicator = DVB_BitParserGetBits(pstBitParser, 1);//b_language_indicator;
    BOOLEAN b_serialized_language_tag = FALSE;
    BOOLEAN b_start_tag = FALSE;
    U16BIT language_tag_chunk = 0;
    U8BIT n_language_tag_bytes = 0;
    U8BIT language_tag_bytes = 0;

    AUD_AC4_INFO("[%s, %d] b_language_indicator: %d", __FUNCTION__, __LINE__, b_language_indicator);

    if (b_language_indicator) {
        b_serialized_language_tag = DVB_BitParserGetBits(pstBitParser, 1);//b_serialized_language_tag;

        AUD_AC4_INFO("[%s, %d] b_serialized_language_tag: %d", __FUNCTION__, __LINE__, b_serialized_language_tag);
        if (b_serialized_language_tag) {
            b_start_tag = DVB_BitParserGetBits(pstBitParser, 1);//b_start_tag
            language_tag_chunk = DVB_BitParserGetBits(pstBitParser, 16);//language_tag_chunk
        } else {
            n_language_tag_bytes = DVB_BitParserGetBits(pstBitParser, 6);//n_language_tag_bytes
            AUD_AC4_INFO("[%s, %d] n_language_tag_bytes: %d", __FUNCTION__, __LINE__, n_language_tag_bytes);
            for (U8BIT i = 0; i < n_language_tag_bytes; i++) {
                language_tag_bytes = DVB_BitParserGetBits(pstBitParser, 8);//language_tag_bytes
                AUD_AC4_INFO("[%s, %d] n_language_tag_bytes: %d, %c", __FUNCTION__, __LINE__, language_tag_bytes, language_tag_bytes);
            }
        }
    }

    return TRUE;
}


static BOOLEAN AC4ParseSubstreamGroupInfo(DVB_BITPARSER * pstBitParser, TOCINFO * ptocInfo)
{
    BOOLEAN b_substreams_present = DVB_BitParserGetBits(pstBitParser, 1);//b_substreams_present
    BOOLEAN b_hsf_ext = DVB_BitParserGetBits(pstBitParser, 1);//b_hsf_ext
    BOOLEAN b_single_substream = DVB_BitParserGetBits(pstBitParser, 1);//b_single_substream
    BOOLEAN b_channel_coded = FALSE;
    BOOLEAN b_oamd_substream = FALSE;
    BOOLEAN b_content_type = FALSE;
    BOOLEAN b_ajoc = FALSE;
    U8BIT n_lf_substreams = 0;
    U8BIT n_lf_substreams_minus2 = 0;
    U8BIT sus_ver = 0;

    AUD_AC4_INFO("[%s, %d] b_single_substream: %d", __FUNCTION__, __LINE__, b_single_substream);

    if (b_single_substream) {
        n_lf_substreams = 1;
    }
    else {
        n_lf_substreams_minus2 = DVB_BitParserGetBits(pstBitParser, 2);//n_lf_substreams_minus2
        n_lf_substreams = n_lf_substreams_minus2 + 2;
        if (n_lf_substreams == 5) {
            n_lf_substreams += ReadVariableBits(pstBitParser, 2);
        }
    }
    AUD_AC4_INFO("[%s, %d] n_lf_substreams: %d", __FUNCTION__, __LINE__, n_lf_substreams);

    b_channel_coded = DVB_BitParserGetBits(pstBitParser, 1);//b_channel_coded

    AUD_AC4_INFO("[%s, %d] b_channel_coded: %d", __FUNCTION__, __LINE__, b_channel_coded);
    if (b_channel_coded) {
        AUD_AC4_INFO("[%s, %d] n_lf_substreams: %d", __FUNCTION__, __LINE__, n_lf_substreams);
        for (U8BIT sus = 0; sus < n_lf_substreams; sus++) {
            if (1 == ptocInfo->bitstream_version) {
                sus_ver = DVB_BitParserGetBits(pstBitParser, 1);//sus_ver
            }
            else {
                sus_ver = 1;
            }
            AC4SubstreamInfoChan(pstBitParser, ptocInfo, b_substreams_present);
            AUD_AC4_INFO("[%s, %d] b_hsf_ext: %d", __FUNCTION__, __LINE__, b_hsf_ext);
            if (b_hsf_ext) {
                AC4HsfExtSubstreamInfo(pstBitParser, ptocInfo, b_substreams_present);
            }
        }
    }
    else {
        b_oamd_substream = DVB_BitParserGetBits(pstBitParser, 1);//b_oamd_substream
        AUD_AC4_INFO("[%s, %d] b_oamd_substream: %d", __FUNCTION__, __LINE__, b_oamd_substream);
        if (b_oamd_substream) {
            OamdSubstreamInfo(pstBitParser, ptocInfo, b_substreams_present);
        }
        AUD_AC4_INFO("[%s, %d] n_lf_substreams: %d", __FUNCTION__, __LINE__, n_lf_substreams);
        for (U8BIT sus = 0; sus < n_lf_substreams; sus++) {
            b_ajoc = DVB_BitParserGetBits(pstBitParser, 1);//b_ajoc
            AUD_AC4_INFO("[%s, %d] b_ajoc: %d", __FUNCTION__, __LINE__, b_ajoc);
            if (b_ajoc) {
                AC4SubstreamInfoAjoc(pstBitParser, ptocInfo, b_substreams_present);
                AUD_AC4_INFO("[%s, %d] b_hsf_ext: %d", __FUNCTION__, __LINE__, b_hsf_ext);
                if (b_hsf_ext) {
                    AC4HsfExtSubstreamInfo(pstBitParser, ptocInfo, b_substreams_present);
                }
            }
            else {
                AC4SubstreamInfoObj(pstBitParser, ptocInfo, b_substreams_present);
                if (b_hsf_ext) {
                    AC4HsfExtSubstreamInfo(pstBitParser, ptocInfo, b_substreams_present);
                }
            }
        }
    }

    b_content_type = DVB_BitParserGetBits(pstBitParser, 1);//b_content_type
    if (b_content_type) {
        ContentType(pstBitParser, ptocInfo);
    }

    return TRUE;
}

static BOOLEAN AC4SubstreamInfo(DVB_BITPARSER * pstBitParser)
{
    U8BIT channel_mode = ReadVariableBits(pstBitParser, 1);
    if (0x7F == channel_mode) {
        channel_mode += ReadVariableBits(pstBitParser, 2);
    }

    return TRUE;
}

/*******************************************************************************************/
/* main thread.                                              */
/*******************************************************************************************/
#if 0
static void AUD_AC4_Task(void* pParam)
{
    BOOLEAN b8Status = FALSE;
    U8BIT *pes_ptr = NULL;

    while (1)
    {
        b8Status = STB_OSReadQueue(gpAC4EventQueue, &pes_ptr, sizeof(U8BIT *), TIMEOUT_NEVER);

        if (b8Status == TRUE)
        {
            AC4ParsePesHeader(pes_ptr);
            if (pes_ptr != NULL)
            {
                STB_FreeMemory(pes_ptr);
                pes_ptr = NULL;
            }
        }
        else
        {
            AUD_AC4_INFO("read queue failed.");
        }
    }

    return;
}
#endif

/*****************************************************************************
*                    End Of File
*****************************************************************************/




