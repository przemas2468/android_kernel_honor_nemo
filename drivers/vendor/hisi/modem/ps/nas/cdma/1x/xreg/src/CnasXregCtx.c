

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "CnasXregCtx.h"
#include  "CnasXregFsmMainTbl.h"
#include  "CnasMntn.h"
#include  "CnasXregRegingProcTbl.h"
#include  "CnasXregFsmSwitchOnTbl.h"

#include  "CnasCcb.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_XREG_CTX_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
CNAS_XREG_CTX_STRU                      g_stCnasXregCtx;


/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -save -e958*/

VOS_VOID CNAS_XREG_InitCurrFsmCtx(
    CNAS_XREG_FSM_CTX_STRU             *pstCurrFsmCtx
)
{
    pstCurrFsmCtx->enFsmId                  =   CNAS_XREG_FSM_L1_MAIN;
    pstCurrFsmCtx->pstFsmDesc               =   CNAS_XREG_GetMainFsmDescAddr();
    pstCurrFsmCtx->enParentFsmId            =   CNAS_XREG_FSM_BUTT;
    pstCurrFsmCtx->ulParentEventType        =   0;
    pstCurrFsmCtx->ulState                  =   CNAS_XREG_L1_STA_NULL;

    pstCurrFsmCtx->stEntryMsg.ulEventType   =   0;

    PS_MEM_SET(pstCurrFsmCtx->stEntryMsg.aucMsgBuffer,
               0x00,
               CNAS_XREG_MAX_MSG_BUFFER_LEN);

    /* unFsmCtx���ε������漰 */
}


VOS_VOID CNAS_XREG_InitFsmStackCtx(
    CNAS_XREG_FSM_STACK_STRU           *pstFsmStack
)
{
    VOS_UINT32                          i;

    pstFsmStack->usStackDepth   = 0;
    pstFsmStack->usStackPopFlg  = VOS_FALSE;

    for ( i = 0 ; i < CNAS_XREG_MAX_FSM_STACK_DEPTH ; i++ )
    {
        pstFsmStack->astFsmStack[i].pstFsmDesc  = VOS_NULL_PTR;
        pstFsmStack->astFsmStack[i].enFsmId     = CNAS_XREG_FSM_BUTT;
        pstFsmStack->astFsmStack[i].ulState     = CNAS_XREG_L1_STA_BUTT;
    }
}


VOS_VOID CNAS_XREG_InitCacheMsgQueue(
    CNAS_XREG_CACHE_MSG_QUEUE_STRU     *pstCacheMsgQueue
)
{
    VOS_UINT32                          i;

    pstCacheMsgQueue->ulCurrIndex     = 0;
    pstCacheMsgQueue->ucCacheMsgNum   = 0;

    for (i = 0; i < CNAS_XREG_MAX_CACHE_MSG_QUEUE_NUM; i++)
    {
        pstCacheMsgQueue->pastCacheMsg[i] = VOS_NULL_PTR;
    }
}


VOS_VOID CNAS_XREG_InitCtx(VOS_VOID)
{
    CNAS_XREG_CTX_STRU                 *pstXregCtx = VOS_NULL_PTR;

    pstXregCtx = CNAS_XREG_GetCtxAddr();

    /* ��ʼ��XREG��ǰ״̬�������� */
    CNAS_XREG_InitCurrFsmCtx(&(pstXregCtx->stCurFsm));

    /* ��ʼ��XREG״̬��ջ������ */
    CNAS_XREG_InitFsmStackCtx(&(pstXregCtx->stFsmStack));

    /* ��ʼ��XREG������Ϣ���� */
    CNAS_XREG_InitCacheMsgQueue(&(pstXregCtx->stCacheMsgQueue));

    return;
}


VOS_VOID CNAS_XREG_InitFsmCtx_L1Main(VOS_VOID)
{
    /* ���ε������漰 */

    return;
}



