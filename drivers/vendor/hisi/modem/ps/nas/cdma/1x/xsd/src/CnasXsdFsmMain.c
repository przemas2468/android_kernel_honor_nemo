

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "CnasXsdFsmMain.h"
#include  "CnasXsdFsmMainTbl.h"
#include  "CnasXsdCtx.h"
#include  "CnasMntn.h"
#include  "CnasXsdComFunc.h"
#include  "CnasXsdSndMscc.h"
#include  "CnasXsdSndCas.h"
#include  "cas_1x_control_main_nas_pif.h"
#include  "CnasPrlApi.h"
#include  "CnasCcb.h"
#include  "CnasTimerMgmt.h"
#include  "CnasXsdSndXreg.h"

#include  "CnasXsdSndXcc.h"
#include  "CnasXsdMntn.h"
#include  "NasOmInterface.h"
#include "CnasXsdSysAcqStrategy.h"
#include  "CnasXsdSndMscc.h"
#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                        PS_FILE_ID_CNAS_XSD_FSM_MAIN_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/*****************************************************************************/
typedef VOS_VOID  (*pCNasXsdSystemDetermineIndProcFunc)(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU   *pstSdIndInfo
);

typedef struct
{
    VOS_UINT32                                  ulSdReason;
    pCNasXsdSystemDetermineIndProcFunc          pFuncSDIndProc;
}CNAS_XSD_SYSTEM_DETERMINE_IND_PROC_TBL_FUNC;

CNAS_XSD_SYSTEM_DETERMINE_IND_PROC_TBL_FUNC                 g_astCnasXsdSystemDetermineIndL1MainProcFuncTbl[] =
{
    {CAS_CNAS_1X_SD_RESCAN_IND,                         CNAS_XSD_ProcCasSystemDetermineIndWithRescanReason_Idle},
    {CAS_CNAS_1X_SD_PROTO_MISMATCH_IND,                 CNAS_XSD_ProcCasSystemDetermineIndWithProtoMismatchReason_Idle},
    {CAS_CNAS_1X_SD_SYS_LOST_IND,                       CNAS_XSD_ProcCasSystemDetermineIndWithSysLostReason_Idle},
    {CAS_CNAS_1X_SD_LOCK_IND,                           CNAS_XSD_ProcCasSystemDetermineIndWithLockReason_Idle},
    {CAS_CNAS_1X_SD_UNLOCK_IND,                         CNAS_XSD_ProcCasSystemDetermineIndWithUnlockReason_Idle},
    {CAS_CNAS_1X_SD_ACCESS_DENIED_IND,                  CNAS_XSD_ProcCasSystemDetermineIndWithAccessDeniedReason_Idle},
    {CAS_CNAS_1X_SD_ACCT_BLOCKED_IND,                   CNAS_XSD_ProcCasSystemDetermineIndWithAcctBlockReason_Idle},
    {CAS_CNAS_1X_SD_NDSS_OFF_IND,                       CNAS_XSD_ProcCasSystemDetermineIndWithNdssOffReason_Idle},
    {CAS_CNAS_1X_SD_ERR_IND,                            CNAS_XSD_ProcCasSystemDetermineIndWithErrReason_Idle},
    {CAS_CNAS_1X_SD_REDIR_IND,                          CNAS_XSD_ProcCasSystemDetermineIndWithRedirectionReason_Idle},
    {CAS_CNAS_1X_SD_REGIST_REJ_IND,                     CNAS_XSD_ProcCasSystemDetermineIndWithRegistRejReason_Idle},
    {CAS_CNAS_1X_SD_RELEASE_IND,                        CNAS_XSD_ProcCasSystemDetermineIndWithReleaseReason_Idle},
    {CAS_CNAS_1X_SD_RESELECTION_IND,                    CNAS_XSD_ProcCasSystemDetermineIndWithReselectionReason_Idle},
    {CAS_CNAS_1X_SD_SYSTEM_RESELECTION_IND,             CNAS_XSD_ProcCasSystemDetermineIndWithSystemReselectionReason_Idle},
    {CAS_CNAS_1X_SD_NEW_SYSTEM_IND,                     CNAS_XSD_ProcCasSystemDetermineIndWithNewSystemReason_Idle},
    {CAS_CNAS_1X_SD_MAX_ACCESS_FAILURE,                 CNAS_XSD_ProcCasSystemDetermineIndWithMaxAccessFailureReason_Idle},
    {CAS_CNAS_1X_SD_ABNORMAL_IND,                       CNAS_XSD_ProcCasSystemDetermineIndWithAbnormalReason_Idle},
    {CAS_CNAS_1X_SD_NO_RF,                              CNAS_XSD_ProcCasSystemDetermineIndWithNoRfReason_Idle},
    {CAS_CNAS_1X_SD_ACQUISITION_FAIL,                   CNAS_XSD_ProcCasSystemDetermineIndWithAcquisitionFail_Idle},
};



/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -save -e958*/

VOS_UINT32 CNAS_XSD_RcvStartReq_Null(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ����L2����״̬�� */
    CNAS_XSD_InitFsmL2(CNAS_XSD_FSM_SWITCH_ON);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvSwitchOnRsltCnf_Null(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_XSD_SWITCH_ON_RSLT_CNF_STRU                    *pstSwitchOnMsg = VOS_NULL_PTR;

    pstSwitchOnMsg = (CNAS_XSD_XSD_SWITCH_ON_RSLT_CNF_STRU *)pstMsg;

    /* �յ�������������ɹ�Ǩ�Ƶ�CNAS_XSD_L1_STA_INITIAL״̬��
       ʧ�ܱ���CNAS_XSD_L1_STA_NULL״̬
    */
    if (CNAS_XSD_SWITCH_ON_RESULT_SUCCESS == pstSwitchOnMsg->enRslt)
    {
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);
    }
    else
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_RcvSwitchOnRsltCnf_Null:switch on fail");
    }


    return VOS_TRUE;
}


VOS_UINT32  CNAS_XSD_RcvPowerOffReq_Null(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֱ�ӻظ��ػ������MSCC */
    CNAS_XSD_SndMsccPowerOffCnf();

    return VOS_TRUE;

}


VOS_UINT32 CNAS_XSD_RcvStartReq_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* If XSD receives a Power On request In L1Main initial state
        then XSD sends Power On success Result to MSCC */
    CNAS_XSD_SndMsccStartCnf(XSD_MSCC_START_RESULT_SUCC);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvPowerOffReq_L1Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ���ohmƵ����Ϣ */
    CNAS_CCB_ClearCdmaOhmFreq();

    /* ����L2�ػ�״̬�� */
    CNAS_XSD_InitFsmL2(CNAS_XSD_FSM_POWER_OFF);

    return VOS_TRUE;
}



VOS_UINT32 CNAS_XSD_RcvPowerOffRsltCnf_L1Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_XSD_POWER_OFF_RSLT_CNF_STRU                   *pstPowerOffRsltCnf = VOS_NULL_PTR;

    pstPowerOffRsltCnf = (CNAS_XSD_XSD_POWER_OFF_RSLT_CNF_STRU *)pstMsg;

    if (CNAS_XSD_POWEROFF_REASON_POWER_OFF == pstPowerOffRsltCnf->enPowerOffReason)
    {
        /* �ͷű����GEO�����б��buffer */
        CNAS_XSD_FreeBufferOfGeoSrchList();

        /* ֹͣXSD���ж�ʱ�� */
        CNAS_StopAllTimerWithSpecifiedPid(UEPS_PID_XSD);

        /* ����ʱ�ж��Ƿ����ʼ��PRL */

        /* ��ʼ��XSDģ��CTX */
        CNAS_XSD_InitCtx(CNAS_XSD_INIT_CTX_POWEROFF);

        /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_NULL */
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_NULL);
    }
    else if (CNAS_XSD_POWEROFF_REASON_POWER_SAVE == pstPowerOffRsltCnf->enPowerOffReason)
    {
        /* Stop all timers except slice reverse protect timer */
        CNAS_XSD_StopAllTimerExceptSpecifiedTimerId(TI_CNAS_XSD_SLICE_REVERSE_PROTECT_TIMER);

        /* Send Power Save Cnf to MSCC */
        if (CNAS_XSD_POWEROFF_RSLT_SUCCESS == pstPowerOffRsltCnf->enRslt)
        {
            CNAS_XSD_SndMsccPowerSaveCnf(NAS_MSCC_PIF_POWER_SAVE_SUCCESS);

            CNAS_MNTN_LogKeyEvent(NAS_OM_EVENT_1X_POWER_SAVE, UEPS_PID_XSD);


            /* Set Next State to Deactive State */
            CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_DEACTIVE);
        }
        else
        {
            CNAS_XSD_SndMsccPowerSaveCnf(NAS_MSCC_PIF_POWER_SAVE_FAILURE);
        }
    }
    else
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_RcvPowerOffRsltCnf_L1Main: Invalid Result!");
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvSysAcqReq_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_GEO_LIST_SRCH_INFO_STRU   *pstGeoList = VOS_NULL_PTR;

    /* �������벶��״̬��֮ǰ����ȡPRL��GEO��������Ӧ��ϵͳ���е��׸����� */
    pstGeoList = CNAS_XSD_GetGeoSrchListInfoAddr();
    CNAS_XSD_GetGeoListFromPrlSysRecordInfo(pstGeoList);

    /* log GEO list����״̬ */
    CNAS_XSD_LogGeoListSrchStatus(pstGeoList);

    /* ���ohmƵ����Ϣ */
    CNAS_CCB_ClearCdmaOhmFreq();

    /* ������Ƶ��Դ */
    CNAS_XSD_SndCasBeginSessionNotify(CNAS_CAS_1X_SESSION_TYPE_NETWORK_SEARCH);

    /* ����L2 System Acquire״̬��, Reference to CDG 130 Figure 5-4 Example System Selection */
    CNAS_XSD_InitFsmL2(CNAS_XSD_FSM_SYS_ACQ);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvInternalSysAcqReq_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_XSD_SYSTEM_ACQUIRED_REQ_STRU                  *pstIntSysAcqReq         = VOS_NULL_PTR;
    CNAS_CAS_1X_SESSION_TYPE_ENUM_UINT16                    enSessionType;

    pstIntSysAcqReq = (CNAS_XSD_XSD_SYSTEM_ACQUIRED_REQ_STRU *)pstMsg;

    enSessionType   = CNAS_CAS_1X_SESSION_TYPE_ENUM_BUTT;

    if (CNAS_XSD_SYS_ACQ_SCENE_CALL_RELEASED == pstIntSysAcqReq->enSysAcqScene)
    {
        enSessionType = CNAS_CAS_1X_SESSION_TYPE_TCH_REL_NETWORK_SEARCH;
    }
    else
    {
        enSessionType = CNAS_CAS_1X_SESSION_TYPE_NETWORK_SEARCH;
    }

    /* ������Ƶ��Դ */
    CNAS_XSD_SndCasBeginSessionNotify(enSessionType);

    /* ���ohmƵ����Ϣ */
    CNAS_CCB_ClearCdmaOhmFreq();

    /* ����L2 System Acquire״̬�� */
    CNAS_XSD_InitFsmL2(CNAS_XSD_FSM_SYS_ACQ);

    return VOS_TRUE;
}



