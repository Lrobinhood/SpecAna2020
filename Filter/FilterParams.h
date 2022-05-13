/******************************************************************************************



	author:	Robin
	date:	2013-10-25

******************************************************************************************/

#ifndef __SECTIONFILTER_PARAMS_H__
#define __SECTIONFILTER_PARAMS_H__


#define DRV_TSPU_FILT_SECTION_PATTERN_LENGTH                (5)    /*!< The maximum length of Section filter Match & Mask pattern  */
#define DRV_TSPU_FILT_PES_PATTERN_LENGTH                    (4)     /*!< The maximum length of PES filter Match & Mask pattern      */
#define DRV_TSPU_FILT_ES_PATTERN_LENGTH                     (4)     /*!< The maximum length of ES filter Match & Mask pattern       */
#define DRV_TSPU_DESC_DVB_CSA_CW_LENGTH                     (8)     /*!< The length of DVB CSA Control Word                         */
#define DRV_TSPU_DESC_DVB_CSA3_CW_LENGTH                    (16)    /*!< The length of DVB CSA3 Control Word                        */
#define DRV_TSPU_DESC_ATSC_TDES_CW_LENGTH                   (8)     /*!< The length of ATSC TDES Control Word                       */
#define DRV_TSPU_DESC_CI_PLUS_AES_CW_LENGTH                 (16)    /*!< The length of CI Plus AES Control Word                     */
#define DRV_TSPU_DESC_CI_PLUS_DES_CW_LENGTH                 (8)     /*!< The length of CI Plus DES Control Word                     */
#define DRV_TSPU_DESC_CA_AES_CW_LENGTH                      (16)    /*!< The length of CA AES Control Word                          */
#define DRV_TSPU_DESC_CA_DES_CW_LENGTH                      (8)     /*!< The length of CA DES Control Word                          */
#define DRV_TSPU_DESC_ISDB_MULTI2_DATA_CW_LENGTH            (8)     /*!< The length of ISDB MULTI2 Data Control Word                */
#define DRV_TSPU_DESC_ISDB_MULTI2_SYSTEM_CW_LENGTH          (32)    /*!< The length of ISDB MULTI2 System Control Word              */
#define DRV_TSPU_CRYPTO_MULTI2_CTRL_MAX_ROUND_NO		    (256)   /*!< The maximum number of Multi2 round for Crypto-Engine       */

/**
 * \brief
 *  enumurate to indicate the Filter Type
 */
typedef enum
{
    EN_DRV_TSPU_FILT_TYPE_NONE,                                     /*!< Filter Type None 					        */
    EN_DRV_TSPU_FILT_TYPE_SECTION,                                  /*!< Section Filter							    */
    EN_DRV_TSPU_FILT_TYPE_PES,                                      /*!< PES Filter       							*/
    EN_DRV_TSPU_FILT_TYPE_ES,                                       /*!< ES Filter      							*/
    EN_DRV_TSPU_FILT_TYPE_TS,                                       /*!< TS Filter      							*/
    EN_DRV_TSPU_FILT_TYPE_M2TS,                                     /*!< M2TS Filter      							*/
    EN_DRV_TSPU_FILT_TYPE_NUM,
}EN_DRV_TSPU_FILT_TYPE;

/**
 * \brief
 *  the filter open structure
 */
typedef struct
{
    EN_DRV_TSPU_FILT_TYPE           enFiltType;                     /*!< Filter Type  */
}ST_DRV_TSPU_FILT_OPEN_PARAMS, *PST_DRV_TSPU_FILT_OPEN_PARAMS;

/**
 * \brief
 *  the Section Filter structure
 */
typedef struct
{
    u8                              au8Match[DRV_TSPU_FILT_SECTION_PATTERN_LENGTH];     /*!< Filter Match pattern               */
    u8                              au8Mask[DRV_TSPU_FILT_SECTION_PATTERN_LENGTH];      /*!< Filter Mask pattern                */
    u8                              au8Excl[DRV_TSPU_FILT_SECTION_PATTERN_LENGTH];      /*!< Filter Exclude Mask Pattern        */
}ST_DRV_TSPU_FILT_SECTION_PARAMS, *PST_DRV_TSPU_FILT_SECTION_PARAMS;

/**
 * \brief
 *  the PES Filter structure
 */
typedef struct
{
    u8                              au8Match[DRV_TSPU_FILT_PES_PATTERN_LENGTH];         /*!< Filter Match pattern               */
    u8                              au8Mask[DRV_TSPU_FILT_PES_PATTERN_LENGTH];          /*!< Filter Mask pattern                */
}ST_DRV_TSPU_FILT_PES_PARAMS, *PST_DRV_TSPU_FILT_PES_PARAMS;

/**
 * \brief
 *  the ES Filter structure
 */
typedef struct
{
    u8                              au8Match[DRV_TSPU_FILT_ES_PATTERN_LENGTH];          /*!< Filter Match pattern               */
    u8                              au8Mask[DRV_TSPU_FILT_ES_PATTERN_LENGTH];           /*!< Filter Mask pattern                */
    u16                             u16RequestLength;                                   /*!< Filter Notify Request Length       */
}ST_DRV_TSPU_FILT_ES_PARAMS, *PST_DRV_TSPU_FILT_ES_PARAMS;

/**
 * \brief
 *  the TS Filter structure
 */
typedef struct
{
    u16                             u16RequestLength;                                   /*!< Filter Notify Request Length       */
}ST_DRV_TSPU_FILT_TS_PARAMS, *PST_DRV_TSPU_FILT_TS_PARAMS;

/**
 * \brief
 *  the M2TS Filter structure
 */
typedef struct
{
    u16                             u16RequestLength;                                   /*!< Filter Notify Request Length       */
}ST_DRV_TSPU_FILT_M2TS_PARAMS, *PST_DRV_TSPU_FILT_M2TS_PARAMS;



typedef union
{
    ST_DRV_TSPU_FILT_SECTION_PARAMS     stSectionParams;        /*!< Section Filter Parameters      */
    ST_DRV_TSPU_FILT_PES_PARAMS         stPesParams;            /*!< PES Filter Parameters          */
    ST_DRV_TSPU_FILT_ES_PARAMS          stEsParams;             /*!< ES Filter Parameters           */
    ST_DRV_TSPU_FILT_TS_PARAMS          stTsParams;             /*!< TS Filter Parameters           */
    ST_DRV_TSPU_FILT_M2TS_PARAMS        stM2tsParams;           /*!< M2TS Filter Parameters         */
}UN_DRV_TSPU_FILTER_CFG;

/*

    transport_packet()
    {
            sync_byte                                   8 bslbf         BYTE 0

            transport_error_indicator                   1 bslbf         BYTE 1, BYTE 2
            payload_unit_start_indicator                1 bslbf
            transport_priority                          1 bslbf
            PID                                         13 uimsbf

            transport_scrambling_control                2 bslbf         BYTE 3
            adaptation_field_control                    2 bslbf
            continuity_counter                          4 uimsbf

            if(adaptation_field_control = = '10' || adaptation_field_control = = '11')
            {
                adaptation_field()
            }

            if(adaptation_field_control = = '01' || adaptation_field_control = = '00')
            {
                for (i = 0; i < N; i++)
                {
                    data_byte                           8 bslbf
                }
            }
    }


    adaptation_field()
    {
        adaptation_field_length                         8 uimsbf

        ... ...
    }
*/

#define TS_PACKAGE_HEAD_LEN     (4)

inline u8 sync_byte(u8 *pTsPackageHead)
{
    return *pTsPackageHead;
}

inline u8 transport_error_indicator(u8 *pTsPackageHead)
{
    return (pTsPackageHead[1] & BIT7) >> 7;
}

inline u8 payload_unit_start_indicator(u8 *pTsPackageHead)
{
    return (pTsPackageHead[1] & BIT6) >> 6;
}

inline u8 transport_priority(u8 *pTsPackageHead)
{
    return (pTsPackageHead[1] & BIT5) >> 5;
}

inline u8 pid(u8 *pTsPackageHead)
{
    return ((pTsPackageHead[1] & (BIT4|BIT3|BIT2|BIT1|BIT0)) << 8) + pTsPackageHead[2];
}

inline u8 transport_scrambling_control(u8 *pTsPackageHead)
{
    return (pTsPackageHead[3] & (BIT7 | BIT6)) >> 6;
}

inline u8 adaptation_field_control(u8 *pTsPackageHead)
{
    return (pTsPackageHead[3] & (BIT5 | BIT4)) >> 4;
}

inline u8 continuity_counter(u8 *pTsPackageHead)
{
    return (pTsPackageHead[3] & (BIT3 | BIT2 | BIT1 | BIT0));
}

inline u8 adaptation_field_length(u8 *pTsPackageHead)
{
    return pTsPackageHead[4];
}



#endif



