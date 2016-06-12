


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "PsLib.h"

#include "PsCommonDef.h"
#include "TafMmaCtx.h"
#include "TafMmaFsmMainTbl.h"
#include "TafMmaFsmPhoneModeTbl.h"
#include "TafLog.h"
#include "TafMmaSndInternalMsg.h"
#include "Taf_Tafm_Remote.h"
#include "TafMmaMntn.h"
#include "TafOamInterface.h"
#include "MmaUphyInterface.h"

#include "TafSdcLib.h"

#include "TafMmaFsmImsSwitchTbl.h"

#include "TafMmaFsmSysCfgTbl.h"
#include "TafSdcCtx.h"
#include "MmaAppLocal.h"
#include "TafCdmaPlusTranslate.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
/*lint -e(767)*/
#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_CTX_C


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

/* MMA CTX,���ڱ���MMA״̬��,������ */
TAF_MMA_CONTEXT_STRU                    g_stTafMmaCtx;

/*****************************************************************************
  3 ���Ͷ���
*****************************************************************************/

/*****************************************************************************
  4 ��������
*****************************************************************************/

/*****************************************************************************
  5 ��������
*****************************************************************************/

/*lint -save -e958 */

/*****************************************************************************
  6 ��������
*****************************************************************************/


VOS_VOID TAF_MMA_SaveCurEntryMsg(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_MMA_ENTRY_MSG_STRU             *pstEntryMsg  = VOS_NULL_PTR;
    MSG_HEADER_STRU                    *pstMsgHeader = VOS_NULL_PTR;
    VOS_UINT32                          ulLen;

    /* ��õ�ǰ����MMA�����Ϣ�Ļ�������ַ */
    pstEntryMsg                 = TAF_MMA_GetCurrFsmMsgAddr();
    pstMsgHeader                = (MSG_HEADER_STRU*)pstMsg;

    ulLen                       = pstMsgHeader->ulLength + VOS_MSG_HEAD_LENGTH;

    /* ����ǰ����������Ϣ������Context��Ӧ�Ļ����� */
    PS_MEM_CPY( pstEntryMsg->aucEntryMsgBuffer, pstMsg, ulLen );

    /* ��дContext�еĲ��� */
    pstEntryMsg->ulEventType    =   ulEventType;

    return;

}


VOS_VOID  TAF_MMA_FSM_SetCurrState(
    VOS_UINT32                          ulCurrState
)
{
    TAF_MMA_FSM_CTX_STRU               *pstCurFsm   = VOS_NULL_PTR;

    /* ��ȡ��ǰ��״̬��Ϣ */
    pstCurFsm                   = TAF_MMA_GetCurFsmAddr();

    /* ���õ�ǰ״̬ */
    pstCurFsm->ulState          = ulCurrState;

    /*  ��ӡ��ǰ��L1��L2״̬ */
    TAF_MMA_LogFsmInfo();

    return;
}



VOS_UINT32 TAF_MMA_FSM_GetFsmTopState( VOS_VOID )
{
    TAF_MMA_FSM_CTX_STRU               *pstCurFsm   = VOS_NULL_PTR;
    VOS_UINT32                          ulState;

    /* ��ȡ��ǰ״̬�� */
    pstCurFsm               = TAF_MMA_GetCurFsmAddr();
    ulState                 = pstCurFsm->ulState;

    return ulState;
}


TAF_MMA_CONTEXT_STRU* TAF_MMA_GetMmaCtxAddr(VOS_VOID)
{
    return &(g_stTafMmaCtx);
}


TAF_MMA_FSM_CTX_STRU* TAF_MMA_GetCurFsmAddr(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stCurFsm);
}


TAF_MMA_FSM_STACK_STRU* TAF_MMA_GetFsmStackAddr(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stFsmStack);
}


TAF_FSM_DESC_STRU* TAF_MMA_GetCurFsmDesc(VOS_VOID)
{
    return (TAF_MMA_GetMmaCtxAddr()->stCurFsm.pstFsmDesc);
}


TAF_MMA_ENTRY_MSG_STRU* TAF_MMA_GetCurrFsmMsgAddr(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stCurFsm.stEntryMsg);
}


TAF_MMA_FSM_ID_ENUM_UINT32 TAF_MMA_GetCurrFsmId(VOS_VOID)
{
    return (TAF_MMA_GetMmaCtxAddr()->stCurFsm.enFsmId);
}


TAF_MMA_MSG_QUEUE_STRU* TAF_MMA_GetCachMsgBufferAddr(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stBufferEntryMsgQueue);
}


VOS_VOID TAF_MMA_FSM_PushFsm(
    TAF_MMA_FSM_STACK_STRU              *pstFsmStack,
    TAF_MMA_FSM_CTX_STRU                *pstNewFsm
)
{
    TAF_MMA_FSM_CTX_STRU                *pstCurFsm;

    if ( (VOS_NULL_PTR == pstFsmStack)
      || (VOS_NULL_PTR == pstNewFsm) )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA,
                       "TAF_MMA_FSM_PushFsm,Para null ptr,pstFsmStack,pstNewFsm");

        return;
    }

    if ( pstFsmStack->usStackDepth >= TAF_MMA_MAX_STACK_DEPTH )
    {
        TAF_ERROR_LOG1(WUEPS_PID_MMA,
                       "TAF_MMA_FSM_PushFsm,Fsm Stack Depth Error!",
                       pstFsmStack->usStackDepth);

        return;
    }

    pstCurFsm               = &((pstFsmStack->astFsmStack)[pstFsmStack->usStackDepth]);

    /* ����ǰ״̬������CTX����Ҫ�������� */
    PS_MEM_CPY(pstCurFsm, pstNewFsm, sizeof(TAF_MMA_FSM_CTX_STRU));

    pstFsmStack->usStackDepth++;


    return;
}


VOS_VOID TAF_MMA_FSM_PopFsm( VOS_VOID )
{
    VOS_UINT16                          usPopFsmPos;
    TAF_MMA_FSM_STACK_STRU             *pstFsmStack = VOS_NULL_PTR;
    TAF_MMA_FSM_CTX_STRU               *pstCurFsm   = VOS_NULL_PTR;

    /* ���ݲ���״̬��ջ������״̬����ȡ��ǰ״̬��������ַ  */
    pstFsmStack                     = TAF_MMA_GetFsmStackAddr();
    pstCurFsm                       = TAF_MMA_GetCurFsmAddr();

    /* ջ�ѿ� */
    if ( 0 == (pstFsmStack->usStackDepth) )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_FSM_PopFsm,FSM Stack empty");

        return;
    }

    pstFsmStack->usStackDepth--;
    usPopFsmPos   = pstFsmStack->usStackDepth;

    /* ���õ�ǰ״̬�� */
    PS_MEM_CPY(pstCurFsm,
              &((pstFsmStack->astFsmStack)[usPopFsmPos]),
              sizeof(TAF_MMA_FSM_CTX_STRU));

    return;
}


