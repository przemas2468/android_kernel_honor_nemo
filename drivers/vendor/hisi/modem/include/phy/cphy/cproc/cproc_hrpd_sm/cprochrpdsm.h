/** ****************************************************************************

                    Huawei Technologies Sweden AB (C), 2001-2015

 ********************************************************************************
 * @author    Automatically generated by DAISY
 * @version
 * @date      2015-12-14 14:27:17
 * @file
 * @brief
 * CPROCHRPDSM is the measurement interface towards HRPD_Layer_3_AS.
 * -  Input: Neighbor measurement configuration. Frequency: When Neighbor set
 *    changes.
 * -  Output: Confirms of configurations to the requester. Frequency: Once per
 *    configuration.
 * -  Output: Measurement Report with measured values per cell and channel.
 *    Frequency: Every measurement period (80 ms or paging period).
 * @copyright Huawei Technologies Sweden AB
 *******************************************************************************/
#ifndef CPROCHRPDSM_H
#define CPROCHRPDSM_H

/*******************************************************************************
 1. Other files included
*******************************************************************************/

#include "vos.h"
#include "cproc_hrpd.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#pragma pack(4)

/*******************************************************************************
 2. Macro definitions
*******************************************************************************/

/**
 * Indicates a not valid value.
 */
#define CPROC_HRPD_NOT_VALID            ( 0x7FFF )
/**
 * The maximum number of pilots in the neighbor list.
 */
#define CPROC_HRPD_MAX_NEIGHBOR_NUM     ( 44 )

/*******************************************************************************
 3. Enumerations declarations
*******************************************************************************/

