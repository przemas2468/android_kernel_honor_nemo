

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "CnasXccCtx.h"
#include "PsCommonDef.h"
#include "CnasXccFsmMoCallingTbl.h"
#include "CnasXccFsmMtCallingTbl.h"
#include "CnasXsdMntn.h"
#include "CnasXccFsmMainTbl.h"
#include "cas_1x_access_ctrl_proc_nas_pif.h"

#include "CnasXccProcNvim.h"
#include "NasMntn.h"

#include  "Nas_Mem.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_XCC_CTX_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/* XCC CTX,���ڱ���MMC״̬��,������ */
CNAS_XCC_CTX_STRU                       g_stCnasXccCtx;

/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -save -e958*/

CNAS_XCC_CTX_STRU* CNAS_XCC_GetCtxAddr(VOS_VOID)
{
    return &(g_stCnasXccCtx);
}


CNAS_XCC_FSM_STACK_STRU* CNAS_XCC_GetFsmStackAddr(VOS_VOID)
{
    return &(CNAS_XCC_GetCtxAddr()->pstCurFsmEntity->stFsmStack);
}


CNAS_XCC_FSM_CTX_STRU* CNAS_XCC_GetCurFsmAddr(VOS_VOID)
{
    return &(CNAS_XCC_GetCtxAddr()->pstCurFsmEntity->stCurFsm);
}


VOS_UINT32 CNAS_XCC_GetFsmTopState(VOS_VOID)
{
    return (CNAS_XCC_GetCurFsmAddr()->ulState);
}


VOS_UINT8 CNAS_XCC_GetFsmConnectId(VOS_VOID)
{
    return (CNAS_XCC_GetCurFsmAddr()->ucConnectId);
}



CNAS_XCC_MAIN_CTRL_CTX_STRU* CNAS_XCC_GetMainCtrlCtx(VOS_VOID)
{
    return &(CNAS_XCC_GetCtxAddr()->stMainCtrlCtx);
}


VOS_VOID CNAS_XCC_InitEmergencyCallCtrlCtx(
    CNAS_XCC_EMERGENCY_CALL_CTRL_STRU  *pstEmcCtrl
)
{
    pstEmcCtrl->ucWaitFlashMsgRspFlg    = VOS_FALSE;
    pstEmcCtrl->ucFlashMsgSeqNum        = 0;
    pstEmcCtrl->ucEmcCallId             = CNAS_XCC_INVAILD_CALL_ID;
    pstEmcCtrl->ucMtVoiceCallId         = CNAS_XCC_INVAILD_CALL_ID;
}



VOS_VOID CNAS_XCC_InitMainCtrlCtx(
    CNAS_XCC_INIT_TYPE_ENUM_UINT8       enInitType,
    CNAS_XCC_MAIN_CTRL_CTX_STRU        *pstMainCtrlCtx
)
{
    VOS_UINT32                          i;

    /* �ϵ�ʱ�޶�̬�ڴ����,����ֱ������ */
    /* �ػ���power saveʱ���ܴ��ڻ��������δ�ͷ��ڴ��ָ��,��Ҫ�ͷ��ڴ�,�����ڴ�й© */
    if ((CNAS_XCC_INIT_TYPE_POWER_DOWN == enInitType)
     || (CNAS_XCC_INIT_TYPE_POWER_SAVE == enInitType))
    {
        for (i = 0; i < (VOS_UINT32)pstMainCtrlCtx->stBufferMsgQueue.ucMsgNum; i++)
        {
            if (VOS_NULL_PTR != pstMainCtrlCtx->stBufferMsgQueue.pastBufferMsg[i])
            {
                PS_MEM_FREE(UEPS_PID_XCC, pstMainCtrlCtx->stBufferMsgQueue.pastBufferMsg[i]);
                pstMainCtrlCtx->stBufferMsgQueue.pastBufferMsg[i] = VOS_NULL_PTR;
            }
        }

        pstMainCtrlCtx->stBufferMsgQueue.ucMsgNum = 0;
    }

    NAS_MEM_SET_S(pstMainCtrlCtx,
                  sizeof(CNAS_XCC_MAIN_CTRL_CTX_STRU),
                  0,
                  sizeof(CNAS_XCC_MAIN_CTRL_CTX_STRU));

    CNAS_XCC_InitEmergencyCallCtrlCtx(&(pstMainCtrlCtx->stEmcCallCtrl));
}


VOS_VOID CNAS_XCC_InitCallNvimCfg(
    CNAS_XCC_CALL_NVIM_CFG_STRU        *pstCallNvimCfg
)
{
    NAS_MEM_SET_S(pstCallNvimCfg, sizeof(CNAS_XCC_CALL_NVIM_CFG_STRU), 0, sizeof(CNAS_XCC_CALL_NVIM_CFG_STRU));

}


VOS_VOID CNAS_XCC_Init1xPagingRspSoCfg(
    CNAS_XCC_1X_PAGING_RSP_SO_CFG_STRU *pstPagingRspSoCfg
)
{
    NAS_MEM_SET_S(pstPagingRspSoCfg, sizeof(CNAS_XCC_1X_PAGING_RSP_SO_CFG_STRU), 0, sizeof(CNAS_XCC_1X_PAGING_RSP_SO_CFG_STRU));
}



VOS_VOID CNAS_XCC_ReadNvimInfo()
{
    CNAS_XCC_ReadCallCfgNvim();

    CNAS_XCC_Read1xPagingRspSoCfg();

#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT)
    CNAS_XCC_ReadVoiceEncryptCapCfgPara();
#endif
}


VOS_VOID CNAS_XCC_InitCtx(CNAS_XCC_INIT_TYPE_ENUM_UINT8 enInitType)
{
    VOS_UINT32                          i;
    CNAS_XCC_FSM_ENTITY_STRU           *pstFsmEntity = VOS_NULL_PTR;

    for ( i = 0 ; i < CNAS_XCC_MAX_FSM_ENTITY_NUM ; i++ )
    {
        /* ��ȡÿ��״̬�� */
        pstFsmEntity = &(CNAS_XCC_GetCtxAddr()->astFsmEntity[i]);

        /* ״̬�������ĳ�ʼ�� */
        CNAS_XCC_InitCurrFsmCtx(enInitType, &(pstFsmEntity->stCurFsm));

        /* ״̬��ջ�����ʼ��   */
        CNAS_XCC_InitFsmStackCtx(&(pstFsmEntity->stFsmStack));

        /* call infoʵ���ʼ��   */
        CNAS_XCC_InitCallInfoCtx(&(pstFsmEntity->stCallInfoCtx), CNAS_XCC_INIT_CALLINFO_SCENE_POWER_ON);
    }

    CNAS_XCC_GetCtxAddr()->pstCurFsmEntity = VOS_NULL_PTR;

    /* CNAS XCC������Ϣ���г�ʼ�� */
    CNAS_XCC_InitInternalBuffer(&(CNAS_XCC_GetCtxAddr()->stIntMsgQueue));

    CNAS_XCC_InitMainCtrlCtx(enInitType, &(CNAS_XCC_GetCtxAddr()->stMainCtrlCtx));

    CNAS_XCC_ClearAllOrigCallOrderIndex();

    CNAS_XCC_INIT_SRID_INFO();

    CNAS_XCC_InitCallNvimCfg(&(CNAS_XCC_GetCtxAddr()->stCallNvimCfg));

    CNAS_XCC_Init1xPagingRspSoCfg(&(CNAS_XCC_GetCtxAddr()->stPagingRspSoCfg));

    CNAS_XCC_ReadNvimInfo();
}