VOS_VOID  TAF_MMA_LoadSubFsm(
    TAF_MMA_FSM_ID_ENUM_UINT32          enFsmId,
    TAF_MMA_FSM_CTX_STRU               *pstCurFsm
)
{
    switch ( enFsmId )
    {
        case TAF_MMA_FSM_PHONE_MODE :
            pstCurFsm->ulState                  = TAF_MMA_PHONE_MODE_STA_INIT;
            pstCurFsm->pstFsmDesc               = TAF_MMA_GetPhoneModeFsmDescAddr();
            break;

#if (FEATURE_IMS == FEATURE_ON)
        case TAF_MMA_FSM_IMS_SWITCH :
            pstCurFsm->ulState                  = TAF_MMA_IMS_SWITCH_STA_INIT;
            pstCurFsm->pstFsmDesc               = TAF_MMA_GetImsSwitchFsmDescAddr();
            break;
#endif

        case TAF_MMA_FSM_SYS_CFG :
            pstCurFsm->ulState                  = TAF_MMA_SYS_CFG_STA_INIT;
            pstCurFsm->pstFsmDesc               = TAF_MMA_GetSysCfgFsmDescAddr();
            break;


        default:
            TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_LoadSubFsm:Invalid Fsm Id");
            return;
    }


    /*ִ�г�ʼ��sub״̬���ĺ���*/
    if (VOS_NULL_PTR != pstCurFsm->pstFsmDesc->pfInitHandle)
    {
        pstCurFsm->pstFsmDesc->pfInitHandle();
    }
}


VOS_VOID TAF_MMA_FSM_InitSubFsm(
    TAF_MMA_FSM_ID_ENUM_UINT32          enFsmId
)
{
    TAF_MMA_FSM_STACK_STRU             *pstFsmStack = VOS_NULL_PTR;
    TAF_MMA_FSM_CTX_STRU               *pstCurFsm   = VOS_NULL_PTR;


    pstCurFsm                           = TAF_MMA_GetCurFsmAddr();
    pstFsmStack                         = TAF_MMA_GetFsmStackAddr();

    /*  ��ӡѹջǰ��main��sub״̬ */
    TAF_MMA_LogFsmInfo();

    /* ����ǰ״̬��ѹջ���� */
    TAF_MMA_FSM_PushFsm(pstFsmStack, pstCurFsm);

    /* ���õ�ǰ״̬�� */
    pstCurFsm->enFsmId                  = enFsmId;

    /* ����sub״̬�� */
    TAF_MMA_LoadSubFsm(enFsmId, pstCurFsm);

    /*  ��ӡ��ǰ��main��sub״̬ */
    TAF_MMA_LogFsmInfo();
}



VOS_VOID TAF_MMA_FSM_QuitSubFsm( VOS_VOID )
{
    TAF_MMA_FSM_CTX_STRU               *pstCurFsm   = VOS_NULL_PTR;

    pstCurFsm                           = TAF_MMA_GetCurFsmAddr();

    /* QUITʱ��յ�ǰsub״̬����CTX */
    if (VOS_NULL_PTR != pstCurFsm->pstFsmDesc->pfInitHandle)
    {
        pstCurFsm->pstFsmDesc->pfInitHandle();
    }

    /*  ��ӡѹջǰ��main��sub״̬ */
    TAF_MMA_LogFsmInfo();

    /* ״̬����ջ���� */
    TAF_MMA_FSM_PopFsm();

    /*  ��ӡ��ǰ��main��sub״̬ */
    TAF_MMA_LogFsmInfo();

    return;
}


VOS_UINT16  TAF_MMA_GetFsmStackDepth( VOS_VOID )
{
    TAF_MMA_FSM_STACK_STRU             *pstFsmStack = VOS_NULL_PTR;

    pstFsmStack = TAF_MMA_GetFsmStackAddr();

    return pstFsmStack->usStackDepth;
}


VOS_UINT32  TAF_MMA_IsExistCacheMsg(
    VOS_UINT32                          ulEventType,
    VOS_UINT32                         *pulIndex
)
{
    TAF_MMA_MSG_QUEUE_STRU             *pstMsgQueue = VOS_NULL_PTR;
    VOS_UINT32                          i;

    pstMsgQueue                         = TAF_MMA_GetCachMsgBufferAddr();
   *pulIndex                           = 0;

    for ( i = 0 ; i < pstMsgQueue->ucCacheMsgNum ; i++ )
    {
        if ( ulEventType == pstMsgQueue->astMsgQueue[i].stMsgEntry.ulEventType)
        {
            *pulIndex   = i;
            return VOS_TRUE;
        }
    }

    TAF_INFO_LOG(WUEPS_PID_MMA, "TAF_MMA_IsExistCacheMsg:not match MSG");

    return VOS_FALSE;
}


VOS_UINT32  TAF_MMA_IsNeedCacheMsg(
    VOS_UINT32                          ulEventType,
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT32                          ulIndex;

    switch (ulEventType)
    {
        /* ��ǰ��״ָ̬ʾ��Ϣ�Ļ���ֻ��Ҫ����һ�� */
        case TAF_BuildEventType(WUEPS_PID_MMA, MMA_MMA_INTER_USIM_STATUS_CHANGE_IND):

            if (VOS_TRUE == TAF_MMA_IsExistCacheMsg(TAF_BuildEventType(WUEPS_PID_MMA, MMA_MMA_INTER_USIM_STATUS_CHANGE_IND), &ulIndex))
            {
                return VOS_FALSE;
            }

            break;

        default:
            break;
    }

    /* Ĭ�Ϸ��ر������Ϣ */
    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_SaveCacheMsgInMsgQueue(
    VOS_UINT32                          ulEventType,
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_MSG_QUEUE_STRU              *pstMsgQueue   = VOS_NULL_PTR;
    MSG_HEADER_STRU                     *pstMsgHeader  = VOS_NULL_PTR;

    pstMsgHeader = (MSG_HEADER_STRU*)pstMsg;

    pstMsgQueue = TAF_MMA_GetCachMsgBufferAddr();

    if ( pstMsgQueue->ucCacheMsgNum >= TAF_MMA_MAX_MSG_QUEUE_NUM )
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SaveCacheMsgInMsgQueue:buffer full");

        TAF_MMA_LogBufferQueueMsg(VOS_TRUE);

        return VOS_FALSE;
    }

    /* �Ƚ���Ϣ���ȼ�,�ж��Ƿ���Ҫ�����ڻ������ */
    if (VOS_FALSE == TAF_MMA_IsNeedCacheMsg(ulEventType, pstMsg))
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SaveCacheMsgInMsgQueue:already exist msg");

        return VOS_FALSE;
    }

    pstMsgQueue->astMsgQueue[pstMsgQueue->ucCacheMsgNum].stMsgEntry.ulEventType   = ulEventType;
    PS_MEM_CPY((pstMsgQueue->astMsgQueue[pstMsgQueue->ucCacheMsgNum].stMsgEntry.aucEntryMsgBuffer),
               pstMsg,
               pstMsgHeader->ulLength + VOS_MSG_HEAD_LENGTH);
    pstMsgQueue->ucCacheMsgNum++;

    TAF_MMA_LogBufferQueueMsg(VOS_FALSE);

    TAF_INFO_LOG1(WUEPS_PID_MMA, "TAF_MMA_SaveCacheMsgInMsgQueue:Cache Num", pstMsgQueue->ucCacheMsgNum);

    return VOS_TRUE;
}