VOS_UINT32 CNAS_XSD_RcvSysAcqRsltCnf_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_XSD_SYSTEM_ACQUIRED_RSLT_CNF_STRU             *pstSysAcqRslt = VOS_NULL_PTR;
    CNAS_CAS_1X_SESSION_TYPE_ENUM_UINT16                    enSessionType;

    pstSysAcqRslt = (CNAS_XSD_XSD_SYSTEM_ACQUIRED_RSLT_CNF_STRU *)pstMsg;

    enSessionType   = CNAS_CAS_1X_SESSION_TYPE_ENUM_BUTT;

    if (CNAS_XSD_SYS_ACQ_SCENE_CALL_RELEASED == pstSysAcqRslt->enCurAcqScene)
    {
        enSessionType = CNAS_CAS_1X_SESSION_TYPE_TCH_REL_NETWORK_SEARCH;
    }
    else
    {
        enSessionType = CNAS_CAS_1X_SESSION_TYPE_NETWORK_SEARCH;
    }

    /* ������Ƶ��Դ */
    CNAS_XSD_SndCasEndSessionNotify(enSessionType);

    if (CNAS_XSD_SYSTEM_ACQUIRED_RESULT_SUCCESS == pstSysAcqRslt->enRslt)
    {
        CNAS_XSD_ProcSearchNetworkRslt_Succ(pstSysAcqRslt);

        CNAS_MNTN_LogKeyEvent(NAS_OM_EVENT_1X_SYS_ACQUIRE_SUCCESS, UEPS_PID_XSD);
    }
    else if (CNAS_XSD_SYSTEM_ACQUIRED_RESULT_ABORTED == pstSysAcqRslt->enRslt)
    {
        CNAS_XSD_ProcSearchNetworkRslt_Abort(pstSysAcqRslt);

        CNAS_MNTN_LogKeyEvent(NAS_OM_EVENT_1X_SYS_LOST, UEPS_PID_XSD);
    }
    else
    {
        CNAS_XSD_ProcSearchNetworkRslt_Fail(pstSysAcqRslt);

        CNAS_MNTN_LogKeyEvent(NAS_OM_EVENT_1X_SYS_LOST, UEPS_PID_XSD);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvMsccCallRedialSystemAcquireNtf_Idle(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MSCC_XSD_CALL_REDIAL_SYSTEM_ACQUIRE_NTF_STRU           *pstCallRedialNtfMsg = VOS_NULL_PTR;

    pstCallRedialNtfMsg = (MSCC_XSD_CALL_REDIAL_SYSTEM_ACQUIRE_NTF_STRU*)pstMsg;

    /* �����������ݺ����� */
    if ((NAS_MSCC_PIF_CDMA_CALL_TYPE_1X_NORMAL_VOICE_CALL == pstCallRedialNtfMsg->enCallType)
     || (NAS_MSCC_PIF_CDMA_CALL_TYPE_1X_NORMAL_DATA_CALL  == pstCallRedialNtfMsg->enCallType))
    {
        if (VOS_NULL_PTR == CNAS_XSD_GetCallRedialScanChanListAddr()->pstScanChanInfo)
        {
            return VOS_TRUE;
        }

        /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_INITIAL */
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        /*���������ڲ�������Ϣ */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_NORMAL_CALL_REDIAL, 0, VOS_NULL_PTR);

        return VOS_TRUE;
    }

    /* �Գ峡������: ��Я����ԭ��ֵΪNO SERVICE������������ */
    if (NAS_MSCC_PIF_CDMA_CALL_CAUSE_INTERNAL_NO_SERVICE == pstCallRedialNtfMsg->enCause)
    {
        return VOS_TRUE;
    }

    /* ���������� */
    if (VOS_NULL_PTR == CNAS_XSD_GetEmcCallRedialScanChanListAddr()->pstScanChanInfo)
    {
        return VOS_TRUE;
    }

    CNAS_XSD_ProcEmcRedialSysAcq_Idle();

    return VOS_TRUE;
}


VOS_VOID CNAS_XSD_ProcEmcRedialSysAcq_Idle()
{

    VOS_UINT32                                              ulIndex;
    CNAS_XSD_EMC_REDIAL_SYS_ACQ_CFG_STRU                   *pstEmcRedialSysAcqCfg = VOS_NULL_PTR;
    CNAX_XSD_CAMPED_SYS_INFO_STRU                          *pstSysCampedSysInfo   = VOS_NULL_PTR;

    pstEmcRedialSysAcqCfg = CNAS_XSD_GetEmcRedialSysAcqCfgInfo();
    pstSysCampedSysInfo   = CNAS_XSD_GetCurCampedSysInfo();

    /* ���������µ�ϵͳ�����¼��� */
    if ((pstSysCampedSysInfo->stSystem.usSid              != pstEmcRedialSysAcqCfg->stLastCampedSys.usSid)
     || (pstSysCampedSysInfo->stSystem.usNid              != pstEmcRedialSysAcqCfg->stLastCampedSys.usNid)
     || (pstSysCampedSysInfo->stSystem.stFreq.enBandClass != pstEmcRedialSysAcqCfg->stLastCampedSys.stFreq.enBandClass)
     || (pstSysCampedSysInfo->stSystem.stFreq.usChannel   != pstEmcRedialSysAcqCfg->stLastCampedSys.stFreq.usChannel))
    {
        pstEmcRedialSysAcqCfg->ucRedialCount = 0;

        NAS_MEM_CPY_S(&(pstEmcRedialSysAcqCfg->stLastCampedSys),
                      sizeof(CNAS_PRL_1X_SYSTEM_STRU),
                      &(pstSysCampedSysInfo->stSystem),
                      sizeof(CNAS_PRL_1X_SYSTEM_STRU));

    }

    /* ��ȡ��ǰϵͳ��mru list�е����� */
    ulIndex = 0;


    if ((VOS_TRUE == CNAS_XSD_GetCurSysIndexOfEmcRedialMruList(&(pstSysCampedSysInfo->stSystem), &ulIndex))
     && (pstEmcRedialSysAcqCfg->ucRedialCount < pstEmcRedialSysAcqCfg->aucRedialTimes[ulIndex]))
    {
        pstEmcRedialSysAcqCfg->ucRedialCount++;
    }
    else
    {
        /* ��ǰפ��Ƶ����mru�б��У��ز��������ڸó����µ�Ӧ�ز��Ĵ��������ߵ�ǰפ��Ƶ�㲻��mru�б���������*/

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR);

    }

    return;
}



VOS_UINT32 CNAS_XSD_RcvInternalSysAcqReq_Idle(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_XSD_SYSTEM_ACQUIRED_REQ_STRU                  *pstIntSysAcqReq         = VOS_NULL_PTR;
    CNAS_CAS_1X_SESSION_TYPE_ENUM_UINT16                    enSessionType;

    pstIntSysAcqReq = (CNAS_XSD_XSD_SYSTEM_ACQUIRED_REQ_STRU *)pstMsg;

    enSessionType   = CNAS_CAS_1X_SESSION_TYPE_ENUM_BUTT;

    if (CNAS_XSD_SYS_ACQ_SCENE_CALL_RELEASED == pstIntSysAcqReq->enSysAcqScene)
    {
        enSessionType = CNAS_CAS_1X_SESSION_TYPE_TCH_REL_NETWORK_SEARCH;
    }
    else
    {
        enSessionType = CNAS_CAS_1X_SESSION_TYPE_NETWORK_SEARCH;
    }

    /* ������Ƶ��Դ */
    CNAS_XSD_SndCasBeginSessionNotify(enSessionType);

    /* ���ohmƵ����Ϣ */
    CNAS_CCB_ClearCdmaOhmFreq();

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* ����L2 System Acquire״̬�� */
    CNAS_XSD_InitFsmL2(CNAS_XSD_FSM_SYS_ACQ);

    return VOS_TRUE;

}


VOS_UINT32 CNAS_XSD_RcvMsccCallRedialSystemAcquireNtf_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MSCC_XSD_CALL_REDIAL_SYSTEM_ACQUIRE_NTF_STRU           *pstCallRedialNtfMsg = VOS_NULL_PTR;

    pstCallRedialNtfMsg = (MSCC_XSD_CALL_REDIAL_SYSTEM_ACQUIRE_NTF_STRU*)pstMsg;

    /* �����������ݺ����� */
    if ((NAS_MSCC_PIF_CDMA_CALL_TYPE_1X_NORMAL_VOICE_CALL == pstCallRedialNtfMsg->enCallType)
     || (NAS_MSCC_PIF_CDMA_CALL_TYPE_1X_NORMAL_DATA_CALL  == pstCallRedialNtfMsg->enCallType))
    {
        /* if scan list is null, nothing to do */
        if (VOS_NULL_PTR == CNAS_XSD_GetCallRedialScanChanListAddr()->pstScanChanInfo)
        {
            return VOS_TRUE;
        }

        /* ֹͣ����OOC�¶�ʱ�� */
        CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_TIMER);

        CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_MRU0_TIMER);


        /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_INITIAL */
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        /*���������ڲ�������Ϣ */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_NORMAL_CALL_REDIAL, 0, VOS_NULL_PTR);

        return VOS_TRUE;
    }

    /* ���������� */
    if (VOS_NULL_PTR == CNAS_XSD_GetEmcCallRedialScanChanListAddr()->pstScanChanInfo)
    {
        return VOS_TRUE;
    }

    /* ֹͣ����OOC�¶�ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_TIMER);

    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_MRU0_TIMER);


    /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_INITIAL */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /*���������ڲ�������Ϣ */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}



VOS_UINT32 CNAS_XSD_RcvMsccCallRedialSystemAcquireNtf_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MSCC_XSD_CALL_REDIAL_SYSTEM_ACQUIRE_NTF_STRU           *pstCallRedialNtfMsg = VOS_NULL_PTR;
    CNAS_XSD_SERVICE_STATUS_ENUM_UINT32                     enSrvStatus;

    pstCallRedialNtfMsg = (MSCC_XSD_CALL_REDIAL_SYSTEM_ACQUIRE_NTF_STRU*)pstMsg;

    /* �˳�CallBack״̬ */
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        /* ֹͣCALLBACK������ʱ�� */
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_MODE_PROTECT_TIMER);
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER);
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER);

        CNAS_XSD_QuitCallBackMode();
    }

    /* �����������ݺ����� */
    if ((NAS_MSCC_PIF_CDMA_CALL_TYPE_1X_NORMAL_VOICE_CALL == pstCallRedialNtfMsg->enCallType)
     || (NAS_MSCC_PIF_CDMA_CALL_TYPE_1X_NORMAL_DATA_CALL  == pstCallRedialNtfMsg->enCallType))
    {
        /*******************************************************************************************
        * ע: ���泡���Ǵ����յ����������������ز�ʱ����ʱXSD��������״̬���еȴ�CAS��overheader��Ϣ
        *     �ϱ�, ��XSD�ȴ���overheader��Ϣ���˳�L2״̬�������������ʱ������Ǩ��״̬���ڴ�״̬��
        *     (initial״̬)������������е��ز���Ϣʱ������Ѿ�פ���ɹ�������normal serice������Ҫ
        *     �ٴ���������ֱ��ʹ�õ�ǰפ������ϵͳ�Ϸ�����оͿ�����
        *******************************************************************************************/
        enSrvStatus = CNAS_XSD_GetCurSrvStatus();
        if ((VOS_TRUE                               == CNAS_XSD_GetSystemCampOnFlag())
         && (CNAS_XSD_SERVICE_STATUS_NORMAL_SERVICE == enSrvStatus))
        {
            CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_IDLE);

            CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);

            return VOS_TRUE;
        }

        /* if scan list is null, nothing to do */
        if (VOS_NULL_PTR == CNAS_XSD_GetCallRedialScanChanListAddr()->pstScanChanInfo)
        {
            CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_OOC);

            /* abornormal protect,satrt availablt timer here */
            CNAS_XSD_StartTimer(TI_CNAS_XSD_AVAILABLE_TIMER, TI_CNAS_XSD_AVAILABLE_LEN);

            return VOS_TRUE;
        }

        /*���������ڲ�������Ϣ */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_NORMAL_CALL_REDIAL, 0, VOS_NULL_PTR);

        return VOS_TRUE;
    }

    /* ���������̴��� */
    /* ������������ϵ�ǰ�����������ɹ��������������õ�ǰ��������� */
    if (VOS_TRUE == CNAS_XSD_GetSystemCampOnFlag())
    {
        CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_IDLE);

        return VOS_TRUE;
    }

    /* δ������쳣������� */
    if (VOS_NULL_PTR == CNAS_XSD_GetEmcCallRedialScanChanListAddr()->pstScanChanInfo)
    {
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_OOC);

        CNAS_XSD_StartTimer(TI_CNAS_XSD_AVAILABLE_TIMER, TI_CNAS_XSD_AVAILABLE_LEN);

        return VOS_TRUE;
    }

    /* �����������ز����� */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}




VOS_UINT32 CNAS_XSD_RcvSysDeterminInd_Idle(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd       = VOS_NULL_PTR;
    pCNasXsdSystemDetermineIndProcFunc                      pfunSdProc;
    VOS_UINT32                                              i;

    pstSysDeterminInd = (CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU *)pstMsg;

    pfunSdProc = VOS_NULL_PTR;

    for (i = 0; i < (sizeof(g_astCnasXsdSystemDetermineIndL1MainProcFuncTbl)/sizeof(CNAS_XSD_SYSTEM_DETERMINE_IND_PROC_TBL_FUNC)); i++)
    {
        if (pstSysDeterminInd->enSdReason == g_astCnasXsdSystemDetermineIndL1MainProcFuncTbl[i].ulSdReason)
        {
            pfunSdProc = g_astCnasXsdSystemDetermineIndL1MainProcFuncTbl[i].pFuncSDIndProc;
            break;
        }
    }

    if (VOS_NULL_PTR != pfunSdProc)
    {
        pfunSdProc(pstSysDeterminInd);
    }
    else
    {
        /*���ԭ��ֵ�������������б��У�Ĭ��OOC��������*/
        CNAS_XSD_ProcCasSystemDetermineIndWithSysLostReason_Idle(pstSysDeterminInd);
    }

    /* ��Ϣ����������������������MSCC�ϱ��޷���MSCC֪ͨMMA��MMA������ʱ�ϱ���ʱ�� */
    if (VOS_TRUE == CNAS_XSD_IsNeedReportNoService(pstSysDeterminInd))
    {
        CNAS_XSD_SndMscc1xNoSrvInd();
    }

    return VOS_TRUE;
}



VOS_UINT32 CNAS_XSD_RcvCasOhmInd_Idle(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CAS_CNAS_1X_OHM_IND_STRU           *pstOhmInd       = VOS_NULL_PTR;
    CNAS_PRL_1X_SYSTEM_STRU             stCurSysInfo;
    VOS_UINT32                          ulIsNegSys;
    VOS_UINT32                          ulIsCallOrigSysExist;
    CNAS_PRL_MATCHED_GEO_LIST_INFO_STRU stGeoListInfo;
    /* ����XCC״̬�жϵ�ǰ�Ƿ����ҵ��̬������ҵ��̬��Negativeϵͳ��Ҳ�ݲ����� */
    VOS_UINT32                          ulIsCurCallStateIdle;

    ulIsCurCallStateIdle                = CNAS_CCB_IsCurCallStateIdle();

    NAS_MEM_SET_S(&stGeoListInfo, sizeof(stGeoListInfo), 0x00, sizeof(CNAS_PRL_MATCHED_GEO_LIST_INFO_STRU));

    NAS_MEM_SET_S(&stCurSysInfo, sizeof(stCurSysInfo), 0x00, sizeof(CNAS_PRL_1X_SYSTEM_STRU));
    ulIsCallOrigSysExist                = CNAS_XSD_IsEmcCallOriginalSysExist();

    pstOhmInd       = (CAS_CNAS_1X_OHM_IND_STRU *)pstMsg;

    stCurSysInfo.usSid              = pstOhmInd->usSid;
    stCurSysInfo.usNid              = pstOhmInd->usNid;
    stCurSysInfo.stFreq.enBandClass = pstOhmInd->usBandClass;
    stCurSysInfo.stFreq.usChannel   = pstOhmInd->usFreq;

    /***********************************************************************************************
     * 1. ���µ�ǰϵͳƵ�㵽��ʷͬ���ɹ�Ƶ���Ƶ���б���
     **********************************************************************************************/
    CNAS_XSD_UpdateHistorySyncedSysFreqList(&(stCurSysInfo.stFreq));

    CNAS_PRL_Get1xSysGeoList(&stCurSysInfo, &stGeoListInfo);

    if (VOS_FALSE == CNAS_PRL_Is1xSysNegativeInPrl(&stCurSysInfo, &stGeoListInfo))
    {
        CNAS_CCB_SetCdmaOhmFreq(&(stCurSysInfo.stFreq));
    }

    /***********************************************************************************************
     * 2. check��ǰϵͳ�Ƿ���negative�������negativeϵͳ�����޽��������ڣ�����Ҫ��������
     **********************************************************************************************/
    ulIsNegSys = CNAS_XSD_IsCurSysNotSuitableSys(&stCurSysInfo,
                                                 pstOhmInd->enServiceInfoIncl,
                                                 pstOhmInd->stServiceInfo.usMcc);


    if ((VOS_NULL_PTR   == CNAS_XSD_GetEmcCallRedialScanChanListAddr()->pstScanChanInfo)
     && (VOS_FALSE      == ulIsCallOrigSysExist)
     && (VOS_TRUE       == ulIsNegSys)
     && (VOS_TRUE       == ulIsCurCallStateIdle))
    {
        CNAS_XSD_SndMsccSidNidInd(stCurSysInfo.usSid, stCurSysInfo.usNid);

        /* �����negϵͳ������������ */
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

        return VOS_TRUE;
    }

    CNAS_XSD_ProcCasOhmInd(pstOhmInd, ulIsNegSys);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvTiAvailTimerExpired_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_HRPD_SYSTEM_STRU           stPrlHrpdSystem;
    VOS_UINT16                          usAvailTimerLen;
    CNAS_XSD_OOC_SCHEDULE_INFO_STRU    *pstOocScheduleInfo = VOS_NULL_PTR;
 
    NAS_MEM_SET_S(&stPrlHrpdSystem, sizeof(CNAS_PRL_HRPD_SYSTEM_STRU), 0x0, sizeof(CNAS_PRL_HRPD_SYSTEM_STRU));  

    if (VOS_TRUE == CNAS_XSD_GetTestConfig()->ucIsMod1xAvailTimerLen)
    {
        pstOocScheduleInfo  = CNAS_XSD_GetOocScheduleInfo();
        usAvailTimerLen     = CNAS_XSD_GetNextOocScheduleTimerLen(CNAS_XSD_GetOocScheduleInfo());
    
        
        if ((0   != PS_MEM_CMP(&stPrlHrpdSystem, CNAS_HSD_GetCasOhmHrpdSysAddr(), sizeof(CNAS_PRL_HRPD_SYSTEM_STRU)))
         && (300 != usAvailTimerLen))
        {
            /*�������Ϊ5���� */
            usAvailTimerLen = 300;

            pstOocScheduleInfo->stConfigInfo.ucMru0SearchTimerLen = 255;
        
            pstOocScheduleInfo->stOocCtxInfo.ucCurrentPhase                = 0;
            pstOocScheduleInfo->stOocCtxInfo.usCurrentTimes                = 0;
            pstOocScheduleInfo->stConfigInfo.astOocTimerInfo[0].usTimes    = 10;
            pstOocScheduleInfo->stConfigInfo.astOocTimerInfo[0].usTimerLen = 300;

            /* ����OOC available��ʱ�� */
            CNAS_XSD_StartTimer(TI_CNAS_XSD_AVAILABLE_TIMER, (usAvailTimerLen * 1000));
             
            return VOS_TRUE;
         
        } 
    }
   
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_GetOocSearchScene(),
                                  0,
                                  VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvInternalRedirectionInd_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ������Ƶ��Դ */
    CNAS_XSD_SndCasBeginSessionNotify(CNAS_CAS_1X_SESSION_TYPE_NETWORK_SEARCH);

    /* ���ohmƵ����Ϣ */
    CNAS_CCB_ClearCdmaOhmFreq();

    /* ����L2 redireciton״̬�� */
    CNAS_XSD_InitFsmL2(CNAS_XSD_FSM_REDIRECTION);

    return VOS_TRUE;
}


VOS_VOID CNAS_XSD_ProcRedirectionRsltCnf_Success( VOS_VOID )
{
    if (VOS_TRUE == CNAS_XSD_GetNdssIndFlag())
    {
        CNAS_XSD_SndXccNdssResultInd(XSD_XCC_NDSS_RESULT_SUCCESS);

        CNAS_XSD_SetNdssIndFlag(VOS_FALSE);
    }

    CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);

    /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_IDLE */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_IDLE);

}


VOS_VOID CNAS_XSD_ProcRedirectionRsltCnf_Abort( VOS_VOID )
{
    if (VOS_TRUE == CNAS_XSD_GetNdssIndFlag())
    {
        CNAS_XSD_SndXccNdssResultInd(XSD_XCC_NDSS_RESULT_FAILURE);

        CNAS_XSD_SetNdssIndFlag(VOS_FALSE);
    }


    /* �����д���״̬Ǩ�� */
}


VOS_VOID CNAS_XSD_ProcRedirectionRsltCnf_Norf( VOS_VOID )
{
    NAS_MSCC_PIF_1X_SYS_SRV_INFO_STRU   stSysSrvInfo;

    /* ��RRM����ע��,RF���ú�RRM��֪ͨXSD */
    CNAS_XSD_RegisterRrmResourceNtf(RRM_PS_TASK_TYPE_1X_NETWORK_SEARCH);

    /* ��MSCC����System srv Info��Ϣ */
    NAS_MEM_SET_S(&stSysSrvInfo, sizeof(stSysSrvInfo), 0x00, sizeof(NAS_MSCC_PIF_1X_SYS_SRV_INFO_STRU));
    stSysSrvInfo.enServiceStatus = NAS_MSCC_PIF_SERVICE_STATUS_NO_SERVICE;
    CNAS_XSD_SndMsccSysSrvInfoInd(&stSysSrvInfo);

    CNAS_XSD_SetCurSrvStatus(CNAS_XSD_SERVICE_STATUS_NO_SERVICE);

    if (VOS_TRUE == CNAS_XSD_GetNdssIndFlag())
    {
        CNAS_XSD_SndXccNdssResultInd(XSD_XCC_NDSS_RESULT_FAILURE);

        CNAS_XSD_SetNdssIndFlag(VOS_FALSE);
    }

    if (VOS_TRUE == CNAS_CCB_Get1XRfAvailFlg())
    {
       /* �ϱ�MSCC NO RF��Ϣ */
       CNAS_XSD_SndMsccRfAvailInd(VOS_FALSE);

       /* ��¼RF������ */
       CNAS_CCB_Set1XRfAvailFlg(VOS_FALSE);
    }

    /* �ϱ��޷��� */
    CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL);

    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    CNAS_XSD_SetSystemCampOnFlag(VOS_FALSE);

    /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_OOC */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_OOC);
}


VOS_VOID CNAS_XSD_ProcRedirectionRsltCnf_Fail( VOS_VOID )
{
    NAS_MSCC_PIF_1X_SYS_SRV_INFO_STRU   stSysSrvInfo;

    /* ��MSCC����System srv Info��Ϣ */
    NAS_MEM_SET_S(&stSysSrvInfo, sizeof(stSysSrvInfo), 0x00, sizeof(NAS_MSCC_PIF_1X_SYS_SRV_INFO_STRU));
    stSysSrvInfo.enServiceStatus = NAS_MSCC_PIF_SERVICE_STATUS_NO_SERVICE;
    CNAS_XSD_SndMsccSysSrvInfoInd(&stSysSrvInfo);

    CNAS_XSD_SetCurSrvStatus(CNAS_XSD_SERVICE_STATUS_NO_SERVICE);

    if (VOS_TRUE == CNAS_XSD_GetNdssIndFlag())
    {
        CNAS_XSD_SndXccNdssResultInd(XSD_XCC_NDSS_RESULT_FAILURE);

        CNAS_XSD_SetNdssIndFlag(VOS_FALSE);
    }

    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    /* �ϱ��޷��� */
    CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL);


    /* CallBackģʽ���յ��ض���DeterminInd */
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        /* ���ü����� */
        CNAS_XSD_SetEmcCallBackSrchCounter(0);

        /* ����CAlLBACKģʽ15s���� */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST, 0, VOS_NULL_PTR);

        /* 15s CALLBACK����������ʱ�� */
        CNAS_XSD_StartTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER, TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER_LEN);

        return;
    }

    /* ���������� */
    if (VOS_NULL_PTR != CNAS_XSD_GetEmcCallRedialScanChanListAddr()->pstScanChanInfo)
    {
        /* �������ز����� */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR);

        return;
    }

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_REDIRECTION_FAIL, 0, VOS_NULL_PTR);
}


VOS_UINT32 CNAS_XSD_RcvRedirectionRsltCnf_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_XSD_REDIRECTION_RSLT_CNF_STRU                 *pstRedirRslt = VOS_NULL_PTR;

    pstRedirRslt = (CNAS_XSD_XSD_REDIRECTION_RSLT_CNF_STRU *)pstMsg;

    /* �ͷ���Ƶ��Դ */
    CNAS_XSD_SndCasEndSessionNotify(CNAS_CAS_1X_SESSION_TYPE_NETWORK_SEARCH);

    /* ���ݽ�����д��� */
    if (CNAS_XSD_REDIRECTION_RESULT_SUCCESS == pstRedirRslt->enRslt)
    {
        CNAS_XSD_ProcRedirectionRsltCnf_Success();
    }
    else if (CNAS_XSD_REDIRECTION_RESULT_ABORTED == pstRedirRslt->enRslt)
    {
        CNAS_XSD_ProcRedirectionRsltCnf_Abort();
    }
    else if (CNAS_XSD_REDIRECTION_RESULT_NORF == pstRedirRslt->enRslt)
    {
        CNAS_XSD_ProcRedirectionRsltCnf_Norf();
    }
    else
    {
        CNAS_XSD_ProcRedirectionRsltCnf_Fail();
    }

    return VOS_TRUE;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithRedirectionReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAS_CAS_1X_REDIRECTION_INFO_STRU  *pstCasRedirInfo = VOS_NULL_PTR;

    pstCasRedirInfo     = &(pstSysDeterminInd->unSysDeterminInfo.stRedirectionInfo);

    /* �ο���˴�������:
       1.��ЧƵ�� Ƶ�������Ϊ0,return_if_fail=1,����CDG143��Ϊ����Ч�ض���,����������;CAS��������Ч�ض�����;
       2.��ЧƵ�� Ƶ�������Ϊ0,return_if_fail=0,����CDG143����ȫƵ�Ѳ�AVOID��ǰƵ��30S;
       3.��ЧƵ��,Ƶ�����Ϊ0,return_if_fail=0,����ͬ2;
       4.��ЧƵ��,Ƶ�����Ϊ0,return_if_fail=1,����ȫƵ����;
       5.��ЧƵ��,Ƶ�����Ϊ0,return_if_fail=0,����ͬ2;
       6.��ЧƵ��,Ƶ�����Ϊ0,return_if_fail=1,����ͬ4;
    */

    /* If the redirection message is invalid, clear the redirection flag */
    if (VOS_TRUE == pstCasRedirInfo->ucInvalidRedir)
    {
        /* clear the rediretion falg and stay on the current system */
        CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

        /* ��ҵ���ض���ʧ�ܴ��� */
        CNAS_XSD_SndXccNdssResultInd(XSD_XCC_NDSS_RESULT_FAILURE);

        return;
    }

    /* notify CREG to delete zone_lists and sid_nid_lists */
    CNAS_XSD_SndXregRedirInd();

    /* Ƶ�β�֧�ֻ�Ƶ�����Ϊ0,��Ϊ�쳣���,����ȫƵ����;��Ӧ����2/3/4/5/6���ֳ��� */
    if ((VOS_TRUE  == pstCasRedirInfo->ucInvalidBandClass)
     || (0         == pstCasRedirInfo->ucChannelNum))
    {
        /* return_if_fail is false,avoid the current system for 30s */
        if (VOS_FALSE == pstCasRedirInfo->ucReturnIfFail)
        {
            CNAS_XSD_ProcRequiredAvoidedFreqByAvoidReason(CNAS_XSD_AVOID_REDIRECTION_WITH_INVALID_GSRDM, (CNAS_PRL_FREQENCY_CHANNEL_STRU *)(&(pstSysDeterminInd->stCurChannelInfo)));
        }

        CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        /* ��ҵ���ض���ʧ�ܴ��� */
        CNAS_XSD_SndXccNdssResultInd(XSD_XCC_NDSS_RESULT_FAILURE);

        /* ������δ�յ�Succ����,���н��������� */
        if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_REDIRECTION_WITH_INVALID_GSRDM_EMC, 0, VOS_NULL_PTR, VOS_NULL_PTR))
        {
            return;
        }

        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_REDIRECTION_WITH_INVALID_GSRDM, 0, VOS_NULL_PTR);

        return;
    }

    CNAS_XSD_SetNdssIndFlag(VOS_TRUE);

    /* first clear the existed redirection info */
    CNAS_XSD_InitRedirectionInfo(CNAS_XSD_INIT_CTX_POWEROFF, CNAS_XSD_GetRedirectionInfoAddr());

    /* set the redirection flag to enabled */
    CNAS_XSD_SetRedirectionFlag(VOS_TRUE);

    CNAS_XSD_SetEmcRedirectionFlg();

    /* save the redirection global variables here */
    CNAS_XSD_SaveRedirectionInfo(pstCasRedirInfo);

    /* send the redirection internal message to load the redirection state machine */
    CNAS_XSD_SndInternalRedirInd(CNAS_XSD_GetRedirectionInfoAddr());

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    return;
}



VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithProtoMismatchReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    /* avoid the curret camped frequency fro 60s */
    CNAS_XSD_ProcRequiredAvoidedFreqByAvoidReason(CNAS_XSD_AVOID_P_REV_MISMATCH, (CNAS_PRL_FREQENCY_CHANNEL_STRU *)(&(pstSysDeterminInd->stCurChannelInfo)));

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* check the redirection flag */
    if (VOS_TRUE == CNAS_XSD_GetRedirectionFlag())
    {
        CNAS_XSD_SetEmcRedirectionFlg();

        /* send redirection internal message to load the redirection state machine */
        CNAS_XSD_SndInternalRedirInd(CNAS_XSD_GetRedirectionInfoAddr());

        return;
    }

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, VOS_NULL_PTR))
    {
        return;
    }

    /* send system acquire request internal message with new scene to load the system acquire state machine */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_PROTO_MISMATCH, 0, VOS_NULL_PTR);

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithReleaseReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      stFreq;

    NAS_MEM_SET_S(&stFreq, sizeof(stFreq), 0x00, sizeof(CNAS_PRL_FREQENCY_CHANNEL_STRU));

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* ��determin ind��Ϣ�����system id */
    stFreq.enBandClass = pstSysDeterminInd->stCurChannelInfo.usBandClass;
    stFreq.usChannel   = pstSysDeterminInd->stCurChannelInfo.usChannel;

    /* CALLBACK�ж��� */
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        /* ����CAlLBACKģʽ15s���� */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALL_RELEASED, 1, &stFreq);

        return;
    }

    /* ������CallRelease��ǰ��֧��CallBackģʽ�Ĵ���,��֧��CallBack�����н�����CallRelease���� */
    if (VOS_TRUE == CNAS_XSD_IsEmcCallOriginalSysExist())
    {
        if (VOS_TRUE == CNAS_XSD_GetCallBackCfg()->ulCallBackEnableFlg)
        {
            CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

            CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALL_RELEASED, 1, &stFreq);

            return;
        }

        CNAS_XSD_ClearEmcCallOriginalSys();

        /* ��֧��CallBack�ҵ�ǰפ��������Чϵͳ������Lost���� */
        if (VOS_FALSE == CNAS_XSD_IsCurSystemValid())
        {
            CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

            CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

            return;
        }
    }

    if (VOS_TRUE == CNAS_XSD_GetRedirectionFlag())
    {
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_CALL_RELEASED_WITH_REDIR, 1, &stFreq);
    }
    else
    {
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_CALL_RELEASED, 1, &stFreq);
    }

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithSysLostReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    CNAS_XSD_SetSystemCampOnFlag(VOS_FALSE);

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, pstSysDeterminInd))
    {
        return;
    }

    CNAS_XSD_SetOocSearchScene(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST);
    CNAS_XSD_ProcOocSchedule(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST);

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithAcquisitionFail_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    /*����ͬsystem lost*/
    CNAS_XSD_ProcCasSystemDetermineIndWithSysLostReason_Idle(pstSysDeterminInd);

    return;
}



VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithRescanReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_RESCAN, 0, VOS_NULL_PTR);

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithLockReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    /* According to discussing with CAS,we need not do system acquire flow.
       And CAS will control the access process with special fail reason */
    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);
    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithUnlockReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithAccessDeniedReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    /* avoid the curret camped frequency fro 60s */
    CNAS_XSD_ProcRequiredAvoidedFreqByAvoidReason(CNAS_XSD_AVOID_ACCESS_DENIED, (CNAS_PRL_FREQENCY_CHANNEL_STRU *)(&(pstSysDeterminInd->stCurChannelInfo)));

    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_ACCESS_DENINED, 0, VOS_NULL_PTR);

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithAcctBlockReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    /* avoid the curret camped frequency fro 60s */
    CNAS_XSD_ProcRequiredAvoidedFreqByAvoidReason(CNAS_XSD_AVOID_ACCT_BLOCKED_IND, (CNAS_PRL_FREQENCY_CHANNEL_STRU *)(&(pstSysDeterminInd->stCurChannelInfo)));

    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_ACCT_BLOCKED, 0, VOS_NULL_PTR);

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithNdssOffReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{

    /* ����NDSS offָʾֵ */
    CNAS_XSD_SetNdssIndFlag(VOS_TRUE);

    /* ���redirection��־ */
    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* �����ڲ����� */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_NDSS_OFF, 0, VOS_NULL_PTR);

    return;
}



VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithErrReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    /* avoid the curret camped frequency fro 60s */
     CNAS_XSD_ProcRequiredAvoidedFreqByAvoidReason(CNAS_XSD_AVOID_ERROR_IND, (CNAS_PRL_FREQENCY_CHANNEL_STRU *)(&(pstSysDeterminInd->stCurChannelInfo)));

     CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_ERR, 0, VOS_NULL_PTR);

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithRegistRejReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAS_PRL_1X_SYSTEM_STRU            *pstRedirOrigSys = VOS_NULL_PTR;

    pstRedirOrigSys = CNAS_XSD_GetRedirectionOrignalSys();

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    if (VOS_TRUE == CNAS_XSD_GetRedirectionFlag())
    {
        CNAS_XSD_SetRedirectionFlag(VOS_FALSE);
        CNAS_CCB_Set1xReturnCause(CNAS_CCB_1X_RETURN_CAUSE_REG_REJ);

        if (VOS_TRUE == CNAS_XSD_GetRedirectionReturnIfFailFlag())
        {
            /* ��������������Any���� */
            if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_REGIS_REJ_WITH_REDIR_AND_RTF_EMC, 1, &pstRedirOrigSys->stFreq, VOS_NULL_PTR))
            {
                return;
            }

            CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_REGIS_REJ_WITH_REDIR_AND_RTF, 1, &pstRedirOrigSys->stFreq);

            return;
        }

        CNAS_XSD_ProcRequiredAvoidedFreqByAvoidReason(CNAS_XSD_AVOID_REDIRECTION_FAIL, &(pstRedirOrigSys->stFreq));
    }

    CNAS_XSD_ProcRequiredAvoidedFreqByAvoidReason(CNAS_XSD_AVOID_REG_REJECTED, (CNAS_PRL_FREQENCY_CHANNEL_STRU *)(&(pstSysDeterminInd->stCurChannelInfo)));

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_REGIS_REJ, 0, VOS_NULL_PTR);
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithReselectionReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    /* XSD need not do the system acquire flow for CAS will handle the reselection process */
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithSystemReselectionReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAX_XSD_CAMPED_SYS_INFO_STRU                          *pstCurCampedSys = VOS_NULL_PTR;
    CNAS_CAS_1X_SYS_RESELECTION_INFO_STRU                  *pstSysReselInfo = VOS_NULL_PTR;
    CNAS_PRL_MATCHED_GEO_LIST_INFO_STRU                     stGeoListInfo;
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          astReselChanList[CNAS_CAS_1X_MAX_SYS_RESELECTION_CHAN_NUM];
    VOS_UINT16                                              i;

    NAS_MEM_SET_S(&stGeoListInfo, sizeof(stGeoListInfo), 0x0, sizeof(CNAS_PRL_MATCHED_GEO_LIST_INFO_STRU));
    NAS_MEM_SET_S(astReselChanList, sizeof(astReselChanList), 0x0, sizeof(CNAS_PRL_FREQENCY_CHANNEL_STRU) * CNAS_CAS_1X_MAX_SYS_RESELECTION_CHAN_NUM);

    pstCurCampedSys = CNAS_XSD_GetCurCampedSysInfo();
    pstSysReselInfo = &pstSysDeterminInd->unSysDeterminInfo.stSysReselInfo;



    for (i = 0; i < pstSysReselInfo->usChannelNum; i++)
    {
        astReselChanList[i].enBandClass = pstSysReselInfo->astChannelList[i].usBandClass;
        astReselChanList[i].usChannel   = pstSysReselInfo->astChannelList[i].usChannel;
    }

    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);


    CNAS_PRL_Get1xSysGeoList(&pstCurCampedSys->stSystem, &stGeoListInfo);

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* check the current system is a prefered PRL system or not */
    if (0 != stGeoListInfo.ucGeoNum)
    {
        if (VOS_FALSE == CNAS_XSD_IsCurSysNotSuitableSys(&pstCurCampedSys->stSystem, VOS_FALSE, 0))
        {
            if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_SYS_RESEL_IN_PRL_AND_PREF_EMC, pstSysReselInfo->usChannelNum, astReselChanList, VOS_NULL_PTR))
            {
                return;
            }

            CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYS_RESEL_IN_PRL_AND_PREF, pstSysReselInfo->usChannelNum, astReselChanList);

            return;
        }
    }

    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_SYS_RESEL_NOT_IN_PRL_OR_NEG_EMC, pstSysReselInfo->usChannelNum, astReselChanList, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYS_RESEL_NOT_IN_PRL_OR_NEG, pstSysReselInfo->usChannelNum, astReselChanList);

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithNewSystemReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAS_CAS_1X_NEW_SYSTEM_INFO_STRU                           *pstNewSysInfo;

    pstNewSysInfo = &pstSysDeterminInd->unSysDeterminInfo.stNewSysInfo;

    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    if (PS_TRUE == pstNewSysInfo->enIsRelease)
    {
        /* �������������յ�SD�Ĵ��� */
        if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST_SKIP_MRU0_EMC, 0, VOS_NULL_PTR, VOS_NULL_PTR))
        {
            return;
        }

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST_SKIP_MRU0, 0, VOS_NULL_PTR);
    }
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithMaxAccessFailureReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAX_XSD_CAMPED_SYS_INFO_STRU      *pstCurCampedSys = VOS_NULL_PTR;
    CNAS_PRL_MATCHED_GEO_LIST_INFO_STRU stGeoListInfo;
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astCurMapChannel[1];

    astCurMapChannel[0].enBandClass = pstSysDeterminInd->stCurChannelInfo.usBandClass;
    astCurMapChannel[0].usChannel   = pstSysDeterminInd->stCurChannelInfo.usChannel;

    NAS_MEM_SET_S(&stGeoListInfo, sizeof(stGeoListInfo), 0x0, sizeof(CNAS_PRL_MATCHED_GEO_LIST_INFO_STRU));

    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    pstCurCampedSys = CNAS_XSD_GetCurCampedSysInfo();

    /* ����CDG��1x�������볢��ʧ�ܵĴ������ڽ���Ƶ����Ҫavoid,����ʱ��δȷ��������
       ��һ������ֵ,�˴��ο���˽����Ż�,����Ƶ����ڹ����б�������,�ȴ�����Ƶ�㶼����
       ��ɺ��ٳ��Ըý���ʧ�ܵ�Ƶ�� */


    CNAS_PRL_Get1xSysGeoList(&pstCurCampedSys->stSystem, &stGeoListInfo);

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* check the current system is a PRL system or not */
    if (0 != stGeoListInfo.ucGeoNum)
    {
        /* �������������յ�SD�Ĵ��� */
        if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_MAX_ACCESS_FAILURE_IN_PRL_EMC, 1, astCurMapChannel, VOS_NULL_PTR))
        {
            return;
        }

        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_MAX_ACCESS_FAILURE_IN_PRL, 1, astCurMapChannel);

        return;
    }

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_MAX_ACCESS_FAILURE_NOT_IN_PRL_EMC, 1, astCurMapChannel, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_MAX_ACCESS_FAILURE_NOT_IN_PRL, 1, astCurMapChannel);

    return;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithAbnormalReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    /* �������������յ�SD�Ĵ��� */
    if (VOS_TRUE == CNAS_XSD_ProcEmcSysAcqFlow_Idle(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR, VOS_NULL_PTR))
    {
        return;
    }

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_ABNORMAL, 0, VOS_NULL_PTR);

    return;
}




VOS_UINT32 CNAS_XSD_RcvTiAvailMru0TimerExpired_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* ����MRU0 */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_CALL_MRU0, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvPowerOffReq_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣ����OOC�¶�ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_TIMER);

    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_MRU0_TIMER);

    /* ���ohmƵ����Ϣ */
    CNAS_CCB_ClearCdmaOhmFreq();

    /* ����L2�ػ�״̬�� */
    CNAS_XSD_InitFsmL2(CNAS_XSD_FSM_POWER_OFF);

    return VOS_TRUE;
}



VOS_VOID CNAS_XSD_UpdateOocSearchPhaseAndCurrentTimes(
    CNAS_XSD_OOC_SCHEDULE_INFO_STRU    *pstOocScheduleInfo
)
{
    VOS_UINT8                           ucCurrentPhase;
    VOS_UINT16                          usOocSchPhaseTimes;

    /* ���ݵ�ǰOOC�׶λ�ȡ�´�����OOC����ʱ��, ע: ��ǰ�׶��Ǵ�0��ʼ�� */
    ucCurrentPhase    = pstOocScheduleInfo->stOocCtxInfo.ucCurrentPhase;
    usOocSchPhaseTimes  = pstOocScheduleInfo->stConfigInfo.astOocTimerInfo[ucCurrentPhase].usTimes;

    /* ˢ�µ�ǰ�����Ľ׶κʹ��� */
    if (pstOocScheduleInfo->stConfigInfo.ucPhaseNum == ucCurrentPhase + 1)
    {
        /* ��¼��ǰ����������δ����ӡ�鿴 */
        pstOocScheduleInfo->stOocCtxInfo.usCurrentTimes++;
    }
    else
    {
        /* ������ĩλ�Ľ׶Σ�������ۼӣ������ǰ�Ĵ����Ѿ����˸ý׶ε�����������׶����ӣ�
             ���Ѵ�����0 */
        pstOocScheduleInfo->stOocCtxInfo.usCurrentTimes++;
        if (pstOocScheduleInfo->stOocCtxInfo.usCurrentTimes >= usOocSchPhaseTimes)
        {
            pstOocScheduleInfo->stOocCtxInfo.ucCurrentPhase++;
            pstOocScheduleInfo->stOocCtxInfo.usCurrentTimes = 0;
        }
    }
}


VOS_UINT16 CNAS_XSD_GetNextOocScheduleTimerLen(
    CNAS_XSD_OOC_SCHEDULE_INFO_STRU    *pstOocScheduleInfo
)
{
    VOS_UINT16                          usNextTimerLen;
    VOS_UINT8                           ucOocSchPhaseNum;

    usNextTimerLen = 0;

    /* ���ݵ�ǰOOC�׶λ�ȡ�´�����OOC����ʱ�� */
    ucOocSchPhaseNum    = pstOocScheduleInfo->stOocCtxInfo.ucCurrentPhase;
    usNextTimerLen      = pstOocScheduleInfo->stConfigInfo.astOocTimerInfo[ucOocSchPhaseNum].usTimerLen;

    /* Cdg143 5.11.1b (DO Traffic Operation):
       No more than one acquisition attempt every 5 sec for the first four acquisition attempts
       No more than one acquisition attempt every 10 sec thereafter
       ��ǰʹ��NV����
     */
    /* HRPD ����ҵ���Ƿ������� */
    if (VOS_TRUE == pstOocScheduleInfo->stOocCtxInfo.ucDoTrafficFlag)
    {
        /* check ��DO traffic�£�ϵͳ�����Ƿ��Ѿ��ﵽ�Ĵ� */
        if (VOS_TRUE == pstOocScheduleInfo->stOocCtxInfo.ucFirstFourAcqArrivedFlag)
        {
            /* ����´�����OOCʱ��С��10s��ʹ��10sʱ��, ����ʹ�õ�ǰʱ�� */
            if  (usNextTimerLen < pstOocScheduleInfo->stConfigInfo.uc1xOocDoTchPhase2TimerLen)
            {
                usNextTimerLen = pstOocScheduleInfo->stConfigInfo.uc1xOocDoTchPhase2TimerLen;
            }
        }
        else
        {
            /* ����´�����OOCʱ��С��5s��ʹ��5sʱ��, ����ʹ�õ�ǰʱ�� */
            if  (usNextTimerLen < pstOocScheduleInfo->stConfigInfo.uc1xOocDoTchPhase1TimerLen)
            {
                usNextTimerLen = pstOocScheduleInfo->stConfigInfo.uc1xOocDoTchPhase1TimerLen;
            }

            /* ����ʹ�õ�ǰ��ʱ�� */
            /* ˢ��DO traffic��1Xϵͳ�������� */
            pstOocScheduleInfo->stOocCtxInfo.ucAttemptTimesInDoTraffic++;

            /* ���ϵͳ���������ﵽ4�Σ�����ucFirstFourAcqArrivedFlag����ΪTRUE, �����������Ϊ0 */
            if ((CNAS_XSD_ATTEMP_IN_DO_TRAFFIC_4_TIMES == pstOocScheduleInfo->stOocCtxInfo.ucAttemptTimesInDoTraffic)
             && (VOS_FALSE                             == pstOocScheduleInfo->stOocCtxInfo.ucFirstFourAcqArrivedFlag))
            {
                pstOocScheduleInfo->stOocCtxInfo.ucFirstFourAcqArrivedFlag = VOS_TRUE;
            }
        }
    }

    return usNextTimerLen;
}


VOS_VOID CNAS_XSD_ProcOocSchedule(
    CNAS_XSD_SYS_ACQ_SCENE_ENUM_UINT32  enAcqScene
)
{
    CNAS_XSD_OOC_SCHEDULE_INFO_STRU    *pstOocScheduleInfo = VOS_NULL_PTR;
    VOS_UINT16                          usNextAcqTimerLen;
    VOS_UINT8                           ucMru0SearchTimerLen;
    VOS_UINT32                          ulRemainTimerLen;
    CNAS_XSD_SYS_ACQ_SCENE_ENUM_UINT32  enInSysAcqScene;

    pstOocScheduleInfo = CNAS_XSD_GetOocScheduleInfo();

    ucMru0SearchTimerLen = pstOocScheduleInfo->stConfigInfo.ucMru0SearchTimerLen;

    if (CNAS_XSD_SYS_ACQ_SCENE_NO_RF == enAcqScene)
    {
        /* ��RRM����ע��,RF���ú�RRM��֪ͨXSD */
        CNAS_XSD_RegisterRrmResourceNtf(RRM_PS_TASK_TYPE_1X_NETWORK_SEARCH);

        if (VOS_TRUE == CNAS_CCB_Get1XRfAvailFlg())
        {
            /* �ϱ�MSCC NO RF��Ϣ */
            CNAS_XSD_SndMsccRfAvailInd(VOS_FALSE);

            /* ���õ�ǰRF���ñ��Ϊ:RF������ */
            CNAS_CCB_Set1XRfAvailFlg(VOS_FALSE);
        }

        if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
        {
            /* 15s CALLBACK����������ʱ�� */
            if (CNAS_TIMER_STATUS_RUNNING != CNAS_GetSpecifiedTimerStatus(UEPS_PID_XSD, TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER, 0))
            {
                CNAS_XSD_StartTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER, TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER_LEN);
            }
        }

        return;
    }

    /* �еȴ�OOC������ */
    if (VOS_TRUE == CNAS_XSD_GetOocWaitSearchFlag())
    {
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        CNAS_XSD_SndInternalSysAcqReq(pstOocScheduleInfo->stOocCtxInfo.enOocSearchScene, 0, VOS_NULL_PTR);

        /* ������ */
        CNAS_XSD_SetOocWaitSearchFlag(VOS_FALSE);

        return;
    }

    /* ����MRU0ʧ�ܣ��ٴ�check MRU0��ʱ���Ƿ�������� */
    if (CNAS_XSD_SYS_ACQ_SCENE_CALL_MRU0 == enAcqScene)
    {
        /* ��ȡavailableʣ��ʱ�� */
        ulRemainTimerLen = CNAS_GetSpecifiedTimerRemainLen(UEPS_PID_XSD, TI_CNAS_XSD_AVAILABLE_TIMER, 0);

        /* ���ʣ��ʱ����Ȼ����2��MRU0ʱ�������Գ���MRU0, ����MRU0��ʱ�� */
        if (((VOS_UINT32)ucMru0SearchTimerLen * 2 * 1000) < ulRemainTimerLen)
        {
            CNAS_XSD_StartTimer(TI_CNAS_XSD_AVAILABLE_MRU0_TIMER, (ucMru0SearchTimerLen * 1000));
        }

        return;
    }

    /* LOG OOC������Ϣ */
    CNAS_XSD_LogOocCtxInfo(&(pstOocScheduleInfo->stOocCtxInfo));

    usNextAcqTimerLen = CNAS_XSD_GetNextOocScheduleTimerLen(pstOocScheduleInfo);

    /* ���OOC����ʱ��Ϊ0�������������� */
    if (0 == usNextAcqTimerLen)
    {
        /* ����L1 main״̬Ϊinitial״̬ */
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        if (CNAS_XSD_SYS_ACQ_SCENE_CALL_RELEASED == enAcqScene)
        {
            enInSysAcqScene = CNAS_XSD_SYS_ACQ_SCENE_CALL_RELEASE_SYSTEM_LOST;
        }
        else
        {
            enInSysAcqScene = pstOocScheduleInfo->stOocCtxInfo.enOocSearchScene;
        }

        /* ����OOC������ϵͳ���� */
        CNAS_XSD_SndInternalSysAcqReq(enInSysAcqScene, 0, VOS_NULL_PTR);
    }
    else
    {
        /* ����L1 main״̬ΪOOC״̬ */
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_OOC);

        /* ����OOC available��ʱ�� */
        CNAS_XSD_StartTimer(TI_CNAS_XSD_AVAILABLE_TIMER, (usNextAcqTimerLen * 1000));

        /* �����OOC��������������Ƿ���Ҫ����MRU0���� */
        if (CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST == pstOocScheduleInfo->stOocCtxInfo.enOocSearchScene)
        {
            /* 2��MRU0����ʱ���Ƿ�С�ڵ�ǰOOC sleepʱ��������ǣ�����MRU0 available��ʱ�� */
            if ((VOS_UINT16)ucMru0SearchTimerLen*2 < usNextAcqTimerLen)
            {
                CNAS_XSD_StartTimer(TI_CNAS_XSD_AVAILABLE_MRU0_TIMER, (ucMru0SearchTimerLen * 1000));
            }
        }
    }

    /* ����OOC�����׶μ���ǰ�׶��������� */
    CNAS_XSD_UpdateOocSearchPhaseAndCurrentTimes(pstOocScheduleInfo);

    /* LOG OOC���º�ĵ�����Ϣ */
    CNAS_XSD_LogOocCtxInfo(&(pstOocScheduleInfo->stOocCtxInfo));

    return;
}


VOS_VOID CNAS_XSD_SetOocSearchScene(
    CNAS_XSD_SYS_ACQ_SCENE_ENUM_UINT32  enAcqScene
)
{
    CNAS_XSD_OOC_SCHEDULE_INFO_STRU    *pstOocScheduleInfo = VOS_NULL_PTR;

    pstOocScheduleInfo = CNAS_XSD_GetOocScheduleInfo();

    if (VOS_FALSE == pstOocScheduleInfo->stOocCtxInfo.ucSceneSetFlag)
    {
        if (CNAS_XSD_SYS_ACQ_SCENE_SWITCH_ON != enAcqScene)
        {
            /* ������ǿ�������ʧ�ܣ���ͳһ����ΪOOC���� */
            pstOocScheduleInfo->stOocCtxInfo.enOocSearchScene = CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST;
        }
        else
        {
            pstOocScheduleInfo->stOocCtxInfo.enOocSearchScene = enAcqScene;
        }

        pstOocScheduleInfo->stOocCtxInfo.ucSceneSetFlag       = VOS_TRUE;
    }
}


CNAS_XSD_SYS_ACQ_SCENE_ENUM_UINT32 CNAS_XSD_GetOocSearchScene(VOS_VOID)
{
    CNAS_XSD_OOC_SCHEDULE_INFO_STRU    *pstOocScheduleInfo = VOS_NULL_PTR;

    pstOocScheduleInfo = CNAS_XSD_GetOocScheduleInfo();

    return (pstOocScheduleInfo->stOocCtxInfo.enOocSearchScene);
}


VOS_VOID CNAS_XSD_ProcNormalCallRedialSysAcqFailScene()
{
    /*�������������ز���ѭ���������� */
    VOS_UINT8                           ucCallExistFlg;

    ucCallExistFlg = CNAS_XSD_GetCallExistFlg();

    if (0 != (ucCallExistFlg & CNAS_XSD_NORMAL_VOICE_CALL_EXIST_FLAG))
    {
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_NORMAL_CALL_REDIAL, 0, VOS_NULL_PTR);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    }
    else
    {
         CNAS_XSD_SetOocSearchScene(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST);
         CNAS_XSD_ProcOocSchedule(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST);
    }

    return;
}


VOS_VOID CNAS_XSD_ProcEmergencyCallSysAcqFailScene()
{
    /* �������ز�����ʧ������ѭ������ */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR);

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    return;

}


VOS_VOID CNAS_XSD_ProcEmcCallReleaseSysAcqFailScene()
{
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        /* ���ü����� */
        CNAS_XSD_SetEmcCallBackSrchCounter(0);

        /* ������һ��CallBack���� */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST, 0, VOS_NULL_PTR);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        CNAS_XSD_StartTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER, TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER_LEN);

    }

    return;

}


VOS_VOID CNAS_XSD_ProcEmcCallBackSystemLostSysAcqFailScene()
{
    VOS_UINT8                                       ucCallBackSrchCounter;
    CNAS_XSD_EMC_CALLBACK_CANDIDATE_FREQ_INFO_STRU *pstEmcCallBackCfInfo = VOS_NULL_PTR;

    pstEmcCallBackCfInfo    = CNAS_XSD_GetEmcCallBackCandidateFreqInfo();

    ucCallBackSrchCounter = CNAS_XSD_GetEmcCallBackSrchCounter();

    ucCallBackSrchCounter++;

    /* ������һ��CallBack���� */
    if (ucCallBackSrchCounter < CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_MAX_TRY_TIMES)
    {
        CNAS_XSD_SetEmcCallBackSrchCounter(ucCallBackSrchCounter);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        /* �����������Ƶ����Ƶ�㣬�������Ƶ����Ƶ����Ϣ */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST,
                                      pstEmcCallBackCfInfo->usCfChannelNum,
                                      pstEmcCallBackCfInfo->astChannelList);

        return;
    }

    /* ����������ʱ�� */
    /* ���ü����� */
    CNAS_XSD_SetEmcCallBackSrchCounter(0);

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* �ﵽ10�Σ���ʱ������ */
    CNAS_XSD_StartTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER, TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER_LEN);

    return;

}


VOS_VOID CNAS_XSD_ProcCallReleaseWithRedirSysAcqFailScene(
    CNAS_XSD_XSD_SYSTEM_ACQUIRED_RSLT_CNF_STRU     *pstSysAcqRslt
)
{
    if (VOS_TRUE == pstSysAcqRslt->ucImmediateSysAcq)
    {
        CNAS_XSD_SetEmcRedirectionFlg();

        CNAS_XSD_SndInternalRedirInd(CNAS_XSD_GetRedirectionInfoAddr());

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    }

    return;
}




