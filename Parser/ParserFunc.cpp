/******************************************************************************************



    Author: Robin
    Date:   2020-10-15

******************************************************************************************/

#include "field.h"
#include "FieldContainer.h"
#include "DescriptorBase.h"
#include "MultipleStringStructureBase.h"

#include "ParserFunc.h"

void SDT_Parser(u8 *pBuf, u32 u32DataLen)
{
    CFieldContainer fieldContainer("SDT Actual");

    // Table Head
    fieldContainer.addField(new CField("table_id", 8));
    fieldContainer.addField(new CField("section_syntax_indicator", 1));
    fieldContainer.addField(new CField("0", 1));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("section_length", 12));
    fieldContainer.addField(new CField("transport_stream_id", 16));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("vesion_number", 5));
    fieldContainer.addField(new CField("current_next_indicator", 1));
    fieldContainer.addField(new CField("section_number", 8));
    fieldContainer.addField(new CField("last_section_number", 8));

    fieldContainer.addField(new CField("original_network_id", 16));
    fieldContainer.addField(new CField("reserved_future_use", 8));


    // loop1: stream info loop
    CFieldLoopContainer *pfieldLoopContainer = new CFieldLoopContainer("sdt loop1 service info");

    pfieldLoopContainer->addField(new CField("service_id", 16));

    pfieldLoopContainer->addField(new CField("reserved_future_use", 6));
    pfieldLoopContainer->addField(new CField("EIT_schedule_flag", 1));
    pfieldLoopContainer->addField(new CField("EIT_present_following_flag", 1));

    pfieldLoopContainer->addField(new CField("runing_status", 3));
    pfieldLoopContainer->addField(new CField("free_CA_mode", 1));


    pfieldLoopContainer->addField(new CDescriptorLoopContainer("descriptors_loop_len", 12));

    fieldContainer.addField(pfieldLoopContainer);

    // CRC
    fieldContainer.addField(new CField("CRC_32", 32));

    fieldContainer.parserData(pBuf, u32DataLen, 0);

    fieldContainer.showData();
}



void NIT_Parser(u8 *pBuf, u32 u32DataLen)
{
    CFieldContainer fieldContainer("NIT Actual");

    // Table Head
    fieldContainer.addField(new CField("table_id", 8));
    fieldContainer.addField(new CField("section_syntax_indicator", 1));
    fieldContainer.addField(new CField("0", 1));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("section_length", 12));
    fieldContainer.addField(new CField("network_id", 16));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("vesion_number", 5));
    fieldContainer.addField(new CField("current_next_indicator", 1));
    fieldContainer.addField(new CField("section_number", 8));
    fieldContainer.addField(new CField("last_section_number", 8));

    fieldContainer.addField(new CField("reserved_future_use", 4));

    // Loop 1: network_descriptors
    fieldContainer.addField(new CDescriptorLoopContainer("network_descriptors", 12));

    fieldContainer.addField(new CField("reserved_future_use", 4));
    fieldContainer.addField(new CField("transport_stream_loop_length", 12));

    //loop 2: tranport stream info
    CFieldLoopContainer *pfieldLoopContainer = new CFieldLoopContainer("transport stram info");

    pfieldLoopContainer->addField(new CField("transport_stream_id", 16));
    pfieldLoopContainer->addField(new CField("original_network_id", 16));
    pfieldLoopContainer->addField(new CField("reserved_future_use", 4));

    pfieldLoopContainer->addField(new CDescriptorLoopContainer("transport stream descriptors", 12));

    fieldContainer.addField(pfieldLoopContainer);

    // CRC
    fieldContainer.addField(new CField("CRC_32", 32));

    fieldContainer.parserData(pBuf, u32DataLen, 0);

    fieldContainer.showData();
}


