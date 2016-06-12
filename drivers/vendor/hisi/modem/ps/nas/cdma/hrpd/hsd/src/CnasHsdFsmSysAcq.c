

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "CnasHsdCtx.h"
#include  "CnasCcb.h"
#include  "CnasPrlApi.h"
#include  "CnasHsdComFunc.h"
#include  "CnasHsdSndInternalMsg.h"
#include  "CnasHsdSndMscc.h"
#include  "CnasHsdSndCas.h"
#include  "CnasHsdFsmSysAcq.h"
#include  "CnasHsdFsmSysAcqTbl.h"
#include  "CnasHsdSysAcqStrategy.h"
#include  "CnasHsdAvoidStrategy.h"
#include  "CnasMntn.h"
#include  "CnasHsdMntn.h"
#include  "cas_hrpd_overheadmsg_nas_pif.h"
#include  "CnasHsdSndHsm.h"
#include  "CnasHsdFsmMainTbl.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HSD_FSM_SYS_ACQ_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/* ���嵱ǰ�ڻ��ģʽ���յ�MMA������1X��������ϱ���״̬ */
CNAS_HSD_SYS_ACQ_STATUS_ENUM_UINT32                     g_enSysAcqStatus = 0;

/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -save -e958*/

VOS_VOID CNAS_HSD_SndSysAcqAbortRlst(VOS_VOID)
{
    if (CNAS_HSD_ABORT_REASON_DATA_CALL == CNAS_HSD_GetAbortReason_SysAcq())
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_ABORTED,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_ACQ_IMMEDIATE);
    }
    else
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_ABORTED,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);
    }
}


VOS_UINT32 CNAS_HSD_RcvCasOverheadInd_SysAcq_WaitCasInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_HRPD_SYSTEM_STRU                              *pstHrpdSys;
    CAS_CNAS_HRPD_OVERHEAD_MSG_IND_STRU                    *pstCasOhmMsg;
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                                              ulFreqNum;
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene;
    VOS_UINT32                                              ulMccFlg;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_OHM_IND);

    pstCasOhmMsg = (CAS_CNAS_HRPD_OVERHEAD_MSG_IND_STRU *)pstMsg;

    pstHrpdSys   = CNAS_HSD_GetCasOhmHrpdSysAddr();

    pstHrpdSys->stFreq.enBandClass = pstCasOhmMsg->usBandClass;
    pstHrpdSys->stFreq.usChannel   = pstCasOhmMsg->usFreq;
    pstHrpdSys->ucSubnetMask       = pstCasOhmMsg->ucSubNetMask;
    PS_MEM_CPY(pstHrpdSys->aucSubnet, pstCasOhmMsg->aucSectorId, CNAS_PRL_BYTES_IN_SUBNET);

    ulMccFlg = CNAS_HSD_IsMccInOperLockWhiteSidList(pstCasOhmMsg->usCountryCode);

    /* ���·������� */
    if ((VOS_FALSE == CNAS_HSD_IsCurrFreqStay(pstHrpdSys))
     || (VOS_FALSE == ulMccFlg))
    {
        if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
        {
            CNAS_HSD_SndSysAcqAbortRlst();

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();

            return VOS_TRUE;
        }

        ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);
        if (0 == ulFreqNum)
        {
            /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
            CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                                   CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                                   CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();
        }
        else
        {
            /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
            CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);;

            /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
            CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);

            CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
        }
    }
    else
    {
        /* avoidģ�鴦�� */
        CNAS_HSD_ProcAvoidEventInSubnetIdChange(pstHrpdSys);

        /* ����history list */
        CNAS_HSD_UpdateHistorySysList(pstHrpdSys);

        /* Pilot searchʱ�ɹ��ȵ�ohm��Ϣ���ɽ�������  */
        enSysAcqScene = CNAS_HSD_GetCurSysAcqScene_SysAcq();

        if (CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH == enSysAcqScene)
        {
            /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
            CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_SUCCESS,
                                   CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH,
                                   CNAS_HSD_SYS_ACQ_FAIL_ACT_BUTT);

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();

            return VOS_TRUE;
        }


        /* HSD�յ�CAS��overhead��Ϣʱ��Ҫ��HSMת��overhead��Ϣ */
        CNAS_HSD_SndHsmOverheadMsgInd(pstCasOhmMsg);

        CNAS_HSD_SndMsccOverheadMsgInd(pstCasOhmMsg,pstHrpdSys);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SESSION_IND״̬, ������������ʱ��TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_SESSION_NEG_RSLT_IND);
        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND, TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvTiWaitCasOhmIndExpired_SysAcq_WaitCasOhmInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;

    CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvTiWaitCasOhmIndExpired_SysAcq_WaitCasOhmInd: timer out");

    /* �жϵ�ǰ�Ƿ��д�ϣ���������˳��������� */
    if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
    {
        CNAS_HSD_SndSysAcqAbortRlst();

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }
    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);
    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    else
    {
        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);

        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasRedirectionInd_SysAcq_WaitCasInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CAS_CNAS_HRPD_NETWORK_LOST_IND_STRU                    *pstCasRedirMsg;
    CNAS_HSD_REDIRECTION_STRU                              *pstRedirection;
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                                              ulFreqNum;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    pstCasRedirMsg = (CAS_CNAS_HRPD_NETWORK_LOST_IND_STRU*)pstMsg;

    if (0 != pstCasRedirMsg->ucChanNum)
    {
        pstRedirection = CNAS_HSD_GetRedirectionFreqAddr();

        /* �յ���Ƶ�����ȫ�ֱ��������ڹ��������б� */
        pstRedirection->ucChanNum = pstCasRedirMsg->ucChanNum;

        PS_MEM_CPY(pstRedirection->aulChannel,
                   pstCasRedirMsg->aulChannel,
                   sizeof(CAS_CNAS_HRPD_DERECTION_CHANNEL_STRU) * pstCasRedirMsg->ucChanNum);

        /* ��ӡ�յ���Ƶ����Ϣ */
        CNAS_HSD_LogRedirectionFreqList(pstRedirection);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_REDIRECTION,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;

    }

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

    /* �����scan listΪ�� */
    if (0 == ulFreqNum)
    {

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
    CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

    /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasNoRf_SysAcq_WaitCasInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT16                          usCurIndex = 0;

    /* �����ǰindex ���ڵ���1 ����ǰλ�ü�1 (Ϊ��������SYNC CNF SUCC���Ǹ�Ƶ��) */
    usCurIndex = CNAS_HSD_GetCurScanFreqIndex();

    if (1 <= usCurIndex)
    {
        CNAS_HSD_SetCurScanFreqIndex(usCurIndex - 1);
    }

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_OHM_IND);

    /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
    CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_NORF,
                           CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                           CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

    /* �˳����״̬�� */
    CNAS_HSD_QuitFsmSysAcq_SysAcq();

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasNetworkLost_SysAcq_WaitCasInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);
    /* �Ѿ�û��Ƶ��Ҫ�� */
    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                                CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    /* ����������㷢�������� */
    else
    {
        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);

        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvTiAvailableTimerExpired_WaitMru0(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSD_SetOocWaitSearchFlag(VOS_TRUE);

    return VOS_TRUE;
}