VOS_VOID  CNAS_XCC_LoadSubFsm(
    CNAS_XCC_FSM_ID_ENUM_UINT32         enFsmId,
    CNAS_XCC_FSM_CTX_STRU              *pstCurFsm
)
{
    switch (enFsmId)
    {
        /* ����״̬�� */
        case CNAS_XCC_FSM_MO_CALLING :
            pstCurFsm->ulState    = CNAS_XCC_MO_CALLING_STA_INIT;
            pstCurFsm->pstFsmDesc = CNAS_XCC_GetMoCallingFsmDescAddr();
            break;

        /* ����״̬�� */
        case CNAS_XCC_FSM_MT_CALLING :
            pstCurFsm->ulState    = CNAS_XCC_MT_CALLING_STA_INIT;
            pstCurFsm->pstFsmDesc = CNAS_XCC_GetMtCallingFsmDescAddr();
            break;

        default:
            CNAS_ERROR_LOG(UEPS_PID_XCC, "CNAS_XCC_LoadSubFsm:Invalid Fsm Id");
            return;
    }

    /*ִ�г�ʼ��L2״̬���ĺ���*/
    if (VOS_NULL_PTR != pstCurFsm->pstFsmDesc->pfInitHandle)
    {
        pstCurFsm->pstFsmDesc->pfInitHandle();
    }
}



VOS_VOID CNAS_XCC_PushFsm(
    CNAS_XCC_FSM_STACK_STRU            *pstFsmStack,
    CNAS_XCC_FSM_CTX_STRU              *pstNewFsm
)
{
    CNAS_XCC_FSM_CTX_STRU              *pstCurFsm = VOS_NULL_PTR;

    /* ���ָ���� */
    if ((VOS_NULL_PTR == pstFsmStack)
     || (VOS_NULL_PTR == pstNewFsm))
    {
        CNAS_ERROR_LOG(UEPS_PID_XCC,
                       "CNAS_XCC_PushFsm: pstFsmStack or pstNewFsm is null pointer!");

        return;
    }

    /* ջ��ȼ�� */
    if (pstFsmStack->usStackDepth >= CNAS_XCC_MAX_FSM_STACK_DEPTH)
    {
        CNAS_ERROR_LOG1(UEPS_PID_XCC,
                       "CNAS_XCC_PushFsm: Fsm Stack Depth = %d Error!",
                       pstFsmStack->usStackDepth);

        return;
    }

    /* push��ǰ״̬����������ջ */
    pstCurFsm = &((pstFsmStack->astFsmStack)[pstFsmStack->usStackDepth]);

    NAS_MEM_CPY_S(pstCurFsm, sizeof(CNAS_XCC_FSM_CTX_STRU), pstNewFsm, sizeof(CNAS_XCC_FSM_CTX_STRU));

    /* ջ������� */
    pstFsmStack->usStackDepth++;
}


VOS_VOID CNAS_XCC_PopFsm(VOS_VOID)
{
    CNAS_XCC_FSM_STACK_STRU            *pstFsmStack = VOS_NULL_PTR;
    CNAS_XCC_FSM_CTX_STRU              *pstCurFsm   = VOS_NULL_PTR;

    /* ���ݲ���״̬��ջ������״̬����ȡ��ǰ״̬��������ַ  */
    pstFsmStack = CNAS_XCC_GetFsmStackAddr();
    pstCurFsm   = CNAS_XCC_GetCurFsmAddr();

    /* ���ջ�ռ��Ƿ�Ϊ�� */
    if (0 == pstFsmStack->usStackDepth)
    {
        CNAS_ERROR_LOG(UEPS_PID_XCC, "CNAS_XCC_PopFsm: FSM Stack is empty");

        return;
    }

    /* ״̬��ջ��ȼ��� */
    pstFsmStack->usStackDepth--;

    /* ���õ�ǰ״̬�� */
    NAS_MEM_CPY_S(pstCurFsm,
                  sizeof(CNAS_XCC_FSM_CTX_STRU),
                  &((pstFsmStack->astFsmStack)[pstFsmStack->usStackDepth]),
                  sizeof(CNAS_XCC_FSM_CTX_STRU));

}


VOS_VOID CNAS_XCC_InitFsmL2(
    CNAS_XCC_FSM_ID_ENUM_UINT32          enFsmId
)
{
    CNAS_XCC_FSM_STACK_STRU            *pstFsmStack = VOS_NULL_PTR;
    CNAS_XCC_FSM_CTX_STRU              *pstCurFsm   = VOS_NULL_PTR;
    CNAS_XCC_FSM_ID_ENUM_UINT32         enParentFsmId;
    VOS_UINT32                          ulParentEventType;

    /* ��ӡѹջǰ��״̬����״̬ID */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XCC, CNAS_XCC_GetCurrFsmId(), CNAS_XCC_GetFsmTopState(), CNAS_XCC_GetFsmConnectId());

    /* ��¼ѹջǰ��״̬�����������Ϣ���� */
    enParentFsmId     = CNAS_XCC_GetCurrFsmId();
    ulParentEventType = CNAS_XCC_GetCurFsmAddr()->stEntryMsg.ulEventType;

    /* ����ǰ״̬��ѹջ���� */
    pstFsmStack = CNAS_XCC_GetFsmStackAddr();
    pstCurFsm   = CNAS_XCC_GetCurFsmAddr();
    CNAS_XCC_PushFsm(pstFsmStack, pstCurFsm);

    /* ���õ�ǰ״̬�� */
    pstCurFsm->enFsmId = enFsmId;

    /* ����L2״̬�� */
    CNAS_XCC_LoadSubFsm(enFsmId, pstCurFsm);

    /* ��¼�ϲ�״̬�����������Ϣ���� */
    pstCurFsm->enParentFsmId     = enParentFsmId;
    pstCurFsm->ulParentEventType = ulParentEventType;

    /* ��ӡѹջǰ��״̬����״̬ID */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XCC, CNAS_XCC_GetCurrFsmId(), CNAS_XCC_GetFsmTopState(), CNAS_XCC_GetFsmConnectId());

    return;
}