VOS_VOID  CNAS_XREG_LoadSubFsm(
    CNAS_XREG_FSM_ID_ENUM_UINT32         enFsmId,
    CNAS_XREG_FSM_CTX_STRU              *pstCurFsm
)
{
    /* �ڱ��ο����׶Σ�ֻ�п��ػ�״̬�� */
    switch (enFsmId)
    {
        case CNAS_XREG_FSM_SWITCH_ON:
            pstCurFsm->ulState                  = CNAS_XREG_SWITCH_ON_STA_INIT;
            pstCurFsm->pstFsmDesc               = CNAS_XREG_GetSwitchOnFsmDescAddr();
            break;

        case CNAS_XREG_FSM_REGING :
            pstCurFsm->ulState                  = CNAS_XREG_REGING_STA_INIT;
            pstCurFsm->pstFsmDesc               = CNAS_XREG_GetRegingFsmDescAddr();
            break;

        default:
            CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_LoadSubFsm:Invalid Fsm Id");
            return;
    }

    /*ִ�г�ʼ��L2״̬���ĺ���*/
    if (VOS_NULL_PTR != pstCurFsm->pstFsmDesc->pfInitHandle)
    {
        pstCurFsm->pstFsmDesc->pfInitHandle();
    }
}



VOS_VOID CNAS_XREG_PushFsm(
    CNAS_XREG_FSM_STACK_STRU           *pstFsmStack,
    CNAS_XREG_FSM_CTX_STRU             *pstNewFsm
)
{
    CNAS_XREG_FSM_CTX_STRU             *pstCurFsm = VOS_NULL_PTR;

    /* ���ָ���� */
    if ((VOS_NULL_PTR == pstFsmStack)
     || (VOS_NULL_PTR == pstNewFsm))
    {
        CNAS_ERROR_LOG(UEPS_PID_XREG,
                       "CNAS_XREG_PushFsm: pstFsmStack or pstNewFsm is null pointer!");

        return;
    }

    /* ջ��ȼ�� */
    if (pstFsmStack->usStackDepth >= CNAS_XREG_MAX_FSM_STACK_DEPTH)
    {
        CNAS_ERROR_LOG1(UEPS_PID_XREG,
                       "CNAS_XREG_PushFsm: Fsm Stack Depth = %d Error!",
                       pstFsmStack->usStackDepth);

        return;
    }

    /* push��ǰ״̬����������ջ */
    pstCurFsm = &((pstFsmStack->astFsmStack)[pstFsmStack->usStackDepth]);

    PS_MEM_CPY(pstCurFsm, pstNewFsm, sizeof(CNAS_XREG_FSM_CTX_STRU));

    /* ջ������� */
    pstFsmStack->usStackDepth++;

    return;
}


VOS_VOID CNAS_XREG_PopFsm(VOS_VOID)
{
    CNAS_XREG_FSM_STACK_STRU           *pstFsmStack = VOS_NULL_PTR;
    CNAS_XREG_FSM_CTX_STRU             *pstCurFsm   = VOS_NULL_PTR;

    /* ���ݲ���״̬��ջ������״̬����ȡ��ǰ״̬��������ַ  */
    pstFsmStack = CNAS_XREG_GetFsmStackAddr();
    pstCurFsm   = CNAS_XREG_GetCurFsmAddr();

    /* ���ջ�ռ��Ƿ�Ϊ�� */
    if (0 == pstFsmStack->usStackDepth)
    {
        CNAS_ERROR_LOG(UEPS_PID_XREG, "CNAS_XREG_PopFsm: FSM Stack is empty");

        return;
    }

    /* ״̬��ջ��ȼ��� */
    pstFsmStack->usStackDepth--;

    /* ���õ�ǰ״̬�� */
    PS_MEM_CPY(pstCurFsm,
              &((pstFsmStack->astFsmStack)[pstFsmStack->usStackDepth]),
              sizeof(CNAS_XREG_FSM_CTX_STRU));
}