VOS_UINT32 CNAS_HSD_RcvCasNetworkLost_SysAcq_WaitCasIndProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CAS_CNAS_HRPD_NETWORK_LOST_IND_STRU                    *pstCasNetworkLostMsg;

    pstCasNetworkLostMsg = (CAS_CNAS_HRPD_NETWORK_LOST_IND_STRU*)pstMsg;

    if (CAS_CNAS_HRPD_NETWORK_LOST_REASON_ENUM_BUTT <= pstCasNetworkLostMsg->enNetworkLostReason)
    {
        return VOS_TRUE;
    }

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_OHM_IND);

    if (CAS_CNAS_HRPD_NETWORK_LOST_NO_RF == pstCasNetworkLostMsg->enNetworkLostReason)
    {
        if (CNAS_HSD_ABORT_REASON_DATA_CALL == CNAS_HSD_GetAbortReason_SysAcq())
        {
            CNAS_HSD_InitCacheMsgQueue(&(CNAS_HSD_GetHsdCtxAddr()->stCacheMsgQueue), CNAS_HSD_INIT_CTX_POWEROFF);

            CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_NO_RF);
        }

        CNAS_HSD_RcvCasNoRf_SysAcq_WaitCasInd(ulEventType, pstMsg);

        return VOS_TRUE;
    }

    /* �жϵ�ǰ�Ƿ��д�ϣ���������˳��������� */
    if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
    {
        CNAS_HSD_SndSysAcqAbortRlst();

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    if (CAS_CNAS_HRPD_NETWORK_LOST_REDIRECT == pstCasNetworkLostMsg->enNetworkLostReason)
    {
        /* �ض�����Ϣָʾ */
        CNAS_HSD_RcvCasRedirectionInd_SysAcq_WaitCasInd(ulEventType, pstMsg);
    }
    else
    {
        CNAS_HSD_RcvCasNetworkLost_SysAcq_WaitCasInd(ulEventType, pstMsg);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccSysAcqReq_SysAcq_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;
    MSCC_HSD_SYSTEM_ACQUIRE_REQ_STRU   *pstSysAcqReq;

    pstSysAcqReq    = (MSCC_HSD_SYSTEM_ACQUIRE_REQ_STRU *)pstMsg;

    if (MSCC_HSD_ACQUIRE_REASON_POWER_UP == pstSysAcqReq->enReason)
    {
        PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

        /* ���������Ϣ */
        CNAS_HSD_SaveCurEntryMsg(ulEventType, pstMsg);

        /* ����scan list */
        CNAS_HSD_BuildHrpdScanList(CNAS_HSD_SYS_ACQ_SCENE_SWITCH_ON);

        ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

        /* �����scan listΪ�� */
        if (0 == ulFreqNum)
        {
            /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
            CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                                   CNAS_HSD_SYS_ACQ_SCENE_SWITCH_ON,
                                   CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();

            return VOS_TRUE;
        }

        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* ��MMA����ID_HSD_MMA_SYSTEM_ACQUIRE_START_IND */
        CNAS_HSD_SndMsccSysAcqStartInd();

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);
        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);

        /* �����ǰפ����Hrpd��Ϣ */
        CNAS_HSD_ClearHrpdSysInfo();
    }
    else
    {
        ;
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvHsdSysAcqReq_SysAcq_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32  enSysAcqScene;
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    /* ���������Ϣ */
    CNAS_HSD_SaveCurEntryMsg(ulEventType, pstMsg);

    enSysAcqScene = CNAS_HSD_GetCurSysAcqScene_SysAcq();

    if (CNAS_HSD_SYS_ACQ_SCENE_NO_RF_RESUME == enSysAcqScene)
    {
        /* ������֮ǰ��Ƶ�㣬�����½��� */
         ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);
    }
    else
    {
        /* ����scan list */
        CNAS_HSD_BuildHrpdScanList(enSysAcqScene);

        ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);
    }

    /* �����scan listΪ�� */
    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
    CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

    /* ��MMA����ID_HSD_MMA_SYSTEM_ACQUIRE_START_IND */
    CNAS_HSD_SndMsccSysAcqStartInd();

    /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);
    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);

    /* �����ǰפ����Hrpd��Ϣ */
    CNAS_HSD_ClearHrpdSysInfo();

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasSysSyncCnf_SysAcq_WaitCasSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CAS_CNAS_HRPD_SYSTEM_SYNC_CNF_STRU *pstSyncCnf;

    pstSyncCnf                  = (CAS_CNAS_HRPD_SYSTEM_SYNC_CNF_STRU *)pstMsg;

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF);
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF);

    /* ����ͬ��ȷ�Ͻ�� */
    switch (pstSyncCnf->enSyncRslt)
    {
        case CNAS_CAS_HRPD_RSLT_SUCCESS:
            CNAS_HSD_ProcCasSyncCnfSucc_SysAcq(pstSyncCnf);
            break;

        case CNAS_CAS_HRPD_RSLT_NO_RF:
            CNAS_HSD_ProcCasSyncCnfNoRf_SysAcq(pstSyncCnf);
            break;

        case CNAS_CAS_HRPD_RSLT_FAILURE:
        default:
            CNAS_HSD_ProcCasSyncCnfFail_SysAcq(pstSyncCnf);
            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvTiWaitCasSysSyncCnfExpired_SysAcq_WaitCasSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvTiWaitCasSysSyncCnfExpired_SysAcq_WaitCasSysSyncCnf: timer out");

    /* ��CAS����ID_CNAS_CAS_HRPD_STOP_SYSTEM_SYNC_REQ��Ϣֹͣͬ�� */
    CNAS_HSD_SndCasStopSysSyncReq();

    /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_STOP_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_STOP_SYNC_CNF);
    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF_LEN);

    return VOS_TRUE;
}



