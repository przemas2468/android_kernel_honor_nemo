

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "CnasMain.h"
#include "CnasXsdMain.h"
#include "CnasXregMain.h"
#include "CnasXccMain.h"
#include "CnasMntn.h"
#include "CnasCcb.h"
#include "CnasPrlApi.h"
#include "CnasHluMain.h"

#include "CnasHsdMain.h"

#include  "CnasHsmMain.h"

#include "CnasEhsmMain.h"

#include  "Nas_Mem.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_MAIN_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
CNAS_INTERNAL_MSG_CTX_STRU              g_stCnasIntCtxInfo;

/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -save -e958 */

CNAS_INTERNAL_MSG_CTX_STRU *CNAS_GetIntMsgCtxAddr(VOS_VOID)
{
    return &(g_stCnasIntCtxInfo);
}


VOS_VOID CNAS_InitIntMsgCtx(VOS_VOID)
{
    CNAS_INTERNAL_MSG_CTX_STRU         *pstInternalMsgCtx = VOS_NULL_PTR;

    /* ��ʼ��CNAS�ڲ���Ϣ���� */
    pstInternalMsgCtx                   = CNAS_GetIntMsgCtxAddr();
    CNAS_InitIntMsgBuf(pstInternalMsgCtx);

}


VOS_VOID CNAS_InitIntMsgBuf(
    CNAS_INTERNAL_MSG_CTX_STRU         *pstInternalMsgCtx
)
{
    VOS_UINT32                          i;

    for (i = 0; i < CNAS_INTERNAL_MSG_MAX_QUEUE_SIZE; i++)
    {
        pstInternalMsgCtx->pastIntMsg[i] = VOS_NULL_PTR;
    }

    pstInternalMsgCtx->ucIntMsgNum = 0;
}


VOS_VOID *CNAS_GetIntMsgSendBuf(
    VOS_UINT32                          ulModuleId,
    VOS_UINT32                          ulBufLen
)
{
    VOS_VOID         *pstSendMsgBuf = VOS_NULL_PTR;

    pstSendMsgBuf = PS_MEM_ALLOC(ulModuleId, ulBufLen);

    return pstSendMsgBuf;
}


VOS_UINT32  CNAS_SndInternalMsg(
    VOS_VOID                           *pSndMsg
)
{
    VOS_UINT8                           ucIntMsgNum;
    MSG_HEADER_STRU                    *pstIntMsg = VOS_NULL_PTR;

    ucIntMsgNum = CNAS_GetIntMsgCtxAddr()->ucIntMsgNum;
    pstIntMsg   = (MSG_HEADER_STRU *)pSndMsg;

    /* ����ڲ���Ϣ���У��Ƿ��л���������� */
    if (CNAS_INTERNAL_MSG_MAX_QUEUE_SIZE <= ucIntMsgNum)
    {
        PS_MEM_FREE(pstIntMsg->ulSenderPid, pSndMsg);

        CNAS_ERROR_LOG(pstIntMsg->ulSenderPid, "CNAS_SndInternalMsg: msg queue is full!" );

        return VOS_FALSE;
    }

    /* ���浱ǰ��Ϣ */
    CNAS_GetIntMsgCtxAddr()->pastIntMsg[ucIntMsgNum] = pSndMsg;

    CNAS_GetIntMsgCtxAddr()->ucIntMsgNum = (ucIntMsgNum + 1);

    return VOS_TRUE;
}



