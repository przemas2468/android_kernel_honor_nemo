

#ifndef _MTCNASINTERFACE_H_
#define _MTCNASINTERFACE_H_

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
#include "PsTypeDef.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

enum MTC_NAS_MSG_ID_ENUM
{
    /* ��Ϣ���� */                         /* ��ϢID */                         /* ��ע */
    /* NAS --> MTC */
    ID_NAS_MTC_TC_STATUS_INFO_IND          = 0x0001,                            /* _H2ASN_MsgChoice NAS_MTC_TC_STATUS_INFO_IND_STRU */

    /* MTC --> NAS */

    ID_MTC_NAS_MSG_ID_BUTT
};
typedef VOS_UINT32  MTC_NAS_MSG_ID_ENUM_UINT32;


enum MTC_NAS_TC_STATUS_ENUM
{
    MTC_NAS_TC_STOP                      = 0,                                   /* ����ֹͣ */
    MTC_NAS_TC_START,                                                           /* �������� */
    MTC_NAS_TC_BUTT
};
typedef VOS_UINT16 MTC_NAS_TC_STATUS_ENUM_UINT16;



enum MTC_NAS_RAT_MODE_ENUM
{
    MTC_NAS_RAT_MODE_GSM                     = 0,                               /* GSM */
    MTC_NAS_RAT_MODE_WCDMA,                                                     /* WCDMA */
    MTC_NAS_RAT_MODE_TDSCDMA,                                                   /* TDSCDMA */
    MTC_NAS_RAT_MODE_LTE,                                                       /* LTE */
    MTC_NAS_RAT_MODE_CDMA,                                                      /* CDMA */
    MTC_NAS_RAT_MODE_BUTT
};
typedef VOS_UINT16 MTC_NAS_RAT_MODE_ENUM_UINT16;

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
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgName;
    MTC_NAS_TC_STATUS_ENUM_UINT16       enTcStatus;
    MTC_NAS_RAT_MODE_ENUM_UINT16        enRatMode;
}NAS_MTC_TC_STATUS_INFO_IND_STRU;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/

/*****************************************************************************
  H2ASN������Ϣ�ṹ����
*****************************************************************************/
typedef struct
{
    MTC_NAS_MSG_ID_ENUM_UINT32          enMsgID;                                /*_H2ASN_MsgChoice_Export MTC_NAS_MSG_ID_ENUM_UINT32*/
    VOS_UINT8                           aucMsgBlock[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          MTC_NAS_MSG_ID_ENUM_UINT32
    ****************************************************************************/
} MTC_NAS_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    MTC_NAS_MSG_DATA                    stMsgData;
} MtcNasInterface_MSG;

/*****************************************************************************
  10 ��������
*****************************************************************************/

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of MtcNasInterface.h */

