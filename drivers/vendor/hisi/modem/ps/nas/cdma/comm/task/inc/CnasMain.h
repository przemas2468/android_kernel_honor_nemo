
#ifndef _CNAS_MAIN_H_
#define _CNAS_MAIN_H_

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
#include "PsCommonDef.h"
#include "PsLogdef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#pragma pack(4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/* DMT ʹ�� */
#if (VOS_WIN32 == VOS_OS_VER)
#define CNAS_TASK_PRIO                                (137)
#endif

/* ������Ϣsize��С���� */
#define CNAS_MAX_INTERNAL_MSG_LEN                     (1600)

/* queue��Ϣsize��С���� */
#define CNAS_INTERNAL_MSG_MAX_QUEUE_SIZE              (10)

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/


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
    VOS_UINT8                           ucIntMsgNum;                                /* ������Ϣ�ĸ��� */
    VOS_UINT8                           aucReserve[3];
    VOS_UINT8                          *pastIntMsg[CNAS_INTERNAL_MSG_MAX_QUEUE_SIZE]; /* CNAS���ڲ���Ϣ�������飬�����Ϣָ�� */
}CNAS_INTERNAL_MSG_CTX_STRU;

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
CNAS_INTERNAL_MSG_CTX_STRU *CNAS_GetIntMsgCtxAddr(VOS_VOID);

VOS_VOID CNAS_InitIntMsgCtx(VOS_VOID);

VOS_VOID CNAS_InitIntMsgBuf(
    CNAS_INTERNAL_MSG_CTX_STRU          *pstInternalMsgCtx
);

VOS_VOID *CNAS_GetIntMsgSendBuf(
    VOS_UINT32                          ulModuleId,
    VOS_UINT32                          ulBufLen
);

VOS_UINT32  CNAS_SndInternalMsg(
    VOS_VOID                           *pSndMsg
);

VOS_UINT8 *CNAS_GetNextInternalMsg(VOS_VOID);



VOS_VOID  CNAS_InitTask(VOS_VOID);

VOS_UINT32  CNAS_FidMsgProc(
    struct MsgCB                        *pRcvMsg
);

typedef VOS_VOID  (*pCnasPidMsgProc)(struct MsgCB* pMsg);
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