VOS_UINT8 *CNAS_GetNextInternalMsg(VOS_VOID)
{
    VOS_UINT8                           ucIntMsgNum;
    VOS_UINT8                          *pstIntMsg = VOS_NULL_PTR;

    ucIntMsgNum = CNAS_GetIntMsgCtxAddr()->ucIntMsgNum;

    if (0 < ucIntMsgNum)
    {
        /* ��ȡ��һ���������Ϣ��Ϣ */
        pstIntMsg = CNAS_GetIntMsgCtxAddr()->pastIntMsg[0];

        /* �ڲ���Ϣ��Ŀ����һ�� */
        ucIntMsgNum--;

        if (0 != ucIntMsgNum)
        {
            /* ǰ���ڲ���Ϣ */
            NAS_MEM_MOVE_S(&(CNAS_GetIntMsgCtxAddr()->pastIntMsg[0]),
                           CNAS_INTERNAL_MSG_MAX_QUEUE_SIZE * sizeof(VOS_UINT8 *),
                           &(CNAS_GetIntMsgCtxAddr()->pastIntMsg[1]),
                           ucIntMsgNum * sizeof(VOS_UINT8 *));
        }

        /* ����Ϣ������β������Ϣ��Ϊ��ָ�� */
        CNAS_GetIntMsgCtxAddr()->pastIntMsg[ucIntMsgNum] = VOS_NULL_PTR;

        CNAS_GetIntMsgCtxAddr()->ucIntMsgNum = ucIntMsgNum;
    }

    return pstIntMsg;
}



pCnasPidMsgProc CNAS_FindPidMsgProc(
    VOS_UINT32                          ulRcvPid
)
{
    pCnasPidMsgProc                     pMsgProc = VOS_NULL_PTR;

    switch (ulRcvPid)
    {
        case UEPS_PID_XSD:
            pMsgProc = CNAS_XSD_MsgProc;
            break;

        case UEPS_PID_XREG:
            pMsgProc = CNAS_XREG_MsgProc;
            break;

        case UEPS_PID_XCC:
            pMsgProc = CNAS_XCC_MsgProc;
            break;

        case UEPS_PID_HLU:
            pMsgProc = CNAS_HLU_DispatchMsg;
            break;

        case UEPS_PID_HSD:
            pMsgProc = CNAS_HSD_MsgProc;
            break;

       case UEPS_PID_HSM:
           pMsgProc = CNAS_HSM_MsgProc;
           break;

        case UEPS_PID_EHSM:
            pMsgProc = CNAS_EHSM_MsgProcEntry;
            break;

        default:
            break;
    }

    return pMsgProc;
}


VOS_UINT32  CNAS_FidMsgProc(
    struct MsgCB                        *pRcvMsg
)
{
    pCnasPidMsgProc                     pMsgProc  = VOS_NULL_PTR;
    MSG_HEADER_STRU                    *pstIntMsg = VOS_NULL_PTR;
    MSG_HEADER_STRU                    *pstMsgHeader = VOS_NULL_PTR;

    /* ��ڲ������ */
    if (VOS_NULL_PTR == pRcvMsg)
    {
        /* ��ӡerror��Ϣ */
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_FidMsgProc: pRcvMsg pointer is NULL!");
        return VOS_FALSE;
    }

    pstMsgHeader = (MSG_HEADER_STRU *)pRcvMsg;
    CNAS_MNTN_LogMsgInfo(pstMsgHeader);

    pMsgProc = CNAS_FindPidMsgProc(pRcvMsg->ulReceiverPid);

    if (VOS_NULL_PTR == pMsgProc)
    {
        /* ��ӡerror��Ϣ */
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_FidMsgProc: pMsgProc pointer is NULL!");

        return VOS_FALSE;
    }

    pMsgProc(pRcvMsg);

    //��������Ϣ���¼ʱ��
    CNAS_MNTN_UpdateMsgExitTime();

    /* ��ȡCNAS�ڲ�������Ϣ */
    pstIntMsg = (MSG_HEADER_STRU *)CNAS_GetNextInternalMsg();

    while (VOS_NULL_PTR != pstIntMsg)
    {
        /* ��Ϣ���� */
        CNAS_MNTN_TraceIntMsg(pstIntMsg->ulSenderPid,
                              pstIntMsg->ulLength + VOS_MSG_HEAD_LENGTH,
                              (VOS_VOID *)pstIntMsg);

        /* ��ȡ��Ϣ������ָ�� */
        pMsgProc = CNAS_FindPidMsgProc(pstIntMsg->ulReceiverPid);

        if (VOS_NULL_PTR != pMsgProc)
        {
            /* ������Ϣ */
            pMsgProc((struct MsgCB *)pstIntMsg);
        }
        else
        {
            /* ��ӡerror��Ϣ */
            CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_FidMsgProc: pMsgProc pointer is NULL!");
        }

        /* �ͷ��ڲ���Ϣ */
        PS_MEM_FREE(pstIntMsg->ulSenderPid, pstIntMsg);

        /* ��ȡ��һ��CNAS�ڲ�������Ϣ */
        pstIntMsg = (MSG_HEADER_STRU *)CNAS_GetNextInternalMsg();
    }

    return VOS_TRUE;
}