VOS_UINT32 CNAS_HSD_RcvCasStopSysSyncCnf_SysAcq_WaitCasStopSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;
    CAS_CNAS_HRPD_STOP_SYSTEM_SYNC_CNF_STRU                *pstStopCnf;

    pstStopCnf  = (CAS_CNAS_HRPD_STOP_SYSTEM_SYNC_CNF_STRU*)pstMsg;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF);

    /* �����Ѿ�ʧ�ܵ�Ƶ�� */
    CNAS_HSD_RefreshScanListIndex((VOS_UINT16)pstStopCnf->ulSyncFailFreqNum);

    /* �ж��Ƿ��ڴ�������� */
    if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
    {
        CNAS_HSD_SndSysAcqAbortRlst();

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    else
    {
        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);
        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvTiWaitCasStopSysSyncCnfExpired_SysAcq_WaitCasStopSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    /* �ж��Ƿ��ڴ�������� */
    if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
    {
        CNAS_HSD_SndSysAcqAbortRlst();

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    else
    {
        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);
        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
    }

    return VOS_TRUE;
}


VOS_VOID CNAS_HSD_QuitFsmSysAcq_SysAcq(VOS_VOID)
{
#if 0
    CNAS_HSD_ACQUIRE_SYSTEM_LIST_STRU  *pstAcqSysList = VOS_NULL_PTR;

    pstAcqSysList = CNAS_HSD_GetSysAcqListAddr_SysAcq();

    /* �ͷ�ϵͳ�����б���ڴ� */
    if (VOS_NULL_PTR != pstAcqSysList->pstAcqSysInfo)
    {
        PS_MEM_FREE(UEPS_PID_HSD, pstAcqSysList->pstAcqSysInfo);
        pstAcqSysList->pstAcqSysInfo = VOS_NULL_PTR;
    }
#endif
    CNAS_HSD_QuitFsmL2();

    return;
}



VOS_VOID CNAS_HSD_SetSysAcqStatusByBit(VOS_UINT32 ulBit)
{
    g_enSysAcqStatus |= BIT_N(ulBit);

    return;
}


CNAS_HSD_SYS_ACQ_STATUS_ENUM_UINT32 CNAS_HSD_GetSysAcqStatus(VOS_VOID)
{
    return g_enSysAcqStatus;
}


VOS_VOID  CNAS_HSD_CleanSysAcqStatus(VOS_VOID)
{
    g_enSysAcqStatus = CNAS_HSD_SYS_ACQ_NULL;

    return;
}


VOS_VOID  CNAS_HSD_ClearSysAcqStatusBit(VOS_UINT32 ulBit)
{
    CNAS_HSD_SYS_ACQ_STATUS_ENUM_UINT32 enSysAcqStatus;

    enSysAcqStatus      = BIT_N(ulBit);

    g_enSysAcqStatus    = (g_enSysAcqStatus & (~enSysAcqStatus));

    return;
}


VOS_VOID  CNAS_HSD_ClearHrpdSysAcqStatusBits(VOS_VOID)
{
    CNAS_HSD_ClearSysAcqStatusBit(CNAS_HSD_POWER_ON_SYS_ACQ);
    CNAS_HSD_ClearSysAcqStatusBit(CNAS_HSD_HIGH_PRI_SYS_ACQ);
    CNAS_HSD_ClearSysAcqStatusBit(CNAS_HSD_OTHER_SYS_ACQ);

    return;
}


CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32 CNAS_HSD_GetCurSysAcqScene_SysAcq(VOS_VOID)
{
    CNAS_HSD_MSG_STRU                                      *pstEntryMsg;
    CNAS_HSD_HSD_SYSTEM_ACQUIRED_REQ_STRU                  *pstIntSysAcqReq;

    /* ��õ�ǰ����CNAS HSD�����Ϣ�Ļ�������ַ */
    pstEntryMsg                 = CNAS_HSD_GetCurrFsmEntryMsgAddr();

    if (CNAS_BuildEventType(UEPS_PID_MSCC, ID_MSCC_HSD_SYSTEM_ACQUIRE_REQ)
            == pstEntryMsg->ulEventType)
    {
        return CNAS_HSD_SYS_ACQ_SCENE_SWITCH_ON;
    }

    if (CNAS_BuildEventType(UEPS_PID_HSD, ID_CNAS_HSD_HSD_SYSTEM_ACQUIRED_REQ)
            == pstEntryMsg->ulEventType)
    {
        pstIntSysAcqReq = (CNAS_HSD_HSD_SYSTEM_ACQUIRED_REQ_STRU *)pstEntryMsg->aucMsgBuffer;

        return pstIntSysAcqReq->enSysAcqScene;
    }

    return CNAS_HSD_SYS_ACQ_SCENE_BUTT;
}



