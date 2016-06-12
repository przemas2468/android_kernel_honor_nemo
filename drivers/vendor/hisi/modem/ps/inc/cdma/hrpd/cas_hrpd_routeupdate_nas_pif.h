/** ****************************************************************************

                    Huawei Technologies Sweden AB (C), 2001-2015

 ********************************************************************************
 * @author    Automatically generated by DAISY
 * @version
 * @date      2015-10-26
 * @file
 * @brief
 * @copyright Huawei Technologies Sweden AB
 *******************************************************************************/
#ifndef CAS_HRPD_ROUTEUPDATE_NAS_PIF_H
#define CAS_HRPD_ROUTEUPDATE_NAS_PIF_H

/*******************************************************************************
 1. Other files included
*******************************************************************************/

#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#pragma pack(4)

/*******************************************************************************
 2. Macro definitions
*******************************************************************************/


/*******************************************************************************
 3. Enumerations declarations
*******************************************************************************/

/** ****************************************************************************
 * Name        : CAS_HRPD_ROUTEUPDATE_NAS_PIF_MSG_TYPE_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum CAS_HRPD_ROUTEUPDATE_NAS_PIF_MSG_TYPE_ENUM
{
    ID_CAS_CNAS_HRPD_IDLE_HO_IND                            = 0x8100, /**< @sa CAS_CNAS_HRPD_IDLE_HO_IND_STRU */
    ID_CNAS_CAS_HRPD_SET_SIGNAL_QUALITY_REQ                 = 0x8101, /**< @sa CNAS_CAS_HRPD_SET_SIGNAL_QUALITY_REQ_STRU */
    ID_CAS_CNAS_HRPD_SET_SIGNAL_QUALITY_CNF                 = 0x8102, /**< @sa CAS_CNAS_HRPD_SET_SIGNAL_QUALITY_CNF_STRU */
    ID_CAS_CNAS_HRPD_SIGNAL_QUALITY_IND                     = 0x8103, /**< @sa CAS_CNAS_HRPD_SIGNAL_QUALITY_IND_STRU */
    ID_CAS_CNAS_HRPD_DATA_SERVICE_AVAILBLE_IND              = 0x8104, /**< @sa CAS_CNAS_HRPD_DATA_SERVICE_AVAILBLE_IND_STRU */
    ID_CAS_HRPD_ROUTEUPDATE_NAS_PIF_MSG_TYPE_ENUM_BUTT      = 0x8105

};
typedef VOS_UINT16 CAS_HRPD_ROUTEUPDATE_NAS_PIF_MSG_TYPE_ENUM_UINT16;

/** ****************************************************************************
 * Name        : CAS_CNAS_HRPD_SIGNAL_QUALITY_CNF_RSLT_ENUM_UINT8
 *
 * Description :
 *******************************************************************************/
enum CAS_CNAS_HRPD_SIGNAL_QUALITY_CNF_RSLT_ENUM
{
    CAS_CNAS_HRPD_SIGNAL_QUALITY_CNF_RSLT_SUCCESS           = 0x00,
    CAS_CNAS_HRPD_SIGNAL_QUALITY_CNF_RSLT_FAIL              = 0x01,
    CAS_CNAS_HRPD_SIGNAL_QUALITY_CNF_RSLT_ENUM_BUTT         = 0x02
};
typedef VOS_UINT8 CAS_CNAS_HRPD_SIGNAL_QUALITY_CNF_RSLT_ENUM_UINT8;

/*******************************************************************************
 4. Message Header declaration
*******************************************************************************/

/*******************************************************************************
 5. Message declaration
*******************************************************************************/

/*******************************************************************************
 6. STRUCT and UNION declaration
*******************************************************************************/

/*******************************************************************************
 7. OTHER declarations
*******************************************************************************/

/** ****************************************************************************
 * Name        : CAS_CNAS_HRPD_IDLE_HO_IND_STRU
 *
 * Description : This msg is send from HRUP to HSM.
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    CAS_HRPD_ROUTEUPDATE_NAS_PIF_MSG_TYPE_ENUM_UINT16       enMsgId;
    VOS_UINT16                                              usOpId;
    VOS_UINT8                                               aucReserve[4];
} CAS_CNAS_HRPD_IDLE_HO_IND_STRU;

/** ****************************************************************************
 * Name        : CNAS_CAS_HRPD_SET_SIGNAL_QUALITY_REQ_STRU
 *
 * Description : This msg is send from HSD to HRUP.
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    CAS_HRPD_ROUTEUPDATE_NAS_PIF_MSG_TYPE_ENUM_UINT16       enMsgId;
    VOS_UINT16                                              usOpId;
    VOS_UINT8                                               ucRptRssiThreshold; /**< in 1db,when rssi change above this threshold ,report ind to nas */
    VOS_UINT8                                               ucRptEcIoThreshold; /**< in 1db,when ecIo change above this threshold ,report ind to nas */
    VOS_UINT8                                               ucRptSinrThreshold; /**< in 1db,when Sinr change above this threshold ,report ind to nas */
    VOS_UINT8                                               ucTimeInterval;     /**< time interval */
} CNAS_CAS_HRPD_SET_SIGNAL_QUALITY_REQ_STRU;

/** ****************************************************************************
 * Name        : CAS_CNAS_HRPD_SET_SIGNAL_QUALITY_CNF_STRU
 *
 * Description : This msg is send from HRUP to HSD.
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    CAS_HRPD_ROUTEUPDATE_NAS_PIF_MSG_TYPE_ENUM_UINT16       enMsgId;
    VOS_UINT16                                              usOpId;
    CAS_CNAS_HRPD_SIGNAL_QUALITY_CNF_RSLT_ENUM_UINT8        enRslt;
    VOS_UINT8                                               aucReserve[3];
} CAS_CNAS_HRPD_SET_SIGNAL_QUALITY_CNF_STRU;

/** ****************************************************************************
 * Name        : CAS_CNAS_HRPD_SIGNAL_QUALITY_IND_STRU
 *
 * Description : This msg is send from HRUP to HSD.
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    CAS_HRPD_ROUTEUPDATE_NAS_PIF_MSG_TYPE_ENUM_UINT16       enMsgId;
    VOS_UINT16                                              usOpId;
    VOS_INT16                                               sHrpdRssi;     /**< in 1db, [-135,0],power of antenna0 ,invalid value =0x7fff */
    VOS_INT16                                               sHrpdSinr;     /**< in 1db,[0,8],invalid value = 0x7fff */
    VOS_INT16                                               sHrpdEcIo;     /**< in 0.125db,[-200,0],invalid value = 0x7fff */
    VOS_UINT8                                               aucReserve[2];
} CAS_CNAS_HRPD_SIGNAL_QUALITY_IND_STRU;

/** ****************************************************************************
 * Name        : CAS_CNAS_HRPD_DATA_SERVICE_AVAILBLE_IND_STRU
 *
 * Description : This msg is send from HRUP to HSM.
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    CAS_HRPD_ROUTEUPDATE_NAS_PIF_MSG_TYPE_ENUM_UINT16       enMsgId;
    VOS_UINT16                                              usOpId;
    VOS_UINT8                                               aucReserve[4];
} CAS_CNAS_HRPD_DATA_SERVICE_AVAILBLE_IND_STRU;

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
