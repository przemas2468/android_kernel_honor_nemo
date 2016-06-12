
#ifndef _CNAS_CCB_H_
#define _CNAS_CCB_H_

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
#include "PsCommonDef.h"
#include "PsLogdef.h"
#include "PsTypeDef.h"
#include "cas_1x_access_ctrl_proc_nas_pif.h"

#include "CnasPrlParse.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#pragma pack(4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define P_REV_IN_USE_7                                      (7)

#define P_REV_IN_USE_6                                      (6)


#define CNAS_CCB_MAX_HOME_SID_NID_LIST                      (20)

#define CNAS_CCB_ICCID_OCTET_LEN                            (10)

/* the byte num of EFRUIMID read from card  5 or 8*/
#define CNAS_CCB_EFRUIMID_OCTET_LEN_EIGHT                   (8)
#define CNAS_CCB_EFRUIMID_OCTET_LEN_FIVE                    (5)

#define CNAS_CCB_UIMID_OCTET_LEN                            (4)
#define CNAS_CCB_MAX_LOG_MSG_STATE_NUM                      (100)

#define CNAS_CCB_MAX_WHITE_LOCK_SYS_NUM                     (20)

#define CNAS_CCB_MAX_FREQ_NUM                               (10)

#define CNAS_CCB_MNTN_1X_CALL_STATE_MAX_NUM                 (4)     /* match with the num of max xcc fsm entity */

#define CNAS_CCB_MEID_OCTET_LEN                             (7)
#define CNAS_CCB_ESN_OCTET_LEN                              (4)
#define CNAS_CCB_ESN_MEID_OCTET_LEN                         (8)
#define CNAS_CCB_MAX_AUTHDATA_USERNAME_LEN                  (253)   /* HRPD ppp AN��Ȩ�û�����û�����
                                                                       �ο�C.S0023-D section 3.4.53�Լ�
                                                                       C.S0016-D,section 3.5.8.13,
                                                                       �ó��������Ϊ255-2(NAI������
                                                                       ����Ȩ�㷨�����ֽ�ռ�õ��ֽ�) */

#define CNAS_CCB_BYTE_LEN_8_BIT                             (8)

/* ���� Pid�Լ�usMsgName ����EventType  */
/*
    Լ��: PID�ṹ Bit0~Bit11: ģ��ID  Bit12~Bit15: ���� Bit16~Bit19: CPU ID Bit20~Bit31: ����
    ��: PID: 0x000A0BCD  ->  0x0000ABCD  ->  0xABCD0000
        MSG: 0x0000abcd
        EVT: 0xABCDabcd
*/
#define CNAS_BuildEventType(ulSndPid,usMsgName) \
    (((VOS_UINT32)(/*lint -e778*/((ulSndPid) & 0xF0000)/*lint +e778*/ \
    | (((ulSndPid) & 0xFFF) << 4)) << 12) | (VOS_UINT16)(usMsgName))      /* ���� Pid�Լ�usMsgName ����EventType  */

#define CNAS_MIN(x, y)\
        (((x)<(y))?(x):(y))

#define CNAS_MAX(x, y)\
        (((x)>(y))?(x):(y))

/*lint -e778 */
/*lint -e572 */
#ifndef CNAS_NTOHL                   /* ��С�ֽ���ת��*/
#if VOS_BYTE_ORDER==VOS_BIG_ENDIAN
#define CNAS_NTOHL(x)    (x)
#define CNAS_HTONL(x)    (x)
#define CNAS_NTOHS(x)    (x)
#define CNAS_HTONS(x)    (x)
#else
#define CNAS_NTOHL(x)    ((((x) & 0x000000ffU) << 24) | \
                         (((x) & 0x0000ff00U) <<  8) | \
                         (((x) & 0x00ff0000U) >>  8) | \
                         (((x) & 0xff000000U) >> 24))

#define CNAS_HTONL(x)    ((((x) & 0x000000ffU) << 24) | \
                         (((x) & 0x0000ff00U) <<  8) | \
                         (((x) & 0x00ff0000U) >>  8) | \
                         (((x) & 0xff000000U) >> 24))

#define CNAS_NTOHS(x)    ((((x) & 0x00ff) << 8) | \
                         (((x) & 0xff00) >> 8))