VOS_UINT32 CNAS_HSD_CasSysAcqVaildFreqCheck(CAS_CNAS_HRPD_SYSTEM_SYNC_CNF_STRU *pstSyncCnf)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU     *pstSrcFreq;
    CNAS_PRL_FREQENCY_CHANNEL_STRU      stDstFreq;
    VOS_UINT16                          usIndex;
    VOS_UINT16                          i;

    usIndex = CNAS_HSD_GetCurScanFreqIndex();

    for (i = 0; i < pstSyncCnf->ulSyncFailFreqNum; i++)
    {
        stDstFreq.enBandClass   = pstSyncCnf->astSyncRsltList[i].stFreq.usBandClass;
        stDstFreq.usChannel     = pstSyncCnf->astSyncRsltList[i].stFreq.usChannel;

        pstSrcFreq = CNAS_HSD_GetScanFreqByIndex(usIndex + i);

        if (VOS_NULL_PTR == pstSrcFreq)
        {
            return VOS_ERR;
        }

        if (VOS_FALSE == CNAS_PRL_CmpFreqValue(pstSrcFreq, &stDstFreq))
        {
            return VOS_ERR;
        }
    }

    if (CNAS_CAS_HRPD_RSLT_SUCCESS == pstSyncCnf->enSyncRslt)
    {
        stDstFreq.enBandClass   = pstSyncCnf->stFreq.usBandClass;
        stDstFreq.usChannel     = pstSyncCnf->stFreq.usChannel;

        pstSrcFreq = CNAS_HSD_GetScanFreqByIndex((VOS_UINT16)(usIndex + pstSyncCnf->ulSyncFailFreqNum));

        if (VOS_NULL_PTR == pstSrcFreq)
        {
            return VOS_ERR;
        }

        if (VOS_FALSE == CNAS_PRL_CmpFreqValue(pstSrcFreq, &stDstFreq))
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


VOS_UINT32 CNAS_HSD_RcvMsccPowerOffReq_SysAcq_WaitCasSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF);

    (VOS_VOID)CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_POWER_OFF);

    /* ����ػ����� */
    CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF_LEN);

    /* ��CAS����ID_CNAS_CAS_HRPD_STOP_SYSTEM_SYNC_REQ��Ϣֹͣͬ�� */
    CNAS_HSD_SndCasStopSysSyncReq();

    CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_STOP_SYNC_CNF);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccPowerOffReq_SysAcq_WaitCasStopSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��Ҫ�Ƚ�DataCall��Ϣ�ظ� */
    if (CNAS_HSD_ABORT_REASON_DATA_CALL == CNAS_HSD_GetAbortReason_SysAcq())
    {
        CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED);
    }

    /* �ϲ���POWEROFF��POWERSAVE���п��ƣ������Ѿ����͹�POWERSAVE������ֱ�Ӵ���POWEROFF��Ϣ */
    if (VOS_TRUE == CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_POWER_OFF))
    {
        /* ��ջ��� */
        CNAS_HSD_InitCacheMsgQueue(&(CNAS_HSD_GetHsdCtxAddr()->stCacheMsgQueue), CNAS_HSD_INIT_CTX_POWEROFF);

        /* ����ػ����� */
        CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccPowerOffReq_SysAcq_WaitCasInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
        /* ��Ҫ�Ƚ�DataCall��Ϣ�ظ� */
    if (CNAS_HSD_ABORT_REASON_DATA_CALL == CNAS_HSD_GetAbortReason_SysAcq())
    {
        CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED);
    }

    /* �ϲ���POWEROFF��POWERSAVE���п��ƣ������Ѿ����͹�POWEROFF������POWERSAVE��Ϣ */
    if (VOS_TRUE == CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_POWER_OFF))
    {
        /* ֹͣ������ʱ�� */
        CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_OHM_IND);

        /* ��ջ��� */
        CNAS_HSD_InitCacheMsgQueue(&(CNAS_HSD_GetHsdCtxAddr()->stCacheMsgQueue), CNAS_HSD_INIT_CTX_POWEROFF);

        /* ����ػ����� */
        CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_ABORTED,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitCasInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��ǰ�Ѿ�����data call��ϣ�ֱ�ӷ��� */
    if (VOS_TRUE == CNAS_HSD_IsCurAbortFlgSet_SysAcq(CNAS_HSD_ABORT_REASON_DATA_CALL))
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitCasInd: Data call abort is set.");

        return VOS_TRUE;
    }

    /* ��ǰ�Ѿ��ǿ��ѳ�����������Ӧ�������� */
    if (CNAS_HSD_SYS_ACQ_SCENE_DATA_CALL_QUICK == CNAS_HSD_GetCurSysAcqScene_SysAcq())
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitSessionInd: current srch scene is  quick srch.");

        return VOS_TRUE;
    }

    if (VOS_TRUE == CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_DATA_CALL))
    {
        /* ����Data Call���� */
        CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);
    }
    else
    {
        CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasPilotSearchStopInd_SysAcq_WaitCasSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �����pilot search�ĳ�����ֱ���˳����������������������Ϣ������Ч */
    /* ��������:ʧ��ʱL1��δ���? */
    if (CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH == CNAS_HSD_GetCurSysAcqScene_SysAcq())
    {
        /* ֹͣ������ʱ�� */
        CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH,
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasPilotSearchStopInd_SysAcq_WaitCasStopSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �����pilot search�ĳ�����ֱ���˳����������������������Ϣ������Ч */
    if (CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH == CNAS_HSD_GetCurSysAcqScene_SysAcq())
    {
        /* ֹͣ������ʱ�� */
        CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH,
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitCasStopSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��ǰ�Ѿ�����data call��ϣ�ֱ�ӷ��� */
    if (VOS_TRUE == CNAS_HSD_IsCurAbortFlgSet_SysAcq(CNAS_HSD_ABORT_REASON_DATA_CALL))
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitCasStopSysSyncCnf: Data call abort is set.");

        return VOS_TRUE;
    }

    /* ��ǰ�Ѿ��ǿ��ѳ�����������Ӧ�������� */
    if (CNAS_HSD_SYS_ACQ_SCENE_DATA_CALL_QUICK == CNAS_HSD_GetCurSysAcqScene_SysAcq())
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitCasStopSysSyncCnf: current srch scene is  quick srch.");

        return VOS_TRUE;
    }

    if (VOS_TRUE == CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_DATA_CALL))
    {
        /* ����Data Call���� */
        CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);
    }
    else
    {
        CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasPilotSearchStopInd_SysAcq_WaitCasInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �����pilot search�ĳ�����ֱ���˳����������������������Ϣ������Ч */
    if (CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH == CNAS_HSD_GetCurSysAcqScene_SysAcq())
    {
        /* ֹͣ������ʱ�� */
        CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_OHM_IND);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH,
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitCasSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��ǰ�Ѿ��ǿ��ѳ�����������Ӧ�������� */
    if (CNAS_HSD_SYS_ACQ_SCENE_DATA_CALL_QUICK == CNAS_HSD_GetCurSysAcqScene_SysAcq())
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitCasSysSyncCnf: current srch scene is  quick srch.");

        return VOS_TRUE;
    }

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF);

    /* �ж��Ƿ��ڴ��������,���ڴ�������У��Ŵ��������Ϣ */
    (VOS_VOID)CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_DATA_CALL);

    /* ����Data Call���� */
    CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);

    /* ��CAS����ID_CNAS_CAS_HRPD_STOP_SYSTEM_SYNC_REQ��Ϣֹͣͬ�� */
    CNAS_HSD_SndCasStopSysSyncReq();

    CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_STOP_SYNC_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF_LEN);

    return VOS_TRUE;
}