VOS_VOID CNAS_XREG_InitFsmL2(
    CNAS_XREG_FSM_ID_ENUM_UINT32        enFsmId
)
{
    CNAS_XREG_FSM_STACK_STRU           *pstFsmStack = VOS_NULL_PTR;
    CNAS_XREG_FSM_CTX_STRU             *pstCurFsm   = VOS_NULL_PTR;
    CNAS_XREG_FSM_ID_ENUM_UINT32        enParentFsmId;
    VOS_UINT32                          ulParentEventType;

    /* ��ӡѹջǰ��״̬����״̬ID */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XREG, CNAS_XREG_GetCurrFsmId(), CNAS_XREG_GetFsmTopState(), 0);

    /* ��¼ѹջǰ��״̬�����������Ϣ���� */
    enParentFsmId     = CNAS_XREG_GetCurrFsmId();
    ulParentEventType = CNAS_XREG_GetCurFsmAddr()->stEntryMsg.ulEventType;

    /* ����ǰ״̬��ѹջ���� */
    pstFsmStack = CNAS_XREG_GetFsmStackAddr();
    pstCurFsm   = CNAS_XREG_GetCurFsmAddr();
    CNAS_XREG_PushFsm(pstFsmStack, pstCurFsm);

    /* ���õ�ǰ״̬�� */
    pstCurFsm->enFsmId = enFsmId;

    /* ����L2״̬�� */
    CNAS_XREG_LoadSubFsm(enFsmId, pstCurFsm);

    /* ��¼�ϲ�״̬�����������Ϣ���� */
    pstCurFsm->enParentFsmId     = enParentFsmId;
    pstCurFsm->ulParentEventType = ulParentEventType;

    /* ��ӡѹջǰ��״̬����״̬ID */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XREG, CNAS_XREG_GetCurrFsmId(), CNAS_XREG_GetFsmTopState(), 0);
}


VOS_VOID CNAS_XREG_QuitFsmL2(VOS_VOID)
{
    CNAS_XREG_FSM_CTX_STRU             *pstCurFsm   = VOS_NULL_PTR;

    pstCurFsm = CNAS_XREG_GetCurFsmAddr();

    /* QUITʱ��յ�ǰL2״̬����CTX */
    if (VOS_NULL_PTR != pstCurFsm->pstFsmDesc->pfInitHandle)
    {
        pstCurFsm->pstFsmDesc->pfInitHandle();
    }

    /*  ��ӡѹջǰ��L1��L2״̬ */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XREG, CNAS_XREG_GetCurrFsmId(), CNAS_XREG_GetFsmTopState(), 0);

    /* ״̬����ջ���� */
    CNAS_XREG_PopFsm();

    /*  ��ӡ��ǰ��L1��L2״̬ */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XREG, CNAS_XREG_GetCurrFsmId(), CNAS_XREG_GetFsmTopState(), 0);

    return;
}



CNAS_XREG_CTX_STRU* CNAS_XREG_GetCtxAddr(VOS_VOID)
{
    return &(g_stCnasXregCtx);
}


CNAS_XREG_FSM_CTX_STRU* CNAS_XREG_GetCurFsmAddr(VOS_VOID)
{
    return &(CNAS_XREG_GetCtxAddr()->stCurFsm);
}


CNAS_XREG_FSM_STACK_STRU* CNAS_XREG_GetFsmStackAddr(VOS_VOID)
{
    return &(CNAS_XREG_GetCtxAddr()->stFsmStack);
}


NAS_FSM_DESC_STRU* CNAS_XREG_GetCurFsmDesc(VOS_VOID)
{
    return (CNAS_XREG_GetCtxAddr()->stCurFsm.pstFsmDesc);
}


CNAS_XREG_MSG_STRU* CNAS_XREG_GetCurrFsmEntryMsgAddr(VOS_VOID)
{
    return &(CNAS_XREG_GetCtxAddr()->stCurFsm.stEntryMsg);
}


CNAS_XREG_FSM_ID_ENUM_UINT32 CNAS_XREG_GetCurrFsmId(VOS_VOID)
{
    return (CNAS_XREG_GetCtxAddr()->stCurFsm.enFsmId);
}


VOS_UINT32 CNAS_XREG_GetFsmTopState(VOS_VOID)
{
    return (CNAS_XREG_GetCurFsmAddr()->ulState);
}


VOS_UINT16  CNAS_XREG_GetFsmStackDepth(VOS_VOID)
{
    CNAS_XREG_FSM_STACK_STRU            *pstFsmStack = VOS_NULL_PTR;

    pstFsmStack = CNAS_XREG_GetFsmStackAddr();

    return pstFsmStack->usStackDepth;
}


VOS_VOID CNAS_XREG_SetCurrFsmState(
    VOS_UINT32                          ulCurrState
)
{
    CNAS_XREG_FSM_CTX_STRU             *pstCurFsm   = VOS_NULL_PTR;

    /* ��ȡ��ǰ��״̬��Ϣ */
    pstCurFsm                   = CNAS_XREG_GetCurFsmAddr();

    /* ���õ�ǰ״̬ */
    pstCurFsm->ulState          = ulCurrState;

    /*  ��ӡ��ǰ��L1��L2״̬ */
    CNAS_MNTN_LogFsmInfo(UEPS_PID_XREG, CNAS_XREG_GetCurrFsmId(), ulCurrState, 0);

    return;
}


VOS_VOID  CNAS_XREG_SetFsmStackPopFlg(
    VOS_UINT16                          ucStackPopFlg
)
{
    CNAS_XREG_FSM_STACK_STRU           *pstFsmStack = VOS_NULL_PTR;

    pstFsmStack = CNAS_XREG_GetFsmStackAddr();

    pstFsmStack->usStackPopFlg  = ucStackPopFlg;
}


VOS_UINT16  CNAS_XREG_GetFsmStackPopFlg(VOS_VOID)
{
    CNAS_XREG_FSM_STACK_STRU           *pstFsmStack = VOS_NULL_PTR;

    pstFsmStack = CNAS_XREG_GetFsmStackAddr();

    return pstFsmStack->usStackPopFlg;
}


VOS_VOID CNAS_XREG_SaveCurEntryMsg(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_XREG_MSG_STRU                 *pstEntryMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulLen;

    /* ��õ�ǰ����CNAS XSD�����Ϣ�Ļ�������ַ */
    pstEntryMsg                 = CNAS_XREG_GetCurrFsmEntryMsgAddr();

    ulLen                       = pstMsg->ulLength + VOS_MSG_HEAD_LENGTH;

    /* ����ǰ����������Ϣ������Context��Ӧ�Ļ����� */
    PS_MEM_CPY( pstEntryMsg->aucMsgBuffer, pstMsg, ulLen );

    /* ��дContext�еĲ��� */
    pstEntryMsg->ulEventType    =   ulEventType;

    return;
}


VOS_UINT32  CNAS_XREG_GetCacheMsgNum(VOS_VOID)
{
    CNAS_XREG_CACHE_MSG_QUEUE_STRU     *pstMsgQueue = VOS_NULL_PTR;

    pstMsgQueue                         = CNAS_XREG_GetCacheMsgAddr();

    return pstMsgQueue->ucCacheMsgNum;
}


CNAS_XREG_CACHE_MSG_QUEUE_STRU* CNAS_XREG_GetCacheMsgAddr(VOS_VOID)
{
    return &(CNAS_XREG_GetCtxAddr()->stCacheMsgQueue);
}