#define CNAS_HTONS(x)    ((((x) & 0x00ff) << 8) | \
                         (((x) & 0xff00) >> 8))
#endif  /* _BYTE_ORDER==_LITTLE_ENDIAN */
#endif
/*lint -e572 */
/*lint -e778 */

#define     CNAS_CAS_INVALID_SUBSTA     (0xFF)

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
enum CNAS_CCB_CARD_STATUS_ENUM
{
    CNAS_CCB_CARD_STATUS_ABSENT         = 0x0000,
    CNAS_CCB_CARD_STATUS_UIM_PRESENT    = 0x0001,                   /* uim present */
    CNAS_CCB_CARD_STATUS_CSIM_PRESENT   = 0x0002,                   /* csim present */
    CNAS_CCB_CARD_STATUS_SIM_PRESENT    = 0x0003,                   /* sim present */
    CNAS_CCB_CARD_STATUS_USIM_PRESENT   = 0x0004,                   /* usim present */
    CNAS_CCB_CARD_STATUS_BUTT           = 0x0005
};
typedef VOS_UINT8 CNAS_CCB_CARD_STATUS_ENUM_UINT8;

/** ****************************************************************************
 * Name        : CNAS_CCB_PS_RATTYPE_ENUM
 *
 * Description :
 *******************************************************************************/
enum CNAS_CCB_PS_RATTYPE_ENUM
{
    CNAS_CCB_PS_RATTYPE_NULL            = 0x00000000,
    CNAS_CCB_PS_RATTYPE_1X              = 0x00000001,
    CNAS_CCB_PS_RATTYPE_HRPD            = 0x00000002,
    CNAS_CCB_PS_RATTYPE_EHRPD           = 0x00000003,
    CNAS_CCB_PS_RATTYPE_LTE             = 0x00000004,
    CNAS_CCB_PS_RATTYPE_BUTT            = 0x00000005
};
typedef VOS_UINT32 CNAS_CCB_PS_RATTYPE_ENUM_UINT32;


/** ****************************************************************************
 * Name        : CNAS_CCB_1X_CAS_STATE_ENUM_UINT8
 *
 * Description : When MS is in the different state, MS should perform
 * different actions.
 * Refers to  C.S0017 2.6.5.5
 *******************************************************************************/
enum CNAS_CCB_1X_CAS_STATE_ENUM
{
    CNAS_CCB_1X_CAS_INIT_STATE          = 0x00,
    CNAS_CCB_1X_CAS_IDLE_STATE          = 0x01,
    CNAS_CCB_1X_CAS_ACCESS_STATE        = 0x02,
    CNAS_CCB_1X_CAS_TCH_STATE           = 0x03,
    CNAS_CCB_1X_CAS_NO_SERVICE_STATE    = 0x04,
    CNAS_CCB_1X_CAS_STATE_BUTT          = 0x05
};
typedef VOS_UINT8 CNAS_CCB_1X_CAS_STATE_ENUM_UINT8;


/** ****************************************************************************
 * Name        : CNAS_CCB_1X_CAS_SUB_STATE_ENUM_UINT8
 *
 * Description : .
 * Refers to  C.S0017 2.6.5.5
 *******************************************************************************/
enum CNAS_CCB_1X_CAS_SUB_STATE_ENUM
{
    CNAS_CCB_1X_CAS_SUB_STATE_NULL                          = 0x00,
    CNAS_CCB_1X_CAS_INIT_DETERM_SUB_STATE                   = 0x01,
    CNAS_CCB_1X_CAS_PILOT_CH_ACQ_SUB_STATE                  = 0x02,
    CNAS_CCB_1X_CAS_SYNC_CH_ACQ_SUB_STATE                   = 0x03,
    CNAS_CCB_1X_CAS_TIMING_CHNG_SUB_STATE                   = 0x04,
    CNAS_CCB_1X_CAS_IDLE_SUB_STATE                          = 0x10,
    CNAS_CCB_1X_CAS_UPDATE_OVERHEAD_SUB_STATE               = 0x20,
    CNAS_CCB_1X_CAS_MOB_STATION_ORIG_ATTEMPT_SUB_STATE      = 0x21,
    CNAS_CCB_1X_CAS_PAG_RESP_SUB_STATE                      = 0x22,
    CNAS_CCB_1X_CAS_MOB_STATION_ORDR_RESP_SUB_STATE         = 0x23,
    CNAS_CCB_1X_CAS_REG_ACCESS_SUB_STATE                    = 0x24,
    CNAS_CCB_1X_CAS_MOB_STATION_MSG_TRANS_SUB_STATE         = 0x25,
    CNAS_CCB_1X_CAS_TCH_INIT_SUB_STATE                      = 0x30,
    CNAS_CCB_1X_CAS_REL_SUB_STATE                           = 0x31,
    CNAS_CCB_1X_CAS_WAIT_FOR_SERVICE_SUB_STATE              = 0x40,
    CNAS_CCB_1X_CAS_SUB_STATE_ENUM_BUTT                     = 0x41
};
typedef VOS_UINT8 CNAS_CCB_1X_CAS_SUB_STATE_ENUM_UINT8;