VOS_UINT32 CNAS_HSD_RcvMsccPowerSaveReq_SysAcq_WaitCasSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF);

    /* ���ô�ϱ�� */
    (VOS_VOID)CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_POWER_SAVE);

    /* ����POWER SAVE���� */
    CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF_LEN);

    /* ��CAS����ID_CNAS_CAS_HRPD_STOP_SYSTEM_SYNC_REQ��Ϣֹͣͬ�� */
    CNAS_HSD_SndCasStopSysSyncReq();

    CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_STOP_SYNC_CNF);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccPowerSaveReq_SysAcq_WaitCasStopSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��Ҫ�Ƚ�DataCall��Ϣ�ظ� */
    if (CNAS_HSD_ABORT_REASON_DATA_CALL == CNAS_HSD_GetAbortReason_SysAcq())
    {
        CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED);
    }

    /* �ϲ���POWEROFF��POWERSAVE���п��ƣ������Ѿ����͹�POWEROFF������POWERSAVE��Ϣ */
    if (VOS_TRUE == CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_POWER_SAVE))
    {
        /* ��ջ��� */
        CNAS_HSD_InitCacheMsgQueue(&(CNAS_HSD_GetHsdCtxAddr()->stCacheMsgQueue), CNAS_HSD_INIT_CTX_POWEROFF);

        /* ����PowerSave���� */
        CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);
    }

    return VOS_TRUE;
}