VOS_VOID  TAF_MMA_ClearCacheMsg(
    VOS_UINT32                          ulEventType
)
{
    TAF_MMA_MSG_QUEUE_STRU             *pstMsgQueue = VOS_NULL_PTR;
    VOS_INT8                            i;
    VOS_INT8                            cCachePos;

    pstMsgQueue                         = TAF_MMA_GetCachMsgBufferAddr();
    cCachePos                           = -1;

    /* ���ҵ���Ҫ������Ļ��� */
    for ( i = 0 ; i < pstMsgQueue->ucCacheMsgNum ; i++ )
    {
        if ( ulEventType == pstMsgQueue->astMsgQueue[i].stMsgEntry.ulEventType )
        {
            PS_MEM_SET(&(pstMsgQueue->astMsgQueue[i].stMsgEntry),
                       0x00,
                       sizeof(TAF_MMA_ENTRY_MSG_STRU));

            cCachePos = (VOS_INT8)i;
        }
    }

    /* δ�ҵ�����Ϣ����ֱ�ӷ��� */
    if ( -1 == cCachePos )
    {
        TAF_INFO_LOG1(WUEPS_PID_MMA, "TAF_MMA_ClearCacheMsg:invalid msg type", ulEventType);
        return;
    }

    /* Ϊ��֤�����ȳ�,��Ҫ���������Ϣ�Ƶ�ǰ�� */
    for ( i = cCachePos ; i < (pstMsgQueue->ucCacheMsgNum - 1) ; i++ )
    {
        /* COPY��Ϣ���� */
        PS_MEM_CPY(&(pstMsgQueue->astMsgQueue[i].stMsgEntry),
                   &(pstMsgQueue->astMsgQueue[i + 1].stMsgEntry),
                   sizeof(TAF_MMA_ENTRY_MSG_STRU));
    }

    /* �����������һ */
    pstMsgQueue->ucCacheMsgNum--;

    /* ����ǰ�������ӡ���� */
    TAF_MMA_LogBufferQueueMsg(VOS_FALSE);

    return;
}




VOS_VOID  TAF_MMA_InitFsmCtx_PhoneMode(VOS_VOID)
{
    TAF_MMA_SetCurPhoneErrorCode_PhoneMode(TAF_ERR_NO_ERROR);


    return;
}


VOS_VOID  TAF_MMA_InitFsmCtx_SysCfg(VOS_VOID)
{
    TAF_MMA_FSM_SYS_CFG_CTX_STRU       *pstSysCfgCtx = VOS_NULL_PTR;

    pstSysCfgCtx = &(TAF_MMA_GetMmaCtxAddr()->stCurFsm.unFsmCtx.stSysCfgCtx);

    PS_MEM_SET(pstSysCfgCtx, 0 , sizeof(TAF_MMA_FSM_SYS_CFG_CTX_STRU));

    pstSysCfgCtx->usOperType   = 0;
    pstSysCfgCtx->ucAttachOpId = 0xFF;

    pstSysCfgCtx->usOrigOperType = 0;

    return;
}


VOS_VOID  TAF_MMA_InitCurrFsmCtx(
    TAF_MMA_FSM_CTX_STRU                *pstCurrFsmCtx
)
{
    pstCurrFsmCtx->enFsmId                  =   TAF_MMA_FSM_MAIN;
    pstCurrFsmCtx->pstFsmDesc               =   TAF_MMA_GetMainFsmDescAddr();
    pstCurrFsmCtx->ulState                  =   TAF_MMA_MAIN_STA_IDLE;
    pstCurrFsmCtx->stEntryMsg.ulEventType   =   0;

    PS_MEM_SET(pstCurrFsmCtx->stEntryMsg.aucEntryMsgBuffer,
               0x00,
               TAF_MMA_MAX_MSG_BUFFER_LEN);
}


VOS_VOID  TAF_MMA_InitFsmStackCtx(
    TAF_MMA_FSM_STACK_STRU              *pstFsmStack
)
{
    VOS_UINT32                          i;

    pstFsmStack->usStackDepth   = 0;
    pstFsmStack->usStackPopFlg  = VOS_FALSE;

    for ( i = 0 ; i < TAF_MMA_MAX_STACK_DEPTH ; i++ )
    {
        pstFsmStack->astFsmStack[i].pstFsmDesc  = VOS_NULL_PTR;
        pstFsmStack->astFsmStack[i].enFsmId     = TAF_MMA_FSM_BUTT;
        pstFsmStack->astFsmStack[i].ulState     = TAF_MMA_MAIN_STA_BUTT;
    }
}


VOS_VOID  TAF_MMA_InitInternalBuffer(
    TAF_MMA_MSG_QUEUE_STRU             *pstBufferEntryMsgQueue
)
{
    pstBufferEntryMsgQueue->ucCacheMsgNum   = 0;
    PS_MEM_SET(pstBufferEntryMsgQueue->astMsgQueue,
               0x00,
               sizeof(TAF_MMA_CACH_MSG_INFO_STRU)*TAF_MMA_MAX_MSG_QUEUE_NUM);
}


VOS_VOID  TAF_MMA_InitInternalMsgQueue(
    TAF_MMA_INTERNAL_MSG_QUEUE_STRU     *pstInternalMsgQueue                   /* MMA���ڲ���Ϣ���� */
)
{
    VOS_UINT32                          i;

    pstInternalMsgQueue->ucHeader   = 0;
    pstInternalMsgQueue->ucTail     = 0;

    for ( i = 0 ; i < TAF_MMA_MAX_MSG_QUEUE_SIZE; i++ )
    {
        pstInternalMsgQueue->astMmaMsgQueue[i].usMsgID    = TAF_MMA_INVALID_INTERNAL_MSG_ID;
        PS_MEM_SET(pstInternalMsgQueue->astMmaMsgQueue[i].aucBuf,
                   0x00,
                   TAF_MMA_MAX_MSG_BUFFER_LEN);
    }

}


VOS_VOID  TAF_MMA_InitMaintainInfo(
    TAF_MMA_MAINTAIN_CTX_STRU          *pstMaintainInfo
)
{
    pstMaintainInfo->ucMmaLogInfoFlag   = VOS_FALSE;
}


VOS_VOID TAF_MMA_InitExternalModuleInitStatus(
    TAF_MMA_EXTERNAl_MODULE_INIT_STATUS_STRU      *pstInitStatusctx
)
{
    PS_MEM_SET(pstInitStatusctx, 0, sizeof(TAF_MMA_EXTERNAl_MODULE_INIT_STATUS_STRU));

    /* �������ȴ�������ʼ��������DSDS�����һ�� */
    pstInitStatusctx->enPhyInitStatus      = MMA_UPHY_INIT_STATUS_BUTT;

    return;

}


TAF_MMA_PHONE_MODE_CTRL_CTX_STRU *TAF_MMA_GetPhoneModeCtrl(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stPhoneModeCtrl);
}


VOS_UINT32  TAF_MMA_GetCacheNum( VOS_VOID )
{
    TAF_MMA_MSG_QUEUE_STRU             *pstMsgQueue = VOS_NULL_PTR;

    pstMsgQueue                         = TAF_MMA_GetCachMsgBufferAddr();

    return pstMsgQueue->ucCacheMsgNum;
}


TAF_MMA_MSG_CACHE_PRIO_ENUM_UINT8   TAF_MMA_GetCachedMsgPrio(
    TAF_MMA_ENTRY_MSG_STRU             *pstEntryMsg
)
{
    TAF_MMA_MSG_CACHE_PRIO_ENUM_UINT8   enPrio;

    switch (pstEntryMsg->ulEventType)
    {
        case TAF_BuildEventType(WUEPS_PID_SPY, OAM_MMA_PHONE_MODE_SET_REQ):
        case TAF_BuildEventType(CCPU_PID_CBT, OAM_MMA_PHONE_MODE_SET_REQ):
        case TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_PHONE_MODE_SET_REQ):

            enPrio = TAF_MMA_MSG_CACHE_PRIO_HIGH;
            break;

        case TAF_BuildEventType(WUEPS_PID_MMA, MMA_MMA_INTER_USIM_STATUS_CHANGE_IND):

            enPrio = TAF_MMA_MSG_CACHE_PRIO_MIDDLE;
            break;


        /* ��ѯԤ�����ȷ�����  */
        case TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_SYS_CFG_SET_REQ):
        case TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_PLMN_SPECIAL_SEL_REQ):
        case TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_PLMN_LIST_REQ):
        case TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_ATTACH_REQ):

        case TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_DETACH_REQ):
        case TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_PLMN_AUTO_RESEL_REQ):