VOS_VOID CNAS_XCC_QuitFsmL2(VOS_VOID)
{

    CNAS_XCC_FSM_CTX_STRU              *pstCurFsm   = VOS_NULL_PTR;

    pstCurFsm = CNAS_XCC_GetCurFsmAddr();

    /* QUITʱ��յ�ǰL2״̬����CTX */
    if (VOS_NULL_PTR != pstCurFsm->pstFsmDesc->pfInitHandle)
    {
        pstCurFsm->pstFsmDesc->pfInitHandle();
    }

    /*  ��ӡ��ǰ��״̬��ID�Լ�״̬ */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XCC, CNAS_XCC_GetCurrFsmId(), CNAS_XCC_GetFsmTopState(), CNAS_XCC_GetFsmConnectId());

    /* ״̬����ջ���� */
    CNAS_XCC_PopFsm();

    /*  ��ӡ��ǰ��״̬��ID�Լ�״̬ */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XCC, CNAS_XCC_GetCurrFsmId(), CNAS_XCC_GetFsmTopState(), CNAS_XCC_GetFsmConnectId());

    return;
}


VOS_UINT16 CNAS_XCC_GetFsmStackDepth(VOS_VOID)
{
    CNAS_XCC_FSM_STACK_STRU            *pstFsmStack = VOS_NULL_PTR;

    pstFsmStack = CNAS_XCC_GetFsmStackAddr();

    return pstFsmStack->usStackDepth;
}


CNAS_XCC_FSM_ID_ENUM_UINT32 CNAS_XCC_GetCurrFsmId(VOS_VOID)
{
    return (CNAS_XCC_GetCtxAddr()->pstCurFsmEntity->stCurFsm.enFsmId);
}


NAS_FSM_DESC_STRU* CNAS_XCC_GetCurFsmDesc(VOS_VOID)
{
    return (CNAS_XCC_GetCurFsmAddr()->pstFsmDesc);
}


VOS_UINT8 CNAS_XCC_GetBufferedMsgNum(VOS_VOID)
{
    return (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.ucMsgNum);
}


VOS_UINT8 *CNAS_XCC_GetNextBufferMsg(VOS_VOID)
{
    return (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[0]);
}


VOS_VOID CNAS_XCC_FreeCurrentBufferMsg(VOS_VOID)
{
    VOS_UINT8                           ucMsgNum;

    ucMsgNum = CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.ucMsgNum;

    /* �쳣���� */
    if (0 == ucMsgNum)
    {
        return;
    }

    ucMsgNum--;

    /* ���»�����Ϣ��Ŀ */
    CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.ucMsgNum = ucMsgNum;

    if (VOS_NULL_PTR != (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[0]))
    {
        PS_MEM_FREE(UEPS_PID_XCC, (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[0]));

        (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[0]) = VOS_NULL_PTR;
    }

    if (0 < ucMsgNum)
    {
        NAS_MEM_MOVE_S(&(CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[0]),
                       CNAS_XCC_MAX_BUFFER_MSG_QUEUE_NUM * sizeof(VOS_UINT8 *),
                       &(CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[1]),
                       (ucMsgNum) * sizeof(VOS_UINT8 *));
    }

    (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucMsgNum]) = VOS_NULL_PTR;

}


VOS_UINT8  *CNAS_XCC_GetSpecifiedIndexBufferMsg(
    VOS_UINT8                           ucIndex
)
{
    return (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucIndex]);
}


VOS_VOID CNAS_XCC_FreeSpecifiedIndexBufferMsg(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucMsgNum;

    ucMsgNum = CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.ucMsgNum;

    if (0 == ucMsgNum)
    {
        return;
    }

    ucMsgNum--;

    /* ���»�����Ϣ��Ŀ */
    CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.ucMsgNum = ucMsgNum;

    if (VOS_NULL_PTR != (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucIndex]))
    {
        PS_MEM_FREE(UEPS_PID_XCC, (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucIndex]));

        (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucIndex]) = VOS_NULL_PTR;
    }

    if (0 < ucMsgNum)
    {
        NAS_MEM_MOVE_S(&(CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucIndex]),
                       (CNAS_XCC_MAX_BUFFER_MSG_QUEUE_NUM - ucIndex) * sizeof(VOS_UINT8 *),
                       &(CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucIndex + 1]),
                       (ucMsgNum - ucIndex) * sizeof(VOS_UINT8 *));
    }

    (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucMsgNum]) = VOS_NULL_PTR;
}


VOS_VOID CNAS_XCC_FreeAllBufferMsgInQueue(VOS_VOID)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulMsgNum;

    ulMsgNum = (CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.ucMsgNum);

    for (ulIndex = 0; ulIndex < ulMsgNum; ulIndex++)
    {
        CNAS_XCC_FreeSpecifiedIndexBufferMsg((VOS_UINT8)ulIndex);
    }
}



VOS_VOID CNAS_XCC_BufferMsgInQueue(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    MSG_HEADER_STRU                     *pstMsgHdr;

    pstMsgHdr = (MSG_HEADER_STRU *)pstMsg;

    ucIndex = CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.ucMsgNum;

    if (ucIndex < CNAS_XCC_MAX_BUFFER_MSG_QUEUE_NUM)
    {
        if (CNAS_XCC_MAX_MSG_BUFFER_LEN < pstMsg->ulLength)
        {
            CNAS_ERROR_LOG( UEPS_PID_XCC, "CNAS_XCC_BufferMsgInQueue: message is too length!");
            return;
        }

        CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucIndex]
                = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_XCC, pstMsgHdr->ulLength + VOS_MSG_HEAD_LENGTH);

        if (VOS_NULL_PTR == CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucIndex])
        {
            return;
        }

        NAS_MEM_CPY_S((CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.pastBufferMsg[ucIndex]),
                       pstMsgHdr->ulLength + VOS_MSG_HEAD_LENGTH,
                       pstMsgHdr,
                       pstMsgHdr->ulLength + VOS_MSG_HEAD_LENGTH);

        CNAS_XCC_GetMainCtrlCtx()->stBufferMsgQueue.ucMsgNum++;
    }
}


CNAS_XCC_INT_MSG_QUEUE_STRU* CNAS_XCC_GetIntMsgQueueAddr(VOS_VOID)
{
    return &(CNAS_XCC_GetCtxAddr()->stIntMsgQueue);
}


VOS_VOID CNAS_XCC_InitInternalBuffer(
    CNAS_XCC_INT_MSG_QUEUE_STRU        *pstIntMsgQueue
)
{
    VOS_UINT8                          i;

    for (i = 0; i < CNAS_XCC_MAX_INT_MSG_QUEUE_NUM; i++)
    {
        pstIntMsgQueue->pastIntMsg[i] = VOS_NULL_PTR;
    }

    pstIntMsgQueue->ucIntMsgNum = 0;
}


VOS_VOID CNAS_XCC_PutMsgInIntMsgQueue(
    VOS_UINT8                          *pstMsg
)
{
    VOS_UINT8                           ucIntMsgNum;

    if (CNAS_XCC_MAX_INT_MSG_QUEUE_NUM <= CNAS_XCC_GetIntMsgNum())
    {
        /* �ͷ��ڲ���Ϣ */
        PS_MEM_FREE(UEPS_PID_XCC, pstMsg);

        CNAS_ERROR_LOG( UEPS_PID_XCC, "CNAS_XCC_PutMsgInIntMsgQueue: msg queue is full!" );

        return;
    }

    ucIntMsgNum = CNAS_XCC_GetIntMsgQueueAddr()->ucIntMsgNum;

    /* ���浱ǰ��Ϣ */
    CNAS_XCC_GetIntMsgQueueAddr()->pastIntMsg[ucIntMsgNum] = pstMsg;

    CNAS_XCC_GetIntMsgQueueAddr()->ucIntMsgNum = (ucIntMsgNum + 1);

    return;
}


VOS_UINT8 *CNAS_XCC_GetNextIntMsg(VOS_VOID)
{
    VOS_UINT8                           ucIntMsgNum;
    VOS_UINT8                          *pstIntMsg = VOS_NULL_PTR;

    ucIntMsgNum = CNAS_XCC_GetIntMsgQueueAddr()->ucIntMsgNum;

    if (0 < ucIntMsgNum)
    {
        /* ��ȡ��һ���������Ϣ��Ϣ */
        pstIntMsg = CNAS_XCC_GetIntMsgQueueAddr()->pastIntMsg[0];

        /* �ڲ���Ϣ��Ŀ����һ�� */
        ucIntMsgNum--;

        if (0 != ucIntMsgNum)
        {
            /* ǰ���ڲ���Ϣ */
            NAS_MEM_MOVE_S(&(CNAS_XCC_GetIntMsgQueueAddr()->pastIntMsg[0]),
                           CNAS_XCC_MAX_INT_MSG_QUEUE_NUM * sizeof(VOS_UINT8 *),
                           &(CNAS_XCC_GetIntMsgQueueAddr()->pastIntMsg[1]),
                           ucIntMsgNum * sizeof(VOS_UINT8 *));
        }

        /* ����Ϣ������β������Ϣ��Ϊ��ָ�� */
        CNAS_XCC_GetIntMsgQueueAddr()->pastIntMsg[ucIntMsgNum] = VOS_NULL_PTR;

        CNAS_XCC_GetIntMsgQueueAddr()->ucIntMsgNum = ucIntMsgNum;
    }

    return pstIntMsg;
}


VOS_UINT32 CNAS_XCC_GetIntMsgNum(VOS_VOID)
{
    CNAS_XCC_INT_MSG_QUEUE_STRU      *pstMsgQueue = VOS_NULL_PTR;

    pstMsgQueue = CNAS_XCC_GetIntMsgQueueAddr();

    return pstMsgQueue->ucIntMsgNum;
}


VOS_VOID CNAS_XCC_InitFsmCtx_L1Main(VOS_VOID)
{

    /* ��ʱû����Ҫ��ʼ����ȫ�ֱ����������б������ʼ�����ڴ���� */
}


VOS_VOID CNAS_XCC_InitFsmCtx_MoCalling(VOS_VOID)
{
    /* ��ʱû����Ҫ��ʼ����ȫ�ֱ����������б������ʼ�����ڴ���� */
}


VOS_VOID CNAS_XCC_InitFsmCtx_MtCalling(VOS_VOID)
{
    /* ��ʱû����Ҫ��ʼ����ȫ�ֱ����������б������ʼ�����ڴ���� */
}


VOS_VOID  CNAS_XCC_InitCurrFsmCtx(
    CNAS_XCC_INIT_TYPE_ENUM_UINT8       enInitType,
    CNAS_XCC_FSM_CTX_STRU              *pstCurrFsmCtx
)
{
    /* ��ǰ��״̬�������ʼ�� */
    pstCurrFsmCtx->enFsmId                  = CNAS_XCC_FSM_L1_MAIN;
    pstCurrFsmCtx->pstFsmDesc               = CNAS_XCC_GetMainFsmDescAddr();

    /* ֻ�йػ�״̬�ó�POWER_OFF�������͹���״̬���ó�IDLE */
    if (CNAS_XCC_INIT_TYPE_POWER_DOWN == enInitType)
    {
        pstCurrFsmCtx->ulState              = CNAS_XCC_L1_STA_POWER_OFF;
    }
    else
    {
        pstCurrFsmCtx->ulState              = CNAS_XCC_L1_STA_IDLE;
    }

    pstCurrFsmCtx->stEntryMsg.ulEventType   = 0;
    pstCurrFsmCtx->enParentFsmId            = CNAS_XCC_FSM_BUTT;
    pstCurrFsmCtx->ulParentEventType        = 0;

    pstCurrFsmCtx->ucConnectId              = CNAS_XCC_INVAILD_CON_ID;

    NAS_MEM_SET_S(pstCurrFsmCtx->ucReserved, sizeof(pstCurrFsmCtx->ucReserved), 0x00, sizeof(pstCurrFsmCtx->ucReserved));

    NAS_MEM_SET_S(pstCurrFsmCtx->stEntryMsg.aucMsgBuffer,
                  CNAS_XCC_MAX_MSG_BUFFER_LEN,
                  0x00,
                  CNAS_XCC_MAX_MSG_BUFFER_LEN);
}



VOS_VOID  CNAS_XCC_InitFsmStackCtx(
    CNAS_XCC_FSM_STACK_STRU            *pstFsmStack
)
{
    VOS_UINT32                          i;

    pstFsmStack->usStackDepth   = 0;

    for ( i = 0 ; i < CNAS_XCC_MAX_FSM_STACK_DEPTH ; i++ )
    {
        pstFsmStack->astFsmStack[i].pstFsmDesc  = VOS_NULL_PTR;
        pstFsmStack->astFsmStack[i].enFsmId     = CNAS_XCC_FSM_BUTT;
        pstFsmStack->astFsmStack[i].ulState     = CNAS_XCC_L1_STA_BUTT;
    }
}