/** ****************************************************************************
 * Name        : CNAS_CCB_1X_CAS_P_REV_ENUM_UINT8
 *
 * Description : .
 * Refers to
 *******************************************************************************/
enum CNAS_CCB_1X_CAS_P_REV_ENUM
{
    CNAS_CCB_1X_CAS_P_REV_JST_008               = 0x01,
    CNAS_CCB_1X_CAS_P_REV_IS_95                 = 0x02,
    CNAS_CCB_1X_CAS_P_REV_IS_95A                = 0x03,
    CNAS_CCB_1X_CAS_P_REV_IS_95B_CUSTOM         = 0x04,
    CNAS_CCB_1X_CAS_P_REV_IS_95B                = 0x05,
    CNAS_CCB_1X_CAS_P_REV_IS_2000               = 0x06,
    CNAS_CCB_1X_CAS_P_REV_IS_2000A              = 0x07,

    CNAS_CCB_1X_CAS_P_REV_ENUM_BUTT
};
typedef VOS_UINT8 CNAS_CCB_1X_CAS_P_REV_ENUM_UINT8;



enum CNAS_CCB_1X_CALL_STATE_ENUM
{
    CNAS_CCB_1X_CALL_STATE_WAIT_FOR_ORDER,
    CNAS_CCB_1X_CALL_STATE_WAIT_FOR_ANSWER,
    CNAS_CCB_1X_CALL_STATE_CONVERSATION,
    CNAS_CCB_1X_CALL_STATE_IDLE,
    CNAS_CCB_1X_CALL_STATE_INCOMMING_CALL,
    CNAS_CCB_1X_CALL_STATE_ORIG_CALL,

    CNAS_CCB_1X_CALL_STATE_BUTT
};
typedef VOS_UINT8 CNAS_CCB_1X_CALL_STATE_ENUM_UINT8;



enum CNAS_CCB_1X_SO_TYPE_ENUM
{
    CNAS_CCB_1X_SO_TYPE_VOICE,
    CNAS_CCB_1X_SO_TYPE_SMS,
    CNAS_CCB_1X_SO_TYPE_PS,
    CNAS_CCB_1X_SO_TYPE_LOOPBACK,
    CNAS_CCB_1X_SO_TYPE_AGPS,
    CNAS_CCB_1X_SO_TYPE_BUTT
};
typedef VOS_UINT8 CNAS_CCB_1X_SO_TYPE_ENUM_UINT8;


enum CNAS_CCB_1X_RETURN_CAUSE_ENUM
{
    CNAS_CCB_1X_RETURN_CAUSE_NORMAL_ACCESS             = 0x00,
    CNAS_CCB_1X_RETURN_CAUSE_SYSTEM_NOT_ACQ            = 0x01,
    CNAS_CCB_1X_RETURN_CAUSE_PROTOCOL_MISMATCH         = 0x02,
    CNAS_CCB_1X_RETURN_CAUSE_REG_REJ                   = 0x03,
    CNAS_CCB_1X_RETURN_CAUSE_WRONG_SID                 = 0x04,
    CNAS_CCB_1X_RETURN_CAUSE_WRONG_NID                 = 0x05,

    CNAS_CCB_1X_RETURN_CAUSE_BUTT
};
typedef VOS_UINT8 CNAS_CCB_1X_RETURN_CAUSE_ENUM_UINT8;


enum CNAS_CCB_ESN_MEID_TYPE_ENUM
{
    CNAS_CCB_ESN_MEID_TYPE_ESN      = 0,
    CNAS_CCB_ESN_MEID_TYPE_MEID        ,

    CNAS_CCB_ESN_MEID_TYPE_BUTT
};
typedef VOS_UINT8 CNAS_CCB_ESN_MEID_TYPE_ENUM_UINT8;


enum CNAS_CCB_SERVICE_STATUS_ENUM
{
    CNAS_CCB_SERVICE_STATUS_NO_SERVICE                      = 0,        /* �޷���   */
    CNAS_CCB_SERVICE_STATUS_LIMITED_SERVICE                 = 1,        /* ���Ʒ��� */
    CNAS_CCB_SERVICE_STATUS_NORMAL_SERVICE                  = 2,        /* �������� */

    CNAS_CCB_SERVICE_STATUS_BUTT
};
typedef VOS_UINT32 CNAS_CCB_SERVICE_STATUS_ENUM_UINT32;

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  7 STRUCT����
*****************************************************************************/


typedef struct
{
    CNAS_CCB_1X_CALL_STATE_ENUM_UINT8                       en1xCallState;
    CNAS_CCB_1X_SO_TYPE_ENUM_UINT8                          en1xSoType;
    VOS_UINT8                                               aucRslv[2];
}CNAS_CCB_1X_CALL_STATE_STRU;



typedef struct
{
    VOS_UINT8                           aucEFRUIMID[CNAS_CCB_EFRUIMID_OCTET_LEN_EIGHT]; /* all the bytes in the EFRUIMID  */
}CNAS_CCB_UIMID_STRU;


typedef struct
{
    CNAS_CCB_ESN_MEID_TYPE_ENUM_UINT8   enEsnMeidType;

    union
    {
        VOS_UINT8                       aucEsn[CNAS_CCB_ESN_OCTET_LEN];
        VOS_UINT8                       aucMeid[CNAS_CCB_MEID_OCTET_LEN];
    }u;
}CNAS_CCB_ESNMEIDME_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucAccessAuthAvailFlag;     /* VOS_TRUE/VOS_FALSE */
    VOS_UINT8                           ucAccessAuthUserNameLen;
    VOS_UINT8                           aucReserved[1];
    VOS_UINT8                           aucAccessAuthUserName[CNAS_CCB_MAX_AUTHDATA_USERNAME_LEN];
}CNAS_CCB_HRPD_ACCESS_AUTH_INFO_STRU;


typedef struct
{
    CNAS_CCB_CARD_STATUS_ENUM_UINT8                         enCsimCardStatus;  /* CSIM��״̬ */
    CNAS_CCB_CARD_STATUS_ENUM_UINT8                         enUsimCardStatus;  /* USIM��״̬ */
    VOS_UINT8                                               aucCardIccId[CNAS_CCB_ICCID_OCTET_LEN];     /* ���ļ�2fe2�����б����ICCID*/
    CNAS_CCB_UIMID_STRU                                     stUIMID;                  /* ���ļ�6f31�����б����UIMID */

    CNAS_CCB_ESNMEIDME_INFO_STRU                            stEsnMeidMe;              /* ���ļ�6f38��esn meid me */
    CNAS_CCB_HRPD_ACCESS_AUTH_INFO_STRU                     stHrpdAccessAuthInfo;     /* ���ļ�6f57����ǰ���ļ��е�hrpd ppp an ��Ȩ�û��� */
}CNAS_CCB_CARD_INFO_STRU;


typedef struct
{
    VOS_UINT16                          usSid;
    VOS_UINT16                          usNid;
    VOS_UINT16                          usBandClass;
    VOS_UINT16                          usChannel;
}CNAS_CCB_1X_SYS_INFO_STRU;


typedef struct
{
    VOS_UINT8                               ucConcurrentSupported;                  /* ����ҵ���Ƿ�֧�� */
    VOS_UINT8                               ucPRevInUse;                            /* Э��ʹ�ð汾 */
    CNAS_CCB_1X_CAS_STATE_ENUM_UINT8        enCasSta;                               /* CAS��״̬ */

    CNAS_CCB_1X_CAS_SUB_STATE_ENUM_UINT8    enCasSubSta;                            /* CAS����״̬ */
    CNAS_CCB_1X_CAS_P_REV_ENUM_UINT8        enProtocolRev;                          /* CAS�ϱ����ֻ�֧�ֵ����Э��汾 */
    VOS_UINT8                               uc1XRfAvailFlg;                           /* RF��Դ���ñ��:VOS_TRUE :Ĭ��RF��Դ����
                                                                                                    VOS_FALSE:RF��Դ������ */

    VOS_UINT8                               aucReserved[2];

    CNAS_CCB_1X_SYS_INFO_STRU               st1xSysInfo;
}CNAS_CCB_NW_INFO_STRU;


typedef struct
{
    /* ע��ģ�鸳ֵ��XCC��ʹ�� */
    VOS_UINT8                           ucIsMtCallInRoamingAcc;     /*Mobile terminated calls*/
    VOS_UINT8                           aucReserved[3];
}CNAS_CALL_CONFIG_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucPowerOffCampOnCtrlFlg;                /* �ػ�פ�����Ʊ�� */
    VOS_UINT8                           aucReserved[3];
}CNAS_CCB_CUSTOM_CFG_INFO_STRU;



typedef struct
{
    VOS_UINT16                          usSid;
    VOS_UINT16                          usNid;
    VOS_UINT16                          usBand;
    VOS_UINT16                          usReserved;
}CNAS_CCB_1X_HOME_SID_NID_STRU;


typedef struct
{
    VOS_UINT8                           ucSysNum;
    VOS_UINT8                           aucReserved[3];
    CNAS_CCB_1X_HOME_SID_NID_STRU       astHomeSidNid[CNAS_CCB_MAX_HOME_SID_NID_LIST];
}CNAS_CCB_1X_HOME_SID_NID_LIST_STRU;


typedef struct
{
    MODEM_ID_ENUM_UINT16                enCdmaModeModemId;
    VOS_UINT8                           aucReserve[2];
}CNAS_CCB_MODEM_INFO_STRU;


typedef struct
{
    CNAS_CCB_SERVICE_STATUS_ENUM_UINT32 enCurSrvStatus; /* �洢1X��ǰ����״̬ */
    VOS_UINT8                           aucReserved[4];
}CNAS_CCB_1X_SRV_INFO_STRU;


typedef struct
{
    VOS_UINT32                          ulReceiveTime;  /* ����ʱ�� */
    VOS_UINT32                          ulExitTime;     /* ����ʱ�� */
    VOS_UINT16                          usSendPid;      /* ����PID */
    VOS_UINT16                          usReceivePid;   /* ����PID */
    VOS_UINT16                          usMsgName;      /* ��Ϣ�� */
    VOS_UINT8                           ucCnasFsmId;    /* ��Ӧģ��״̬��ID  */
    VOS_UINT8                           ucCnasState;    /* ��Ӧģ���s��ǰ״̬  */
}CNAS_CCB_MSG_STATE_STRU;

typedef struct
{
    CNAS_CCB_MSG_STATE_STRU             stMsgState[CNAS_CCB_MAX_LOG_MSG_STATE_NUM];
    VOS_UINT32                          ucLatestIndex;
}CNAS_CCB_LOG_MSG_STATUS_STRU;

typedef struct
{
    CNAS_CCB_LOG_MSG_STATUS_STRU            stLogMsgState;
}CNAS_CCB_MNTN_CTX_STRU;


typedef struct
{
    VOS_UINT16                          usStartSid;
    VOS_UINT16                          usEndSid;
    VOS_UINT32                          ulMcc;
}CNAS_CCB_SYS_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucEnable;                          /* �������Ƿ�ʹ�� */
    VOS_UINT8                           ucReserved;
    VOS_UINT16                          usWhiteSysNum;                     /* ֧�ְ������ĸ���,����Ϊ0ʱ��ʾ��֧�ְ����� */
    CNAS_CCB_SYS_INFO_STRU              astSysInfo[CNAS_CCB_MAX_WHITE_LOCK_SYS_NUM];
}CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU;


typedef struct
{
    VOS_UINT16                          usChannel;
    VOS_UINT8                           aucReserved[2];
}CNAS_CCB_FREQENCY_CHANNEL_STRU;


typedef struct
{
    VOS_UINT8                           ucEnableFlg;
    VOS_UINT8                           ucReserved;
    VOS_UINT16                          usFreqNum;
    CNAS_CCB_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CCB_MAX_FREQ_NUM];
}CNAS_CCB_CTCC_CUSTOMIZE_FREQ_LIST_STRU;


typedef struct
{
    CNAS_CCB_CTCC_CUSTOMIZE_FREQ_LIST_STRU                  stCustFreqList;
}CNAS_CCB_CTCC_CUSTOMIZE_INFO_STRU;


typedef struct
{
    VOS_UINT16                          usPrimaryA;
    VOS_UINT16                          usPrimaryB;
    VOS_UINT16                          usSecondaryA;
    VOS_UINT16                          usSecondaryB;
}CNAS_CCB_CDMA_STANDARD_CHANNELS_STRU;


typedef struct
{
    CNAS_CCB_CARD_INFO_STRU                                 stCardInfo;
    CNAS_CALL_CONFIG_INFO_STRU                              stCallInfoCfg;
    CNAS_CCB_NW_INFO_STRU                                   stNwInfo;
    CNAS_CCB_CUSTOM_CFG_INFO_STRU                           stCustomCfg;                    /* NVIM�еĶ�����Ϣ */

    CNAS_CCB_1X_HOME_SID_NID_LIST_STRU                      stHomeSidNidList;
    CNAS_CCB_MODEM_INFO_STRU                                stModemInfo;

    CNAS_CCB_PS_RATTYPE_ENUM_UINT32                         enCurrPsRatType;
    CNAS_CCB_PS_RATTYPE_ENUM_UINT32                         enPrevPsRatType;

    CNAS_CCB_1X_CALL_STATE_STRU                             astMntn1xCallState[CNAS_CCB_MNTN_1X_CALL_STATE_MAX_NUM];

    CNAS_CCB_1X_RETURN_CAUSE_ENUM_UINT8                     en1xReturnCause;

    VOS_UINT8                                               uc1XCallExistCount;             /* ���м����� */
    VOS_UINT8                                               ucHomeSidNidDependOnPrlFlg;
    VOS_UINT8                                               aucRsved[1];

    CNAS_CCB_MNTN_CTX_STRU                                  stMntnInfo;
    CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU                  stOperLockSysWhiteList;

    CNAS_CCB_CTCC_CUSTOMIZE_INFO_STRU                       stCTCCCustInfo;

    CNAS_CCB_CDMA_STANDARD_CHANNELS_STRU                    stCdmaStandardChan;

    CNAS_PRL_FREQENCY_CHANNEL_STRU                          stOhmFreq;

    CNAS_CCB_1X_SRV_INFO_STRU                               st1XSrvInfo;
}CNAS_CCB_CTX_STRU;


typedef struct
{
    VOS_UINT32                           ulModem0Pid;
    VOS_UINT32                           ulModem1Pid;
}CNAS_CCB_MODEM_PID_TAB_STRU;


/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

CNAS_CCB_CARD_STATUS_ENUM_UINT8 CNAS_CCB_GetCsimCardStatus( VOS_VOID );

VOS_VOID CNAS_CCB_InitCcbCtx(VOS_VOID);

CNAS_CCB_CTX_STRU* CNAS_CCB_GetCcbCtxAddr( VOS_VOID );

VOS_VOID CNAS_CCB_SetCsimCardStatus(
    CNAS_CCB_CARD_STATUS_ENUM_UINT8     enCsimCardStatus
);
VOS_UINT16 CNAS_CCB_GetCasCardStatus(VOS_VOID);

VOS_UINT8 CNAS_CCB_GetConcurrentSupportedFlag(VOS_VOID);

VOS_VOID CNAS_CCB_SetConcurrentSupportedFlag(
    VOS_UINT8                           ucConcurrentSupported
);

VOS_UINT8 CNAS_CCB_GetPRevInUse(VOS_VOID);

VOS_VOID CNAS_CCB_SetPRevInUse(
    VOS_UINT8                           ucPRevInUse
);
VOS_VOID CNAS_CCB_SetMtCallInRoamingAccFlg(
    VOS_UINT8                           ucIsMtCallInRoamingAcc
);