#if (FEATURE_ON == FEATURE_CSG)
        case TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_CSG_LIST_SEARCH_REQ):
#endif
        default:

            enPrio = TAF_MMA_MSG_CACHE_PRIO_LOW;
            break;
    }

    return enPrio;
}



VOS_UINT32  TAF_MMA_GetNextPrioCachedMsg(
    TAF_MMA_ENTRY_MSG_STRU             *pstEntryMsg,
    TAF_MMA_MSG_CACHE_PRIO_ENUM_UINT8   enPrio
)
{
    TAF_MMA_MSG_QUEUE_STRU             *pstMsgQueue = VOS_NULL_PTR;
    VOS_UINT32                          i;

    pstMsgQueue                         = TAF_MMA_GetCachMsgBufferAddr();

    for (i = 0; i < pstMsgQueue->ucCacheMsgNum; i++)
    {
        if (enPrio == TAF_MMA_GetCachedMsgPrio(&pstMsgQueue->astMsgQueue[i].stMsgEntry))
        {
            PS_MEM_CPY(pstEntryMsg,
                       &(pstMsgQueue->astMsgQueue[i].stMsgEntry),
                       sizeof(TAF_MMA_ENTRY_MSG_STRU));

            /* ��Ϣ�Ѿ�ȡ���������ǰ���� */
            TAF_MMA_ClearCacheMsg(pstEntryMsg->ulEventType);

            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32  TAF_MMA_GetNextCachedMsg(
    TAF_MMA_ENTRY_MSG_STRU             *pstEntryMsg
)
{
    TAF_MMA_MSG_QUEUE_STRU             *pstMsgQueue = VOS_NULL_PTR;

    pstMsgQueue                         = TAF_MMA_GetCachMsgBufferAddr();

    if (0 == pstMsgQueue->ucCacheMsgNum )
    {
        TAF_INFO_LOG(WUEPS_PID_MMA, "TAF_MMA_GetNextCachedMsg:Empty buffer");

        return VOS_FALSE;
    }

    if (VOS_TRUE == TAF_MMA_GetNextPrioCachedMsg(pstEntryMsg, TAF_MMA_MSG_CACHE_PRIO_HIGH))
    {
        return VOS_TRUE;
    }

    if (VOS_TRUE == TAF_MMA_GetNextPrioCachedMsg(pstEntryMsg, TAF_MMA_MSG_CACHE_PRIO_MIDDLE))
    {
        return VOS_TRUE;
    }

    if (VOS_TRUE == TAF_MMA_GetNextPrioCachedMsg(pstEntryMsg, TAF_MMA_MSG_CACHE_PRIO_LOW))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_VOID  TAF_MMA_SetFsmStackPopFlg(
    VOS_UINT16                          ucStackPopFlg
)
{
    TAF_MMA_FSM_STACK_STRU             *pstFsmStack = VOS_NULL_PTR;

    pstFsmStack = TAF_MMA_GetFsmStackAddr();

    pstFsmStack->usStackPopFlg  = ucStackPopFlg;
}


VOS_UINT16  TAF_MMA_GetFsmStackPopFlg( VOS_VOID )
{
    TAF_MMA_FSM_STACK_STRU             *pstFsmStack = VOS_NULL_PTR;

    pstFsmStack = TAF_MMA_GetFsmStackAddr();

    return pstFsmStack->usStackPopFlg;
}


VOS_VOID  TAF_MMA_SetAutoInitPsFlg(
    VOS_UINT32                          ulAutoInitPsFlg
)
{
    TAF_MMA_GetPhoneModeCtrl()->ulAutoInitPsFlg = ulAutoInitPsFlg;
}


VOS_UINT32  TAF_MMA_GetAutoInitPsFlg(VOS_VOID)
{
    return (TAF_MMA_GetPhoneModeCtrl()->ulAutoInitPsFlg);
}


VOS_VOID  TAF_MMA_SetAutoSwitchOnFlg(
    VOS_UINT32                          ulAutoSwitchOnFlg
)
{
    TAF_MMA_GetPhoneModeCtrl()->ulAutoSwitchOnFlg = ulAutoSwitchOnFlg;
}


VOS_UINT32  TAF_MMA_GetAutoSwitchOnFlg(VOS_VOID)
{
    return (TAF_MMA_GetPhoneModeCtrl()->ulAutoSwitchOnFlg);
}





VOS_VOID  TAF_MMA_SetCurPhoneErrorCode_PhoneMode(
    TAF_ERROR_CODE_ENUM_UINT32          enErrorCode
)
{
    /* �����ǰ״̬������PHONE MODE, �쳣��ӡ */
    if (TAF_MMA_FSM_PHONE_MODE != TAF_MMA_GetCurrFsmId())
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_SetCurPhoneErrorCode_PhoneMode,ERROR:FsmId Error");
        return;
    }

    TAF_MMA_GetMmaCtxAddr()->stCurFsm.unFsmCtx.stPhoneModeCtx.enPhoneError = enErrorCode;
}



TAF_ERROR_CODE_ENUM_UINT32  TAF_MMA_GetCurPhoneErrorCode_PhoneMode(VOS_VOID)
{
    /* �����ǰ״̬������PHONE MODE, �쳣��ӡ */
    if (TAF_MMA_FSM_PHONE_MODE != TAF_MMA_GetCurrFsmId())
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "TAF_MMA_GetCurPhoneErrorCode_PhoneMode,ERROR:FsmId Error");
    }
    return (TAF_MMA_GetMmaCtxAddr()->stCurFsm.unFsmCtx.stPhoneModeCtx.enPhoneError);
}


TAF_MMA_TIMER_CTX_STRU*  TAF_MMA_GetTimerCtxAddr( VOS_VOID )
{
    return TAF_MMA_GetMmaCtxAddr()->astMmaTimerCtx;
}



TAF_MMA_OPER_CTX_STRU*  TAF_MMA_GetOperCtxAddr( VOS_VOID )
{
    return TAF_MMA_GetMmaCtxAddr()->astMmaOperCtx;
}




VOS_UINT8  TAF_MMA_GetMmaLogInfoFlag( VOS_VOID )
{
    return TAF_MMA_GetMmaCtxAddr()->stMaintainInfo.ucMmaLogInfoFlag;
}


VOS_VOID  TAF_MMA_SetMmaLogInfoFlag(
    VOS_UINT8                           ucMmaLogInfoFlag
)
{
    TAF_MMA_GetMmaCtxAddr()->stMaintainInfo.ucMmaLogInfoFlag = ucMmaLogInfoFlag;
}




VOS_VOID TAF_MMA_SetNetworkCapabilityInfo(
    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode,
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstNwCapInfo
)
{
    if (TAF_SDC_SYS_MODE_LTE == enSysMode)
    {
        TAF_SDC_SetLteNwCapInfo(pstNwCapInfo);
    }
    else
    {
        TAF_SDC_SetGuNwCapInfo(pstNwCapInfo);
    }
}


VOS_VOID TAF_MMA_SetPhyInitStatus(
    MMA_UPHY_INIT_STATUS_ENUM_UINT16    enFlag
)
{
    TAF_MMA_GetMmaCtxAddr()->stMmaExternalModuleInitStatusCtx.enPhyInitStatus = enFlag;
}


MMA_UPHY_INIT_STATUS_ENUM_UINT16 TAF_MMA_GetPhyInitStatus( VOS_VOID )
{
    return TAF_MMA_GetMmaCtxAddr()->stMmaExternalModuleInitStatusCtx.enPhyInitStatus;
}



VOS_UINT16 TAF_MMA_IsExternalModuleInitCompeleted( VOS_VOID )
{
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enUsimstatus;
    MMA_UPHY_INIT_STATUS_ENUM_UINT16    enUphyInitStatus;

    enUsimstatus     = TAF_SDC_GetSimStatus();
    enUphyInitStatus = TAF_MMA_GetPhyInitStatus();

    if((TAF_SDC_USIM_STATUS_BUTT     == enUsimstatus)
    || (MMA_UPHY_INIT_STATUS_BUTT    == enUphyInitStatus))
    {
        return VOS_FALSE;
    }
    else
    {
        return VOS_TRUE;
    }
}


VOS_UINT32 TAF_MMA_IsOperTypeUsed(
    TAF_MMA_OPER_TYPE_ENUM_UINT32       enOperType
)
{
    VOS_UINT32                          ulIndex;
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx;

    pstMmaOperCtx = TAF_MMA_GetOperCtxAddr();

    /*  enOperType�Ƿ����ڱ�ʹ��  */
    for (ulIndex = 0; ulIndex < TAF_MMA_MAX_OPER_NUM; ulIndex++)
    {
        if ( (enOperType == pstMmaOperCtx[ulIndex].enOperType)
          && (VOS_TRUE   == pstMmaOperCtx[ulIndex].ucUsedFlag) )
        {
            /* ����ǰOPER_CTX��ϢLOG���� */
            TAF_MMA_LogOperCtxInfo();

            return VOS_TRUE;
        }
    }

    return VOS_FALSE;

}



VOS_UINT32 TAF_MMA_GetSpecOperTypeIndex(
    TAF_MMA_OPER_TYPE_ENUM_UINT32       enOperType,
    VOS_UINT8                          *pucCtxIndex
)
{
    VOS_UINT32                          ulIndex;
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx;

    pstMmaOperCtx = TAF_MMA_GetOperCtxAddr();

    /*  enOperType�Ƿ����ڱ�ʹ��  */
    for (ulIndex = 0; ulIndex < TAF_MMA_MAX_OPER_NUM; ulIndex++)
    {
        if ( (enOperType == pstMmaOperCtx[ulIndex].enOperType)
          && (VOS_TRUE   == pstMmaOperCtx[ulIndex].ucUsedFlag) )
        {
            *pucCtxIndex = (VOS_UINT8)ulIndex;

            return VOS_TRUE;
        }
    }

    return VOS_FALSE;

}



VOS_UINT32 TAF_MMA_GetUnusedOperCtxIndex(
    VOS_UINT8                          *pucCtxIndex
)
{
    VOS_UINT32                          ulIndex;
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx;

    pstMmaOperCtx = TAF_MMA_GetOperCtxAddr();

    /*  �Ƿ��п����Ctx   */
    for (ulIndex = 0; ulIndex < TAF_MMA_MAX_OPER_NUM; ulIndex++)
    {
        /* �ҵ��˿����CTX */
        if (VOS_FALSE == pstMmaOperCtx[ulIndex].ucUsedFlag)
        {
            *pucCtxIndex = (VOS_UINT8)ulIndex;

            return VOS_TRUE;
        }

    }

    /* ����ǰOPER_CTX��ϢLOG���� */
    TAF_MMA_LogOperCtxInfo();

    /* û���ҵ������CTX */
    return VOS_FALSE;

}


VOS_VOID TAF_MMA_SetOperCtx(
    TAF_MMA_CTRL_STRU                   stCtrl,
    TAF_MMA_OPER_TYPE_ENUM_UINT32       enOperType,
    VOS_UINT8                           ucCtxIndex
)
{
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx;

    pstMmaOperCtx = TAF_MMA_GetOperCtxAddr();

    pstMmaOperCtx[ucCtxIndex].stCtrl.ucOpId     = stCtrl.ucOpId;
    pstMmaOperCtx[ucCtxIndex].stCtrl.usClientId = stCtrl.usClientId;
    pstMmaOperCtx[ucCtxIndex].stCtrl.ulModuleId = stCtrl.ulModuleId;
    pstMmaOperCtx[ucCtxIndex].enOperType        = enOperType;
    pstMmaOperCtx[ucCtxIndex].ucUsedFlag        = VOS_TRUE;

    /* ����ǰOPER_CTX��ϢLOG���� */
    TAF_MMA_LogOperCtxInfo();

    return;
}


VOS_VOID TAF_MMA_ClearOperCtx(
    VOS_UINT8                           ucCtxIndex
)
{
    TAF_MMA_OPER_CTX_STRU              *pstMmaOperCtx;

    pstMmaOperCtx = TAF_MMA_GetOperCtxAddr();

    PS_MEM_SET(&(pstMmaOperCtx[ucCtxIndex]), 0, sizeof(TAF_MMA_OPER_CTX_STRU));

    pstMmaOperCtx[ucCtxIndex].ucUsedFlag = VOS_FALSE;
    pstMmaOperCtx[ucCtxIndex].enOperType = TAF_MMA_OPER_BUTT;

    /* ����ǰOPER_CTX��ϢLOG���� */
    TAF_MMA_LogOperCtxInfo();

    return;
}



TAF_MMA_CFREQ_LOCK_SET_PARA_STRU* TAF_MMA_GetCFreqLockSetPara(VOS_VOID)
{
    return &g_stTafMmaCtx.stCFreqLockValue;
}


VOS_VOID TAF_MMA_InitCFreqLockPara(VOS_VOID)
{
    TAF_MMA_CFREQ_LOCK_SET_PARA_STRU   *pstTafMmaCFreqLockPara;

    pstTafMmaCFreqLockPara = TAF_MMA_GetCFreqLockSetPara();

    PS_MEM_SET(pstTafMmaCFreqLockPara, 0x0, sizeof(TAF_MMA_CFREQ_LOCK_SET_PARA_STRU));

    return;
}




VOS_VOID TAF_MMA_SetSysCfgOperType_SysCfg(
    TAF_MMA_SYS_CFG_OPER_TYPE           usOperType
)
{
    TAF_MMA_GetMmaCtxAddr()->stCurFsm.unFsmCtx.stSysCfgCtx.usOperType = usOperType;

    return;
}


VOS_VOID TAF_MMA_SaveSysCfgOrigOperType_SysCfg(
    TAF_MMA_SYS_CFG_OPER_TYPE           usOperType
)
{
    TAF_MMA_GetMmaCtxAddr()->stCurFsm.unFsmCtx.stSysCfgCtx.usOrigOperType = usOperType;

    return;
}



TAF_MMA_SYS_CFG_OPER_TYPE TAF_MMA_GetSysCfgOrigOperType_SysCfg(VOS_VOID)
{
    return (TAF_MMA_GetMmaCtxAddr()->stCurFsm.unFsmCtx.stSysCfgCtx.usOrigOperType);
}




VOS_VOID TAF_MMA_SetSysCfgPara_SysCfg(
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara
)
{
    PS_MEM_CPY(&(TAF_MMA_GetMmaCtxAddr()->stCurFsm.unFsmCtx.stSysCfgCtx.stSysCfgPara),
                pstSysCfgPara,
                sizeof(TAF_MMA_SYS_CFG_PARA_STRU));
    return;
}


TAF_MMA_SYS_CFG_PARA_STRU *TAF_MMA_GetSysCfgPara_SysCfg(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stCurFsm.unFsmCtx.stSysCfgCtx.stSysCfgPara);
}