void CalcYMDfromMJD(unsigned short mjd, int &y, int &m, int &d)
{
    int k = 0;
    int yy = int((mjd - 15078.2) / 365.35);
    int mm = int((mjd - 14956.1 - int(yy * 365.25)) / 30.6001);

    d = mjd - 14956 - int(yy * 365.25) - int(mm * 30.6001);

    if (14 == mm || 15 == mm)
        k = 1;

    y = yy + k;
    if (y >= 100)
        y = y % 100 + 2000;
    else
        y = 1900 + y;

    m = mm - 1  - k * 12;

}

#define BCD_BYTE(a) ((a >> 4) * 10 + (a & 0x0f) )
void show_start_time(u8 * buf, u32 buflen)
{
    if (5 != buflen)
    {
        printf("[%s] buflen(%u) is error, must be (40)!", __FUNCTION__, buflen);
    }
    else
    {
        char tmp[32];
        int y, m, d;

        CalcYMDfromMJD(buf[0] << 8 | buf[1], y, m, d);

        snprintf(tmp, 32, "%d-%d-%d %2u:%2u:%2u", y, m, d, BCD_BYTE(buf[2]), BCD_BYTE(buf[3]), BCD_BYTE(buf[4]));
        printf("%s", tmp);
    }
}

void show_duration(u8 * buf, u32 buflen)
{
    if (3 != buflen)
    {
       printf("[%s] buflen(%u) is error, must be (40)!", __FUNCTION__, buflen);
    }
    else
    {
        char tmp[32];

        snprintf(tmp, 32, "%2u:%2u:%2u", BCD_BYTE(buf[0]), BCD_BYTE(buf[1]), BCD_BYTE(buf[2]));
        printf("%s", tmp);

    }

}

void EIT_Parser(u8   *pBuf, u32 u32DataLen)
{
    CFieldContainer fieldContainer("EIT Actual");

    // Table Head
    fieldContainer.addField(new CField("table_id", 8));
    fieldContainer.addField(new CField("section_syntax_indicator", 1));
    fieldContainer.addField(new CField("reserved_future_use", 1));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("section_length", 12));
    fieldContainer.addField(new CField("service_id", 16));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("vesion_number", 5));
    fieldContainer.addField(new CField("current_next_indicator", 1));
    fieldContainer.addField(new CField("section_number", 8));
    fieldContainer.addField(new CField("last_section_number", 8));

    fieldContainer.addField(new CField("transport_stream_id", 16));
    fieldContainer.addField(new CField("original_network_id", 16));
    fieldContainer.addField(new CField("segment_last_section_number", 8));
    fieldContainer.addField(new CField("last_table_id", 8));


    //loop 1: event
    CFieldLoopContainer *pfieldLoopContainer = new CFieldLoopContainer("event info");

    pfieldLoopContainer->addField(new CField("event_id", 16));
    pfieldLoopContainer->addField(new CField("start_time", 40, show_start_time));
    pfieldLoopContainer->addField(new CField("duration", 24, show_duration));

    pfieldLoopContainer->addField(new CField("running_status", 3));
    pfieldLoopContainer->addField(new CField("free_CA_mode", 1));
    pfieldLoopContainer->addField(new CDescriptorLoopContainer("event descriptors", 12));

    fieldContainer.addField(pfieldLoopContainer);

    // CRC
    fieldContainer.addField(new CField("CRC_32", 32));

    fieldContainer.parserData(pBuf, u32DataLen, 0);

    fieldContainer.showData();
}



void PAT_Parser(u8 *pBuf, u32 u32DataLen)
{
    CFieldContainer fieldContainer("PAT");

    // Table Head
    fieldContainer.addField(new CField("table_id", 8));
    fieldContainer.addField(new CField("section_syntax_indicator", 1));
    fieldContainer.addField(new CField("0", 1));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("section_length", 12));
    fieldContainer.addField(new CField("transport_stream_id", 16));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("vesion_number", 5));
    fieldContainer.addField(new CField("current_next_indicator", 1));
    fieldContainer.addField(new CField("section_number", 8));
    fieldContainer.addField(new CField("last_section_number", 8));


    // loop1: stream info loop
    CFieldLoopContainer *pfieldLoopContainer = new CFieldLoopContainer("program loop");

    pfieldLoopContainer->addField(new CField("program_number", 16));

    pfieldLoopContainer->addField(new CField("reserved", 3));
    pfieldLoopContainer->addField(new CField("net_or_program_pid", 13));

    fieldContainer.addField(pfieldLoopContainer);

    // CRC
    fieldContainer.addField(new CField("CRC_32", 32));

    fieldContainer.parserData(pBuf, u32DataLen, 0);

    fieldContainer.showData();
}

void PMT_Parser(u8 *pBuf, u32 u32DataLen)
{
    CFieldContainer fieldContainer("PMT");

    // Table Head
    fieldContainer.addField(new CField("table_id", 8));
    fieldContainer.addField(new CField("section_syntax_indicator", 1));
    fieldContainer.addField(new CField("0", 1));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("section_length", 12));
    fieldContainer.addField(new CField("program_number", 16));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("vesion_number", 5));
    fieldContainer.addField(new CField("current_next_indicator", 1));
    fieldContainer.addField(new CField("section_number", 8));
    fieldContainer.addField(new CField("last_section_number", 8));

    fieldContainer.addField(new CField("reserved", 3));
    fieldContainer.addField(new CField("PCR_PID", 13));
    fieldContainer.addField(new CField("reserved", 4));

    // loop1: program info descriptor loop
    fieldContainer.addField(new CDescriptorLoopContainer("programe_info", 12));

    // loop2: element info loop
    CFieldLoopContainer *pfieldLoopContainer = new CFieldLoopContainer("element loop");

    pfieldLoopContainer->addField(new CField("stream_type", 8));

    pfieldLoopContainer->addField(new CField("reserved", 3));
    pfieldLoopContainer->addField(new CField("elementary_PID", 13));

    pfieldLoopContainer->addField(new CField("reserved", 4));

    // loop2-1: element stream info loop
    pfieldLoopContainer->addField(new CDescriptorLoopContainer("element_info", 12));

    fieldContainer.addField(pfieldLoopContainer);

    // CRC
    fieldContainer.addField(new CField("CRC_32", 32));

    fieldContainer.parserData(pBuf, u32DataLen, 0);

    fieldContainer.showData();
}



void BAT_Parser(u8 *pBuf, u32 u32DataLen)
{
    CFieldContainer fieldContainer("BAT Actual");

    // Table Head
    fieldContainer.addField(new CField("table_id", 8));
    fieldContainer.addField(new CField("section_syntax_indicator", 1));
    fieldContainer.addField(new CField("reserved_future_use", 1));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("section_length", 12));
    fieldContainer.addField(new CField("bouquet_id", 16));
    fieldContainer.addField(new CField("Reserved", 2));
    fieldContainer.addField(new CField("vesion_number", 5));
    fieldContainer.addField(new CField("current_next_indicator", 1));
    fieldContainer.addField(new CField("section_number", 8));
    fieldContainer.addField(new CField("last_section_number", 8));

    fieldContainer.addField(new CField("reserved_future_use", 4));

    // Loop 1: network_descriptors
    fieldContainer.addField(new CDescriptorLoopContainer("bouquet_descriptors_length", 12));

    fieldContainer.addField(new CField("reserved_future_use", 4));
    fieldContainer.addField(new CField("transport_stream_loop_length", 12));

    //loop 2: tranport stream info
    CFieldLoopContainer *pfieldLoopContainer = new CFieldLoopContainer("transport stram info");

    pfieldLoopContainer->addField(new CField("transport_stream_id", 16));
    pfieldLoopContainer->addField(new CField("original_network_id", 16));
    pfieldLoopContainer->addField(new CField("reserved_future_use", 4));

    pfieldLoopContainer->addField(new CDescriptorLoopContainer("transport stream descriptors", 12));

    fieldContainer.addField(pfieldLoopContainer);

    // CRC
    fieldContainer.addField(new CField("CRC_32", 32));

    fieldContainer.parserData(pBuf, u32DataLen, 0);

    fieldContainer.showData();
}