VOS_VOID  CNAS_XCC_InitCallInfoCtx(
    CNAS_XCC_CALL_INFO_CTX_STRU                            *pstCallInfo,
    CNAS_XCC_INIT_CALLINFO_SCENE_ENUM_UINT8                 enScene
)
{
    VOS_UINT8                                               uc1XCallExistCount;

    uc1XCallExistCount                                      = CNAS_CCB_Get1XCallExistCount();

    /* ��ʼ����������ʱ��������ENDEDʱ������Ҫ��SO/CONNECT ID/CONNECT REF��Ϣ��� */
    if ( (CNAS_XCC_INIT_CALLINFO_SCENE_POWER_ON == enScene)
      || (CNAS_XCC_INIT_CALLINFO_SCENE_ENDED == enScene) )
    {
        pstCallInfo->ucIsNullCc           = VOS_FALSE;
        pstCallInfo->ucIsEntityActive     = VOS_FALSE;
        pstCallInfo->ucConnectId          = CNAS_XCC_INVAILD_CON_ID;
        pstCallInfo->ucConRef             = CNAS_XCC_INVALID_CON_REF;
        pstCallInfo->enInitSo             = CAS_CNAS_1X_SO_BUTT;
        pstCallInfo->enCurrentSo          = CAS_CNAS_1X_SO_BUTT;
    }

    if (CNAS_XCC_INIT_CALLINFO_SCENE_ENDED == enScene)
    {
        if (uc1XCallExistCount > 0)
        {
            uc1XCallExistCount--;
        }

        CNAS_CCB_Set1XCallExistCount(uc1XCallExistCount);
    }

    /* ENDING����ֻ��Ҫ�����沿����Ϣ��ʼ������ */
    pstCallInfo->bitOpCallingPartyNum     = VOS_FALSE;
    pstCallInfo->bitOpCallingPartySubAddr = VOS_FALSE;
    pstCallInfo->bitOpCalledPartySubAddr  = VOS_FALSE;
    pstCallInfo->bitOpQosInfo             = VOS_FALSE;
    pstCallInfo->bitOpSpare               = 0;


    pstCallInfo->ucCallId                 = CNAS_XCC_INVAILD_CALL_ID;
    pstCallInfo->ucIsMoCall               = VOS_FALSE;

    pstCallInfo->ucIsByPassAlertAns       = VOS_FALSE;
    /* ɾ��ucIsStartDtmfAckWait,ucIsStopDtmfAckWait������Ԫ�� */
    pstCallInfo->ucIsBurstDtmfAckWait     = VOS_FALSE;
    pstCallInfo->ucIsSendFlashAckWait     = VOS_FALSE;
    pstCallInfo->ucIsCodecOpen            = VOS_FALSE;
    pstCallInfo->ucIsSoNegCmplt           = VOS_FALSE;
    pstCallInfo->ucMsgSeqNum              = 0;
    pstCallInfo->ucQosInfoLen             = 0;
    NAS_MEM_SET_S(pstCallInfo->aucQosInfo, sizeof(pstCallInfo->aucQosInfo), 0x00, sizeof(pstCallInfo->aucQosInfo));

    NAS_MEM_SET_S(&(pstCallInfo->stCalledPartyNum), sizeof(pstCallInfo->stCalledPartyNum), 0x00, sizeof(pstCallInfo->stCalledPartyNum));
    NAS_MEM_SET_S(&(pstCallInfo->stCallingPartyNum), sizeof(pstCallInfo->stCallingPartyNum), 0x00, sizeof(pstCallInfo->stCallingPartyNum));
    NAS_MEM_SET_S(&(pstCallInfo->stCallingPartySubAddr), sizeof(pstCallInfo->stCallingPartySubAddr), 0x00, sizeof(pstCallInfo->stCallingPartySubAddr));
    NAS_MEM_SET_S(&(pstCallInfo->stCalledPartySubAddr), sizeof(pstCallInfo->stCalledPartySubAddr), 0x00, sizeof(pstCallInfo->stCalledPartySubAddr));

    pstCallInfo->ucReOrigCount        = 0;
    pstCallInfo->ucIsL2ErrReOrig      = VOS_FALSE;
    pstCallInfo->ucIsEmergencyCall    = VOS_FALSE;
    pstCallInfo->ucIsDataReady        = VOS_FALSE;
    pstCallInfo->ucIsReOrigReason     = VOS_FALSE;
    pstCallInfo->ucIsDormDataCall     = VOS_FALSE;

    NAS_MEM_SET_S(&(pstCallInfo->stAWIorEAWIInfoRecoBuffer), sizeof(pstCallInfo->stAWIorEAWIInfoRecoBuffer), 0x00, sizeof(pstCallInfo->stAWIorEAWIInfoRecoBuffer));
    pstCallInfo->stAWIorEAWIInfoRecoBuffer.ucIsBufferValid = VOS_FALSE;

    pstCallInfo->ucIsDataSrvConn      = VOS_FALSE;
    pstCallInfo->ucSrId               = CNAS_XCC_INVALID_SR_ID;

    pstCallInfo->ulIsPrevHrpdFlg      = VOS_FALSE;

    pstCallInfo->enReqPrivacyMode     = XCALL_XCC_PRIVACY_MODE_BUTT;
    pstCallInfo->enNegPrivacyMode     = XCALL_XCC_PRIVACY_MODE_BUTT;

    return;
}



VOS_VOID  CNAS_XCC_InitIntMsgQueue(
    CNAS_XCC_INT_MSG_QUEUE_STRU      *pstIntMsgQueue
)
{
    pstIntMsgQueue->ucIntMsgNum = 0;

    NAS_MEM_SET_S(pstIntMsgQueue->pastIntMsg,
                  CNAS_XCC_MAX_INT_MSG_QUEUE_NUM,
                  0x00,
                  CNAS_XCC_MAX_INT_MSG_QUEUE_NUM);
}


VOS_VOID  CNAS_XCC_SetCurrFsmState(
    VOS_UINT32                          ulCurrState
)
{
    CNAS_XCC_FSM_CTX_STRU              *pstCurFsm   = VOS_NULL_PTR;

    /* ��ȡ��ǰ��״̬��Ϣ */
    pstCurFsm                   = CNAS_XCC_GetCurFsmAddr();

    /* ���õ�ǰ״̬ */
    pstCurFsm->ulState          = ulCurrState;

    /*  ��ӡ��ǰ�ĵ�ǰ״̬��ID�Լ�״̬ */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XCC, pstCurFsm->enFsmId, ulCurrState, pstCurFsm->ucConnectId);

    return;
}


CNAS_XCC_CALL_INFO_CTX_STRU*  CNAS_XCC_GetCurrFsmEntityCallInfoCtx( VOS_VOID )
{
    return &(g_stCnasXccCtx.pstCurFsmEntity->stCallInfoCtx);
}


CNAS_XCC_ENTRY_MSG_STRU* CNAS_XCC_GetCurrFsmMsgAddr(VOS_VOID)
{
    return &(g_stCnasXccCtx.pstCurFsmEntity->stCurFsm.stEntryMsg);
}


CAS_CNAS_1X_SERVICE_OPTION_ENUM_UINT16 CNAS_XCC_GetInitSo(VOS_VOID)
{
    return (g_stCnasXccCtx.pstCurFsmEntity->stCallInfoCtx.enInitSo);
}


VOS_VOID CNAS_XCC_SetInitSo(CAS_CNAS_1X_SERVICE_OPTION_ENUM_UINT16 enSo)
{
    g_stCnasXccCtx.pstCurFsmEntity->stCallInfoCtx.enInitSo = enSo;
}


CAS_CNAS_1X_SERVICE_OPTION_ENUM_UINT16 CNAS_XCC_GetCurrentSo(VOS_VOID)
{
    return (g_stCnasXccCtx.pstCurFsmEntity->stCallInfoCtx.enCurrentSo);
}


VOS_VOID CNAS_XCC_SetCurrentSo(CAS_CNAS_1X_SERVICE_OPTION_ENUM_UINT16 enSo)
{
    g_stCnasXccCtx.pstCurFsmEntity->stCallInfoCtx.enCurrentSo = enSo;
}



VOS_VOID CNAS_XCC_SaveCurEntryMsg(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XCC_ENTRY_MSG_STRU            *pstEntryMsg  = VOS_NULL_PTR;
    MSG_HEADER_STRU                    *pstMsgHeader = VOS_NULL_PTR;
    VOS_UINT32                          ulLen;

    /* ��õ�ǰ����NAS_MMC�����Ϣ�Ļ�������ַ */
    pstEntryMsg                 = CNAS_XCC_GetCurrFsmMsgAddr();
    pstMsgHeader                = (MSG_HEADER_STRU*)pstMsg;

    /* ����ǿ���Ϣ */
    if ( VOS_NULL_PTR == pstMsg )
    {
        pstEntryMsg->ulEventType    =   ulEventType;
        CNAS_WARNING_LOG( UEPS_PID_XCC, "CNAS_XCC_SaveCurEntryMsg Entry Message Pointer is NULL!" );
        return;
    }

    ulLen                       = pstMsgHeader->ulLength + VOS_MSG_HEAD_LENGTH;

    if (ulLen > CNAS_XCC_MAX_MSG_BUFFER_LEN)
    {
        CNAS_WARNING_LOG(UEPS_PID_XCC, "CNAS_XCC_SaveCurEntryMsg:beyond the Size!");

        ulLen = CNAS_XCC_MAX_MSG_BUFFER_LEN;
    }

    /* ����ǰ����������Ϣ������Context��Ӧ�Ļ����� */
    NAS_MEM_CPY_S( pstEntryMsg->aucMsgBuffer, CNAS_XCC_MAX_MSG_BUFFER_LEN, pstMsg, ulLen );

    /* ��дContext�еĲ��� */
    pstEntryMsg->ulEventType    =   ulEventType;

    return;
}


VOS_UINT8 CNAS_XCC_AllocSeqNum(VOS_VOID)
{
    VOS_UINT8                           ucSeqNum;

    ucSeqNum = g_stCnasXccCtx.stMainCtrlCtx.ucSeqNum;

    g_stCnasXccCtx.stMainCtrlCtx.ucSeqNum = (g_stCnasXccCtx.stMainCtrlCtx.ucSeqNum + 1)%CNAS_XCC_MAX_UINT8;

    return ucSeqNum;
}


VOS_UINT32 CNAS_XCC_BufferAWIOrEAWIInfoReco(
    CAS_CNAS_1X_DSCH_DATA_IND_STRU     *pstDschDataInd,
    CNAS_XCC_CALL_INFO_CTX_STRU        *pstCallInfoCtx
)
{
    CNAS_XCC_AWIM_STRU                 *pstAWIM;
    CNAS_XCC_EAWIM_STRU                *pstEAWIM;

    if (CAS_CNAS_1X_RX_TCH_ALERT_WITH_INFORMATION_MSG == pstDschDataInd->enDschMsgType)
    {
        pstAWIM = (CNAS_XCC_AWIM_STRU *)PS_MEM_ALLOC(UEPS_PID_XCC, sizeof(CNAS_XCC_AWIM_STRU));
        if (VOS_NULL_PTR == pstAWIM)
        {
            return VOS_ERR;
        }

        if (VOS_OK != CNAS_XCC_DecodeAlertWithInformationMsg(pstDschDataInd, pstAWIM))
        {
            PS_MEM_FREE(UEPS_PID_XCC, pstAWIM);
            return VOS_ERR;
        }

        /* ����info record */
        if (0 != pstAWIM->ucNumInfoRecs)
        {
            pstCallInfoCtx->stAWIorEAWIInfoRecoBuffer.ucIsBufferValid = VOS_TRUE;
            pstCallInfoCtx->stAWIorEAWIInfoRecoBuffer.enMsgType       = CAS_CNAS_1X_RX_TCH_ALERT_WITH_INFORMATION_MSG;
            NAS_MEM_CPY_S(&(pstCallInfoCtx->stAWIorEAWIInfoRecoBuffer.u.stAWIM),
                          sizeof(CNAS_XCC_AWIM_STRU),
                          pstAWIM,
                          sizeof(CNAS_XCC_AWIM_STRU));
        }

        PS_MEM_FREE(UEPS_PID_XCC, pstAWIM);
    }
    else
    {
        pstEAWIM = (CNAS_XCC_EAWIM_STRU *)PS_MEM_ALLOC(UEPS_PID_XCC, sizeof(CNAS_XCC_EAWIM_STRU));
        if (VOS_NULL_PTR == pstEAWIM)
        {
            return VOS_ERR;
        }

        if (VOS_OK != CNAS_XCC_DecodeExtendedAlertWithInformationMsg(pstDschDataInd, pstEAWIM))
        {
            PS_MEM_FREE(UEPS_PID_XCC, pstEAWIM);
            return VOS_ERR;
        }

        /* ����info record */
        if (0 != pstEAWIM->ucNumInfoRecs)
        {
            pstCallInfoCtx->stAWIorEAWIInfoRecoBuffer.ucIsBufferValid = VOS_TRUE;
            pstCallInfoCtx->stAWIorEAWIInfoRecoBuffer.enMsgType       = CAS_CNAS_1X_RX_TCH_EXT_ALERT_WITH_INFORMATION_MSG;
            NAS_MEM_CPY_S(&(pstCallInfoCtx->stAWIorEAWIInfoRecoBuffer.u.stEAWIM),
                          sizeof(CNAS_XCC_EAWIM_STRU),
                          pstEAWIM,
                          sizeof(CNAS_XCC_EAWIM_STRU));
        }

        PS_MEM_FREE(UEPS_PID_XCC, pstEAWIM);
    }

    return VOS_OK;
}


VOS_VOID CNAS_XCC_ClearAWIOrEAWIInfoRecoBuffer(VOS_VOID)
{
    CNAS_XCC_CALL_INFO_CTX_STRU        *pstCallInfoCtx = VOS_NULL_PTR;

    pstCallInfoCtx = CNAS_XCC_GetCurrFsmEntityCallInfoCtx();

    if (VOS_TRUE == pstCallInfoCtx->stAWIorEAWIInfoRecoBuffer.ucIsBufferValid)
    {
        NAS_MEM_SET_S(&(pstCallInfoCtx->stAWIorEAWIInfoRecoBuffer),
                      sizeof(CNAS_XCC_AWI_EAWI_INFO_RECO_BUFFER_STRU),
                      0,
                      sizeof(CNAS_XCC_AWI_EAWI_INFO_RECO_BUFFER_STRU));

        pstCallInfoCtx->stAWIorEAWIInfoRecoBuffer.ucIsBufferValid = VOS_FALSE;
    }

    return;
}


VOS_VOID CNAS_XCC_SetWaitEmcFlashMsgRspFlg(
    VOS_UINT8                           ucFlg
)
{
    CNAS_XCC_MAIN_CTRL_CTX_STRU        *pstMainCtrl = VOS_NULL_PTR;

    pstMainCtrl = CNAS_XCC_GetMainCtrlCtx();

    pstMainCtrl->stEmcCallCtrl.ucWaitFlashMsgRspFlg = ucFlg;

    return;
}


VOS_UINT8 CNAS_XCC_GetWaitEmcFlashMsgRspFlg(VOS_VOID)
{
    CNAS_XCC_MAIN_CTRL_CTX_STRU        *pstMainCtrl = VOS_NULL_PTR;

    pstMainCtrl = CNAS_XCC_GetMainCtrlCtx();

    return pstMainCtrl->stEmcCallCtrl.ucWaitFlashMsgRspFlg;
}


VOS_VOID CNAS_XCC_SaveEmcCallId(
    VOS_UINT8                           ucCallId
)
{
    CNAS_XCC_MAIN_CTRL_CTX_STRU        *pstMainCtrl = VOS_NULL_PTR;

    pstMainCtrl = CNAS_XCC_GetMainCtrlCtx();

    pstMainCtrl->stEmcCallCtrl.ucEmcCallId = ucCallId;

    return;
}


VOS_UINT8 CNAS_XCC_GetEmcCallId(VOS_VOID)
{
    CNAS_XCC_MAIN_CTRL_CTX_STRU        *pstMainCtrl = VOS_NULL_PTR;

    pstMainCtrl = CNAS_XCC_GetMainCtrlCtx();

    return pstMainCtrl->stEmcCallCtrl.ucEmcCallId;
}


VOS_VOID CNAS_XCC_SaveMtVoiceCallId(
    VOS_UINT8                           ucCallId
)
{
    CNAS_XCC_MAIN_CTRL_CTX_STRU        *pstMainCtrl = VOS_NULL_PTR;

    pstMainCtrl = CNAS_XCC_GetMainCtrlCtx();

    pstMainCtrl->stEmcCallCtrl.ucMtVoiceCallId = ucCallId;

    return;
}


VOS_UINT8 CNAS_XCC_GetMtVoiceCallId(VOS_VOID)
{
    CNAS_XCC_MAIN_CTRL_CTX_STRU        *pstMainCtrl = VOS_NULL_PTR;

    pstMainCtrl = CNAS_XCC_GetMainCtrlCtx();

    return pstMainCtrl->stEmcCallCtrl.ucMtVoiceCallId;
}




VOS_VOID CNAS_XCC_SaveEmcFlashMsgSeqNum(
    VOS_UINT8                           ucSeqNum
)
{
    CNAS_XCC_MAIN_CTRL_CTX_STRU        *pstMainCtrl = VOS_NULL_PTR;

    pstMainCtrl = CNAS_XCC_GetMainCtrlCtx();

    pstMainCtrl->stEmcCallCtrl.ucFlashMsgSeqNum = ucSeqNum;

    return;
}


VOS_UINT8 CNAS_XCC_GetEmcFlashMsgSeqNum(VOS_VOID)
{
    CNAS_XCC_MAIN_CTRL_CTX_STRU        *pstMainCtrl = VOS_NULL_PTR;

    pstMainCtrl = CNAS_XCC_GetMainCtrlCtx();

    return pstMainCtrl->stEmcCallCtrl.ucFlashMsgSeqNum;
}