VOS_VOID CNAS_XSD_ProcSearchNetworkRslt_Fail(
    CNAS_XSD_XSD_SYSTEM_ACQUIRED_RSLT_CNF_STRU     *pstSysAcqRslt
)
{
    NAS_MSCC_PIF_1X_SYS_SRV_INFO_STRU               stSysSrvInfo;


    /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_OOC */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_OOC);

    /* ��MSCC����System srv Info��Ϣ */
    NAS_MEM_SET_S(&stSysSrvInfo, sizeof(stSysSrvInfo), 0x0, sizeof(NAS_MSCC_PIF_1X_SYS_SRV_INFO_STRU));
    stSysSrvInfo.enServiceStatus = NAS_MSCC_PIF_SERVICE_STATUS_NO_SERVICE;
    CNAS_XSD_SndMsccSysSrvInfoInd(&stSysSrvInfo);

    if (VOS_TRUE == CNAS_XSD_GetSrvAcqFlg())
    {
        CNAS_XSD_SndMsccSrvAcqCnf(NAS_MSCC_PIF_SRV_ACQ_RESULT_FAIL);

        CNAS_XSD_SetSrvAcqFlg(VOS_FALSE);
    }

    if (VOS_TRUE == CNAS_XSD_GetNdssIndFlag())
    {
        CNAS_XSD_SetNdssIndFlag(VOS_FALSE);

        CNAS_XSD_SndXccNdssResultInd(XSD_XCC_NDSS_RESULT_FAILURE);
    }

    CNAS_XSD_SetCurSrvStatus(CNAS_XSD_SERVICE_STATUS_NO_SERVICE);

    if (CNAS_XSD_SYS_ACQ_SCENE_SWITCH_ON == pstSysAcqRslt->enCurAcqScene)
    {
        CNAS_XSD_SndMsccSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL);
    }

    /* �ϱ��޷��� */
    CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL);

    CNAS_XSD_SetSystemCampOnFlag(VOS_FALSE);

    /* normaly we have to clear the redirection flag while enter system lost expect trigger
       redirection flow at once */
    if (CNAS_XSD_SYS_ACQ_SCENE_CALL_RELEASED_WITH_REDIR != pstSysAcqRslt->enCurAcqScene)
    {
        CNAS_XSD_SetRedirectionFlag(VOS_FALSE);
    }


    switch(pstSysAcqRslt->enCurAcqScene)
    {
        case CNAS_XSD_SYS_ACQ_SCENE_NORMAL_CALL_REDIAL:
             CNAS_XSD_ProcNormalCallRedialSysAcqFailScene();
             break;

        case CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL:
             CNAS_XSD_ProcEmergencyCallSysAcqFailScene();
             break;

        case CNAS_XSD_SYS_ACQ_SCENE_EMC_CALL_RELEASED:
             CNAS_XSD_ProcEmcCallReleaseSysAcqFailScene();
             break;

        case CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST:
             CNAS_XSD_ProcEmcCallBackSystemLostSysAcqFailScene();
             break;

        case CNAS_XSD_SYS_ACQ_SCENE_CALL_RELEASED_WITH_REDIR:
             CNAS_XSD_ProcCallReleaseWithRedirSysAcqFailScene(pstSysAcqRslt);
             break;

        default:

            if (VOS_TRUE == pstSysAcqRslt->ucImmediateSysAcq)
            {
                CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

                /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_INITIAL */
                CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);
            }
            else
            {
                CNAS_XSD_SetOocSearchScene(pstSysAcqRslt->enCurAcqScene);
                CNAS_XSD_ProcOocSchedule(pstSysAcqRslt->enCurAcqScene);
            }

    }


}


VOS_VOID CNAS_XSD_ProcSearchNetworkRslt_Abort(
    CNAS_XSD_XSD_SYSTEM_ACQUIRED_RSLT_CNF_STRU     *pstSysAcqRslt
)
{
    if (VOS_TRUE == CNAS_XSD_GetSrvAcqFlg())
    {
        CNAS_XSD_SndMsccSrvAcqCnf(NAS_MSCC_PIF_SRV_ACQ_RESULT_FAIL);

        CNAS_XSD_SetSrvAcqFlg(VOS_FALSE);
    }

    if (CNAS_XSD_SYS_ACQ_SCENE_SWITCH_ON == pstSysAcqRslt->enCurAcqScene)
    {
        CNAS_XSD_SndMsccSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED);
    }


    if (VOS_TRUE == CNAS_XSD_GetNdssIndFlag())
    {
        CNAS_XSD_SetNdssIndFlag(VOS_FALSE);

        CNAS_XSD_SndXccNdssResultInd(XSD_XCC_NDSS_RESULT_FAILURE);
    }

    if (CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST == pstSysAcqRslt->enCurAcqScene)
    {
        /* ֹͣ15s CALLBACK����������ʱ�� */
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER);

        /* �����Ƶ����Ƶ����Ϣ */
        CNAS_XSD_ClearEmcCallBackCandidateFreq();
    }


    /* ֹͣ����OOC�¶�ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_TIMER);

    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_MRU0_TIMER);

    return;
}


VOS_VOID CNAS_XSD_ProcSearchNetworkRslt_Succ(
    CNAS_XSD_XSD_SYSTEM_ACQUIRED_RSLT_CNF_STRU     *pstSysAcqRslt
)
{
    /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_IDLE */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_IDLE);

    /* �����ǰ����ҵ�񴥷�����״̬���ظ���� */
    if (VOS_TRUE == CNAS_XSD_GetSrvAcqFlg())
    {
        if ((VOS_FALSE == CNAS_XSD_IsCurSystemValid())
         && (CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL != pstSysAcqRslt->enCurAcqScene))
        {
            /* ��ǰ�����������Чϵͳ������ҵ�񴥷����� */
            CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

            CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

            return;
        }

        CNAS_XSD_SndMsccSrvAcqCnf(NAS_MSCC_PIF_SRV_ACQ_RESULT_SUCCESS);

        CNAS_XSD_SetSrvAcqFlg(VOS_FALSE);
    }

    if (CNAS_XSD_SYS_ACQ_SCENE_SWITCH_ON == pstSysAcqRslt->enCurAcqScene)
    {
        CNAS_XSD_SndMsccSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);
    }

    /* �����ɹ���ֹͣ15s CALLBACK����������ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER);

    CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);

    if (VOS_TRUE == CNAS_XSD_GetNdssIndFlag())
    {
        CNAS_XSD_SetNdssIndFlag(VOS_FALSE);

        CNAS_XSD_SndXccNdssResultInd(XSD_XCC_NDSS_RESULT_SUCCESS);
    }

    /* ��ʼ��OOC��������Ϣ */
    CNAS_XSD_InitOocCtxInfo();

    /* ֹͣ����OOC�¶�ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_TIMER);

    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_MRU0_TIMER);

    if (CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST == pstSysAcqRslt->enCurAcqScene)
    {
        /* �����Ƶ����Ƶ����Ϣ */
        CNAS_XSD_ClearEmcCallBackCandidateFreq();
    }

    return;
}



VOS_UINT32 CNAS_XSD_RcvMsccSysAcqReq_SysAcq_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* ���ohmƵ����Ϣ */
    CNAS_CCB_ClearCdmaOhmFreq();

    /* �յ�MSCC��������ǰ�յ�������Start���������������������������������� */
    if (VOS_NULL_PTR != CNAS_XSD_GetEmcCallRedialScanChanListAddr()->pstScanChanInfo)
    {
        /* �����������ز����� */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR);

        return VOS_TRUE;
    }

    /* ������Ƶ��Դ */
    CNAS_XSD_SndCasBeginSessionNotify(CNAS_CAS_1X_SESSION_TYPE_NETWORK_SEARCH);

    /* Init the System acquire FSM */
    CNAS_XSD_InitFsmL2(CNAS_XSD_FSM_SYS_ACQ);

    return VOS_TRUE;
}




VOS_UINT32 CNAS_XSD_RcvMsccPowerSaveReq_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* XSD Sends power save CNf to MSCC */
    CNAS_XSD_SndMsccPowerSaveCnf(NAS_MSCC_PIF_POWER_SAVE_SUCCESS);

    return VOS_TRUE;
}




VOS_UINT32 CNAS_XSD_RcvMsccPowerSaveReq_L1Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ���ohmƵ����Ϣ */
    CNAS_CCB_ClearCdmaOhmFreq();

    /* �˳�CallBack״̬ */
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        /* ֹͣCALLBACK������ʱ�� */
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_MODE_PROTECT_TIMER);
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER);
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER);

        CNAS_XSD_QuitCallBackMode();
    }

    /* Init the Power Off FSM */
    CNAS_XSD_InitFsmL2(CNAS_XSD_FSM_POWER_OFF);

    return VOS_TRUE;

}


VOS_UINT32 CNAS_XSD_RcvRrmStatusInd_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �ж�RF���ñ�� */
    if (VOS_FALSE == CNAS_CCB_Get1XRfAvailFlg())
    {
        /* ��ǰ����NO RF ��MSCC͸����Ϣ,���������� */
        CNAS_XSD_SndMsccRfAvailInd(VOS_TRUE);

        /* ���õ�ǰRF���ñ��Ϊ:RF���� */
        CNAS_CCB_Set1XRfAvailFlg(VOS_TRUE);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvRrmStatusInd_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �����ǰ����NO RF״̬���򴥷����� */
    if (VOS_FALSE == CNAS_CCB_Get1XRfAvailFlg())
    {
        /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_INITIAL */
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
        {
            /* ���ü����� */
            CNAS_XSD_SetEmcCallBackSrchCounter(0);

            /*���������ڲ�������Ϣ */
            CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST, 0, VOS_NULL_PTR);

            return VOS_TRUE;
        }

        /*���������ڲ�������Ϣ */
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvMsccSrvAcqReq_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ҵ�񴥷����� */
    /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_INITIAL */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /*���������ڲ�������Ϣ */
    if (NAS_MSCC_PIF_SRV_TYPE_CS_MO_EMERGENCY_CALL == ((MSCC_XSD_SRV_ACQ_REQ_STRU *)pstMsg)->enSrvType)
    {
        /* ���ҵ�������ǽ����������������������б� */
        CNAS_XSD_BuildEmcCallRedialScanChanList();

        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR);
    }
    else
    {
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvMsccSrvAcqReq_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ҵ�񴥷����� */
    /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_INITIAL */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /*���������ڲ�������Ϣ */
    if (NAS_MSCC_PIF_SRV_TYPE_CS_MO_EMERGENCY_CALL == ((MSCC_XSD_SRV_ACQ_REQ_STRU *)pstMsg)->enSrvType)
    {
        /* ���ҵ�������ǽ����������������������б� */
        CNAS_XSD_BuildEmcCallRedialScanChanList();

        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR);
    }
    else
    {
        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);
    }

    return VOS_TRUE;
}


VOS_VOID CNAS_XSD_ProcCasSystemDetermineIndWithNoRfReason_Idle(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU                   *pstSysDeterminInd
)
{
    /* ��RRM����ע��,RF���ú�RRM��֪ͨXSD */
    CNAS_XSD_RegisterRrmResourceNtf(RRM_PS_TASK_TYPE_1X_NETWORK_SEARCH);

    if (VOS_TRUE == CNAS_CCB_Get1XRfAvailFlg())
    {
        /* �ϱ�MSCC NO RF��Ϣ */
        CNAS_XSD_SndMsccRfAvailInd(VOS_FALSE);

        /* ���õ�ǰRF���ñ��Ϊ:RF������ */
        CNAS_CCB_Set1XRfAvailFlg(VOS_FALSE);
    }

    /* CALLBACK��NO RF���� */
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        /* 15s CALLBACK����������ʱ��������OOC�ȴ�RF������Ϣ */
        CNAS_XSD_StartTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER, TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER_LEN);
    }


    CNAS_XSD_SetRedirectionFlag(VOS_FALSE);

    CNAS_XSD_SetSystemCampOnFlag(VOS_FALSE);

    CNAS_XSD_SetOocSearchScene(CNAS_XSD_SYS_ACQ_SCENE_NO_RF);

    /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_OOC */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_OOC);

}



VOS_UINT32 CNAS_XSD_RcvMsccSrvAcqReq_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �˳�CallBack״̬ */
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        /* ֹͣCALLBACK������ʱ�� */
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_MODE_PROTECT_TIMER);
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER);
        CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER);

        CNAS_XSD_QuitCallBackMode();
    }

    /* ��ǰפ����ϵͳ��Ч,��ͨ������������д��� */
    if (VOS_TRUE == CNAS_XSD_IsCurSystemValid())
    {
        CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);

        CNAS_XSD_SndMsccSrvAcqCnf(NAS_MSCC_PIF_SRV_ACQ_RESULT_SUCCESS);

        CNAS_XSD_SetSrvAcqFlg(VOS_FALSE);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_IDLE);

        return VOS_TRUE;
    }

    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMERGENCY_CALL, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvMsccMoCallEndNtf_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��ǰפ����ϵͳ��Ч */
    if (VOS_TRUE == CNAS_XSD_IsCurSystemValid())
    {
        CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_IDLE);

        return VOS_TRUE;
    }

    /*����Lost�������� */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvTiEmcCallBackNetWorkSrchTimerExpired_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣCALLBACK������ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_MODE_PROTECT_TIMER);
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER);

    /* �˳�CALLBACK */
    CNAS_XSD_QuitCallBackMode();

    /* ��ǰפ����ϵͳ��Ч */
    if (VOS_TRUE == CNAS_XSD_IsCurSystemValid())
    {
        CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_IDLE);

        return VOS_TRUE;
    }

    /* ����Lost�������� */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvTiEmcCallBackModeProtectTimerExpired_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER);
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER);

    /* �˳�CALLBACK */
    CNAS_XSD_QuitCallBackMode();

    /* ��ǰפ����ϵͳ��Ч */
    if (VOS_TRUE == CNAS_XSD_IsCurSystemValid())
    {
        CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_IDLE);

        return VOS_TRUE;
    }

    /* ����Lost�������� */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvMsccEndEmcCallBackNtf_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣCALLBACK������ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_MODE_PROTECT_TIMER);
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_BREAK_TIMER);
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER);

    /* �˳�CALLBACK */
    CNAS_XSD_QuitCallBackMode();

    /* ��ǰפ����ϵͳ��Ч */
    if (VOS_TRUE == CNAS_XSD_IsCurSystemValid())
    {
        CNAS_XSD_SndMsccSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC);

        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_IDLE);

        return VOS_TRUE;
    }

    /* ����Lost�������� */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvTiEmcCallBackModeProtectTimerExpired_Idle(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �˳�CALLBACK */
    CNAS_XSD_QuitCallBackMode();

    /* ���д��ڲ��������� */
    if (0 != CNAS_CCB_Get1XCallExistCount())
    {
        return VOS_TRUE;
    }

    /* ���פ�����жϵ�ǰϵͳ�Ƿ���Ч(��Idle)������LOST���� */
    if (VOS_TRUE == CNAS_XSD_IsCurSystemValid())
    {
        return VOS_TRUE;
    }

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /*���������ڲ�������Ϣ */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvTiEmcCallBackSrchBreakTimerExpired_Initial(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XSD_EMC_CALLBACK_CANDIDATE_FREQ_INFO_STRU *pstEmcCallBackCfInfo = VOS_NULL_PTR;

    pstEmcCallBackCfInfo    = CNAS_XSD_GetEmcCallBackCandidateFreqInfo();

    /* ����CAlLBACKģʽ15s������CallBackģʽ������Ƶ����Ƶ�� */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST,
                                  pstEmcCallBackCfInfo->usCfChannelNum,
                                  pstEmcCallBackCfInfo->astChannelList);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvMsccEndEmcCallBackNtf_Idle(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣCALLBACK������ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_MODE_PROTECT_TIMER);

    /* �˳�CALLBACK */
    CNAS_XSD_QuitCallBackMode();

    /* ���д��ڲ��������� */
    if (0 != CNAS_CCB_Get1XCallExistCount())
    {
        return VOS_TRUE;
    }

    /* �˳�ʱ�Ե�ǰ��������жϽ���LOST���� */
    /* ���פ�����жϵ�ǰϵͳ�Ƿ���Ч(��Idle)������LOST���� */
    if (VOS_TRUE == CNAS_XSD_IsCurSystemValid())
    {
        return VOS_TRUE;
    }

    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /*���������ڲ�������Ϣ */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvMsccMoCallEndNtf_Idle(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* CALLBACK״̬���� */
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        return VOS_TRUE;
    }

    /* �����ж�LOST���� */
    /* ���פ�����жϵ�ǰϵͳ�Ƿ���Ч����������LOST���� */
    if (VOS_TRUE == CNAS_XSD_IsCurSystemValid())
    {
        return VOS_TRUE;
    }

    /* Ǩ�Ƶ�INITIAL */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /*���������ڲ�������Ϣ */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvMsccMoCallEndNtf_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        /* ���ü����� */
        CNAS_XSD_SetEmcCallBackSrchCounter(0);

        /* ����CAlLBACKģʽ15s���� */
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST, 0, VOS_NULL_PTR);

        /* 15s CALLBACK����������ʱ�� */
        CNAS_XSD_StartTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER, TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvTiEmcCallBackNetWorkSrchTimerExpired_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* CallBack No Rf�������� ֹͣCALLBACK������ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_MODE_PROTECT_TIMER);

    /* �˳�CALLBACK */
    CNAS_XSD_QuitCallBackMode();

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvTiEmcCallBackModeProtectTimerExpired_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣ������ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER);

    /* �˳�CALLBACK */
    CNAS_XSD_QuitCallBackMode();

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RcvMsccEndEmcCallBackNtf_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣ������ʱ�� */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER);
    CNAS_XSD_StopTimer(TI_CNAS_XSD_EMC_CALLBACK_MODE_PROTECT_TIMER);

    /* �˳�CALLBACK */
    CNAS_XSD_QuitCallBackMode();

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_IsCurSystemValid( VOS_VOID )
{
    CNAS_PRL_1X_SYSTEM_STRU            *pstCurSysInfo   = VOS_NULL_PTR;
    VOS_UINT32                          ulIsNegSys;

    ulIsNegSys                          = VOS_TRUE;
    pstCurSysInfo                       = &(CNAS_XSD_GetCurCampedSysInfo()->stSystem);

    if (VOS_FALSE == CNAS_XSD_GetSystemCampOnFlag())
    {
        return VOS_FALSE;
    }

    /* �����������У�����ǰפ��������Ϊ����Чϵͳ */
    if (VOS_NULL_PTR != CNAS_XSD_GetEmcCallRedialScanChanListAddr()->pstScanChanInfo)
    {
        return VOS_TRUE;
    }

    ulIsNegSys = CNAS_XSD_IsCurSysNotSuitableSys(pstCurSysInfo, VOS_FALSE, 0);

    return (!ulIsNegSys);
}



VOS_UINT32 CNAS_XSD_IsNeedReportNoService(
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU *pstSysDeterminInd
)
{
    /* ���޳�����Ҫ�ϱ��޷����ԭ��
    */
    if ((CAS_CNAS_1X_SD_UNLOCK_IND == pstSysDeterminInd->enSdReason)
     || (CAS_CNAS_1X_SD_RESELECTION_IND == pstSysDeterminInd->enSdReason))
    {
        return VOS_FALSE;
    }

    /* ������£�������������ֻ�����redirection ���λ */
    if ((CAS_CNAS_1X_SD_REDIR_IND == pstSysDeterminInd->enSdReason)
     && (VOS_TRUE == pstSysDeterminInd->unSysDeterminInfo.stRedirectionInfo.ucInvalidRedir))
    {
        return VOS_FALSE;
    }

    /* ������£����������� */
    if ((CAS_CNAS_1X_SD_NEW_SYSTEM_IND == pstSysDeterminInd->enSdReason)
     && (PS_TRUE != pstSysDeterminInd->unSysDeterminInfo.stNewSysInfo.enIsRelease))
    {
        return VOS_FALSE;
    }

    /* �绰�Ҷϣ������������������׶Σ���1�׶�ֻ��3���ض�Ƶ��(MRU0 MRU1 ��SDЯ����Ƶ��)��
       һ���������ɹ���ʱ��Ƚ϶̣������ϱ��޷��� */
    if (CAS_CNAS_1X_SD_RELEASE_IND == pstSysDeterminInd->enSdReason)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_ProcEmcSysAcqFlow_Idle(
    CNAS_XSD_SYS_ACQ_SCENE_ENUM_UINT32      enEmcAcqScene,
    VOS_UINT32                              ulFreqNum,
    CNAS_PRL_FREQENCY_CHANNEL_STRU         *pstFreq,
    CAS_CNAS_1X_SYSTEM_DETERMIN_IND_STRU   *pstSysDeterminInd
)
{
    CNAS_XSD_EMC_CALLBACK_CANDIDATE_FREQ_INFO_STRU         *pstEmcCallBackCfInfo    = VOS_NULL_PTR;

    pstEmcCallBackCfInfo    = CNAS_XSD_GetEmcCallBackCandidateFreqInfo();

    /* ������δ�յ�Succ����,���н��������� */
    if (VOS_NULL_PTR != CNAS_XSD_GetEmcCallRedialScanChanListAddr()->pstScanChanInfo)
    {
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        CNAS_XSD_SndInternalSysAcqReq(enEmcAcqScene, ulFreqNum, pstFreq);

        return VOS_TRUE;
    }

    /* CALLBACK�غ�ģʽ�У�����15s���� */
    if (CNAS_XSD_EMC_STATE_CALLBACK == CNAS_XSD_GetEmcState())
    {
        /* ����CAlLBACKģʽ15s���� */
        CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

        /* ���ü����� */
        CNAS_XSD_SetEmcCallBackSrchCounter(0);

        if (VOS_NULL_PTR    != pstSysDeterminInd)
        {
            /* SysLost����������Ƶ����Ƶ�� */
            if (CAS_CNAS_1X_SD_SYS_LOST_IND == pstSysDeterminInd->enSdReason)
            {
                CNAS_XSD_SetEmcCallBackCandidateFreq(pstSysDeterminInd->stCurCfChannelInfo.usCfChannelNum,
                                                    (CNAS_PRL_FREQENCY_CHANNEL_STRU *)pstSysDeterminInd->stCurCfChannelInfo.astChannelList);

                CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST,
                                              pstEmcCallBackCfInfo->usCfChannelNum,
                                              pstEmcCallBackCfInfo->astChannelList);

                CNAS_XSD_StartTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER, TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER_LEN);

                return VOS_TRUE;
            }
        }

        CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_EMC_CALLBACK_SYSTEM_LOST,
                                      0,
                                      VOS_NULL_PTR);

        /* 15s CALLBACK����������ʱ�� */
        CNAS_XSD_StartTimer(TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER, TI_CNAS_XSD_EMC_CALLBACK_NETWORK_SRCH_PROTECT_TIMER_LEN);

        return VOS_TRUE;
    }

    /* ������Succ���յ�SD,����OOC�ȴ�CallEnd */
    if (VOS_TRUE == CNAS_XSD_IsEmcCallOriginalSysExist())
    {
        if (VOS_TRUE == CNAS_XSD_GetCallBackCfg()->ulCallBackEnableFlg)
        {
            CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_OOC);

            return VOS_TRUE;
        }

        CNAS_XSD_ClearEmcCallOriginalSys();
    }

    return VOS_FALSE;
}


VOS_UINT32 CNAS_XSD_RcvMsccXsdSysAcq_Ooc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣavailable��ʱ�������������������У�available��ʱ����ʱ */
    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_MRU0_TIMER);

    CNAS_XSD_StopTimer(TI_CNAS_XSD_AVAILABLE_TIMER);

    /* ״̬Ǩ�Ƶ�CNAS_XSD_L1_STA_INITIAL */
    CNAS_XSD_SetCurrFsmState(CNAS_XSD_L1_STA_INITIAL);

    /* ������Ƶ��Դ */
    CNAS_XSD_SndCasBeginSessionNotify(CNAS_CAS_1X_SESSION_TYPE_NETWORK_SEARCH);

    /*���������ڲ�������Ϣ */
    CNAS_XSD_SndInternalSysAcqReq(CNAS_XSD_SYS_ACQ_SCENE_SYSTEM_LOST, 0, VOS_NULL_PTR);

    return VOS_TRUE;
}
/*lint -restore*/

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