VOS_VOID  CNAS_InitTask(VOS_VOID)
{
    /* ��ʼ��������Ϣ */
    CNAS_MNTN_InitCtx();

    /* ��ʼ����ʱ�� */
    CNAS_InitTimerCtx();

    /* ��ʼ��CNAS�ڲ����� */
    CNAS_InitIntMsgCtx();

    /* ��ʼ��CCB������ */
    CNAS_CCB_InitCcbCtx();

    /* ��ʼ��PRL������ */
    CNAS_PRL_InitPrlCtx(VOS_TRUE);
}
#endif


VOS_UINT32 CNAS_FidInit(enum VOS_INIT_PHASE_DEFINE enInitPhase)
{
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    VOS_UINT32 ulReturnCode;

    switch (enInitPhase)
    {
        case VOS_IP_LOAD_CONFIG:
            ulReturnCode = VOS_RegisterPIDInfo(UEPS_PID_XSD,
                                               (Init_Fun_Type)CNAS_XSD_InitPid,
                                               (Msg_Fun_Type)CNAS_FidMsgProc);
            if (VOS_OK != ulReturnCode)
            {
                return VOS_ERR;
            }

            ulReturnCode = VOS_RegisterPIDInfo(UEPS_PID_XREG,
                                               (Init_Fun_Type)CNAS_XREG_InitPid,
                                               (Msg_Fun_Type)CNAS_FidMsgProc);
            if (VOS_OK != ulReturnCode)
            {
                return VOS_ERR;
            }


            ulReturnCode = VOS_RegisterPIDInfo(UEPS_PID_XCC,
                                               (Init_Fun_Type)CNAS_XCC_InitPid,
                                               (Msg_Fun_Type)CNAS_FidMsgProc);

            if (VOS_OK != ulReturnCode)
            {
                return VOS_ERR;
            }

            ulReturnCode = VOS_RegisterPIDInfo(UEPS_PID_HLU,
                                   (Init_Fun_Type)CNAS_HLU_InitPid,
                                   (Msg_Fun_Type)CNAS_FidMsgProc);

            if (VOS_OK != ulReturnCode)
            {
                return VOS_ERR;
            }

            ulReturnCode = VOS_RegisterPIDInfo(UEPS_PID_HSD,
                                               (Init_Fun_Type)CNAS_HSD_InitPid,
                                               (Msg_Fun_Type)CNAS_FidMsgProc);

            if (VOS_OK != ulReturnCode)
            {
                return VOS_ERR;
            }

            ulReturnCode = VOS_RegisterPIDInfo(UEPS_PID_HSM,
                                               (Init_Fun_Type)CNAS_HSM_InitPid,
                                               (Msg_Fun_Type)CNAS_FidMsgProc);

            if (VOS_OK != ulReturnCode)
            {
                return VOS_ERR;
            }


            ulReturnCode = VOS_RegisterPIDInfo(UEPS_PID_EHSM,
                                               (Init_Fun_Type)CNAS_EHSM_InitPid,
                                               (Msg_Fun_Type)CNAS_FidMsgProc);

            if (VOS_OK != ulReturnCode)
            {
                return VOS_ERR;
            }


            ulReturnCode = VOS_RegisterTaskPrio(UEPS_FID_CNAS, CNAS_TASK_PRIO);

            if (VOS_OK != ulReturnCode)
            {
                return VOS_ERR;
            }

            break;

        case VOS_IP_FARMALLOC:
            /* CNASģ�������ĳ�ʼ�� */
            CNAS_InitTask();

            break;

        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        case VOS_IP_BUTT:
            break;

    }
#endif

    return VOS_OK;
}
/*lint -restore*/

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