VOS_VOID CNAS_XREG_ClearCacheMsg(VOS_VOID)
{
    CNAS_XREG_CACHE_MSG_QUEUE_STRU     *pstMsgQueue = VOS_NULL_PTR;
    VOS_UINT32                          ulOffset;

    pstMsgQueue = CNAS_XREG_GetCacheMsgAddr();

    if ((pstMsgQueue->ucCacheMsgNum < 1)
        ||(pstMsgQueue->ucCacheMsgNum >= CNAS_XREG_MAX_CACHE_MSG_QUEUE_NUM))
    {
        return;
    }

    ulOffset = (pstMsgQueue->ulCurrIndex % CNAS_XREG_MAX_CACHE_MSG_QUEUE_NUM);
	
    if(VOS_NULL_PTR != pstMsgQueue->pastCacheMsg[ulOffset])
    {
        PS_MEM_FREE(UEPS_PID_XREG, pstMsgQueue->pastCacheMsg[ulOffset]);
    }

    pstMsgQueue->ulCurrIndex = (pstMsgQueue->ulCurrIndex + 1) % CNAS_XREG_MAX_CACHE_MSG_QUEUE_NUM;

    /* �����������һ */
    pstMsgQueue->ucCacheMsgNum--;

    return;
}


VOS_UINT32  CNAS_XREG_GetNextCachedMsg(
    CNAS_XREG_MSG_STRU                 *pstEntryMsg
)
{
    /* ��ȡ������Ϣ�ɹ�,���ػ�����Ϣ */
    CNAS_XREG_CACHE_MSG_QUEUE_STRU     *pstMsgQueue    = VOS_NULL_PTR;
    MSG_HEADER_STRU                    *pstCacheMsgHdr = VOS_NULL_PTR;
    REL_TIMER_MSG                      *pstTimerMsg    = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgLen;
    VOS_UINT32                          i;

    if (VOS_NULL_PTR == pstEntryMsg)
    {
        return VOS_FALSE;
    }

    pstMsgQueue = CNAS_XREG_GetCacheMsgAddr();

    if (pstMsgQueue->ucCacheMsgNum < 1)
    {
        return VOS_FALSE;
    }

    if ((pstMsgQueue->ucCacheMsgNum >= CNAS_XREG_MAX_CACHE_MSG_QUEUE_NUM)
        ||(pstMsgQueue->ucCacheMsgNum == VOS_NULL))
    {
        /* �ͷ������ڴ� */
        for (i = 0; i < CNAS_XREG_MAX_CACHE_MSG_QUEUE_NUM; i++)
        {
            if (VOS_NULL_PTR != pstMsgQueue->pastCacheMsg[i])
            {
                PS_MEM_FREE(UEPS_PID_XREG, pstMsgQueue->pastCacheMsg[i]);
            }
        }

        VOS_MemSet(pstMsgQueue, VOS_NULL, sizeof(CNAS_XREG_CACHE_MSG_QUEUE_STRU));

        return VOS_FALSE;
    }

    pstCacheMsgHdr = (MSG_HEADER_STRU *)(pstMsgQueue->pastCacheMsg[pstMsgQueue->ulCurrIndex]);

    if (VOS_PID_TIMER == pstCacheMsgHdr->ulSenderPid)
    {
        pstTimerMsg = (REL_TIMER_MSG *)pstCacheMsgHdr;

        pstEntryMsg->ulEventType = CNAS_BuildEventType(pstCacheMsgHdr->ulSenderPid, pstTimerMsg->ulName);
    }
    else
    {
        pstEntryMsg->ulEventType = CNAS_BuildEventType(pstCacheMsgHdr->ulSenderPid, pstCacheMsgHdr->ulMsgName);
    }

    ulMsgLen = (pstCacheMsgHdr->ulLength + VOS_MSG_HEAD_LENGTH);

    if (CNAS_XREG_MAX_MSG_BUFFER_LEN < ulMsgLen)
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_XREG, "CNAS_XREG_GetNextCachedMsg: beyond msg size ");

        ulMsgLen = CNAS_XREG_MAX_MSG_BUFFER_LEN;
    }

    VOS_MemCpy(&pstEntryMsg->aucMsgBuffer[0], pstCacheMsgHdr, ulMsgLen);

    CNAS_XREG_ClearCacheMsg();

    return VOS_TRUE;
}


VOS_VOID  CNAS_XREG_SaveMsgToCache(
    VOS_UINT32                          ulEventType,
    VOS_UINT8                           *pucMsg
)
{
    /* ��ȡ������Ϣ�ɹ�,���ػ�����Ϣ */
    CNAS_XREG_CACHE_MSG_QUEUE_STRU     *pstMsgQueue = VOS_NULL_PTR;
    MSG_HEADER_STRU                    *pstMsgHeader  = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgLen;
    VOS_UINT32                          ulOffset;
    VOS_UINT32                          i;

    if (VOS_NULL_PTR == pucMsg)
    {
        return ;
    }

    pstMsgQueue = CNAS_XREG_GetCacheMsgAddr();

    if (pstMsgQueue->ucCacheMsgNum >= CNAS_XREG_MAX_CACHE_MSG_QUEUE_NUM)
    {
        /* �ͷ������ڴ� */
        for (i = 0; i < CNAS_XREG_MAX_CACHE_MSG_QUEUE_NUM; i++)
        {
            if (VOS_NULL_PTR != pstMsgQueue->pastCacheMsg[i])
            {
                PS_MEM_FREE(UEPS_PID_XREG, pstMsgQueue->pastCacheMsg[i]);
            }
        }

        PS_MEM_SET(pstMsgQueue, VOS_NULL, sizeof(CNAS_XREG_CACHE_MSG_QUEUE_STRU));
    }

    ulOffset = (pstMsgQueue->ulCurrIndex + pstMsgQueue->ucCacheMsgNum) % CNAS_XREG_MAX_CACHE_MSG_QUEUE_NUM;

    /* �ڴ������ڲ� */
    pstMsgHeader = (MSG_HEADER_STRU*)pucMsg;

    ulMsgLen = (VOS_MSG_HEAD_LENGTH + pstMsgHeader->ulLength);

    pstMsgQueue->pastCacheMsg[ulOffset] = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_XREG, ulMsgLen) ;

    if (VOS_NULL_PTR == pstMsgQueue->pastCacheMsg[ulOffset])
    {
        return;
    }

    PS_MEM_CPY(pstMsgQueue->pastCacheMsg[ulOffset], pucMsg, ulMsgLen);

    pstMsgQueue->ucCacheMsgNum++;

    return ;
}


VOS_VOID CNAS_XREG_InitFsmCtx_SwitchOn(VOS_VOID)
{
    /* ��鵱ǰ״̬���Ƿ�ʱ����״̬�� */
    if (CNAS_XREG_FSM_SWITCH_ON != CNAS_XREG_GetCurrFsmId())
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_XREG, "CNAS_XREG_InitFsmCtx_SwitchOn:State is incorrect.");

        return;
    }

    g_stCnasXregCtx.stCurFsm.unFsmCtx.stSwitchOnCtx.ulXregReadCardFileFlg = CNAS_XREG_READ_CARD_FILE_FLG_NULL;

    return;
}


CNAS_XREG_READ_CARD_FILES_CNF_ENUM_UINT32 CNAS_XREG_GetWaitCardFilesCnfFlg_SwitchOn(VOS_VOID)
{
    /* �����ǰ״̬������switch on */
    if (CNAS_XREG_FSM_SWITCH_ON != CNAS_XREG_GetCurrFsmId())
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_XREG, "CNAS_XREG_GetWaitCardFilesCnfFlg_SwitchOn: State is incorrect.");
    }

    return g_stCnasXregCtx.stCurFsm.unFsmCtx.stSwitchOnCtx.ulXregReadCardFileFlg;
}


VOS_VOID CNAS_XREG_SetWaitCardFilesCnfFlg_SwitchOn(
    CNAS_XREG_READ_CARD_FILES_CNF_ENUM_UINT32               enReadCardFileCnfFlg
)
{
    /* �����ǰ״̬������switch on */
    if (CNAS_XREG_FSM_SWITCH_ON != CNAS_XREG_GetCurrFsmId())
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_XREG, "CNAS_XREG_SetWaitCardFilesCnfFlg_SwitchOn: State is incorrect.");

        return;
    }

    g_stCnasXregCtx.stCurFsm.unFsmCtx.stSwitchOnCtx.ulXregReadCardFileFlg |= enReadCardFileCnfFlg;

    return;
}


VOS_VOID CNAS_XREG_ClearWaitCardFilesCnfFlg_SwitchOn(
    CNAS_XREG_READ_CARD_FILES_CNF_ENUM_UINT32               enReadCardFileCnfFlg
)
{
    /* �����ǰ״̬������switch on */
    if (CNAS_XREG_FSM_SWITCH_ON != CNAS_XREG_GetCurrFsmId())
    {
        /* �쳣�¼� */
        CNAS_WARNING_LOG(UEPS_PID_XREG, "CNAS_XREG_ClearWaitCardFilesCnfFlg_SwitchOn: State is incorrect.");
    }

    g_stCnasXregCtx.stCurFsm.unFsmCtx.stSwitchOnCtx.ulXregReadCardFileFlg &= ~enReadCardFileCnfFlg;

    return;
}


CNAS_XREG_REFRESH_CARD_FILES_IND_ENUM_UINT32 CNAS_XREG_GetReadCardFilesIndFlg(VOS_VOID)
{
    return g_stCnasXregCtx.stRegPreProcCtx.stXregRefreshCardFileStatus.enRefreshReadCard;
}


VOS_VOID CNAS_XREG_SetReadCardFilesIndFlg(
    CNAS_XREG_REFRESH_CARD_FILES_IND_ENUM_UINT32    enRefreshReadCard
)
{
    g_stCnasXregCtx.stRegPreProcCtx.stXregRefreshCardFileStatus.enRefreshReadCard |= enRefreshReadCard;

    return;
}


VOS_VOID CNAS_XREG_ClearReadCardFilesIndFlg(
    VOS_UINT32                          ulEfId
)
{
    switch ( ulEfId )
    {
        case USIMM_CDMA_EFTERM_ID:
        case USIMM_CSIM_EFTERM_ID:
            g_stCnasXregCtx.stRegPreProcCtx.stXregRefreshCardFileStatus.enRefreshReadCard &= ~CNAS_XREG_REFRESH_CARD_FILE_TERM_FLG;
            break;

        case USIMM_CSIM_EFSSCI_ID:
        case USIMM_CDMA_EFSSCI_ID:
            g_stCnasXregCtx.stRegPreProcCtx.stXregRefreshCardFileStatus.enRefreshReadCard &= ~CNAS_XREG_REFRESH_CARD_FILE_SSCI_FLG;
            break;

        default:
            break;
    }

    return;
}


CNAS_XREG_CARD_FILES_CHANGE_ENUM_UINT32 CNAS_XREG_GetCardFilesChangeFlg(VOS_VOID)
{
    return g_stCnasXregCtx.stRegPreProcCtx.stXregRefreshCardFileStatus.enCardFileChange;
}


VOS_VOID CNAS_XREG_SetCardFilesChangeFlg(
    CNAS_XREG_CARD_FILES_CHANGE_ENUM_UINT32    enCardFileChange
)
{
    g_stCnasXregCtx.stRegPreProcCtx.stXregRefreshCardFileStatus.enCardFileChange |= enCardFileChange;

    return;
}


VOS_VOID CNAS_XREG_ClearRefreshCardFilesCtx(VOS_VOID)
{
    g_stCnasXregCtx.stRegPreProcCtx.stXregRefreshCardFileStatus.enCardFileChange  = CNAS_XREG_CARD_FILE_CHANGE_FLG_NULL;
    g_stCnasXregCtx.stRegPreProcCtx.stXregRefreshCardFileStatus.enRefreshReadCard = CNAS_XREG_REFRESH_CARD_FILE_FLG_NULL;

    return;
}

/*lint -restore*/
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