/** ****************************************************************************
 * Name        : CPROCHRPDSM_MSG_TYPE_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum CPROCHRPDSM_MSG_TYPE_ENUM
{
    ID_CAS_CPROC_HRPD_MEAS_PILOTSET_REQ = 0x4600, /**< @sa CAS_CPROC_HRPD_MEAS_PILOTSET_REQ_STRU */
    ID_CPROC_CAS_HRPD_MEAS_PILOTSET_CNF = 0x4601, /**< @sa CPROC_CAS_HRPD_MEAS_PILOTSET_CNF_STRU */
    ID_CPROC_CAS_HRPD_MEAS_PILOTSET_IND = 0x4602, /**< @sa CPROC_CAS_HRPD_MEAS_PILOTSET_IND_STRU */
    ID_CPROCHRPDSM_MSG_TYPE_ENUM_BUTT   = 0x4603
};
typedef VOS_UINT16 CPROCHRPDSM_MSG_TYPE_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CPROC_HRPD_MEASURE_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum CPROC_HRPD_MEASURE_ENUM
{
    CPROC_HRPD_MEASURE_PERIODICALLY     = 0x0000,
    CPROC_HRPD_MEASURE_ONCE             = 0x0001,
    CPROC_HRPD_MEASURE_BUTT             = 0x0002 /**< Identifies if the measurement shall be done periodically or once. */
};
typedef VOS_UINT16 CPROC_HRPD_MEASURE_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CPROC_HRPD_CDMA_TIME_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum CPROC_HRPD_CDMA_TIME_ENUM
{
    CPROC_HRPD_CDMA_TIME_SYNC           = 0x0000,
    CPROC_HRPD_CDMA_TIME_ASYNC          = 0x0001,
    CPROC_HRPD_CDMA_TIME_BUTT           = 0x0002
};
typedef VOS_UINT16 CPROC_HRPD_CDMA_TIME_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CPROC_HRPD_LTE_CDMA_TIME_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum CPROC_HRPD_LTE_CDMA_TIME_ENUM
{
    CPROC_HRPD_LTE_CDMA_TIME_PRESENT                        = 0x0000,
    CPROC_HRPD_LTE_CDMA_TIME_NOT_PRESENT                    = 0x0001,
    CPROC_HRPD_LTE_CDMA_TIME_BUTT                           = 0x0002
};
typedef VOS_UINT16 CPROC_HRPD_LTE_CDMA_TIME_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CPROC_HRPD_WINDOW_SIZE_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum CPROC_HRPD_WINDOW_SIZE_ENUM
{
    CPROC_HRPD_WINDOW_SIZE_4            = 0x0000,
    CPROC_HRPD_WINDOW_SIZE_6            = 0x0001,
    CPROC_HRPD_WINDOW_SIZE_8            = 0x0002,
    CPROC_HRPD_WINDOW_SIZE_10           = 0x0003,
    CPROC_HRPD_WINDOW_SIZE_14           = 0x0004,
    CPROC_HRPD_WINDOW_SIZE_20           = 0x0005,
    CPROC_HRPD_WINDOW_SIZE_28           = 0x0006,
    CPROC_HRPD_WINDOW_SIZE_40           = 0x0007,
    CPROC_HRPD_WINDOW_SIZE_60           = 0x0008,
    CPROC_HRPD_WINDOW_SIZE_80           = 0x0009,
    CPROC_HRPD_WINDOW_SIZE_100          = 0x000A,
    CPROC_HRPD_WINDOW_SIZE_130          = 0x000B,
    CPROC_HRPD_WINDOW_SIZE_160          = 0x000C,
    CPROC_HRPD_WINDOW_SIZE_226          = 0x000D,
    CPROC_HRPD_WINDOW_SIZE_320          = 0x000E,
    CPROC_HRPD_WINDOW_SIZE_452          = 0x000F,
    CPROC_HRPD_USE_DEFAULT              = 0x0010,
    CPROC_HRPD_WINDOW_SIZE_BUTT         = 0x0011
};
typedef VOS_UINT16 CPROC_HRPD_WINDOW_SIZE_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CPROC_HRPD_WINDOW_OFFSET_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum CPROC_HRPD_WINDOW_OFFSET_ENUM
{
    CPROC_HRPD_WINDOW_OFFSET_NO                             = 0x0000,
    CPROC_HRPD_WINDOW_OFFSET_HALF                           = 0x0001,
    CPROC_HRPD_WINDOW_OFFSET_ONE                            = 0x0002,
    CPROC_HRPD_WINDOW_OFFSET_THREE_HALF                     = 0x0003,
    CPROC_HRPD_WINDOW_OFFSET_HALF_NEG                       = 0x0004,
    CPROC_HRPD_WINDOW_OFFSET_ONE_NEG                        = 0x0005,
    CPROC_HRPD_WINDOW_OFFSET_THREE_HALF_NEG                 = 0x0006,
    CPROC_HRPD_WINDOW_OFFSET_BUTT                           = 0x0007
};
typedef VOS_UINT16 CPROC_HRPD_WINDOW_OFFSET_ENUM_UINT16;

/*******************************************************************************
 4. Message Header declaration
*******************************************************************************/

/*******************************************************************************
 5. Message declaration
*******************************************************************************/

/*******************************************************************************
 6. STRUCT and UNION declaration
*******************************************************************************/

/** ****************************************************************************
 * Name        : CPROC_HRPD_PILOT_STRGTH_PARA_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    CPROC_HRPD_CHANNEL_RECORD_STRU      stFreq;            /**< Frequency */
    VOS_UINT16                          usPilotPN;         /**< Pilot PN */
    VOS_INT16                           sStrength;         /**< Strength */
    VOS_UINT32                          ulPnPhase;         /**< PN Phase,in uint of 1/16 chips */
    VOS_INT16                           sRelativeStrength; /**< Sum of pilot strength of AS pilots weaker than this pilot. Only given for AS pilots. -8 to -200 ( 0.125dB resolution ) or CPROC_HRPD_NOT_VALID */
    VOS_INT16                           sHrpdSinr;         /**< [-25,-1] or CPROC_HRPD_NOT_VALID. Only given for AS pilots. */
} CPROC_HRPD_PILOT_STRGTH_PARA_STRU;

/** ****************************************************************************
 * Name        : CPROC_HRPD_NEIGHBOR_SET_PARA_STRU
 *
 * Description : Pilot parameters per pilot in the neighbor list.
 *******************************************************************************/
typedef struct
{
    VOS_UINT16                                              usPilotPN;            /**< Pilot PN, [0..511]. If set to CPROC_HRPD_NOT_VALID all cells shall be measured for this frequency. */
    CPROC_HRPD_WINDOW_SIZE_ENUM_UINT16                      usSearchWindowSize;   /**< Window size, or CPROC_HRPD_WINDOW_SIZE_DEFAULT = CPROC_HRPD_USE_DEFAULT */
    CPROC_HRPD_WINDOW_OFFSET_ENUM_UINT16                    usSearchWindowOffset; /**< Window offset relative serving cell. If no offset is known it is set to CPROC_HRPD_WINDOW_OFFSET_NO */
    VOS_UINT8                                               aucReserve[2];        /**< 32 bit alignment */
    CPROC_HRPD_CHANNEL_RECORD_STRU                          stFreq;               /**< Frequency */
} CPROC_HRPD_NEIGHBOR_SET_PARA_STRU;

/** ****************************************************************************
 * Name        : CPROC_HRPD_SEARCH_PARAMETERS_STRU
 *
 * Description : Parameters for measurements, defined in SearchParameters
 * Attribute, see section 8.7.7.2.1.
 *******************************************************************************/
typedef struct
{
    VOS_UINT16                          usPilotIncrement;        /**< PN sequence increment in units of 64 chips */
    CPROC_HRPD_WINDOW_SIZE_ENUM_UINT16  usSearchWindowActive;    /**< Search window size for the Active Set and Candidate Set. */
    CPROC_HRPD_WINDOW_SIZE_ENUM_UINT16  usSearchWindowRemaining; /**< Search window size for the Remaining Set. */
    VOS_UINT16                          usReserved;              /**< 32-bit alignment. */
} CPROC_HRPD_SEARCH_PARAMETERS_STRU;

/** ****************************************************************************
 * Name        : CPROC_HRPD_CDMA_TIME_STRU
 *
 * Description : The CDMA system time in SIB8 (IE SystemTimeInfoCDMA2000 in
 * 36.331)
 *******************************************************************************/
typedef struct
{
    CPROC_HRPD_CDMA_TIME_ENUM_UINT16    ucSystemTimeType;           /**< Sync or Async */
    VOS_UINT8                           aucReserve[2];              /**< 32 bit alignment */
    VOS_UINT32                          ulSystemTimeLow32BitsValue; /**< The lower 32 bits value of system time */
    VOS_UINT32                          ulSystemTimeHighBitsValue;  /**< The higher bits value of system time */
} CPROC_HRPD_CDMA_TIME_STRU;

/** ****************************************************************************
 * Name        : CPROC_HRPD_LTE_CDMA_TIME_STRU
 *
 * Description : Parameters for synchronization between LTE and CDMA time used
 * in HRPD slave mode.
 *******************************************************************************/
typedef struct
{
    CPROC_HRPD_CDMA_TIME_STRU           stSystemTimeInfoCDMA; /**< absolute time in the current cell */
    VOS_UINT16                          usLteSfn;             /**< SFN boundary at or after the ending boundary of the SI-Window in which SystemInformationBlockType8 is transmitted */
    VOS_UINT8                           aucReserve[2];        /**< 32 bit alignment */
    VOS_UINT32                          aulTimeStamp[2];      /**< TimeStamp[0] is Lte's timing;TimeStamp[1] is  32k comm timing */
} CPROC_HRPD_LTE_CDMA_TIME_STRU;

/*******************************************************************************
 7. OTHER declarations
*******************************************************************************/

/** ****************************************************************************
 * Name        : CAS_CPROC_HRPD_MEAS_PILOTSET_REQ_STRU
 *
 * Description : This signal is used by HRPD_Layer_3_AS to request CPROC_HRPD
 * to measure pilot strength of sectors in neighbor set.
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    CPROCHRPDSM_MSG_TYPE_ENUM_UINT16                        enMsgId;
    VOS_UINT16                                              usOpId;
    VOS_UINT8                                               ucNumNeighborList;                            /**< size of neighbor list 1..CPROC_HRPD_MAX_NEIGHBOR_NUM */
    VOS_UINT8                                               ucReserve;                                    /**< 32 bit alignment */
    VOS_INT16                                               sThresholdStrength;                           /**< The Pilot drop value in 1/8 db increment */
    CPROC_HRPD_LTE_CDMA_TIME_ENUM_UINT16                    enTimePresent;                                /**< valid flag for stLteCdmaTime */
    CPROC_HRPD_MEASURE_ENUM_UINT16                          enMeasType;                                   /**< one shot or periodic */
    CPROC_HRPD_NEIGHBOR_SET_PARA_STRU                       astNeighborList[CPROC_HRPD_MAX_NEIGHBOR_NUM]; /**< Neighbor list */
    CPROC_HRPD_SEARCH_PARAMETERS_STRU                       stSearchParameters;                           /**< Pilot Inc and window sizes */
    CPROC_HRPD_LTE_CDMA_TIME_STRU                           stLteCdmaTime;                                /**< synchronization between LTE and CDMA time */
} CAS_CPROC_HRPD_MEAS_PILOTSET_REQ_STRU;

/** ****************************************************************************
 * Name        : CPROC_CAS_HRPD_MEAS_PILOTSET_CNF_STRU
 *
 * Description : This signal confirm CAS_CPROC_HRPD_MEAS_PILOTSET_REQ.
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    CPROCHRPDSM_MSG_TYPE_ENUM_UINT16    enMsgId;
    VOS_UINT16                          usOpId;
    VOS_UINT32                          ulResult; /**< 0-success, others-failure */
} CPROC_CAS_HRPD_MEAS_PILOTSET_CNF_STRU;

/** ****************************************************************************
 * Name        : CPROC_CAS_HRPD_MEAS_PILOTSET_IND_STRU
 *
 * Description : This signal is used by CPROC_HRPD to indicate measured pilot
 * strength of sectors requested by HRPD_Layer_3_AS in
 * CAS_CPROC_HRPD_MEAS_PILOTSET_REQ.
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    CPROCHRPDSM_MSG_TYPE_ENUM_UINT16    enMsgId;
    VOS_UINT16                          usOpId;
    VOS_UINT32                          ulTimeRefMs;                                     /**< 0..14399999, resolution 1ms. System time mode 14400000, CS0022 TIME_REF_MS */
    VOS_INT16                           sMobSysTOffset;                                  /**< -512..511.9375, resolution 1/16chip CS0022 MOB_SYS_T_OFFSET */
    VOS_UINT16                          usReserve;                                       /**< 32 bit alignment */
    CPROC_HRPD_PILOT_STRGTH_PARA_STRU   astPilotStrgthList[CPROC_HRPD_MAX_NEIGHBOR_NUM]; /**< Measurement results */
    CPROC_HRPD_CHANNEL_RECORD_STRU      stFreqInfo;                                      /**< Frequency of reference pilot */
    VOS_UINT16                          usRefPN;                                         /**< 0..511, PNOffset of the reference pilot */
    VOS_UINT8                           ucNum_pilots;                                    /**< number of pilots in stPilotStrgthList 1..CPROC_HRPD_MAX_NEIGHBOR_NUM */
    VOS_UINT8                           aucReserve[1];                                   /**< 32 bit alignment */
    VOS_INT16                           sTotalAsEcIo;                                    /**< -25..15 resolution 0.125dBm total EcIo of the active set. Valid in TCH only. */
    CPROC_HRPD_MEASURE_ENUM_UINT16      enMeasType;                                      /**< one shot or periodic */
    VOS_INT16                           sServFreqRssi;                                   /**< -880..0, resolution 0.125dBm. Measured RSSI on the serving frequency or CPROC_HRPD_NOT_VALID */
    VOS_UINT8                           aucReserve1[2];                                  /**< 32 bit alignment */
} CPROC_CAS_HRPD_MEAS_PILOTSET_IND_STRU;

/*******************************************************************************
 8. Global  declaration
*******************************************************************************/

/*******************************************************************************
 9. Function declarations
*******************************************************************************/

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif
