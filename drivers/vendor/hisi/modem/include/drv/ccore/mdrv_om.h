
#ifndef _MDRV_OM_H_
#define _MDRV_OM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mdrv_om_common.h"

 typedef struct
 {
     unsigned int     ulInfoLen;
     unsigned int     ulErrNo;
     unsigned int     ulBoardTime;
     unsigned int     ulSliceTime;
 }DRV_OM_HEAD_INFO_S;
 
 typedef struct
 {
     DRV_OM_HEAD_INFO_S      stRecordHead;
     unsigned int            ulRebootReason;
     unsigned int            ulModeId;
     unsigned int            ulArg1;
     unsigned int            ulArg2;
     unsigned char           Imsi[9];
     unsigned char           ImsiStatus;
     unsigned char           reserve[2];
 }DRV_OM_SYSTEM_RESET_S;

 /*****************************************************************************
 * �� �� ��  : mdrv_om_get_system_reset_info
 * ��������  : ��ȡϵͳ��λ��Ϣ
 *
 * �������  : ��
 *
 * �������  : ��
 *
 * �� �� ֵ  : 0 --- �ɹ���-1 --- ʧ��
 *
 *****************************************************************************/
int mdrv_om_get_system_reset_info(DRV_OM_SYSTEM_RESET_S * info);
/**************************************************************************
* �� �� ��  : mdrv_om_set_voicestate
* ��������  : �ṩ��GUNAS�����ڽ����˳��绰ʱ����
* �������  : flag��������˳�����
* �������  :
*             �� �� ֵ  : 0:  �����ɹ���
*                        -1�� ����ʧ�ܡ�
****************************************************************************/
int mdrv_om_set_voicestate(unsigned int flag);

#ifdef __cplusplus
}
#endif
#endif