// B: Common_Table_ParserEx
//------------------------

// Table record structures
typedef unsigned char U8BIT;
typedef unsigned short U16BIT;
typedef unsigned int U32BIT;

typedef struct si_section_record
{
    struct si_section_record *next;
    U8BIT sect_num;
    U16BIT data_len;
    U8BIT data_start;
    size_t offet;
} SI_SECTION_RECORD;

typedef struct si_table_record
{
    U8BIT path;
    U8BIT tid;
    U16BIT xtid;
    U16BIT pid;
    U8BIT version;
    U16BIT num_sect;
    SI_SECTION_RECORD *section_list;
} SI_TABLE_RECORD;

#define NUM_SECT_RECEIVED_BYTES      (256 / 8)

void reinit_table_rec(SI_TABLE_RECORD *table_rec_ptr)
{
    SI_SECTION_RECORD *tmp_sec_rec_p = NULL;

    tmp_sec_rec_p = table_rec_ptr->section_list;
    while (NULL != tmp_sec_rec_p)
    {
        table_rec_ptr->section_list = table_rec_ptr->section_list->next;
        delete [] tmp_sec_rec_p;

        tmp_sec_rec_p = table_rec_ptr->section_list;
    }

    memset(table_rec_ptr, 0, sizeof(SI_TABLE_RECORD));
}

static double sg_ts_rate;

void Common_Set_TsRate(double tsrate)
{
    sg_ts_rate = tsrate;
}

double Common_Get_TsRate()
{
    return sg_ts_rate;
}

static double max_interval  = 0;
static u32 max_interval_idx = 0;

void Common_Reinit_Max_Interval()
{
    max_interval = 0;
    max_interval_idx = 0;
}


void Common_Get_Max_Interval(u32 &interva_idx, double &interval)
{
    interva_idx = max_interval_idx;
    interval    = max_interval;
}


static u8 sg_tbl_version = -1;
static TABLE_PARSER_FUNC sg_tbl_parser_func = NULL;

void Common_Set_Tbl_Parser(TABLE_PARSER_FUNC func)
{
    sg_tbl_version = -1;
    sg_tbl_parser_func = func;
}

static BOOLEAN sg_Match_Version = TRUE;

void Common_Table_ParserEx_SetMatchVersion(BOOLEAN bMatchVersion)
{
    sg_Match_Version = bMatchVersion;
}