VOS_UINT8 CNAS_CCB_GetMtCallInRoamingAccFlg( VOS_VOID );

VOS_VOID CNAS_CCB_SetCasState(
    CNAS_CCB_1X_CAS_STATE_ENUM_UINT8    enSta
);

CNAS_CCB_1X_CAS_STATE_ENUM_UINT8 CNAS_CCB_GetCasState(VOS_VOID);

VOS_VOID CNAS_CCB_SetPowerOffCampOnCtrlFlg(
    VOS_UINT8                           ucPowerOffCampOnFlg
);

VOS_UINT8 CNAS_CCB_GetPowerOffCampOnCtrlFlg( VOS_VOID );

VOS_VOID CNAS_CCB_InitHomeSidNidList(
    CNAS_CCB_1X_HOME_SID_NID_LIST_STRU *pstHomeSidNidList
);

CNAS_CCB_1X_HOME_SID_NID_LIST_STRU* CNAS_CCB_GetHomeSidNidList(VOS_VOID);

CNAS_CCB_1X_CAS_SUB_STATE_ENUM_UINT8 CNAS_CCB_GetCasSubState(VOS_VOID);

VOS_VOID CNAS_CCB_SetCasSubState(
    CNAS_CCB_1X_CAS_SUB_STATE_ENUM_UINT8    enSubSta
);

CNAS_CCB_1X_CAS_P_REV_ENUM_UINT8 CNAS_CCB_GetHighProRev(VOS_VOID);

VOS_VOID CNAS_CCB_SetHighProRev(
    CNAS_CCB_1X_CAS_P_REV_ENUM_UINT8    enProtocolRev
);


CNAS_CCB_1X_CALL_STATE_ENUM_UINT8 CNAS_CCB_Get1xVoiceCallState(VOS_VOID);

CNAS_CCB_1X_CALL_STATE_ENUM_UINT8 CNAS_CCB_Get1xSmsCallState(
    VOS_VOID
);
CNAS_CCB_1X_CALL_STATE_ENUM_UINT8 CNAS_CCB_Get1xPsCallState(
    VOS_VOID
);
CNAS_CCB_1X_SO_TYPE_ENUM_UINT8 CNAS_CCB_ConvertSoToServiceType(
    CAS_CNAS_1X_SERVICE_OPTION_ENUM_UINT16                  en1xSo
);

VOS_VOID CNAS_CCB_Set1xCallState(
    CNAS_CCB_1X_CALL_STATE_ENUM_UINT8                       en1xCallState,
    CNAS_CCB_1X_SO_TYPE_ENUM_UINT8                          en1xSoType
);


CNAS_CCB_1X_RETURN_CAUSE_ENUM_UINT8 CNAS_CCB_Get1xReturnCause(VOS_VOID);

VOS_VOID CNAS_CCB_Set1xReturnCause(
    CNAS_CCB_1X_RETURN_CAUSE_ENUM_UINT8                     en1xReturnCause
);


VOS_VOID CNAS_CCB_Set1XRfAvailFlg(
    VOS_UINT8                           uc1XRfAvailFlg
);

VOS_UINT8 CNAS_CCB_Get1XRfAvailFlg( VOS_VOID );


CNAS_CCB_UIMID_STRU* CNAS_CCB_GetUIMID(VOS_VOID);


VOS_VOID CNAS_CCB_SetCdmaModeModemId(
    MODEM_ID_ENUM_UINT16                enModemId
);

MODEM_ID_ENUM_UINT16 CNAS_CCB_GetCdmaModeModemId( VOS_VOID );

VOS_UINT32 CNAS_CCB_ConvertPidBasedOnModemId(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT32                          ulRcvPid
);

VOS_UINT32 CNAS_CCB_GetModem0Pid(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT32                          ulSenderPid
);

VOS_UINT8* CNAS_CCB_GetCardIccIdAddr(VOS_VOID);

VOS_VOID CNAS_CCB_InitIccId(VOS_VOID);

VOS_VOID CNAS_CCB_InitUIMID(VOS_VOID);

VOS_VOID CNAS_CCB_InitEsnMeidMeInfo(VOS_VOID);

VOS_VOID CNAS_CCB_InitHrpdAccessAuthInfo(VOS_VOID);

extern VOS_VOID CNAS_CCB_SetUsimCardStatus(
    CNAS_CCB_CARD_STATUS_ENUM_UINT8     enUsimCardStatus
);

extern CNAS_CCB_CARD_STATUS_ENUM_UINT8 CNAS_CCB_GetUsimCardStatus( VOS_VOID );

VOS_VOID CNAS_CCB_SetPrevRatType(
    CNAS_CCB_PS_RATTYPE_ENUM_UINT32     enPsRatType
);
CNAS_CCB_PS_RATTYPE_ENUM_UINT32 CNAS_CCB_GetPrevPsRatType(VOS_VOID);
VOS_VOID CNAS_CCB_SetCurrPsRatType(
    CNAS_CCB_PS_RATTYPE_ENUM_UINT32    enPsRatType
);
CNAS_CCB_PS_RATTYPE_ENUM_UINT32 CNAS_CCB_GetCurrPsRatType(VOS_VOID);


CNAS_CAS_1X_RETURN_CAUSE_ENUM_UINT8 CNAS_CCB_CovertReturnCause(
    CNAS_CCB_1X_RETURN_CAUSE_ENUM_UINT8   enCnas1xReturnCause
);

VOS_VOID CNAS_CCB_InitMntnInfo(
    CNAS_CCB_MNTN_CTX_STRU             *pstMntnInfo
);

VOS_VOID CNAS_CCB_Set1XCallExistCount(
    VOS_UINT8                           uc1XCallExistCount
);

VOS_UINT8 CNAS_CCB_Get1XCallExistCount( VOS_VOID );

VOS_UINT8 CNAS_CCB_GetHomeSidNidDependOnPrlFlg(VOS_VOID);

VOS_VOID CNAS_CCB_SetHomeSidNidDependOnPrlFlg(
    VOS_UINT8                           ucFlg
);

CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU* CNAS_CCB_GetOperLockSysWhiteList(VOS_VOID);

CNAS_CCB_CTCC_CUSTOMIZE_FREQ_LIST_STRU* CNAS_CCB_GetCTCCCustomizeFreqList(VOS_VOID);

CNAS_CCB_CDMA_STANDARD_CHANNELS_STRU* CNAS_CCB_GetCdmaStandardChannels(VOS_VOID);

VOS_VOID CNAS_CCB_InitCdmaStandardChannels(
    CNAS_CCB_CDMA_STANDARD_CHANNELS_STRU                   *pstCdmaStandardChan
);

CNAS_PRL_FREQENCY_CHANNEL_STRU* CNAS_CCB_GetCdmaOhmFreq(VOS_VOID);

VOS_VOID CNAS_CCB_SetCdmaOhmFreq(
    CNAS_PRL_FREQENCY_CHANNEL_STRU     *pstOhmFreq
);

VOS_VOID CNAS_CCB_ClearCdmaOhmFreq(VOS_VOID);

VOS_VOID CNAS_CCB_Set1xSysInfo(
    VOS_UINT16                          usSid,
    VOS_UINT16                          usNid,
    VOS_UINT16                          usBandClass,
    VOS_UINT16                          usChannel
);

CNAS_CCB_1X_SYS_INFO_STRU* CNAS_CCB_Get1xSysInfo(VOS_VOID);

CNAS_CCB_ESNMEIDME_INFO_STRU* CNAS_CCB_GetEsnMeidMeInfoAddr(VOS_VOID);

CNAS_CCB_HRPD_ACCESS_AUTH_INFO_STRU* CNAS_CCB_GetHrpdAccesAuthInfoAddr(VOS_VOID);
VOS_UINT32 CNAS_CCB_IsCurCallStateIdle( VOS_VOID );
CNAS_CCB_1X_SRV_INFO_STRU* CNAS_CCB_GetCur1XSrvInfo(VOS_VOID);

CNAS_CCB_SERVICE_STATUS_ENUM_UINT32 CNAS_CCB_GetCur1XSrvStatus(VOS_VOID);

VOS_VOID CNAS_CCB_SetCur1XSrvStatus(
    CNAS_CCB_SERVICE_STATUS_ENUM_UINT32 enSrvStatus
);
#endif

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
