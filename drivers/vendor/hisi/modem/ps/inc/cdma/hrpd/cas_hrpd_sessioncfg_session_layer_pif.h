

#ifndef __CAS_HRPD_SESSIONCFG_SESSION_LAYER_PIF_H__
#define __CAS_HRPD_SESSIONCFG_SESSION_LAYER_PIF_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"


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

/******************************************************************************
�ṹ��      : CAS_HRPD_DEFAULT_SMP_ATTRIB_STRU
Э����    :
ASN.1����   :
�ṹ˵��    :
******************************************************************************/
typedef struct
{
    VOS_UINT16                                  usTsmpClose;
    VOS_UINT8                                   ucResv[2];
}CAS_HRPD_DEFAULT_SMP_ATTRIB_STRU;

/******************************************************************************
�ṹ��      : CAS_HRPD_DEFAULT_AMP_ATTRIB_STRU
Э����    :
ASN.1����   :
�ṹ˵��    :
******************************************************************************/
typedef struct
{
    VOS_UINT16                                  usMaxNoMonitorDistance;
    VOS_UINT16                                  usHardwareSeparableFromSession;
    VOS_UINT16                                  usSupportGAUPMaxNoMonitorDistance;
    VOS_UINT16                                  usReducedSubnetMaskOffset;
    VOS_UINT16                                  usSupportSecondaryColorCodes;
    VOS_UINT8                                   ucResv[2];
}CAS_HRPD_DEFAULT_AMP_ATTRIB_STRU;

/******************************************************************************
�ṹ��      : CAS_HRPD_GENERIC_MULTI_CDP_STRU
Э����    :
ASN.1����   :
�ṹ˵��    :
******************************************************************************/
typedef struct
{
    VOS_UINT8                                   ucSimultaneousCommonChannelTransmit;
    VOS_UINT8                                   ucSimultaneousDedicatedChannelTransmit;
    VOS_UINT8                                   ucSimultaneousCommonChannelReceive;
    VOS_UINT8                                   ucSimultaneousDedicatedChannelReceive;
    VOS_UINT8                                   ucHybridMSAT;
    VOS_UINT8                                   ucReceiveDiversity;
    VOS_UINT8                                   ucResv[2];
}CAS_HRPD_GENERIC_MULTI_CDP_STRU;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/
#ifndef INFUSION_VERSION


extern VOS_VOID CAS_HRPD_HSCP_GetDefaultAmpAttr(CAS_HRPD_DEFAULT_AMP_ATTRIB_STRU *pstDeAmp);


extern VOS_VOID CAS_HRPD_HSCP_GetDefaultSmpAttr(CAS_HRPD_DEFAULT_SMP_ATTRIB_STRU *pstDeSmp);

#endif


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

#endif /* end of cas_hrpd_sessioncfg_session_layer_pif.h */