void Common_Table_ParserEx(u8 *pBuf, u32 u32DataLen, size_t szOffset)
{
    static SI_TABLE_RECORD table_rec = {0, 0, 0, 0, 0, 0, NULL};
    static U8BIT sec_rcv_flag[NUM_SECT_RECEIVED_BYTES] = {0};

    SI_SECTION_RECORD *tmp_sec_rec_p = NULL;

    U16BIT sect_len;
    U8BIT tid;
    U16BIT xtid;
    U8BIT version;
    U8BIT sect_no;
    U8BIT last_sect_no;

    tid = pBuf[0];
    sect_len = (((pBuf[1] & 0x0f) << 8) | pBuf[2]) + 3;

    if ((pBuf[5] & 0x01) == 0)
    {
        printf("current_next_indicator is 0, section isn't for a current table! \n");
        return;
    }

    xtid = (pBuf[3] << 8) | pBuf[4];
    version = (pBuf[5] >> 1) & 0x1f;
    sect_no = pBuf[6];
    last_sect_no = pBuf[7];



    if (NULL == table_rec.section_list)
    {
        memset(sec_rcv_flag, 0, NUM_SECT_RECEIVED_BYTES);
    }

    if (((sec_rcv_flag[sect_no >> 3] >> (sect_no & 0x07)) & 0x01) == 1)
    {
        // the section already received
        printf("the section already received! \n");
        return;
    }

    tmp_sec_rec_p = (SI_SECTION_RECORD *)new U8BIT[sizeof(SI_SECTION_RECORD) + u32DataLen];
    if (NULL != tmp_sec_rec_p)
    {
        tmp_sec_rec_p->sect_num = sect_no;
        tmp_sec_rec_p->data_len = u32DataLen;
        tmp_sec_rec_p->offet    = szOffset;
        tmp_sec_rec_p->next     = NULL;

        memcpy(&(tmp_sec_rec_p->data_start), pBuf, u32DataLen);
    }
    else
    {
        printf("new failed! \n");
        return;
    }

    if (NULL == table_rec.section_list)
    {
        table_rec.num_sect      = 1;
        table_rec.tid           = tid;
        table_rec.xtid          = xtid;
        table_rec.version       = version;
        table_rec.section_list  = tmp_sec_rec_p;

        printf("The first rcv section's table_rec.ver:%u \n", table_rec.version);
    }
    else
    {
        if (sg_Match_Version && version != table_rec.version)
        {
            printf("version not match, reset, rcv:%u, table_rec.ver:%u \n", version, table_rec.version);

            // Reset table_rec
            memset(sec_rcv_flag, 0, NUM_SECT_RECEIVED_BYTES);
            reinit_table_rec(&table_rec);

            table_rec.num_sect      = 1;
            table_rec.tid           = tid;
            table_rec.xtid          = xtid;
            table_rec.version       = version;
            table_rec.section_list  = tmp_sec_rec_p;
        }
        else
        {
            SI_SECTION_RECORD *pre_sec_rec_ptr = NULL;
            SI_SECTION_RECORD *cur_sec_rec_ptr = NULL;

            cur_sec_rec_ptr = table_rec.section_list;

            while (NULL != cur_sec_rec_ptr && sect_no > cur_sec_rec_ptr->sect_num)
            {
                pre_sec_rec_ptr = cur_sec_rec_ptr;
                cur_sec_rec_ptr = cur_sec_rec_ptr->next;
            }

            if(NULL == pre_sec_rec_ptr)
            {
                table_rec.section_list = tmp_sec_rec_p;
            }
            else
            {
                pre_sec_rec_ptr->next = tmp_sec_rec_p;
            }

            tmp_sec_rec_p->next    = cur_sec_rec_ptr;
            table_rec.num_sect++;
        }
    }

    sec_rcv_flag[sect_no >> 3] |=  0x01 << (sect_no & 0x07);


    if (table_rec.num_sect == last_sect_no + 1)
    {
        printf("all tbl section received! \n");


        static double cur_time = 0;
        static double pre_time = 0;
        static double interval = 0;

        static u32 idx = 0;

        cur_time = (double)szOffset * 1000 / sg_ts_rate;

        if (pre_time < 0.001 && pre_time > -0.001)
        {
            interval = 0;
            pre_time = cur_time;
        }
        else
        {
            interval = cur_time - pre_time;
            pre_time = cur_time;
        }

        if (interval > max_interval)
        {
            max_interval = interval;
            max_interval_idx = idx;
        }

        printf("Count:%u, Version:%u, sec_num:%u, Time:%f, interval:%f, tsPackOffset:%zu \n", idx++, version, table_rec.num_sect, cur_time, interval, szOffset);

        // Parser Table
        if (NULL != sg_tbl_parser_func)
        {
            if (sg_tbl_version != version)
            {
               u8 idx = 0;
                tmp_sec_rec_p = table_rec.section_list;

                while (NULL != tmp_sec_rec_p)
                {
                    printf("\n");
                    printf("###########################\n");
                    printf("Parse Table Secion:%u \n", idx++);
                    printf("###########################\n");

                    sg_tbl_parser_func(&tmp_sec_rec_p->data_start, tmp_sec_rec_p->data_len);

                    tmp_sec_rec_p = tmp_sec_rec_p->next;
                }

                sg_tbl_version = version;
            }
        }
        // Reset table_rec
        memset(sec_rcv_flag, 0, NUM_SECT_RECEIVED_BYTES);
        reinit_table_rec(&table_rec);
    }


    return;
}

// E: Common_Table_ParserEx