VOS_VOID TAF_MMA_SetAttachOpId_SysCfg(
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_GetMmaCtxAddr()->stCurFsm.unFsmCtx.stSysCfgCtx.ucAttachOpId = ucOpId;

    return;
}


#if (FEATURE_ON == FEATURE_LTE)

TAF_USER_SET_PREF_BAND64 *TAF_MMA_GetUserSetLteBandAddr(VOS_VOID)
{
    return &(g_stTafMmaCtx.stLastSyscfgSet.stUserSetBand.stUserSetLteBand);
}


TAF_USER_SET_PREF_BAND64 *TAF_MMA_GetUeSupportLteBandAddr(VOS_VOID)
{
    return &(g_stTafMmaCtx.stUeBandCap.stUeSupportLteBand);
}
#endif


TAF_MMA_RAT_ORDER_STRU *TAF_MMA_GetRatPrioListAddr(VOS_VOID)
{
    return &(g_stTafMmaCtx.stLastSyscfgSet.stRatPrioList);
}


TAF_MMA_LAST_SETTED_SYSCFG_SET_STRU *TAF_MMA_GetLastSyscfgSetAddr(VOS_VOID)
{
    return &(g_stTafMmaCtx.stLastSyscfgSet);
}


TAF_MMA_SYSCFG_USER_SET_BAND_STRU *TAF_MMA_GetSyscfgUserSetBandAddr(VOS_VOID)
{
    return &(g_stTafMmaCtx.stLastSyscfgSet.stUserSetBand);
}


TAF_MMA_ROAM_MODE_ENUM_UINT8 TAF_MMA_GetSyscfgUserSetRoamValue(VOS_VOID)
{
    return (g_stTafMmaCtx.stLastSyscfgSet.enRoam);
}


TAF_MMA_UE_BAND_CAPA_ST *TAF_MMA_GetUeSupportBandAddr(VOS_VOID)
{
    return &(g_stTafMmaCtx.stUeBandCap);
}



VOS_UINT8 TAF_MMA_GetAttachOpId(VOS_VOID)
{
    TAF_MMA_CONTEXT_STRU               *pstMmaCtx = VOS_NULL_PTR;
    VOS_UINT8                           ucOpId;

    pstMmaCtx = TAF_MMA_GetMmaCtxAddr();

    ucOpId = pstMmaCtx->stAttachInfo.ucAttachOpId;

    pstMmaCtx->stAttachInfo.ucAttachOpId++;

    pstMmaCtx->stAttachInfo.ucAttachOpId %= 0xFF;

    return ucOpId;
}

#if (FEATURE_ON == FEATURE_CSG)

TAF_MMA_PLMN_LIST_ABORT_PROC_ENUM_UINT8 TAF_MMA_GetCsgListAbortType(VOS_VOID)
{
    return (TAF_MMA_GetMmaCtxAddr()->stCsgListInfo.enCsgListAbortProc);
}


VOS_VOID TAF_MMA_SetCsgListAbortType(
    TAF_MMA_PLMN_LIST_ABORT_PROC_ENUM_UINT8                 enCsgListAbortType
)
{
    TAF_MMA_GetMmaCtxAddr()->stCsgListInfo.enCsgListAbortProc = enCsgListAbortType;
    return;
}



TAF_MMA_CSG_LIST_CTX_STRU* TAF_MMA_GetCsgListCtxAddr(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stCsgListInfo);
}


VOS_VOID TAF_MMA_InitCsgListCtx(VOS_VOID)
{
    TAF_MMA_CSG_LIST_CTX_STRU          *pstCsgListCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulIndex;

    pstCsgListCtx = TAF_MMA_GetCsgListCtxAddr();

    PS_MEM_SET(pstCsgListCtx, 0x0, sizeof(pstCsgListCtx));

    pstCsgListCtx->enCsgListAbortProc = TAF_MMA_PLMN_LIST_ABORT_BUTT;

    for (ulIndex = 0; ulIndex < TAF_SDC_MAX_CSG_ID_LIST_NUM; ulIndex++)
    {
        pstCsgListCtx->astCsgIdListInfo[ulIndex].stPlmnId.Mcc = TAF_SDC_INVALID_MCC;
        pstCsgListCtx->astCsgIdListInfo[ulIndex].stPlmnId.Mnc = TAF_SDC_INVALID_MNC;
        pstCsgListCtx->astCsgIdListInfo[ulIndex].ulCsgId      = TAF_SDC_INVALID_CSG_ID_VALUE;
    }

    return;
}
#endif


TAF_MMA_GET_GEO_CTX_STRU* TAF_MMA_GetGeoCtxAddr(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stGetGeoInfo);
}


VOS_VOID TAF_MMA_InitGetGeoCtx(VOS_VOID)
{
    TAF_MMA_GET_GEO_CTX_STRU           *pstGetGeoCtx = VOS_NULL_PTR;

    pstGetGeoCtx = TAF_MMA_GetGeoCtxAddr();

    PS_MEM_SET(pstGetGeoCtx, 0x0, sizeof(TAF_MMA_GET_GEO_CTX_STRU));

    pstGetGeoCtx->stGetGeoNvCfg.ucCfgGetGeoFlag        = VOS_FALSE;
    pstGetGeoCtx->stGetGeoNvCfg.ucCcpuResetFlag        = VOS_FALSE;
    pstGetGeoCtx->stGetGeoNvCfg.aucReserved[0]         = 0;
    pstGetGeoCtx->stGetGeoNvCfg.aucReserved[1]         = 10;
    pstGetGeoCtx->stGetGeoNvCfg.ulGetGeoTimerLen       = TAF_MMA_GET_GEO_TIMER_DEFAULT_LEN;
    pstGetGeoCtx->stGetGeoNvCfg.ulGeoEffectiveTimeLen  = TAF_MMA_GEO_EFFECTIVE_TIME_DEFAULT_LEN;

    pstGetGeoCtx->ulGetGeoTickTime       = VOS_GetTick() / PRE_SECOND_TO_TICK;
    pstGetGeoCtx->stGeoPlmnId.ulMcc      = TAF_SDC_INVALID_MCC;
    pstGetGeoCtx->stGeoPlmnId.ulMnc      = TAF_SDC_INVALID_MNC;

    pstGetGeoCtx->ulGetGeoCdmaTickTime   = VOS_GetTick() / PRE_SECOND_TO_TICK;
    pstGetGeoCtx->stGeoCdmaLocInfo.ulMcc = TAF_SDC_INVALID_MCC;
    pstGetGeoCtx->stGeoCdmaLocInfo.lSid  = MMA_INVALID_SID;
    pstGetGeoCtx->stGeoCdmaLocInfo.lNid  = MMA_INVALID_NID;
    return;
}


TAF_MMA_SET_DPLMN_CTX_STRU* TAF_MMA_GetSetDplmnCtxAddr(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stSetDplmnCtx);
}


VOS_VOID TAF_MMA_InitSetDplmnCtx(VOS_VOID)
{

    TAF_MMA_SET_DPLMN_CTX_STRU           *pstSetDplmnCtx = VOS_NULL_PTR;

    pstSetDplmnCtx = TAF_MMA_GetSetDplmnCtxAddr();

    PS_MEM_SET(pstSetDplmnCtx, 0x00, sizeof(TAF_MMA_SET_DPLMN_CTX_STRU));

    pstSetDplmnCtx->enApPresetDplmnScene = TAF_MMA_AP_PRESET_DPLMN_SCENE_BUTT;

    return;
}


VOS_VOID TAF_MMA_SetDplmnUpdateScene(
    TAF_MMA_AP_PRESET_DPLMN_SCENE_ENUM_UINT8                enApPresetDplmnScene
)
{
    TAF_MMA_SET_DPLMN_CTX_STRU           *pstSetDplmnCtx = VOS_NULL_PTR;

    pstSetDplmnCtx = TAF_MMA_GetSetDplmnCtxAddr();

    pstSetDplmnCtx->enApPresetDplmnScene = enApPresetDplmnScene;

    return;
}


TAF_MMA_AP_PRESET_DPLMN_SCENE_ENUM_UINT8 TAF_MMA_GetDplmnUpdateScene(VOS_VOID)
{
    TAF_MMA_SET_DPLMN_CTX_STRU           *pstSetDplmnCtx = VOS_NULL_PTR;

    pstSetDplmnCtx = TAF_MMA_GetSetDplmnCtxAddr();

    return pstSetDplmnCtx->enApPresetDplmnScene;
}



VOS_VOID TAF_MMA_InitGetHplmnInfo(VOS_VOID)
{
    TAF_MMA_GET_HPLMN_STRU             *pstGetHplmnInfo = VOS_NULL_PTR;

    pstGetHplmnInfo = TAF_MMA_GetHplmnInfo();

    PS_MEM_SET(pstGetHplmnInfo, 0x0, sizeof(TAF_MMA_GET_HPLMN_STRU));

    pstGetHplmnInfo->ucHplmnMncLen          = 0;
    pstGetHplmnInfo->enWaitSimFileCnfFlg    = TAF_MMA_READ_USIM_FILE_FLG_NULL;
    pstGetHplmnInfo->ulEHplmnNum            = 0;

    return;
}



TAF_MMA_GET_HPLMN_STRU *TAF_MMA_GetHplmnInfo(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stGetHplmnInfo);
}


VOS_VOID TAF_MMA_InitSdcIccId( VOS_VOID )
{
    TAF_SDC_ICCID_INFO_STRU             *pstIccIdInfo   = VOS_NULL_PTR;

    pstIccIdInfo        = TAF_SDC_GetIccIdInfo();

    PS_MEM_SET(pstIccIdInfo, 0xFF, sizeof(TAF_SDC_ICCID_INFO_STRU));

    pstIccIdInfo->ucLen = TAF_SDC_ICC_ID_MAX_NUM + 1;

    return;
}


VOS_UINT32 TAF_MMA_IsValidIccId( VOS_VOID )
{
    TAF_SDC_ICCID_INFO_STRU              stNullIccIdInfo;

    PS_MEM_SET(&stNullIccIdInfo, 0xFF, sizeof(TAF_SDC_ICCID_INFO_STRU));
    stNullIccIdInfo.ucLen   = TAF_SDC_ICC_ID_MAX_NUM + 1;

    if (0 == PS_MEM_CMP(TAF_SDC_GetIccIdInfo(), &stNullIccIdInfo, sizeof(TAF_SDC_ICCID_INFO_STRU)))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT8  TAF_MMA_GetCfgGetGeoFlag(VOS_VOID)
{
    return TAF_MMA_GetGeoCtxAddr()->stGetGeoNvCfg.ucCfgGetGeoFlag;
}


VOS_UINT32 TAF_MMA_GetGeoTimerLen(VOS_VOID)
{
    return TAF_MMA_GetGeoCtxAddr()->stGetGeoNvCfg.ulGetGeoTimerLen;
}


VOS_VOID TAF_MMA_SetCcpuResetFlag(VOS_UINT8 ucCcpuResetFlag)
{
    TAF_MMA_GetGeoCtxAddr()->stGetGeoNvCfg.ucCcpuResetFlag = ucCcpuResetFlag;
}


VOS_UINT8 TAF_MMA_GetCcpuResetFlag(VOS_VOID)
{
    return TAF_MMA_GetGeoCtxAddr()->stGetGeoNvCfg.ucCcpuResetFlag;
}


VOS_UINT32 TAF_MMA_GetGeoEffectiveTimeLen(VOS_VOID)
{
    return TAF_MMA_GetGeoCtxAddr()->stGetGeoNvCfg.ulGeoEffectiveTimeLen;
}


VOS_UINT32 TAF_MMA_IsGeoValid(VOS_VOID)
{
    TAF_MMA_GET_GEO_CTX_STRU           *pstGetGeoCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulCurrTickTime;
    VOS_UINT32                          ulGeoEffectiveTimeLen;
    VOS_UINT32                          ulIsPlmnIdValid;

    ulCurrTickTime          = VOS_GetTick() / PRE_SECOND_TO_TICK;
    ulGeoEffectiveTimeLen   = TAF_MMA_GetGeoEffectiveTimeLen();
    pstGetGeoCtx            = TAF_MMA_GetGeoCtxAddr();
    ulIsPlmnIdValid         = TAF_SDC_IsPlmnIdValid(&pstGetGeoCtx->stGeoPlmnId);

    /* get geo NV�ر�ʱҲ��Ϊ��Ч */
    if (VOS_FALSE == pstGetGeoCtx->stGetGeoNvCfg.ucCfgGetGeoFlag)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��PLMN��Ч���߳�����Чʱ��������Ϊ����������Ч�� */
    if ((VOS_FALSE == ulIsPlmnIdValid)
     || ((ulCurrTickTime - pstGetGeoCtx->ulGetGeoTickTime) > ulGeoEffectiveTimeLen))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_IsNeedGetGeoNoSimStart(VOS_VOID)
{
    VOS_UINT8                           ucCfgGetGeoFlag;
    VOS_UINT8                           ucCcpuResetFlag;

    ucCfgGetGeoFlag  = TAF_MMA_GetCfgGetGeoFlag();
    ucCcpuResetFlag  = TAF_MMA_GetCcpuResetFlag();
#if (FEATURE_ON == FEATURE_LTE)
    /* ���ƽ̨��֧��L����ֱ�ӷ��ز�֧�� */
    if (VOS_TRUE == TAF_SDC_IsPlatformSupportLte())
    {
        if ((VOS_TRUE  == ucCfgGetGeoFlag)
         && (VOS_TRUE  != ucCcpuResetFlag))
        {
            return VOS_TRUE;
        }
    }
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if (VOS_TRUE == TAF_SDC_IsPlatformSupport1x())
    {
        if ((VOS_TRUE  == ucCfgGetGeoFlag)
         && (VOS_TRUE  != ucCcpuResetFlag))
        {
            return VOS_TRUE;
        }
    }
#endif

    return VOS_FALSE;
}



VOS_VOID TAF_MMA_SetTestRoamFlag(VOS_UINT8 ucFlag)
{
    TAF_MMA_GetGeoCtxAddr()->stGetGeoNvCfg.aucReserved[0] = ucFlag;
}


VOS_UINT8 TAF_MMA_GetTestRoamFlag(VOS_VOID)
{
    return TAF_MMA_GetGeoCtxAddr()->stGetGeoNvCfg.aucReserved[0];
}


VOS_VOID TAF_MMA_SetWaitExtTimerLen(VOS_UINT8 ucLen)
{
    TAF_MMA_GetGeoCtxAddr()->stGetGeoNvCfg.aucReserved[1] = ucLen;
}


VOS_UINT8 TAF_MMA_GetWaitExtTimerLen(VOS_VOID)
{
    return TAF_MMA_GetGeoCtxAddr()->stGetGeoNvCfg.aucReserved[1];
}



TAF_MMA_SRV_ACQ_RAT_LIST_STRU* TAF_MMA_GetSrvAcqRatListAddr(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stDsdsInfo.stSrvAcqRatList);
}


VOS_VOID TAF_MMA_UpdateSrvAcqRatList(
    TAF_MMA_SRV_ACQ_RAT_LIST_STRU      *pstSrvAcqRatList
)
{
    TAF_MMA_SRV_ACQ_RAT_LIST_STRU      *pstSrvAcqRatListAddr = VOS_NULL_PTR;

    pstSrvAcqRatListAddr = TAF_MMA_GetSrvAcqRatListAddr();

    PS_MEM_CPY(pstSrvAcqRatListAddr, pstSrvAcqRatList, sizeof(TAF_MMA_SRV_ACQ_RAT_LIST_STRU));
}


VOS_VOID TAF_MMA_ClearSrvAcqRatList(VOS_VOID)
{
    TAF_MMA_SRV_ACQ_RAT_LIST_STRU      *pstSrvAcqRatListAddr = VOS_NULL_PTR;

    pstSrvAcqRatListAddr = TAF_MMA_GetSrvAcqRatListAddr();

    PS_MEM_SET(pstSrvAcqRatListAddr, 0, sizeof(TAF_MMA_SRV_ACQ_RAT_LIST_STRU));
}



VOS_VOID TAF_MMA_SetDelayReportServiceStatusFlg(
    VOS_UINT8 ucDelayReportServiceStatusFlg
)
{
    TAF_MMA_GetMmaCtxAddr()->stDelayRptCtrlNVInfo.stGulDelayRptCtrlInfo.ucEnable= ucDelayReportServiceStatusFlg;
}


VOS_UINT8 TAF_MMA_GetDelayReportServiceStatusFlg(VOS_VOID)
{
    return (TAF_MMA_GetMmaCtxAddr()->stDelayRptCtrlNVInfo.stGulDelayRptCtrlInfo.ucEnable);
}


VOS_VOID TAF_MMA_SetDelayReportServiceStatusTimerLen(
    VOS_UINT32 ulTimerLen
)
{
    TAF_MMA_GetMmaCtxAddr()->stDelayRptCtrlNVInfo.stGulDelayRptCtrlInfo.ulRptSrvStaDelayTimerValue = ulTimerLen;
}


VOS_UINT32 TAF_MMA_GetDelayReportServiceStatusCfgTimerLen(VOS_VOID)
{
    return (TAF_MMA_GetMmaCtxAddr()->stDelayRptCtrlNVInfo.stGulDelayRptCtrlInfo.ulRptSrvStaDelayTimerValue);
}



TAF_MMA_CL_NO_SERVICE_DELAY_RPT_STRU *TAF_MMA_GetCLNoServiceDelayRptInfo(VOS_VOID)
{
    return &(TAF_MMA_GetMmaCtxAddr()->stDelayRptCtrlNVInfo.stCLNoServiceDelayRptInfo);
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_VOID TAF_MMA_SetCLDelayReptSrvStatusFlg(
    VOS_UINT8 ucCLDelayReptSrvStatusFlg
)
{
    TAF_MMA_GetMmaCtxAddr()->stDelayRptCtrlNVInfo.stClDelayRptCtrlInfo.ucClDelayRptEnable = ucCLDelayReptSrvStatusFlg;
}


VOS_UINT8 TAF_MMA_GetCLDelayReptSrvStatusFlg(VOS_VOID)
{
    return (TAF_MMA_GetMmaCtxAddr()->stDelayRptCtrlNVInfo.stClDelayRptCtrlInfo.ucClDelayRptEnable);
}


VOS_VOID TAF_MMA_SetCLDelayReptSrvStatusTimerLen(
    VOS_UINT32 ulTimerLen
)
{
    TAF_MMA_GetMmaCtxAddr()->stDelayRptCtrlNVInfo.stClDelayRptCtrlInfo.ulClRptSrvStaDelayTimerValue = ulTimerLen;
}


VOS_UINT32 TAF_MMA_GetCLDelayReptSrvStatusTimerLen(VOS_VOID)
{
    return (TAF_MMA_GetMmaCtxAddr()->stDelayRptCtrlNVInfo.stClDelayRptCtrlInfo.ulClRptSrvStaDelayTimerValue);
}
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 TAF_MMA_IsGeoCdmaLocValid(VOS_VOID)
{
    TAF_MMA_GET_GEO_CTX_STRU           *pstGetGeoCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulCurrTickTime;
    VOS_UINT32                          ulGeoEffectiveTimeLen;

    ulCurrTickTime          = VOS_GetTick() / PRE_SECOND_TO_TICK;
    ulGeoEffectiveTimeLen   = TAF_MMA_GetGeoEffectiveTimeLen();
    pstGetGeoCtx            = TAF_MMA_GetGeoCtxAddr();

    /* get geo NV�ر�ʱҲ��Ϊ��Ч */
    if (VOS_FALSE == pstGetGeoCtx->stGetGeoNvCfg.ucCfgGetGeoFlag)
    {
        return VOS_FALSE;
    }

    /* ���Mcc����Чֵ������Ϊ����Cdma��λ����Ϣ��Ч */
    /* ��ʱ�������жϣ�λ����Ϣ�Ƿ�ʧЧ */
    if ((TAF_SDC_INVALID_MCC == pstGetGeoCtx->stGeoCdmaLocInfo.ulMcc)
     || ((ulCurrTickTime - pstGetGeoCtx->ulGetGeoCdmaTickTime) > ulGeoEffectiveTimeLen))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID TAF_MMA_UpdateGeoCdmaLocInfo(
    VOS_INT32                   lSid,
    VOS_INT32                   lNid
)
{
    TAF_MMA_GET_GEO_CTX_STRU            *pstGetGeoCtx = VOS_NULL_PTR;

    pstGetGeoCtx        = TAF_MMA_GetGeoCtxAddr();

    /* get geo NV��ر�ʱ����Ҫ���� */
    if (VOS_FALSE == pstGetGeoCtx->stGetGeoNvCfg.ucCfgGetGeoFlag)
    {
        return;
    }

    pstGetGeoCtx->stGeoCdmaLocInfo.lSid  = lSid;
    pstGetGeoCtx->stGeoCdmaLocInfo.lNid  = lNid;
    /* ʹ��Sid��g_astTafCountryList���в���Mcc��������Ҳ�����������Чֵ��ȫF */
    pstGetGeoCtx->stGeoCdmaLocInfo.ulMcc = TAF_QueryMccWithSid(lSid);

    pstGetGeoCtx->ulGetGeoCdmaTickTime   = VOS_GetTick() / PRE_SECOND_TO_TICK;

    return;
}

#endif
/*lint -restore */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



