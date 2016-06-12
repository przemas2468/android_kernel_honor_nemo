



#ifndef __SCCOMMCTX_H__
#define __SCCOMMCTX_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "ScComm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define     SC_RAND_DATA_LEN            (4)         /* ��������ֽ��� */

#define     SC_SEC_AT_KEY_LEN           (16)                /* ��ȫAT key�ĳ��� */

#define     SC_SECURITY_ITEM_NUM        (16)                /* ��ȫ�洢����� */


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

enum SC_AUTH_STATUS_ENUM
{
    SC_AUTH_STATUS_UNDO          = 0x00,             /* δ���м�Ȩ */
    SC_AUTH_STATUS_DONE          = 0x01,             /* �Ѿ���Ȩ */
    SC_AUTH_STATUS_BUTT
};
typedef VOS_UINT8 SC_AUTH_STATUS_ENUM_UINT8;


enum SC_APSEC_FILE_STATE_INIT_ENUM
{
    SC_SECURITY_FILE_STATE_INIT         =   0x5A5A5A5A,     /* ��ȫ�洢�ļ���ʼ����� */
    SC_SECURITY_FILE_STATE_IDEL         =   0x5A5A55AA,     /* ��ȫ�洢�ļ���ʼ��δ��� */
    SC_SECURITY_FILE_STATE_BUTT,
};
typedef VOS_UINT32 SC_APSEC_FILE_STATE_INIT_ENUM_UINT32;

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
    VOS_CHAR                           *acSecretFileName[SC_SECRET_FILE_TYPE_BUTT];
    VOS_CHAR                           *acSignFileName[SC_SECRET_FILE_TYPE_BUTT];
}SC_COMM_GLOBAL_STRU;


typedef struct
{
    SC_AUTH_STATUS_ENUM_UINT8               enScFacAuthStatus;
    VOS_UINT8                               aucScFacRandData[SC_RAND_DATA_LEN];
    /* ������߼�Ȩ��־ VOS_FALSE: δ������߼�Ȩ; VOS_TRUE: �ѷ�����߼�Ȩ */
    VOS_UINT8                               ucIdentifyStartFlg;
    VOS_UINT8                               aucReserved[2];         /* �����ֽڣ��������ֽڶ��� */
}SC_FAC_GLOBAL_STRU;


typedef struct
{
    VOS_UINT32                          ulItemLen;
    VOS_CHAR                           *pucContent;
    VOS_UINT8                           aucHash[SC_HASH_LEN];
}SC_APSEC_ITEM_STRU;


typedef struct
{
    VOS_UINT8                                               ucSeqID;
    VOS_UINT8                                               aucReserved[2];
    VOS_UINT8                                               ucSecATkeyFlg;
    VOS_UINT8                                               aucSecATkey[SC_SEC_AT_KEY_LEN];
    SC_APSEC_FILE_STATE_INIT_ENUM_UINT32                    ulInitState;
    SC_APSEC_ITEM_STRU                                      astApSecItem[SC_SECURITY_ITEM_NUM];
    VOS_CHAR                                               *pApSecFileA;
    VOS_CHAR                                               *pApSecFileC;
}SC_APSEC_GLOBAL_STRU;


typedef struct
{
    SC_COMM_GLOBAL_STRU                 stScCommGlobalVar;
    SC_FAC_GLOBAL_STRU                  stScFacGlobalVar;
    SC_APSEC_GLOBAL_STRU                stScApSecGlobalVar;
}SC_CONTEXT_STRU;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/


extern SC_COMM_GLOBAL_STRU*  SC_CTX_GetCommGlobalVarAddr( VOS_VOID );


extern SC_CONTEXT_STRU* SC_CTX_GetScCtxAddr(VOS_VOID);

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

#endif