VOS_UINT32 CNAS_HSD_RcvMsccPowerSaveReq_SysAcq_WaitCasInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��Ҫ�Ƚ�DataCall��Ϣ�ظ� */
    if (CNAS_HSD_ABORT_REASON_DATA_CALL == CNAS_HSD_GetAbortReason_SysAcq())
    {
        CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED);
    }

    /* �ϲ���POWEROFF��POWERSAVE���п��ƣ������Ѿ����͹�POWEROFF������POWERSAVE��Ϣ */
    if (VOS_TRUE == CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_POWER_SAVE))
    {
        /* ֹͣ������ʱ�� */
        CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_OHM_IND);

        /* ��ջ��� */
        CNAS_HSD_InitCacheMsgQueue(&(CNAS_HSD_GetHsdCtxAddr()->stCacheMsgQueue), CNAS_HSD_INIT_CTX_POWEROFF);

        /* ����PowerSave���� */
        CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_ABORTED,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvSessionNegRsltInd_SysAcq_WaitSessionInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSM_HSD_SESSION_NEG_RESULT_IND_STRU                    *pstSessionNegRlstInd;
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          stFreq;
    CNAS_HSD_REACQ_FALG_ENUM_UINT32                         enReAcqFlag;
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                                              ulFreqNum;
    CNAS_PRL_HRPD_SYSTEM_STRU                              *pstHrpdSys;
    CNAS_PRL_HRPD_SYSTEM_STRU                               stMru;

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND);

    PS_MEM_SET(&stMru, 0, sizeof(CNAS_PRL_HRPD_SYSTEM_STRU));

    pstHrpdSys   = CNAS_HSD_GetCasOhmHrpdSysAddr();

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    pstSessionNegRlstInd = (HSM_HSD_SESSION_NEG_RESULT_IND_STRU*)pstMsg;
    stFreq.enBandClass   = CNAS_HSD_GetHistorySysList()->stCampedHrpdSysInfo[0].stFreq.enBandClass;
    stFreq.usChannel     = CNAS_HSD_GetHistorySysList()->stCampedHrpdSysInfo[0].stFreq.usChannel;
    enReAcqFlag          = CNAS_HSD_NOT_NEED_REACQ;

    /* avoidģ�鴦�� */
    CNAS_HSD_ProcAvoidEventInSessionNegRlstInd(pstSessionNegRlstInd, &stFreq, &enReAcqFlag);

    /* �����ɹ� */
    if (HSM_HSD_SESSION_NEG_RSLT_SUCCESS == pstSessionNegRlstInd->enNegResult)
    {
        /* ����Hrpdפ����Ϣ */
        CNAS_HSD_UpdateHrpdSysInfo(pstHrpdSys);

        /* ��ע�������籣�浽MRU */
        if (VOS_FALSE == CNAS_HSD_GetFreqLockMode())
        {
            stMru.stFreq.enBandClass = CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stSyncFreq.enBandClass;
            stMru.stFreq.usChannel   = CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stSyncFreq.usChannel;
            stMru.ucSubnetMask       = pstHrpdSys->ucSubnetMask;
            PS_MEM_CPY(stMru.aucSubnet, pstHrpdSys->aucSubnet, CNAS_PRL_BYTES_IN_SUBNET);
            CNAS_HSD_UpdateMru0(&stMru);
        }

        CNAS_HSD_SndMsccSessionNegRsltInd(pstSessionNegRlstInd->enNegResult,
                                          pstSessionNegRlstInd->ucIsNewSession,
                                          pstSessionNegRlstInd->enSessionRelType,
                                          pstSessionNegRlstInd->ucIsEhrpdSupport);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_SUCCESS,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_BUTT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    else if (HSM_HSD_SESSION_NEG_RSLT_SESSION_CLOSE != pstSessionNegRlstInd->enNegResult)
    {
        /* �жϵ�ǰ�Ƿ��д�ϣ���������˳��������� */
        if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
        {
            CNAS_HSD_SndSysAcqAbortRlst();

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();

            return VOS_TRUE;
        }

        ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

        if (0 == ulFreqNum)
        {
            /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
            CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                                   CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                                   CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();
        }
        else
        {
            /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
            CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);;

            /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
            CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);

            CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
        }
    }
    /* ����֮����HRPD��������ȷ���session closeȻ��Ȼ���·���session��������յ�����session ind */
    else
    {
        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND, TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvTiWaitSessionNegIndExpired_SysAcq_WaitSessionNegInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;

    CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvTiWaitSessionNegIndExpired_SysAcq_WaitSessionNegInd: timer out");

    /* �жϵ�ǰ�Ƿ��д�ϣ���������˳��������� */
    if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
    {
        CNAS_HSD_SndSysAcqAbortRlst();

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);
    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    else
    {
        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);

        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccPowerOffReq_SysAcq_WaitSessionInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��Ҫ�Ƚ�DataCall��Ϣ�ظ� */
    if (CNAS_HSD_ABORT_REASON_DATA_CALL == CNAS_HSD_GetAbortReason_SysAcq())
    {
        CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED);
    }

    /* �ϲ���POWEROFF��POWERSAVE���п��ƣ������Ѿ����͹�POWEROFF������POWERSAVE��Ϣ */
    if (VOS_TRUE == CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_POWER_OFF))
    {
        /* ֹͣ������ʱ�� */
        CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND);

        /* ��ջ��� */
        CNAS_HSD_InitCacheMsgQueue(&(CNAS_HSD_GetHsdCtxAddr()->stCacheMsgQueue), CNAS_HSD_INIT_CTX_POWEROFF);

        /* ����PowerSave���� */
        CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_ABORTED,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccPowerSaveReq_SysAcq_WaitSessionInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��Ҫ�Ƚ�DataCall��Ϣ�ظ� */
    if (CNAS_HSD_ABORT_REASON_DATA_CALL == CNAS_HSD_GetAbortReason_SysAcq())
    {
        CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED);
    }

    /* �ϲ���POWEROFF��POWERSAVE���п��ƣ������Ѿ����͹�POWEROFF������POWERSAVE��Ϣ */
    if (VOS_TRUE == CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_POWER_SAVE))
    {
        /* ֹͣ������ʱ�� */
        CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND);

        /* ��ջ��� */
        CNAS_HSD_InitCacheMsgQueue(&(CNAS_HSD_GetHsdCtxAddr()->stCacheMsgQueue), CNAS_HSD_INIT_CTX_POWEROFF);

        /* ����PowerSave���� */
        CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_ABORTED,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitSessionInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ��ǰ�Ѿ�����data call��ϣ�ֱ�ӷ��� */
    if (VOS_TRUE == CNAS_HSD_IsCurAbortFlgSet_SysAcq(CNAS_HSD_ABORT_REASON_DATA_CALL))
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitSessionInd: Data call abort is set.");

        return VOS_TRUE;
    }

    /* ��ǰ�Ѿ��ǿ��ѳ�����������Ӧ�������� */
    if (CNAS_HSD_SYS_ACQ_SCENE_DATA_CALL_QUICK == CNAS_HSD_GetCurSysAcqScene_SysAcq())
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvMsccDataCallReq_SysAcq_WaitSessionInd: current srch scene is  quick srch.");

        return VOS_TRUE;
    }

    if (VOS_TRUE == CNAS_HSD_SetAbortFlg_SysAcq(VOS_TRUE, CNAS_HSD_ABORT_REASON_DATA_CALL))
    {
        /* ����Data Call���� */
        CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);
    }
    else
    {
        CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasNetworkLost_SysAcq_WaitSessionIndProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CAS_CNAS_HRPD_NETWORK_LOST_IND_STRU                    *pstCasNetworkLostMsg;

    pstCasNetworkLostMsg = (CAS_CNAS_HRPD_NETWORK_LOST_IND_STRU*)pstMsg;

    if (CAS_CNAS_HRPD_NETWORK_LOST_REASON_ENUM_BUTT <= pstCasNetworkLostMsg->enNetworkLostReason)
    {
        return VOS_TRUE;
    }

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND);

    /* NORF������Ҫ�������� */
    if (CAS_CNAS_HRPD_NETWORK_LOST_NO_RF == pstCasNetworkLostMsg->enNetworkLostReason)
    {
        if (CNAS_HSD_ABORT_REASON_DATA_CALL == CNAS_HSD_GetAbortReason_SysAcq())
        {
            CNAS_HSD_InitCacheMsgQueue(&(CNAS_HSD_GetHsdCtxAddr()->stCacheMsgQueue), CNAS_HSD_INIT_CTX_POWEROFF);

            CNAS_HSD_SndMsccDataCallRedialSysAcqCnf(NAS_MSCC_PIF_ACQUIRED_RESULT_NO_RF);
        }

        CNAS_HSD_RcvCasNoRf_SysAcq_WaitSessionInd(ulEventType, pstMsg);

        return VOS_TRUE;
    }

    /* �жϵ�ǰ�Ƿ��д�ϣ���������˳��������� */
    if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
    {
        CNAS_HSD_SndSysAcqAbortRlst();

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    if (CAS_CNAS_HRPD_NETWORK_LOST_REDIRECT == pstCasNetworkLostMsg->enNetworkLostReason)
    {
        /* �ض�����Ϣָʾ */
        CNAS_HSD_RcvCasRedirectionInd_SysAcq_WaitSessionInd(ulEventType, pstMsg);
    }
    else
    {
        CNAS_HSD_RcvCasNetworkLost_SysAcq_WaitSessionInd(ulEventType, pstMsg);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasRedirectionInd_SysAcq_WaitSessionInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CAS_CNAS_HRPD_NETWORK_LOST_IND_STRU                    *pstCasRedirMsg;
    CNAS_HSD_REDIRECTION_STRU                              *pstRedirection;
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                                              ulFreqNum;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    pstCasRedirMsg = (CAS_CNAS_HRPD_NETWORK_LOST_IND_STRU*)pstMsg;


    if (0 != pstCasRedirMsg->ucChanNum)
    {
        pstRedirection = CNAS_HSD_GetRedirectionFreqAddr();

        /* �յ���Ƶ�����ȫ�ֱ��������ڹ��������� */
        pstRedirection->ucChanNum = pstCasRedirMsg->ucChanNum;

        PS_MEM_CPY(pstRedirection->aulChannel,
                   pstCasRedirMsg->aulChannel,
                   sizeof(CAS_CNAS_HRPD_DERECTION_CHANNEL_STRU) * pstCasRedirMsg->ucChanNum);

        /* ��ӡ�յ���Ƶ����Ϣ */
        CNAS_HSD_LogRedirectionFreqList(pstRedirection);

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_REDIRECTION,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;

    }

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

    /* �����scan listΪ�� */
    if (0 == ulFreqNum)
    {

        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
    CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

    /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasNoRf_SysAcq_WaitSessionInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT16                          usCurIndex = 0;

    /* �����ǰindex ���ڵ���1 ����ǰλ�ü�1 (Ϊ��������SYNC CNF SUCC���Ǹ�Ƶ��) */
    usCurIndex = CNAS_HSD_GetCurScanFreqIndex();

    if (1 <= usCurIndex)
    {
        CNAS_HSD_SetCurScanFreqIndex(usCurIndex - 1);
    }
    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND);

    /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
    CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_NORF,
                           CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                           CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

    /* �˳����״̬�� */
    CNAS_HSD_QuitFsmSysAcq_SysAcq();

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasNetworkLost_SysAcq_WaitSessionInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));



    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);
    /* �Ѿ�û��Ƶ��Ҫ�� */
    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                                CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    /* ����������㷢�������� */
    else
    {
        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);

        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasOhmInd_SysAcq_WaitSessionInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_HRPD_SYSTEM_STRU                              *pstHrpdSys;
    CAS_CNAS_HRPD_OVERHEAD_MSG_IND_STRU                    *pstCasOhmMsg;

    pstCasOhmMsg = (CAS_CNAS_HRPD_OVERHEAD_MSG_IND_STRU *)pstMsg;

    pstHrpdSys   = CNAS_HSD_GetCasOhmHrpdSysAddr();

    pstHrpdSys->stFreq.enBandClass = pstCasOhmMsg->usBandClass;
    pstHrpdSys->stFreq.usChannel   = pstCasOhmMsg->usFreq;
    pstHrpdSys->ucSubnetMask       = pstCasOhmMsg->ucSubNetMask;
    PS_MEM_CPY(pstHrpdSys->aucSubnet, pstCasOhmMsg->aucSectorId, CNAS_PRL_BYTES_IN_SUBNET);

    /* avoidģ�鴦�� */
    CNAS_HSD_ProcAvoidEventInSubnetIdChange(pstHrpdSys);

    /* ����history list */
    CNAS_HSD_UpdateHistorySysList(pstHrpdSys);

    /* HSD�յ�CAS��overhead��Ϣʱ��Ҫ��HSMת��overhead��Ϣ */
    CNAS_HSD_SndHsmOverheadMsgInd(pstCasOhmMsg);

    CNAS_HSD_SndMsccOverheadMsgInd(pstCasOhmMsg,pstHrpdSys);

    return VOS_TRUE;
}



VOS_VOID CNAS_HSD_ProcCasSyncCnfSucc_SysAcq(
    CAS_CNAS_HRPD_SYSTEM_SYNC_CNF_STRU *pstSyncCnf
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU                          astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                                              ulFreqNum;
    VOS_UINT32                                              ulSynFailFreqNum;
    HSD_HSM_SYSTEM_ACQUIRE_TYPE_ENUM_UINT8                  enSysAcqType;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    /***********************************************************************************************
     * 1. �����ǰRF availableΪFALSE��֪ͨ�ϲ�RF available
     **********************************************************************************************/
    if (VOS_FALSE == CNAS_HSD_GetHrpdRfAvailFlg())
    {
        /* �ϱ�MSCC NO RF��Ϣ */
        CNAS_HSD_SndMsccRfAvailInd(VOS_TRUE);

        /* ���õ�ǰRF���ñ��Ϊ:RF���� */
        CNAS_HSD_SetHrpdRfAvailFlg(VOS_TRUE);
    }

    /***********************************************************************************************
     * 2. ���Ƶ���Ƿ�һ��
     **********************************************************************************************/
    /* ���CAS�ϱ���Ƶ�����·���Ƶ���Ƿ�һ�£��粻һ�����·�Ƶ������ */
    if (VOS_OK != CNAS_HSD_CasSysAcqVaildFreqCheck(pstSyncCnf))
    {
        ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

        /* ʣ���scan listΪ�� */
        if (0 == ulFreqNum)
        {
            /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
            CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                                   CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                                   CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();
        }
        else
        {
            /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
            CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

            /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
            CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
        }

        return;
    }

    /***********************************************************************************************
     * 3. ����Ƶʱ���������avoidƵ��
     **********************************************************************************************/
    /* ����avoid�¼� */
    if (VOS_FALSE == CNAS_HSD_GetFreqLockMode())
    {
        CNAS_HSD_ProcAvoidEventInCasSysSyncCnf(pstSyncCnf);
    }

    /***********************************************************************************************
     * 4. ˢ��index
     **********************************************************************************************/
    ulSynFailFreqNum = pstSyncCnf->ulSyncFailFreqNum;

    CNAS_HSD_RefreshScanListIndex((VOS_UINT16)(ulSynFailFreqNum + 1));

    /***********************************************************************************************
     * 5. ����Sync Info
     **********************************************************************************************/
    CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stSyncFreq.enBandClass = pstSyncCnf->stFreq.usBandClass;
    CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stSyncFreq.usChannel   = pstSyncCnf->stFreq.usChannel;

    /***********************************************************************************************
     * 6. �������HSM��MSCC
     **********************************************************************************************/
    /* Abnormal�����������ֻ�ڵ�һ�γɹ�ʱ�ϱ�abnormalģʽ */
    if (VOS_TRUE == CNAS_HSD_GetAbnormalLostFlag())
    {
        enSysAcqType = HSD_HSM_SYSTEM_ACQUIRE_TYPE_ABNORMAL;

        CNAS_HSD_SetAbnormalLostFlag(VOS_FALSE);
    }
    else
    {
        enSysAcqType = HSD_HSM_SYSTEM_ACQUIRE_TYPE_NORMAL;
    }

    CNAS_HSD_SndHsmSysAcqSuccInd(pstSyncCnf->aulSystemTime, enSysAcqType);

    /* ͬ���ɹ����MSCC�ϱ�SYNC IND */
    CNAS_HSD_SndMsccSystemSyncInd();

    /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_IND);
    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_OHM_IND, TI_CNAS_HSD_WAIT_CAS_OHM_IND_LEN);

    return;
}


VOS_VOID CNAS_HSD_ProcCasSyncCnfFail_SysAcq(
    CAS_CNAS_HRPD_SYSTEM_SYNC_CNF_STRU *pstSyncCnf
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;
    VOS_UINT32                          ulSynFailFreqNum;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    /***********************************************************************************************
     * 1. �����ǰRF availableΪFALSE��֪ͨ�ϲ�RF available
     **********************************************************************************************/
    if (VOS_FALSE == CNAS_HSD_GetHrpdRfAvailFlg())
    {
        /* �ϱ�MSCC NO RF��Ϣ */
        CNAS_HSD_SndMsccRfAvailInd(VOS_TRUE);

        /* ���õ�ǰRF���ñ��Ϊ:RF���� */
        CNAS_HSD_SetHrpdRfAvailFlg(VOS_TRUE);
    }

    /***********************************************************************************************
     * 2. ���Ƶ���Ƿ�һ��
     **********************************************************************************************/
    /* ���CAS�ϱ���Ƶ�����·���Ƶ���Ƿ�һ�£��粻һ�����·�Ƶ������ */
    if (VOS_OK != CNAS_HSD_CasSysAcqVaildFreqCheck(pstSyncCnf))
    {
        ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

        /* ʣ���scan listΪ�� */
        if (0 == ulFreqNum)
        {
            /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
            CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                                   CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                                   CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();
        }
        else
        {
            /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
            CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

            /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
            CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
        }

        return;
    }

    /***********************************************************************************************
     * 3. ����Ƶʱ���������avoidƵ��
     **********************************************************************************************/
    /* ����avoid�¼� */
    if (VOS_FALSE == CNAS_HSD_GetFreqLockMode())
    {
        CNAS_HSD_ProcAvoidEventInCasSysSyncCnf(pstSyncCnf);
    }

    /***********************************************************************************************
     * 4. ˢ��index
     **********************************************************************************************/
    ulSynFailFreqNum = pstSyncCnf->ulSyncFailFreqNum;

    CNAS_HSD_RefreshScanListIndex((VOS_UINT16)ulSynFailFreqNum);

    /***********************************************************************************************
     * 5. �������HSM��MSCC
     **********************************************************************************************/
    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);
    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return;
    }

    /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
    CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

    /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);

    return;
}


VOS_VOID CNAS_HSD_ProcCasSyncCnfNoRf_SysAcq(CAS_CNAS_HRPD_SYSTEM_SYNC_CNF_STRU *pstSyncCnf)
{
    /***********************************************************************************************
     * ����NO RF������ֱ���˳���ǰ״̬��
     **********************************************************************************************/
    /* 1.0 �ж�ʧ��Ƶ���Ƿ��������б��� ������ڣ����������б�ָ�� */
    if (VOS_OK == CNAS_HSD_CasSysAcqVaildFreqCheck(pstSyncCnf))
    {
        CNAS_HSD_RefreshScanListIndex((VOS_UINT16)(pstSyncCnf->ulSyncFailFreqNum));

    }

    /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
    CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_NORF,
                           CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                           CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

    /* �˳����״̬�� */
    CNAS_HSD_QuitFsmSysAcq_SysAcq();

    return;
}


VOS_UINT32 CNAS_HSD_RcvCasNetworkLost_SysAcq_WaitCasSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF);

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    else
    {
        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);
        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasNetworkLost_SysAcq_WaitCasStopSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF);

    /* �ж��Ƿ��ڴ�������� */
    if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
    {
        CNAS_HSD_SndSysAcqAbortRlst();

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    else
    {
        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);
        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvCasSyncCnf_SysAcq_WaitCasStopSysSyncCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];
    VOS_UINT32                          ulFreqNum;
    CAS_CNAS_HRPD_SYSTEM_SYNC_CNF_STRU                *pstSyncCnf;

    pstSyncCnf  = (CAS_CNAS_HRPD_SYSTEM_SYNC_CNF_STRU*)pstMsg;

    PS_MEM_SET(astFreqList, 0, sizeof(astFreqList));

    /* ֹͣ������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_STOP_SYSTEM_SYNC_CNF);

    /* �����Ѿ�ʧ�ܵ�Ƶ�� */
    CNAS_HSD_RefreshScanListIndex((VOS_UINT16)pstSyncCnf->ulSyncFailFreqNum);

    /* �ж��Ƿ��ڴ�������� */
    if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_ABORTED,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();

        return VOS_TRUE;
    }

    ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

    if (0 == ulFreqNum)
    {
        /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
        CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                               CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                               CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

        /* �˳����״̬�� */
        CNAS_HSD_QuitFsmSysAcq_SysAcq();
    }
    else
    {
        /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
        CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);

        /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
        CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);
        CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvHsmConnOpenInd_WaitSessionInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSM_HSD_CONN_OPEN_IND_STRU         *pstOpenInd;
    CNAS_PRL_FREQENCY_CHANNEL_STRU      stFreq;
    CNAS_HSD_REACQ_FALG_ENUM_UINT32     enReAcqFlag;
    VOS_UINT32                          ulFreqNum;
    CNAS_PRL_FREQENCY_CHANNEL_STRU      astFreqList[CNAS_CAS_HRPD_MAX_FREQ_NUM];

    pstOpenInd          = (HSM_HSD_CONN_OPEN_IND_STRU *)pstMsg;
    stFreq.enBandClass  = CNAS_HSD_GetHistorySysList()->stCampedHrpdSysInfo[0].stFreq.enBandClass;
    stFreq.usChannel    = CNAS_HSD_GetHistorySysList()->stCampedHrpdSysInfo[0].stFreq.usChannel;
    enReAcqFlag         = CNAS_HSD_NOT_NEED_REACQ;

    /* avoidģ�鴦�� */
    CNAS_HSD_ProcAvoidEventInAccessRlstInd(pstOpenInd, &stFreq, &enReAcqFlag);

    /* �Ƿ���Ҫ�������� */
    if (CNAS_HSD_NEED_REACQ == enReAcqFlag)
    {
        /* ֹͣ������ʱ�� */
        CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_SESSION_NEG_RSLT_IND);

        /* �жϵ�ǰ�Ƿ��д�ϣ���������˳��������� */
        if (VOS_TRUE == CNAS_HSD_GetAbortFlg_SysAcq())
        {
            CNAS_HSD_SndSysAcqAbortRlst();

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();

            return VOS_TRUE;
        }

        ulFreqNum = CNAS_HSD_GetScanListSameLevelFreq(CNAS_CAS_HRPD_MAX_FREQ_NUM, astFreqList);

        if (0 == ulFreqNum)
        {
            /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�� */
            CNAS_HSD_SndSysAcqRslt(CNAS_HSD_SYSTEM_ACQUIRED_RESULT_FAILURE,
                                   CNAS_HSD_GetCurSysAcqScene_SysAcq(),
                                   CNAS_HSD_SYS_ACQ_FAIL_ACT_WAIT_TIMEOUT);

            /* �˳����״̬�� */
            CNAS_HSD_QuitFsmSysAcq_SysAcq();
        }
        else
        {
            /* ��CAS����ID_CNAS_CAS_HRPD_SYSTEM_SYNC_REQ */
            CNAS_HSD_SndCasSysSyncReq(ulFreqNum, astFreqList);;

            /* Ǩ�Ƶ� CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF״̬, ������������ʱ��TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF */
            CNAS_HSD_SetCurrFsmState(CNAS_HSD_SYS_ACQ_STA_WAIT_CAS_SYNC_CNF);

            CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF, TI_CNAS_HSD_WAIT_CAS_SYSTEM_SYNC_CNF_LEN);
        }
    }

    return VOS_TRUE;
}
/*lint -restore*/

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



