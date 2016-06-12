

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "CnasXsdMain.h"
#include "CnasXsdCtx.h"
#include "CnasTimerMgmt.h"
#include "CnasCcb.h"
#include "CnasMntn.h"
#include "CnasXsdFsmMainTbl.h"
#include "CnasXsdFsmSwitchOnTbl.h"
#include "CnasXsdFsmPowerOffTbl.h"
#include "CnasXsdFsmSysAcqTbl.h"
#include "CnasXsdPreProcTbl.h"
#include "CnasXsdFsmRedirTbl.h"

#include "CnasXsdPreProcAct.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_XSD_MAIN_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -save -e958*/

VOS_UINT32 CNAS_XSD_RegPreFsm(VOS_VOID)
{
    /* Ԥ����״̬��ע�� */
    NAS_FSM_RegisterFsm((CNAS_XSD_GetPreFsmDescAddr()),
                         "CNAS:FSM:XSD:PRE",
                         (VOS_UINT32)(CNAS_XSD_GetPreProcessStaTblSize()),
                         CNAS_XSD_GetPreProcStaTbl(),
                         VOS_NULL_PTR,
                         VOS_NULL_PTR);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RegMainL1Fsm(VOS_VOID)
{
    /* main״̬��ע�� */
    NAS_FSM_RegisterFsm((CNAS_XSD_GetMainFsmDescAddr()),
                         "CNAS:FSM:XSD:MAIN",
                         (VOS_UINT32)(CNAS_XSD_GetL1MainStaTblSize()),
                         CNAS_XSD_GetL1MainStaTbl(),
                         VOS_NULL_PTR,
                         CNAS_XSD_InitFsmCtx_L1Main);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_RegL2Fsm(VOS_VOID)
{
    /* SwitchOn״̬��ע�� */
    NAS_FSM_RegisterFsm((CNAS_XSD_GetSwitchOnFsmDescAddr()),
                         "CNAS:FSM:XSD:SwitchOn",
                         (VOS_UINT32)CNAS_XSD_GetSwitchOnStaTblSize(),
                         CNAS_XSD_GetSwitchOnStaTbl(),
                         VOS_NULL_PTR,
                         CNAS_XSD_InitFsmCtx_SwitchOn);

    /* PowerOff״̬��ע�� */
    NAS_FSM_RegisterFsm((CNAS_XSD_GetPowerOffFsmDescAddr()),
                         "CNAS:FSM:XSD:PowerOff",
                         (VOS_UINT32)CNAS_XSD_GetPowerOffStaTblSize(),
                         CNAS_XSD_GetPowerOffStaTbl(),
                         VOS_NULL_PTR,
                         CNAS_XSD_InitFsmCtx_PowerOff);


    /* SysAcq״̬��ע�� */
    NAS_FSM_RegisterFsm((CNAS_XSD_GetSysAcqFsmDescAddr()),
                         "CNAS:FSM:XSD:SysAcq",
                         (VOS_UINT32)CNAS_XSD_GetSysAcqStaTblSize(),
                         CNAS_XSD_GetSysAcqStaTbl(),
                         VOS_NULL_PTR,
                         CNAS_XSD_InitFsmCtx_SysAcq);

    /* redirection state machine register */
    NAS_FSM_RegisterFsm((CNAS_XSD_GetRedirFsmDescAddr()),
                         "CNAS:FSM:XSD:Redirection",
                         (VOS_UINT32)CNAS_XSD_GetRedirStaTblSize(),
                         CNAS_XSD_GetRedirStaTbl(),
                         VOS_NULL_PTR,
                         CNAS_XSD_InitFsmCtx_Redirection);


    return VOS_TRUE;
}


VOS_VOID  CNAS_XSD_RegFsm(VOS_VOID)
{
    CNAS_XSD_RegPreFsm();

    /* ע��L1״̬�� */
    CNAS_XSD_RegMainL1Fsm();

    /* ע��L2״̬�� */
    CNAS_XSD_RegL2Fsm();
}


VOS_UINT32 CNAS_XSD_PreProcessMsg(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet      = VOS_FALSE;
    NAS_ACTION_FUNC                     pActFun    = VOS_NULL_PTR;
    NAS_FSM_DESC_STRU                  *pstFsmDesc = VOS_NULL_PTR;

    /* ��ȡԤ����״̬����������ַ */
    pstFsmDesc = CNAS_XSD_GetPreFsmDescAddr();

    /* ��ȡ״̬ת�Ʊ��е���Ӧ�Ķ��������� */
    pActFun = NAS_FSM_FindAct(pstFsmDesc, CNAS_XSD_L1_STA_PREPROC, ulEventType);

    /* ������ָ���� */
    if (VOS_NULL_PTR != pActFun )
    {
        /* ִ�к������ܲ��� */
        ulRet = (*pActFun) ( ulEventType, pstMsg);
    }

    return ulRet;
}


VOS_UINT32 CNAS_XSD_FsmProcessEvent(
    VOS_UINT32                          ulCurState,
    VOS_UINT32                          ulEventType,
    VOS_VOID                           *pRcvMsg
)
{
    NAS_ACTION_FUNC                     pActFun;
    NAS_FSM_DESC_STRU                  *pstFsmDesc = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;

    CNAS_INFO_LOG2(UEPS_PID_XSD, "CNAS_XSD_FsmProcessEvent", ulCurState, ulEventType);

    pstFsmDesc = CNAS_XSD_GetCurFsmDesc();

    /* ����״̬ת�Ʊ��е���Ӧ�Ķ��������� */
    pActFun = NAS_FSM_FindAct(pstFsmDesc, ulCurState, ulEventType);

    if (VOS_NULL_PTR == pActFun)
    {
        /* ��Ϣδ������������������ */
        return VOS_FALSE;
    }

    /* ������ص��¼���������Ϊ��,�����������¼����� */
    ulRet   = (*pActFun) ( ulEventType,(struct MsgCB*)pRcvMsg);
    CNAS_INFO_LOG1(UEPS_PID_XSD, "CNAS_XSD_FsmProcessEvent", ulRet);

    return ulRet;
}


VOS_UINT32 CNAS_XSD_ProcessMsgInFsm(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRet;
    CNAS_XSD_FSM_CTX_STRU              *pstCurFsm = VOS_NULL_PTR;
    VOS_UINT16                          usOldStackDepth;
    VOS_UINT16                          usNewStackDepth;

    /* ��ȡ��ǰXSD��״̬�������� */
    pstCurFsm           = CNAS_XSD_GetCurFsmAddr();

    /* ��ȡ��ǰXSD��ջ��� */
    usOldStackDepth     = CNAS_XSD_GetFsmStackDepth();

    /* ��Ϣ���� */
    ulRet     = CNAS_XSD_FsmProcessEvent(pstCurFsm->ulState,
                                         ulEventType,
                                         (VOS_VOID*)pstMsg);

    /* ��ȡ��ǰXSD����ջ��� */
    usNewStackDepth    = CNAS_XSD_GetFsmStackDepth();

    /* ջ���û������,����û�д����µ�״̬���������˳�*/
    if (usNewStackDepth <= usOldStackDepth)
    {
        return ulRet;
    }

    /* �µ�ջ��ȴ����ϵ�ջ��� ����������ѹջ,��Ҫ����Ϣ����״̬���м������� */
    /* ���»�ȡ��ǰMMC�Ķ���״̬ */
    pstCurFsm   = CNAS_XSD_GetCurFsmAddr();

    /* ����Ϣ���붥��״̬������������ */
    ulRet       = CNAS_XSD_FsmProcessEvent(pstCurFsm->ulState,
                                           ulEventType,
                                           (VOS_VOID*)pstMsg);


    return ulRet;
}



VOS_VOID CNAS_XSD_ProcessXsdIntMsg(VOS_VOID)
{
    MSG_HEADER_STRU                    *pstIntMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulEventType;

    pstIntMsg = (MSG_HEADER_STRU *)CNAS_XSD_GetNextIntMsg();

    while (VOS_NULL_PTR != pstIntMsg)
    {
        /* ��Ϣ���� */
        CNAS_MNTN_TraceIntMsg(pstIntMsg->ulSenderPid,
                              pstIntMsg->ulLength + VOS_MSG_HEAD_LENGTH,
                              (VOS_VOID *)pstIntMsg);

        ulEventType  = CNAS_BuildEventType(pstIntMsg->ulSenderPid, (VOS_UINT16)pstIntMsg->ulMsgName);

        /* ����Ѿ����������ȡ������Ϣ�������� */
        if (VOS_TRUE == CNAS_XSD_PreProcessMsg(ulEventType, (struct MsgCB*)pstIntMsg))
        {
            /* �ͷ��ڲ���Ϣ */
            PS_MEM_FREE(pstIntMsg->ulSenderPid, pstIntMsg);

            /* ������һ����Ϣ���� */
            pstIntMsg = (MSG_HEADER_STRU *)CNAS_XSD_GetNextIntMsg();

            continue;
        }

        /* ��Ϣ��״̬������ */
        CNAS_XSD_ProcessMsgInFsm(ulEventType, (struct MsgCB *)pstIntMsg);

        /* �ͷ��ڲ���Ϣ */
        PS_MEM_FREE(pstIntMsg->ulSenderPid, pstIntMsg);

        /* ������һ����Ϣ���� */
        pstIntMsg = (MSG_HEADER_STRU *)CNAS_XSD_GetNextIntMsg();
    }

    return;
}


VOS_VOID CNAS_XSD_ProcessBufferMsg(VOS_VOID)
{
    CNAS_XSD_MSG_STRU                  *pstEntryMsg = VOS_NULL_PTR;

    /* �ڴ�ռ���� */
    if (0 < CNAS_XSD_GetCacheMsgNum())
    {
        pstEntryMsg = (CNAS_XSD_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_XSD, sizeof(CNAS_XSD_MSG_STRU));
    }

    if (VOS_NULL_PTR == pstEntryMsg)
    {
        return;
    }

    while (VOS_TRUE == CNAS_XSD_GetNextCachedMsg(pstEntryMsg))
    {
        /* ����Ѿ����������ȡ������Ϣ�������� */
        if (VOS_TRUE == CNAS_XSD_PreProcessMsg(pstEntryMsg->ulEventType, (struct MsgCB*)pstEntryMsg->aucMsgBuffer))
        {
            continue;
        }

        /* ����״̬������ */
        CNAS_XSD_ProcessMsgInFsm(pstEntryMsg->ulEventType, (struct MsgCB*)pstEntryMsg->aucMsgBuffer);

        CNAS_XSD_ProcessXsdIntMsg();
    }

    PS_MEM_FREE(UEPS_PID_XSD, pstEntryMsg);

    return;
}


VOS_VOID CNAS_XSD_MsgProc (struct MsgCB* pstMsg)
{
    MSG_HEADER_STRU                    *pstMsgHeader = VOS_NULL_PTR;
    REL_TIMER_MSG                      *pRcvTimerMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulEventType;
    VOS_UINT32                          usOldStackDepth;
    VOS_UINT32                          usNewStackDepth;
    VOS_UINT32                          ulSenderPid;


    ulSenderPid = CNAS_CCB_GetModem0Pid(CNAS_CCB_GetCdmaModeModemId(), pstMsg->ulSenderPid);

    pstMsg->ulSenderPid = ulSenderPid;

    pstMsgHeader = (MSG_HEADER_STRU *)pstMsg;

    /* �����¼����� */
    if (VOS_PID_TIMER == pstMsgHeader->ulSenderPid)
    {
        pRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

        ulEventType  = CNAS_BuildEventType(pstMsgHeader->ulSenderPid, pRcvTimerMsg->ulName);

        /* ֹͣ��Ӧ�Ķ�ʱ�� */
        CNAS_StopTimer(UEPS_PID_XSD, pRcvTimerMsg->ulName, pRcvTimerMsg->ulPara);
    }
    else
    {
        ulEventType  = CNAS_BuildEventType(pstMsgHeader->ulSenderPid, (VOS_UINT16)pstMsgHeader->ulMsgName);
    }

    /* Ԥ������ɹ���������״̬������ */
    if (VOS_TRUE == CNAS_XSD_PreProcessMsg(ulEventType, pstMsg))
    {
        return;
    }

    /* ��ȡ��ǰXSD��ջ��� */
    usOldStackDepth = CNAS_XSD_GetFsmStackDepth();



    /* ����״̬������ */
    CNAS_XSD_ProcessMsgInFsm(ulEventType, pstMsg);

    /* �����ڲ���Ϣ���� */
    CNAS_XSD_ProcessXsdIntMsg();

    /* ��״̬���˳��󣬻�ȡ�µ�ջ��� */
    usNewStackDepth = CNAS_XSD_GetFsmStackDepth();


    /* ���ջ����ȼ���,˵���϶���������ջ����,����һ�λ��� */
    if (usNewStackDepth < usOldStackDepth)
    {
        CNAS_XSD_ProcessBufferMsg();
    }

    return;
}


VOS_UINT32  CNAS_XSD_InitTask(VOS_VOID)
{
    /* ��ʼ��XSDȫ�ֱ��� */
    CNAS_XSD_InitCtx(CNAS_XSD_INIT_CTX_STARTUP);

    /* XSD ״̬��ע�� */
    CNAS_XSD_RegFsm();

    return VOS_TRUE;
}


VOS_UINT32 CNAS_XSD_InitPid(
    enum VOS_INIT_PHASE_DEFINE         enInitPhase
)
{
    switch (enInitPhase)
    {
        case VOS_IP_LOAD_CONFIG:
            /* ����XSDģ���ʼ������ */
            CNAS_XSD_InitTask();
            break;

        case VOS_IP_INITIAL:
        case VOS_IP_FARMALLOC:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
            break;

        default:
            break;
    }

    return VOS_OK;
}
/*lint -restore*/

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