VOS_UINT32   CNAS_XCC_AllocSrId(
    CAS_CNAS_1X_SERVICE_OPTION_ENUM_UINT16      enSo,
    VOS_UINT8                                  *pucSrId,
    VOS_UINT8                                   ucConnectId
)
{
    CNAS_XCC_CTX_STRU                  *pstXccCtxAddr   = VOS_NULL_PTR;
    VOS_UINT8                           i;

    pstXccCtxAddr = CNAS_XCC_GetCtxAddr();

    if ((CAS_CNAS_1X_SO_33_PPP_PKT_DATA_CDMA2K == enSo)
     || (CAS_CNAS_1X_SO_7_PPP_PKT_DATA == enSo))
    {
        if (VOS_FALSE == pstXccCtxAddr->astSridInfo[CNAS_XCC_RESERVED_SR_ID].ucIsUsed)
        {
            *pucSrId = CNAS_XCC_RESERVED_SR_ID;
            pstXccCtxAddr->astSridInfo[CNAS_XCC_RESERVED_SR_ID].ucIsUsed      = VOS_TRUE;
            pstXccCtxAddr->astSridInfo[CNAS_XCC_RESERVED_SR_ID].ucSrid        = *pucSrId;
            pstXccCtxAddr->astSridInfo[CNAS_XCC_RESERVED_SR_ID].enSo          = enSo;
            pstXccCtxAddr->astSridInfo[CNAS_XCC_RESERVED_SR_ID].ucConnectId   = ucConnectId;
            return VOS_TRUE;
        }
    }

    for (i = CNAS_XCC_RESERVED_SR_ID + 1 ; i <= CNAS_XCC_MAX_FSM_ENTITY_NUM; i++)
    {
        if (VOS_FALSE == pstXccCtxAddr->astSridInfo[i].ucIsUsed)
        {
            *pucSrId = i;
            pstXccCtxAddr->astSridInfo[i].ucIsUsed      = VOS_TRUE;
            pstXccCtxAddr->astSridInfo[i].ucSrid        = *pucSrId;
            pstXccCtxAddr->astSridInfo[i].enSo          = enSo;
            pstXccCtxAddr->astSridInfo[i].ucConnectId   = ucConnectId;
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32   CNAS_XCC_UpdateSrIdUsed(
    CAS_CNAS_1X_SERVICE_OPTION_ENUM_UINT16      enSo,
    VOS_UINT8                                   ucSrId,
    VOS_UINT8                                   ucConnectId
)
{
    CNAS_XCC_CTX_STRU                  *pstXccCtxAddr   = VOS_NULL_PTR;
    VOS_UINT8                           i;

    /* �쳣��������ֹ�����ˢ���쳣��SR ID */
    if (ucSrId > CNAS_XCC_MAX_SRID_NUM)
    {
        CNAS_WARNING_LOG1(UEPS_PID_XCC, "CNAS_XCC_UpdateSrIdUsed: Invalid SR_ID: %d",ucSrId);
        return VOS_TRUE;
    }

    pstXccCtxAddr = CNAS_XCC_GetCtxAddr();

    /* ���֮ǰ�Ѿ��й������SR_ID����Ҫ�������Ȼ�������¸�ֵ */
    for (i = CNAS_XCC_RESERVED_SR_ID; i <= CNAS_XCC_MAX_FSM_ENTITY_NUM; i++)
    {
        if ((VOS_TRUE == pstXccCtxAddr->astSridInfo[i].ucIsUsed)
         && (ucConnectId == pstXccCtxAddr->astSridInfo[i].ucConnectId)
         && (i != ucSrId))
        {
            pstXccCtxAddr->astSridInfo[i].ucIsUsed      = VOS_FALSE;
            pstXccCtxAddr->astSridInfo[i].ucSrid        = CNAS_XCC_INVALID_SR_ID;
            pstXccCtxAddr->astSridInfo[i].enSo          = enSo;
            pstXccCtxAddr->astSridInfo[i].ucConnectId   = CNAS_XCC_INVAILD_CON_ID;
            break;
        }
    }

    /* ����ˢ�����ɣ�Ŀǰû������ͻ��飬����г�ͻ����ǰ�������������ӳ��Ӧ�û������⣬�����Ϊ
        ���ֳ���Ŀǰ������ */
    pstXccCtxAddr->astSridInfo[ucSrId].ucIsUsed      = VOS_TRUE;
    pstXccCtxAddr->astSridInfo[ucSrId].ucSrid        = ucSrId;
    pstXccCtxAddr->astSridInfo[ucSrId].enSo          = enSo;
    pstXccCtxAddr->astSridInfo[ucSrId].ucConnectId   = ucConnectId;

    return VOS_TRUE;


}



VOS_VOID CNAS_XCC_SetOrigCallOrderIndex(
    VOS_UINT8                           ucCallIndex
)
{
    CNAS_XCC_RECORD_ORIG_CALL_INDEX_ORDER_STRU             *pstOrigCallIndexOrder = VOS_NULL_PTR;
    VOS_UINT8                                               ucCallNum;

    pstOrigCallIndexOrder = &(CNAS_XCC_GetCtxAddr()->stOrigCallIndexOrder);

    ucCallNum = pstOrigCallIndexOrder->ucCallNum;

    pstOrigCallIndexOrder->aucCallIndex[ucCallNum] = ucCallIndex;

    pstOrigCallIndexOrder->ucCallNum++;

    return;
}


VOS_VOID CNAS_XCC_ClearSpecificedOrigCallOrderIndex(
    VOS_UINT8                           ucCallIndex
)
{
    CNAS_XCC_RECORD_ORIG_CALL_INDEX_ORDER_STRU             *pstOrigCallIndexOrder = VOS_NULL_PTR;
    VOS_UINT8                                               i;

    pstOrigCallIndexOrder = &(CNAS_XCC_GetCtxAddr()->stOrigCallIndexOrder);

    for (i = 0; i < pstOrigCallIndexOrder->ucCallNum; i++)
    {
        if (pstOrigCallIndexOrder->aucCallIndex[i] == ucCallIndex)
        {
            break;
        }
    }

    if (i == pstOrigCallIndexOrder->ucCallNum)
    {
        /* δ�ҵ������� */
        return;
    }

    if (i == (pstOrigCallIndexOrder->ucCallNum - 1))
    {
        pstOrigCallIndexOrder->aucCallIndex[i] = CNAS_XCC_INVAILD_CALL_INDEX;
    }
    else
    {
        NAS_MEM_MOVE_S(&pstOrigCallIndexOrder->aucCallIndex[i],
                       sizeof(UINT8) * (CNAS_XCC_MAX_FSM_ENTITY_NUM - i),
                       &pstOrigCallIndexOrder->aucCallIndex[i+1],
                       sizeof(UINT8) * (pstOrigCallIndexOrder->ucCallNum - i - 1));

        pstOrigCallIndexOrder->aucCallIndex[pstOrigCallIndexOrder->ucCallNum - 1] = CNAS_XCC_INVAILD_CALL_INDEX;
    }

    pstOrigCallIndexOrder->ucCallNum--;

    return;
}


VOS_VOID CNAS_XCC_ClearAllOrigCallOrderIndex(VOS_VOID)
{
    CNAS_XCC_RECORD_ORIG_CALL_INDEX_ORDER_STRU             *pstOrigCallIndexOrder = VOS_NULL_PTR;
    VOS_UINT8                                               i;

    pstOrigCallIndexOrder = &(CNAS_XCC_GetCtxAddr()->stOrigCallIndexOrder);

    NAS_MEM_SET_S(pstOrigCallIndexOrder, sizeof(CNAS_XCC_RECORD_ORIG_CALL_INDEX_ORDER_STRU), 0, sizeof(CNAS_XCC_RECORD_ORIG_CALL_INDEX_ORDER_STRU));

    for (i = 0; i < CNAS_XCC_MAX_FSM_ENTITY_NUM; i++)
    {
        pstOrigCallIndexOrder->aucCallIndex[i] = CNAS_XCC_INVAILD_CALL_INDEX;
    }

    return;
}


VOS_UINT8 CNAS_XCC_GetLastCallIndex(VOS_VOID)
{
    CNAS_XCC_RECORD_ORIG_CALL_INDEX_ORDER_STRU             *pstOrigCallIndexOrder = VOS_NULL_PTR;

    pstOrigCallIndexOrder = &(CNAS_XCC_GetCtxAddr()->stOrigCallIndexOrder);

    if (0 == pstOrigCallIndexOrder->ucCallNum)
    {
        return CNAS_XCC_INVAILD_CALL_INDEX;
    }

    return pstOrigCallIndexOrder->aucCallIndex[pstOrigCallIndexOrder->ucCallNum - 1];
}


VOS_VOID CNAS_XCC_INIT_SRID_INFO( VOS_VOID )
{
    NAS_MEM_SET_S(CNAS_XCC_GetCtxAddr()->astSridInfo, sizeof(CNAS_XCC_SRID_INFO_STRU) * (CNAS_XCC_MAX_SRID_NUM + 1), 0, sizeof(CNAS_XCC_SRID_INFO_STRU) * (CNAS_XCC_MAX_SRID_NUM + 1));
}


VOS_VOID CNAS_XCC_ClearSpecificedSrIdByConnectId(
    VOS_UINT8                           ucConnectId
)
{
    VOS_UINT32                          i;
    CNAS_XCC_CTX_STRU                  *pstXccCtxAddr   = VOS_NULL_PTR;

    pstXccCtxAddr = CNAS_XCC_GetCtxAddr();

    for (i = 1; i <= CNAS_XCC_MAX_SRID_NUM; i++)
    {
        if ((VOS_TRUE == pstXccCtxAddr->astSridInfo[i].ucIsUsed)
         && (ucConnectId == pstXccCtxAddr->astSridInfo[i].ucConnectId))
        {
            pstXccCtxAddr->astSridInfo[i].ucIsUsed      = VOS_FALSE;
            pstXccCtxAddr->astSridInfo[i].ucSrid        = CNAS_XCC_INVALID_SR_ID;
            pstXccCtxAddr->astSridInfo[i].enSo          = CAS_CNAS_1X_SO_BUTT;
            pstXccCtxAddr->astSridInfo[i].ucConnectId   = CNAS_XCC_INVAILD_CON_ID;
            break;
        }
    }
}


VOS_UINT32 CNAS_XCC_IsExist1xDataOperation(
    TAF_CDMA_SO_TYPE_ENUM_UINT16        enSo
)
{
    if (MODEM_ID_0 == CNAS_CCB_GetCdmaModeModemId())
    {
        return TAF_APS_IsExist1xDataOperation(enSo);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    else if (MODEM_ID_1 == CNAS_CCB_GetCdmaModeModemId())
    {
        return I1_TAF_APS_IsExist1xDataOperation(enSo);
    }
#if ( 3 == MULTI_MODEM_NUMBER )
    else if (MODEM_ID_2 == CNAS_CCB_GetCdmaModeModemId())
    {
        return I2_TAF_APS_IsExist1xDataOperation(enSo);
    }
#endif
#endif
    else
    {
        NAS_WARNING_LOG(UEPS_PID_XCC, "CNAS_XCC_IsExist1xDataOperation invalid modemId!\r\n");
    }

    return VOS_FALSE;

}


/*lint -restore*/
#endif /* end FEATURE_UE_MODE_CDMA */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */




