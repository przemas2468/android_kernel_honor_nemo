

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "vos.h"
#include  "hsd_hlu_pif.h"
#include  "CnasMain.h"
#include  "CnasHsdSndHlu.h"
#include  "cas_hrpd_airlinkmgmt_nas_pif.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HSD_SND_HLU_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
/*lint -save -e958*/

VOS_VOID CNAS_HSD_SndHluStartReq(VOS_VOID)
{
    HSD_HLU_START_REQ_STRU             *pstInternalMsg;
    VOS_UINT32                          ulLen;

    /* ���ڲ���Ϣ�����л�ȡһ����û��ʹ�õĿռ� */
    ulLen           = sizeof(HSD_HLU_START_REQ_STRU);

    pstInternalMsg  = (HSD_HLU_START_REQ_STRU*)CNAS_GetIntMsgSendBuf(UEPS_PID_HSD, ulLen);
    if (VOS_NULL_PTR == pstInternalMsg)
    {
        return;
    }

    /* �����Ϣ */
    pstInternalMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstInternalMsg->ulSenderPid                 = UEPS_PID_HSD;
    pstInternalMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstInternalMsg->ulReceiverPid               = UEPS_PID_HLU;

    pstInternalMsg->enMsgId                     = ID_HSD_HLU_START_REQ;
    pstInternalMsg->usOpId                      = 0;
    pstInternalMsg->ulLength                    = sizeof(HSD_HLU_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* �����ڲ���Ϣ */
    CNAS_SndInternalMsg(pstInternalMsg);

    return;
}


VOS_VOID CNAS_HSD_SndHluPowerOffReq(VOS_VOID)
{
    HSD_HLU_POWER_OFF_REQ_STRU         *pstInternalMsg;
    VOS_UINT32                          ulLen;

    /* ���ڲ���Ϣ�����л�ȡһ����û��ʹ�õĿռ� */
    ulLen           = sizeof(HSD_HLU_POWER_OFF_REQ_STRU);

    pstInternalMsg  = (HSD_HLU_POWER_OFF_REQ_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HSD, ulLen);
    if (VOS_NULL_PTR == pstInternalMsg)
    {
        return;
    }

    /* �����Ϣ */

    pstInternalMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstInternalMsg->ulSenderPid                 = UEPS_PID_HSD;
    pstInternalMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstInternalMsg->ulReceiverPid               = UEPS_PID_HLU;

    pstInternalMsg->enMsgId                     = ID_HSD_HLU_POWER_OFF_REQ;
    pstInternalMsg->usOpId                      = 0;
    pstInternalMsg->ulLength                    = sizeof(HSD_HLU_POWER_OFF_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* �����ڲ���Ϣ */
    CNAS_SndInternalMsg(pstInternalMsg);

    return;
}
/*lint -restore*/
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
