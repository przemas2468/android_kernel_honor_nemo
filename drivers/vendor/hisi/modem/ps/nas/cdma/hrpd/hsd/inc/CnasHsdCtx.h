

#ifndef __CNAS_HSD_CTX_H__
#define __CNAS_HSD_CTX_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
#include "NasFsm.h"
#include "CnasPrlApi.h"
#include "CnasTimerMgmt.h"
#include "cas_hrpd_airlinkmgmt_nas_pif.h"
#include "CnasCcb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define CNAS_HSD_MAX_FSM_STACK_DEPTH                        (4)                 /* ״̬��ջ����� */

#define CNAS_HSD_MAX_CACHE_MSG_QUEUE_NUM                    (8)                 /* ��󻺴���Ϣ�ĸ��� */

#define CNAS_HSD_MAX_MSG_BUFFER_LEN                         (1600)              /* ��Ϣ����󳤶ȣ�����״̬������������Ϣ��CACHE��Ϣ */

/* �洢������MRU������CGD 130 5.2.2 The number of entries in an MRU list is implementation dependent but a typical number may be 12 */
#define CNAS_HSD_MAX_MRU_SYS_NUM                            (12)

#define CNAS_HSD_CHECK_BYTES_SUBNET_ID                      (6)                 /* ��MRU��Ϣʱ��Ҫ�Ƚϵ�Subnet�ֽ��� */

#define CNAS_HSD_MAX_OOC_SCHEDULE_PHASE_NUM                 (8)                 /* NV�д洢����ʱ�����������Ŀ���ݶ�8����ȷ�� */
#define CNAS_HSD_TIMER_LEN_10_SEC                           (10)
#define CNAS_HSD_TIMER_LEN_5_SEC                            (5)
#define CNAS_HSD_ATTEMP_IN_DO_TRAFFIC_4_TIMES               (4)
#define CNAS_HSD_ATTEMP_UNLIMITED_TIMES                     (65535)

#define CNAS_HSD_MAX_INT_MSG_QUEUE_NUM                      (8)

/* �洢������history system�ĸ��� */
#define CNAS_HSD_HRPD_ACQED_SUCC_SYS_MAX_NUM                (20)

/* �洢������avoid freq�ĸ��� */
#define CNAS_HSD_MAX_AVOID_FREQ_NUM                         (100)

#define CNAS_HSD_MAX_SLICE_VALUE                            (0xFFFFFFFF)

#define CNAS_HSD_MAX_PREF_SUBNET_NUM                        ( 16 )          /*�����ȼ�����������*/

#define CNAS_HSD_TIMER_LEN_20_SEC                           (20)

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

enum CNAS_HSD_FSM_ID_ENUM
{
    /***********************************************************************
     * HSD L1״̬����ʶ
     ***********************************************************************/
    /* CNAS HSD L1 main״̬���ı�ʶ */
    CNAS_HSD_FSM_L1_MAIN                                    =0x00,

    /***********************************************************************
     * HSD L2״̬���ı�ʶ
     ***********************************************************************/
    /* Э��ջ��ʼ��L2״̬�� */
    CNAS_HSD_FSM_SWITCH_ON                                  =0x01,

    /* Э��ջ�ػ�L2״̬�� */
    CNAS_HSD_FSM_POWER_OFF                                  =0x02,

    /* Э��ջSYSTEM ACQUIRE L2״̬�� */
    CNAS_HSD_FSM_SYS_ACQ                                    =0x03,

    /* Э��ջPOWER SAVE L2״̬�� */
    CNAS_HSD_FSM_POWER_SAVE                                 =0x04,

    /* Э��ջ��ϵͳ L2״̬�� */
    CNAS_HSD_FSM_INTER_SYS                                  =0x05,



    CNAS_HSD_BUTT

};
typedef VOS_UINT32 CNAS_HSD_FSM_ID_ENUM_UINT32;



enum CNAS_HSD_INIT_CTX_TYPE_ENUM
{
    CNAS_HSD_INIT_CTX_STARTUP                    = 0,
    CNAS_HSD_INIT_CTX_POWEROFF                   = 1,
    CNAS_HSD_INIT_CTX_BUILD_FREQ                 = 2,
    CNAS_HSD_INIT_CTX_BUTT
};
typedef VOS_UINT8 CNAS_HSD_INIT_CTX_TYPE_ENUM_UINT8;



enum   CNAS_HSD_READ_CARD_FILES_CNF_ENUM
{
    CNAS_HSD_READ_CARD_FILE_FLG_NULL                        = 0x00,
    CNAS_HSD_READ_CARD_FILE_PRL_FLG                         = 0x01,
    CNAS_HSD_READ_CARD_FILE_EPRL_FLG                        = 0x02,

    CNAS_HSD_READ_CARD_FILE_CNF_BUTT
};

typedef VOS_UINT32 CNAS_HSD_READ_CARD_FILES_CNF_ENUM_UINT32;



enum CNAS_HSD_SYS_ACQ_SCENE_ENUM
{
    CNAS_HSD_SYS_ACQ_SCENE_SWITCH_ON  = 0,                  /* ���� */
    CNAS_HSD_SYS_ACQ_SCENE_SYSTEM_LOST,                     /* �������� */
    CNAS_HSD_SYS_ACQ_SCENE_REDIRECTION,                     /* �ض��� */
    CNAS_HSD_SYS_ACQ_SCENE_OOC_MRU0,                        /* OOC���� */

    CNAS_HSD_SYS_ACQ_SCENE_DATA_CALL_QUICK,                 /* ���ݺ��п����ѳ��� */

    CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH,                    /*  */

    CNAS_HSD_SYS_ACQ_SCENE_HIGH_PRIORITY,                   /* �����ȼ����� */

    CNAS_HSD_SYS_ACQ_SCENE_MSCC_OTHER,                      /* MSCC���������������ΪOTHER��HSDͳһ����SYS LOST�������� */

    CNAS_HSD_SYS_ACQ_SCENE_CFREQ_LOCK,                      /* ��Ƶ���� */

    CNAS_HSD_SYS_ACQ_SCENE_OOC_SWITCH_ON,                   /* �������ooc���� */

    CNAS_HSD_SYS_ACQ_SCENE_NO_RF_RESUME,                   /* NO_RF��RF AVAIL������ */

    CNAS_HSD_SYS_ACQ_SCENE_BUTT
};
typedef VOS_UINT32 CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32;


enum CNAS_HSD_SYS_MODE_TYPD_ENUM
{
    CNAS_HSD_SYS_MODE_HYBRID = 0,
    CNAS_HSD_SYS_MODE_NONHYBRID,
    CNAS_HSD_SYS_MODE_BUTT,
};

typedef VOS_UINT32 CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32;


enum CNAS_HSD_1X_SYSTEM_STATUS_ENUM
{
    CNAS_HSD_1X_SYSTEM_UNAVAILABLE = 0,
    CNAS_HSD_1X_SYSTEM_AVAILABLE,
    CNAS_HSD_1X_SYSTEM_STATUS_BUTT,
};

typedef VOS_UINT32 CNAS_HSD_1X_SYSTEM_STATUS_ENUM_UINT32;


enum CNAS_HSD_CAS_SESSION_NEG_STATUS_ENUM
{
    CNAS_HSD_NOT_IN_SESSION_NEG = 0,
    CNAS_HSD_IN_SESSION_NEG,
    CNAS_HSD_SESSION_NEG_STATUS_BUTT
};
typedef VOS_UINT32 CNAS_HSD_SESSION_NEG_STATUS_ENUM_UINT32;


enum CNAS_HSD_CAS_STATUS_ENUM
{
    CNAS_HSD_AT_STATUS_ENUM_NONE       = 0x0000,
    CNAS_HSD_AT_STATUS_ENUM_INIT       = 0x0001,
    CNAS_HSD_AT_STATUS_ENUM_IDLE       = 0x0002,
    CNAS_HSD_AT_STATUS_ENUM_CONN       = 0x0003,

    CNAS_HSD_AT_STATUS_ENUM_BUTT       = 0x0004
};
typedef VOS_UINT32 CNAS_HSD_AT_STATUS_ENUM_UINT32;


enum CNAS_HSD_AVOID_ITEM_USED_FALG_ENUM
{
    CNAS_HSD_AVOID_ITEM_UNUSED = 0,
    CNAS_HSD_AVOID_ITEM_USED,

    CNAS_HSD_AVOID_ITEM_USED_FALG_BUTT
};
typedef VOS_UINT32 CNAS_HSD_AVOID_ITEM_USED_FALG_ENUM_UINT32;


enum CNAS_HSD_REACQ_FALG_ENUM
{
    CNAS_HSD_NOT_NEED_REACQ = 0,
    CNAS_HSD_NEED_REACQ,

    CNAS_HSD_REACQ_FALG_BUTT
};
typedef VOS_UINT32 CNAS_HSD_REACQ_FALG_ENUM_UINT32;


enum CNAS_HSD_AVOID_REASON_ENUM
{
    CNAS_HSD_AVOID_PROTOCAL_MISMATCH = 0,               /* protocal mismatch */
    CNAS_HSD_AVOID_REDIRECTION,                         /* redirection */
    CNAS_HSD_AVOID_SESSION_NEGOTIATION_TIMEOUT,         /* session negotiation timeout */
    CNAS_HSD_AVOID_SESSION_NEGOTIATION_FAILURE,         /* session negotiation failure */
    CNAS_HSD_AVOID_PERSISTENCE_TEST_FAILURE,            /* persistence test failure */
    CNAS_HSD_AVOID_MAX_ACCESS_PROBES_FAILURE,           /* max access probes failure */
    CNAS_HSD_AVOID_AUTHEN_OR_BILLING_FAILURE,           /* authen or billing failure */
    CNAS_HSD_AVOID_BUSY_OR_GENERAL_FAILURE,             /* busy or general failure */
    CNAS_HSD_AVOID_TCA_TIMEOUT,                         /* TCA timeout */

    CNAS_HSD_AVOID_FAIL_CONTINUE_THREE_TIMES,

    CNAS_HSD_AVOID_PREFER_CH_NOT_VALID,
    CNAS_HSD_AVOID_REASON_MAX = 12,

    CNAS_HSD_AVOID_REASON_BUTT
};
typedef VOS_UINT8 CNAS_HSD_AVOID_REASON_ENUM_UINT8;


enum CNAS_HSD_ABORT_REASON_ENUM
{
    CNAS_HSD_ABORT_REASON_POWER_OFF     = 0,                /* POWER_OFF*/
    CNAS_HSD_ABORT_REASON_POWER_SAVE,                        /* POWER_SAVE */
    CNAS_HSD_ABORT_REASON_DATA_CALL,                         /* DATA_CALL */

    CNAS_HSD_ABORT_REASON_BUTT
};
typedef VOS_UINT8 CNAS_HSD_ABORT_REASON_ENUM_UINT8;

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
    VOS_UINT32                          ulEventType;
    VOS_UINT8                           aucMsgBuffer[CNAS_HSD_MAX_MSG_BUFFER_LEN];
}CNAS_HSD_MSG_STRU;


typedef struct
{
    VOS_UINT8                           ucCacheMsgNum;                          /* ������Ϣ�ĸ��� */
    VOS_UINT8                           aucReserve[3];
    VOS_UINT8                          *pastCacheMsg[CNAS_HSD_MAX_CACHE_MSG_QUEUE_NUM]; /* XSD����Ϣ�������飬�洢���Ǵ�ͨ����Ϣͷ����Ϣ */
}CNAS_HSD_CACHE_MSG_QUEUE_STRU;


typedef struct
{
    VOS_UINT8                           ucIntMsgNum;                                /* ������Ϣ�ĸ��� */
    VOS_UINT8                           aucReserve[3];
    VOS_UINT8                          *pastIntMsg[CNAS_HSD_MAX_INT_MSG_QUEUE_NUM]; /* HSD����Ϣ�������飬�����Ϣָ�� */
}CNAS_HSD_INT_MSG_QUEUE_STRU;


typedef struct
{
    VOS_UINT32                          ulCurAvailableTimerCount;               /* ��ǰAvailable Timer�������� */
}CNAS_HSD_FSM_L1_MAIN_CTX_STRU;


typedef struct
{
    VOS_UINT32                          ulHsdReadCardFileFlg;                   /* ��ǿ��ļ��Ƿ��ȡ��� */
    VOS_UINT8                           aucReserved[4];
}CNAS_HSD_FSM_SWITCH_ON_CTX_STRU;


typedef struct
{
    VOS_UINT8                                               ucAbortFlg;         /* ��ǰ״̬����־�Ƿ��յ���ֹҪ��,VOS_TRUE:�յ�, VOS_FALSE:δ�յ� */
    CNAS_HSD_ABORT_REASON_ENUM_UINT8                        enAbortReason;      /* ��ǰabort���� */
    VOS_UINT8                                               aucRsved[2];
}CNAS_HSD_FSM_SYS_ACQ_CTX_STRU;


typedef struct
{
    VOS_UINT8                                               ucAbortFlg;         /* ��ǰ״̬����־�Ƿ��յ���ֹҪ��,VOS_TRUE:�յ�, VOS_FALSE:δ�յ� */
    CNAS_HSD_ABORT_REASON_ENUM_UINT8                        enAbortReason;      /* ��ǰabort���� */
    VOS_UINT8                                               aucRsved[2];
}CNAS_HSD_FSM_INTER_SYS_CTX_STRU;


typedef union
{
    CNAS_HSD_FSM_L1_MAIN_CTX_STRU       stL1MainCtx;
    CNAS_HSD_FSM_SWITCH_ON_CTX_STRU     stSwitchOnCtx;
    CNAS_HSD_FSM_SYS_ACQ_CTX_STRU       stSysAcqCtx;
    CNAS_HSD_FSM_INTER_SYS_CTX_STRU     stInterSysCtx;
}CNAS_HSD_FSM_EXTRA_CTX_UNION;


typedef struct
{
    /* ״̬������ָ��,ָ��ΪNULL��ʾ��ǰ״̬����Ч  */
    NAS_FSM_DESC_STRU                  *pstFsmDesc;

    /* ��ǰ״̬����ʶ */
    CNAS_HSD_FSM_ID_ENUM_UINT32         enFsmId;

    /* �ϲ�״̬����ʶ */
    CNAS_HSD_FSM_ID_ENUM_UINT32         enParentFsmId;

    /* �ϲ�״̬�������Ϣ���� */
    VOS_UINT32                          ulParentEventType;

    /* ��ǰ״̬ */
    VOS_UINT32                          ulState;

    /* �����ϢMSGID������  */
    CNAS_HSD_MSG_STRU                   stEntryMsg;

    /* ״̬������ */
    CNAS_HSD_FSM_EXTRA_CTX_UNION        unFsmCtx;
}CNAS_HSD_FSM_CTX_STRU;


typedef struct
{
    VOS_UINT16                          usStackDepth;                           /* ��ǰѹջ��״̬���� */
    VOS_UINT16                          usStackPopFlg;                          /* ��ǰջpop��־ */
    CNAS_HSD_FSM_CTX_STRU               astFsmStack[CNAS_HSD_MAX_FSM_STACK_DEPTH];  /* ״̬��ջ����� */
}CNAS_HSD_FSM_STACK_STRU;


typedef struct
{
    CNAS_HSD_1X_SYSTEM_STATUS_ENUM_UINT32       enStatus;                       /* ָʾ��ǰ1Xϵͳ�Ƿ���Ч */
    CNAS_PRL_1X_SYSTEM_STRU                     st1xSys;                        /* ��ŵ�ǰ1Xϵͳ��Ϣ */
}CNAS_HSD_1X_SYSTEM_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucCurrMruNum;                           /* ָʾ��ǰMRU���� */
    VOS_UINT8                           aucRcv[3];
    CNAS_PRL_HRPD_SYSTEM_STRU           astHrpdSys[CNAS_HSD_MAX_MRU_SYS_NUM];   /* ���MRU��Ϣ */
}CNAS_HSD_HRPD_MRU_LIST_STRU;


typedef struct
{
    CNAS_HSD_AVOID_ITEM_USED_FALG_ENUM_UINT32               enUsedFlag;                 /* ʹ�ñ�־ */
    VOS_UINT32                                              ulStartSlice;               /* ��ʼslice */
    VOS_UINT32                                              ulExpiredSliceNum;          /* ���ڵ�slice���� */
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          stAvoidFreq;
}CNAS_HSD_AVOID_FREQ_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucAvoidItemUsedNum;
    VOS_UINT8                           aucReserve[3];
    VOS_UINT8                           aucCounter[CNAS_HSD_AVOID_REASON_MAX];
    CNAS_HSD_AVOID_FREQ_INFO_STRU       astAvoidFreqInfo[CNAS_HSD_MAX_AVOID_FREQ_NUM];
}CNAS_HSD_AVOID_FREQ_LIST_STRU;


typedef struct
{
    VOS_UINT8                           aucSubnet[CNAS_PRL_BYTES_IN_SUBNET];
}CNAS_HSD_SUBNET_STRU;


typedef struct
{
    VOS_UINT16                                              usAcqIndex;         /* ��ϵͳ��¼ָ���ACQ���Index */
    VOS_UINT16                                              usSysIndex;         /* ��HRPDϵͳ��¼��ϵͳ���е�Index */
    VOS_UINT8                                               ucPrioLevel;        /* ָʾHRPD�����ȼ���ȡֵ��1��ʼ */
    CNAS_PRL_PREF_NEG_SYS_ENUM_UINT8                        enPrefNegSys;       /* ��ϵͳ�Ƿ�Ϊnegative��preferred */
    VOS_UINT8                                               aucRcv[2];
}CNAS_HSD_HRPD_SYS_REC_ITEM_STRU;


typedef struct
{
    VOS_UINT16                          usHrpdSysNum;                           /* ��Ч��HRPDϵͳ��Ϣ���� */
    VOS_UINT8                           aucRcv[2];
    CNAS_HSD_HRPD_SYS_REC_ITEM_STRU    *pstHrpdSysItem;                         /* ָ����HRPDϵͳ��¼�ڴ��ָ�� */
}CNAS_HSD_HRPD_SYS_LIST_STRU;


typedef struct
{
    VOS_UINT8                                               ucPrioLevel;        /* ��Ƶ������ȼ�����0��ʼ���ȼ��𽥽��� */
    CNAS_HSD_FREQ_TYPE_INDICATION_ENUM_UINT8                enFreqType;         /* Ƶ�����ͣ�Ŀǰ���������Ƿ�ΪMRUƵ�� */
    VOS_UINT8                                               aucRsv[2];
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          stFreq;             /* Ƶ����Ϣ */
}CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU;


typedef struct
{
    VOS_UINT16                          usTotalFreqNum;                         /* ȫ��Ƶ�������������������ռ��С */
    VOS_UINT16                          usAvailFreqNum;                         /* ��Ч��Ƶ����� */
    VOS_UINT16                          usCurrIndex;                            /* ָ��ǰƵ������� */
    VOS_UINT8                           ucLowPrioLevel;                         /* ����Ƶ���б��������ȼ�ֵ */
    VOS_UINT8                           ucRsv;
    CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU  *pstScanFreqItem;                        /* ָ����HRPDƵ����Ϣ�ڴ��ָ�� */
}CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU;


typedef struct
{
    VOS_UINT8                                               ucChanNum;
    VOS_UINT8                                               aucReseve[3];
    CAS_CNAS_HRPD_DERECTION_CHANNEL_STRU                    aulChannel[CAS_CNAS_HRPD_MAX_REDIRECT_CH_NUM]; /**< Number of frequencies included in list */
} CNAS_HSD_REDIRECTION_STRU;


typedef struct
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU              astFreq;             /* ���HRPD��Ƶ����Ϣ */
    CNAS_HSD_HRPD_SYS_REC_ITEM_STRU             stHrpdSysItem;       /* �����ȼ�����Ӧ��HRPDϵͳ��Ϣ */
} CNAS_HSD_SYSTEM_ACQUIRE_FREQ_INFO_STRU;


typedef struct
{
    VOS_UINT8                                   ucFreqNum;                                      /* Ƶ����� */
    VOS_UINT8                                   aucReserved[3];
    CNAS_HSD_SYSTEM_ACQUIRE_FREQ_INFO_STRU      astFreqInfo[CNAS_HSD_MAX_PREF_SUBNET_NUM];     /* Ƶ����Ϣ�б� */
} CNAS_HSD_HIGHPRIORITY_STRU;



typedef struct
{
    VOS_UINT8                                               ucWaitSearchFlag;           /* OOC�ȴ�������־������MRU0����ʱ��OOC��ʱ������ */
    VOS_UINT8                                               ucCurrentPhase;             /* ��¼��ǰ�����׶� */
    VOS_UINT16                                              usCurrentTimes;             /* ��¼�ڵ�ǰ�׶��Ѿ��������� */
    VOS_UINT8                                               ucSceneSetFlag;
    VOS_UINT8                                               aucReseve[3];
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enOocSearchScene;
}CNAS_HSD_OOC_CTX_INFO_STRU;


typedef struct
{
    VOS_UINT16                          usTimes;
    VOS_UINT16                          usTimerLen;
}CNAS_HSD_OOC_TIMER_INFO_STRU;


typedef struct
{
    VOS_UINT8                                               ucMru0SearchTimerLen;       /* ����MRU0ʱ�� */
    VOS_UINT8                                               ucPhaseNum;                 /* ����������Ŀ */
    VOS_UINT8                                               ucRsv[2];
    CNAS_HSD_OOC_TIMER_INFO_STRU                            astOocTimerInfo[CNAS_HSD_MAX_OOC_SCHEDULE_PHASE_NUM];
}CNAS_HSD_OOC_CONFIG_INFO_STRU;


typedef struct
{
    CNAS_HSD_OOC_CTX_INFO_STRU                              stOocCtxInfo;
    CNAS_HSD_OOC_CONFIG_INFO_STRU                           stConfigInfo;               /* ����OOC�����£�����������Ϣ */
}CNAS_HSD_OOC_SCHEDULE_INFO_STRU;


enum CNAS_HSD_HRPD_SYSTEM_STATUS_ENUM
{
    CNAS_HSD_HRPD_SYSTEM_UNAVAILABLE = 0,
    CNAS_HSD_HRPD_SYSTEM_AVAILABLE,
    CNAS_HSD_HRPD_SYSTEM_STATUS_BUTT,
};

typedef VOS_UINT32 CNAS_HSD_HRPD_SYSTEM_STATUS_ENUM_UINT32;


typedef struct
{
    CNAS_HSD_HRPD_SYSTEM_STATUS_ENUM_UINT32                 enStatus;
    CNAS_PRL_HRPD_SYSTEM_STRU                               stCampedHrpdSysInfo;
}CNAS_HSD_HRPD_SYSTEM_INFO_STRU;


typedef struct
{
    VOS_UINT32                          ulTotalNum;
    CNAS_PRL_HRPD_SYSTEM_STRU           stCampedHrpdSysInfo[CNAS_HSD_HRPD_ACQED_SUCC_SYS_MAX_NUM];
}CNAS_HSD_HISTORY_SYS_LIST_STRU;


typedef struct
{
    CNAS_PRL_HRPD_SYSTEM_STRU           stHrpdSys;
    VOS_UINT32                          ulLastRecSlice;
    VOS_UINT8                           ucNetwkLostCnt;
    VOS_UINT8                           aucResv[3];
}CNAS_HSD_NETWORK_LOST_SYS_RECORD_STRU;


typedef struct
{
    VOS_UINT8                                       ucFreqLockMode;
    VOS_UINT8                                       aucReserve[1];
    VOS_UINT16                                      usHrpdBandClass;
    VOS_UINT16                                      usHrpdFreq;
    VOS_UINT16                                      usHrpdPn;
}CNAS_HSD_CFREQ_LOCK_SET_PARA_STRU;



typedef struct
{
    VOS_UINT16                          usIndexNum;
    VOS_UINT8                           aucResv[2];
    VOS_UINT16                          ausMatchedHrpdSysIndexList[CNAS_PRL_MAX_MATCHED_HRPD_SYSTEM_NUM];                         /* ���ѵ���HRPDϵͳ��ƥ���sys record index */
}CNAS_HSD_MATCHED_HRPD_SYS_INDEX_LIST_STRU;



typedef struct
{
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32                      enModeType;         /* ���ֻ�Ϻͷǻ��ģʽ */
    CNAS_HSD_1X_SYSTEM_INFO_STRU                            st1xSysInfo;        /* ����1Xϵͳ��Ϣ */
    CNAS_HSD_HRPD_MRU_LIST_STRU                             stMruList;          /* ���MRU���� */
    CNAS_HSD_AVOID_FREQ_LIST_STRU                           stAvoidFreqList;    /* avoid�� */
    CNAS_HSD_HRPD_SYS_LIST_STRU                             stSysRecList;       /* HRPDϵͳ��¼�� */
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                       stScanFreqList;     /* HRPDƵ��� */
    CNAS_HSD_REDIRECTION_STRU                               stRedirection;      /* �ض���Ƶ����Ϣ */
    CNAS_HSD_HIGHPRIORITY_STRU                              stHighPriority;     /* �����ȼ�Ƶ����Ϣ */
    CNAS_HSD_HISTORY_SYS_LIST_STRU                          stHistorySysList;
    CNAS_HSD_HRPD_SYSTEM_INFO_STRU                          stHrpdSysInfo;      /* Hrpdϵͳפ��״̬��Ϣ */
    CNAS_HSD_OOC_SCHEDULE_INFO_STRU                         stOocScheduleInfo;  /* OOC������Ϣ */
    CNAS_PRL_HRPD_SYSTEM_STRU                               stCasOhmHrpdSys;    /* overhead�ϱ���Ƶ����Ϣ */
    CNAS_HSD_NETWORK_LOST_SYS_RECORD_STRU                   stNetwkLostSysRec;  /* ����������ϵͳ��¼ */
    CNAS_HSD_CFREQ_LOCK_SET_PARA_STRU                       stFreqLockPara;     /* ��Ƶ�Ĳ�����Ϣ */
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          stSyncFreq;         /* sync����ϵͳƵ�� */
    CNAS_HSD_1X_SYSTEM_INFO_STRU                            stHrpdMatched1xSysInfo;        /* ����1Xϵͳ��Ϣ */
    CNAS_HSD_MATCHED_HRPD_SYS_INDEX_LIST_STRU               stMatchedHrpdSysIndexList;      /* ƥ���HRPDϵͳ������ */

}CNAS_HSD_SYSTEM_ACQUIRE_CTRL_STRU;


typedef struct
{
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enNoRfScene;
    CNAS_HSD_AT_STATUS_ENUM_UINT32                          enATStatus;         /* AT��ǰ״̬ */
    CNAS_HSD_SESSION_NEG_STATUS_ENUM_UINT32                 enSessionNegStatus; /* ��ǰsession neg״̬ */
    VOS_UINT8                                               ucHrpdRfAvailFlg;
    VOS_UINT8                                               ucMru0RelateFlg;
    VOS_UINT8                                               ucSysCfgFlg;
    CNAS_CCB_1X_CAS_STATE_ENUM_UINT8                        enNoRf1XUeStatus;
    VOS_UINT32                                              ulUeSupportedBand;
    VOS_UINT8                                               ucIsAbnormalLostFLg;   /* ָʾ��ǰ�Ƿ�ΪIDLE̬�յ�CAS��abnormal lost */
    VOS_UINT8                                               aucResv[3];
}CNAS_HSD_SYSTEM_ASSIST_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucReadNvPrlDirectly;
    VOS_UINT8                           ucReadDefaultPrl;        /* ��ȡDefault Prl */
    VOS_UINT8                           ucNoCardModeCfgFlg;
    VOS_UINT8                           ucReserve;
}CNAS_HSD_TEST_CONFIG_STRU;


typedef struct
{
    /**************************************************************************
                        CNAS HSD״̬��״̬����״̬ջ
    **************************************************************************/
    CNAS_HSD_FSM_CTX_STRU               stCurFsm;                               /* ״̬����ǰ��״̬������ */
    CNAS_HSD_FSM_STACK_STRU             stFsmStack;                             /* ״̬����״̬��ջ����   */

    CNAS_HSD_CACHE_MSG_QUEUE_STRU       stCacheMsgQueue;                        /* CNAS HSD������Ϣ���� */

    CNAS_HSD_INT_MSG_QUEUE_STRU         stIntMsgQueue;                          /* CNAS �ڲ���Ϣ���� */

    CNAS_HSD_SYSTEM_ACQUIRE_CTRL_STRU   stSysAcqCtrl;                           /* ����������Ϣ */

    CNAS_HSD_SYSTEM_ASSIST_INFO_STRU    stSysAssistInfo;                        /* ϵͳ������Ϣ */

    CNAS_HSD_TEST_CONFIG_STRU           stTestConfig;                           /* ��Ĭ��PRL���� */

}CNAS_HSD_CTX_STRU;


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

extern VOS_VOID CNAS_HSD_InitCurrFsmCtx(
    CNAS_HSD_FSM_CTX_STRU              *pstCurrFsmCtx
);

extern VOS_VOID CNAS_HSD_InitFsmStackCtx(
    CNAS_HSD_FSM_STACK_STRU            *pstFsmStack
);

extern VOS_VOID CNAS_HSD_InitCacheMsgQueue(
    CNAS_HSD_CACHE_MSG_QUEUE_STRU      *pstCacheMsgQueue,
    CNAS_HSD_INIT_CTX_TYPE_ENUM_UINT8   enInitType
);

extern VOS_VOID CNAS_HSD_InitSysRecList(
    CNAS_HSD_INIT_CTX_TYPE_ENUM_UINT8   enInitType,
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstSysRecList
);

extern VOS_VOID CNAS_HSD_InitScanChanList(
    CNAS_HSD_INIT_CTX_TYPE_ENUM_UINT8   enInitType,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstScanChanList
);

extern VOS_VOID CNAS_HSD_InitSystemAcquireCtrl(
    CNAS_HSD_INIT_CTX_TYPE_ENUM_UINT8   enInitType,
    CNAS_HSD_SYSTEM_ACQUIRE_CTRL_STRU  *pstSysAcqCtrl
);

extern VOS_VOID CNAS_HSD_InitSystemAssistInfo(
    CNAS_HSD_SYSTEM_ASSIST_INFO_STRU   *pstSysAssistInfo
);

extern VOS_VOID CNAS_HSD_InitFsmCtx_L1Main(VOS_VOID);

extern VOS_VOID CNAS_HSD_InitFsmCtx_SwitchOn(VOS_VOID);

extern VOS_VOID CNAS_HSD_InitFsmCtx_SysAcq(VOS_VOID);

extern VOS_VOID CNAS_HSD_InitCtx(
    CNAS_HSD_INIT_CTX_TYPE_ENUM_UINT8   enInitType
);

extern VOS_VOID  CNAS_HSD_LoadSubFsm(
    CNAS_HSD_FSM_ID_ENUM_UINT32         enFsmId,
    CNAS_HSD_FSM_CTX_STRU              *pstCurFsm
);

extern VOS_VOID CNAS_HSD_PushFsm(
    CNAS_HSD_FSM_STACK_STRU            *pstFsmStack,
    CNAS_HSD_FSM_CTX_STRU              *pstNewFsm
);

extern VOS_VOID CNAS_HSD_PopFsm(VOS_VOID);

extern VOS_VOID CNAS_HSD_InitFsmL2(
    CNAS_HSD_FSM_ID_ENUM_UINT32         enFsmId
);

extern VOS_VOID CNAS_HSD_QuitFsmL2(VOS_VOID);

extern CNAS_HSD_CTX_STRU* CNAS_HSD_GetHsdCtxAddr(VOS_VOID);

extern CNAS_HSD_FSM_STACK_STRU* CNAS_HSD_GetFsmStackAddr(VOS_VOID);

extern CNAS_HSD_FSM_CTX_STRU* CNAS_HSD_GetCurFsmAddr(VOS_VOID);

extern VOS_UINT32 CNAS_HSD_GetFsmTopState(VOS_VOID);

extern NAS_FSM_DESC_STRU* CNAS_HSD_GetCurFsmDesc(VOS_VOID);

extern VOS_UINT16 CNAS_HSD_GetFsmStackDepth(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetFsmStackPopFlg(
    VOS_UINT16                          ucStackPopFlg
);

extern VOS_UINT16 CNAS_HSD_GetFsmStackPopFlg(VOS_VOID);

extern CNAS_HSD_CACHE_MSG_QUEUE_STRU* CNAS_HSD_GetCacheMsgAddr(VOS_VOID);

extern VOS_VOID CNAS_HSD_SaveCacheMsgInMsgQueue(
    VOS_UINT32                          ulEventType,
    VOS_VOID                           *pstMsg
);

extern VOS_UINT32  CNAS_HSD_SaveCacheMsg(
    VOS_UINT32                          ulEventType,
    VOS_VOID                           *pstMsg
);

extern VOS_VOID CNAS_HSD_ClearCacheMsg(
    VOS_UINT8                           ucIndex
);

extern VOS_UINT32 CNAS_HSD_GetNextCachedMsg(
    CNAS_HSD_MSG_STRU                  *pstEntryMsg
);

extern VOS_UINT32 CNAS_HSD_GetCacheMsgNum(VOS_VOID);

extern VOS_VOID  CNAS_HSD_SetCurrFsmState(
    VOS_UINT32                          ulCurrState
);

extern CNAS_HSD_MSG_STRU* CNAS_HSD_GetCurrFsmEntryMsgAddr(VOS_VOID);

extern VOS_VOID CNAS_HSD_SaveCurEntryMsg(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern CNAS_HSD_FSM_ID_ENUM_UINT32 CNAS_HSD_GetCurrFsmId(VOS_VOID);

extern CNAS_HSD_INT_MSG_QUEUE_STRU* CNAS_HSD_GetIntMsgQueueAddr(VOS_VOID);

extern VOS_VOID CNAS_HSD_InitInternalBuffer(
    CNAS_HSD_INT_MSG_QUEUE_STRU        *pstIntMsgQueue
);

extern VOS_VOID  CNAS_HSD_PutMsgInIntMsgQueue(
    VOS_UINT8                          *pstMsg
);

extern VOS_UINT8 *CNAS_HSD_GetNextIntMsg(VOS_VOID);

extern VOS_UINT8 CNAS_HSD_GetIntMsgNum(VOS_VOID);

extern CNAS_HSD_READ_CARD_FILES_CNF_ENUM_UINT32 CNAS_HSD_GetWaitCardFilesCnfFlg_SwitchOn(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetWaitCardFilesCnfFlg_SwitchOn(
    CNAS_HSD_READ_CARD_FILES_CNF_ENUM_UINT32                enReadCardFileCnfFlg
);

extern VOS_VOID CNAS_HSD_ClearWaitCardFilesCnfFlg_SwitchOn(
    CNAS_HSD_READ_CARD_FILES_CNF_ENUM_UINT32                enReadCardFileCnfFlg
);

extern CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32 CNAS_HSD_GetModeType(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetSysModeType(CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32 enModeType);

extern CNAS_HSD_1X_SYSTEM_INFO_STRU* CNAS_HSD_Get1xSystemInfo(VOS_VOID);

extern CNAS_HSD_HRPD_MRU_LIST_STRU* CNAS_HSD_GetMruList(VOS_VOID);

extern VOS_VOID CNAS_HSD_UpdateMru0(CNAS_PRL_HRPD_SYSTEM_STRU *pstCurSysId);

extern VOS_UINT8 CNAS_HSD_GetMruListNum(VOS_VOID);

extern VOS_UINT32 CNAS_HSD_GetMru0SysId(
    CNAS_PRL_HRPD_SYSTEM_STRU          *pstMru0SysId
);

extern CNAS_HSD_HRPD_SYS_LIST_STRU* CNAS_HSD_GetSysRecListAddr(VOS_VOID);

extern VOS_UINT16 CNAS_HSD_GetSysRecItemNum(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetSysRecListIndex(
    VOS_UINT16                          usHrpdSysNum
);
extern CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU* CNAS_HSD_GetScanFreqListAddr(VOS_VOID);

extern VOS_UINT16 CNAS_HSD_GetScanFreqNum(VOS_VOID);

extern VOS_UINT16 CNAS_HSD_GetCurScanFreqIndex(VOS_VOID);

extern VOS_UINT16 CNAS_HSD_GetScanListAvailFreqNum(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetCurScanFreqIndex(
    VOS_UINT16                          usCurScanIndex
);

extern CNAS_PRL_FREQENCY_CHANNEL_STRU* CNAS_HSD_GetScanFreqByIndex(
    VOS_UINT16                          usFreqIndex
);

extern CNAS_HSD_REDIRECTION_STRU* CNAS_HSD_GetRedirectionFreqAddr(VOS_VOID);

extern VOS_UINT16 CNAS_HSD_GetRedirectionFreqNum(VOS_VOID);

extern CNAS_HSD_OOC_CONFIG_INFO_STRU *CNAS_HSD_GetOocScheduleConfigInfo(VOS_VOID);

extern CNAS_HSD_OOC_SCHEDULE_INFO_STRU *CNAS_HSD_GetOocScheduleInfo(VOS_VOID);

extern VOS_VOID CNAS_HSD_InitOocScheduleInfo(
    CNAS_HSD_OOC_SCHEDULE_INFO_STRU    *pstOocScheduleInfo
);

extern VOS_VOID CNAS_HSD_InitOocCtxInfo(VOS_VOID);

extern VOS_UINT8 CNAS_HSD_GetOocWaitSearchFlag(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetOocWaitSearchFlag(VOS_UINT8 ucWaitSearchFlag);

extern CNAS_HSD_AVOID_FREQ_LIST_STRU* CNAS_HSD_GetAvoidFreqListAddr(VOS_VOID);

extern VOS_UINT8 CNAS_HSD_GetAvoidFreqListUsedNum(VOS_VOID);

extern CNAS_HSD_SESSION_NEG_STATUS_ENUM_UINT32 CNAS_HSD_GetSessionNegStatus(VOS_VOID);

extern VOS_VOID CNAS_HSD_SaveSessionNegStatus(
    CNAS_HSD_SESSION_NEG_STATUS_ENUM_UINT32                 enSessionNegStatus
);

extern CNAS_HSD_AT_STATUS_ENUM_UINT32 CNAS_HSD_GetATStatus(VOS_VOID);

extern VOS_VOID CNAS_HSD_SaveATStatus(
    CNAS_HSD_AT_STATUS_ENUM_UINT32     enCasStatus
);

extern CNAS_HSD_HISTORY_SYS_LIST_STRU* CNAS_HSD_GetHistorySysList(VOS_VOID);

extern VOS_VOID CNAS_HSD_UpdateHistorySysList(CNAS_PRL_HRPD_SYSTEM_STRU *pstCurrSystem);


extern CNAS_HSD_HRPD_SYSTEM_STATUS_ENUM_UINT32 CNAS_HSD_GetHrpdSysStatus(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetHrpdSysStatus(CNAS_HSD_HRPD_SYSTEM_STATUS_ENUM_UINT32 enStatus);

extern CNAS_PRL_HRPD_SYSTEM_STRU* CNAS_HSD_GetCampedHrpdSystemInfo(VOS_VOID);

extern VOS_VOID CNAS_HSD_UpdateHrpdSysInfo(CNAS_PRL_HRPD_SYSTEM_STRU *pstSystem);

extern VOS_VOID CNAS_HSD_SetNoRfScene(CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32 enNoRfScene);

extern CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32 CNAS_HSD_GetNoRfScene(VOS_VOID);

extern VOS_VOID CNAS_HSD_ClearHrpdSysInfo(VOS_VOID);

extern VOS_UINT8   CNAS_HSD_SetAbortFlg_SysAcq(
    VOS_UINT8                           ucFlg,
    CNAS_HSD_ABORT_REASON_ENUM_UINT8    enAbortReason
);

extern VOS_UINT8 CNAS_HSD_GetAbortFlg_SysAcq(VOS_VOID);

extern CNAS_HSD_ABORT_REASON_ENUM_UINT8 CNAS_HSD_GetAbortReason_SysAcq(VOS_VOID);

extern CNAS_HSD_HIGHPRIORITY_STRU* CNAS_HSD_GetHighPriorityFreqAddr(VOS_VOID);

extern VOS_UINT8 CNAS_HSD_GetHighPriorityFreqNum(VOS_VOID);

extern VOS_VOID CNAS_HSD_InitHighPriorityFreq(CNAS_HSD_HIGHPRIORITY_STRU* pstHighFreq);

extern CNAS_PRL_HRPD_SYSTEM_STRU* CNAS_HSD_GetCasOhmHrpdSysAddr(VOS_VOID);

extern VOS_VOID CNAS_HSD_SortCacheMsgInMsgQueue(VOS_VOID);

extern VOS_UINT8 CNAS_HSD_GetCacheMsgPriority(
    VOS_UINT32                          ulEventType
);

extern VOS_VOID CNAS_HSD_InitFsmCtx_InterSys(VOS_VOID);

extern VOS_UINT8   CNAS_HSD_SetAbortFlg_InterSys(
    VOS_UINT8                           ucFlg,
    CNAS_HSD_ABORT_REASON_ENUM_UINT8    enAbortReason
);

extern VOS_UINT8 CNAS_HSD_GetAbortFlg_InterSys(VOS_VOID);

extern CNAS_HSD_ABORT_REASON_ENUM_UINT8 CNAS_HSD_GetAbortReason_InterSys(VOS_VOID);

extern CNAS_HSD_NETWORK_LOST_SYS_RECORD_STRU* CNAS_HSD_GetNetwkLostSysRec(VOS_VOID);

extern CNAS_HSD_CFREQ_LOCK_SET_PARA_STRU* CNAS_HSD_GetFreqLockAddr(VOS_VOID);

extern VOS_UINT8 CNAS_HSD_GetFreqLockMode(VOS_VOID);

extern VOS_UINT8 CNAS_HSD_GetHrpdRfAvailFlg(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetHrpdRfAvailFlg(VOS_UINT8 ucHrpdRfAvailFlg);

extern VOS_UINT8 CNAS_HSD_GetMru0RelateFlg(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetMru0RelateFlg(VOS_UINT8 ucMru0RelateFlg);

extern VOS_VOID CNAS_HSD_InitCFreqLockValue(VOS_VOID);

extern VOS_UINT32 CNAS_HSD_GetUeSupportedBandClass(VOS_VOID);

extern CNAS_HSD_TEST_CONFIG_STRU* CNAS_HSD_GetTestConfig(VOS_VOID);
CNAS_CCB_1X_CAS_STATE_ENUM_UINT8 CNAS_HSD_GetNoRf1XUeStatus(VOS_VOID);

VOS_VOID CNAS_HSD_SetNoRf1XUeStatus(CNAS_CCB_1X_CAS_STATE_ENUM_UINT8 enStatus);

extern CNAS_HSD_1X_SYSTEM_INFO_STRU* CNAS_HSD_GetScanListMatched1xSystemInfo(VOS_VOID);

extern VOS_VOID CNAS_HSD_SetSysCfgFlag(VOS_UINT8 ucIsSysCfg);

extern VOS_UINT8 CNAS_HSD_GetSysCfgFlag(VOS_VOID);

extern CNAS_HSD_MATCHED_HRPD_SYS_INDEX_LIST_STRU* CNAS_HSD_GetMatchedHrpdSysIndexListAddr(VOS_VOID);

extern VOS_VOID CNAS_HSD_InitMatchedHrpdSysIndexList(VOS_VOID);

extern VOS_UINT8   CNAS_HSD_IsCurAbortFlgSet_SysAcq(
    CNAS_HSD_ABORT_REASON_ENUM_UINT8    enAbortReason
);

extern VOS_VOID CNAS_HSD_SetAbnormalLostFlag(VOS_UINT8 ucIsAbnormalLost);

extern VOS_UINT8 CNAS_HSD_GetAbnormalLostFlag(VOS_VOID);

#endif

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of CnasHsdCtx.h */
