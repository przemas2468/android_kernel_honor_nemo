

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "CnasHsdFsmPowerOff.h"
#include  "CnasHsdFsmPowerOffTbl.h"
#include  "CnasHsdProcCard.h"
#include  "CnasHsdSndMscc.h"
#include  "CnasHsdSndCas.h"
#include  "CnasMntn.h"
#include  "CnasTimerMgmt.h"
#include  "CnasHsdCtx.h"
#include  "CnasHsdComFunc.h"
#include  "CnasHsdSndInternalMsg.h"
#include  "CnasHsdSndHsm.h"
#include  "CnasHsdSndHlu.h"
#include  "CnasHsdFsmSysAcq.h"
#include  "NasMntn.h"
#include  "CnasHsdSndEhsm.h"
#include  "CnasHsdSndRrm.h"


#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HSD_FSM_POWER_OFF_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -save -e958*/


VOS_UINT32  CNAS_HSD_RcvMsccPowerOffReq_PowerOff_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ���������Ϣ */
    CNAS_HSD_SaveCurEntryMsg(ulEventType, pstMsg);

    /* ֹͣHSD���ж�ʱ�� */
    CNAS_StopAllTimerWithSpecifiedPid(UEPS_PID_HSD);

    /* ��RRM����ȥע����Ϣ */
    CNAS_HSD_SndRrmDeRegisterInd(RRM_PS_TASK_TYPE_HRPD_NETWORK_SEARCH);

    /* ֪ͨEHSM�ػ� */
    CNAS_HSD_SndEhsmPowerOffReq(EHSM_HSD_POWER_OFF_CAUSE_NORMAL);

    /* Ǩ�Ƶ� CNAS_HSD_POWER_OFF_STA_WAIT_EHSM_POWEROFF_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_EHSM_POWEROFF_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_OFF_STA_WAIT_EHSM_POWEROFF_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_EHSM_POWEROFF_CNF, TI_CNAS_HSD_WAIT_EHSM_POWEROFF_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvHsmPowerOffCnf_PowerOff_WaitHsmPowerOffCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣHSD��Ӧ��״̬������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_HSM_POWEROFF_CNF);

    /* ֪ͨHLU�ػ� */
    CNAS_HSD_SndHluPowerOffReq();

    /* Ǩ�Ƶ� CNAS_HSD_POWER_OFF_STA_WAIT_HLU_POWEROFF_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_HLU_POWEROFF_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_OFF_STA_WAIT_HLU_POWEROFF_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_HLU_POWEROFF_CNF, TI_CNAS_HSD_WAIT_HLU_POWEROFF_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvTiWaitHsmPowerOffCnfExpired_PowerOff_WaitHsmPowerOffCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �쳣��ӡ */
    CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvTiWaitHsmPowerOffCnfExpired_PowerOff_WaitHsmPowerOffCnf:wait hsm power off expired");

    /* ֪ͨHLU�ػ� */
    CNAS_HSD_SndHluPowerOffReq();

    /* Ǩ�Ƶ� CNAS_HSD_POWER_OFF_STA_WAIT_HLU_POWEROFF_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_HLU_POWEROFF_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_OFF_STA_WAIT_HLU_POWEROFF_CNF);
    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_HLU_POWEROFF_CNF, TI_CNAS_HSD_WAIT_HLU_POWEROFF_CNF_LEN);

    return VOS_TRUE;

}


VOS_UINT32 CNAS_HSD_RcvHluPowerOffCnf_PowerOff_WaitHluPowerOffCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣHSD��Ӧ��״̬������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_HLU_POWEROFF_CNF);

    /* ֪ͨCAS�ػ� */
    CNAS_HSD_SndCasPowerOffReq();

    /* Ǩ�Ƶ� CNAS_HSD_POWER_OFF_STA_WAIT_CAS_POWEROFF_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_CAS_POWEROFF_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_OFF_STA_WAIT_CAS_POWEROFF_CNF);
    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_POWEROFF_CNF, TI_CNAS_HSD_WAIT_CAS_POWEROFF_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvTiWaitHluPowerOffCnfExpired_PowerOff_WaitHluPowerOffCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �쳣��ӡ */
    CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvTiWaitHluPowerOffCnfExpired_PowerOff_WaitHluPowerOffCnf:wait hlu power off expired");

    /* ֪ͨCAS�ػ� */
    CNAS_HSD_SndCasPowerOffReq();

    /* Ǩ�Ƶ� CNAS_HSD_POWER_OFF_STA_WAIT_CAS_POWEROFF_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_CAS_POWEROFF_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_OFF_STA_WAIT_CAS_POWEROFF_CNF);
    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_POWEROFF_CNF, TI_CNAS_HSD_WAIT_CAS_POWEROFF_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvCasPowerOffCnf_PowerOff_WaitCasPowerOffCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_TRACE_HIGH("Receive Msg!");

    /* ֹͣHSD��Ӧ��״̬������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_POWEROFF_CNF);

    /* �ѻ������״̬��� */
    CNAS_HSD_CleanSysAcqStatus();

    /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�ƣ�����MMA�ظ��ػ���Ϣ */
    CNAS_HSD_SndPowerOffRslt();

    /* �ػ���ɣ��˳����״̬��*/
    CNAS_HSD_QuitFsmL2();

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_RcvTiWaitCasPowerOffCnfExpired_PowerOff_WaitCasPowerOffCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvTiWaitCasPowerOffCnfExpired_PowerOff_WaitCasPowerOffCnf:wait cas power off expired");

    /* �ѻ������״̬��� */
    CNAS_HSD_CleanSysAcqStatus();

    /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�ƣ�����MMA�ظ��ػ���Ϣ */
    CNAS_HSD_SndPowerOffRslt();

    /* �ػ���ɣ��˳����״̬��*/
    CNAS_HSD_QuitFsmL2();

    return VOS_TRUE;

}


VOS_UINT32  CNAS_HSD_RcvEhsmPowerOffCnf_PowerOff_WaitEhsmPowerOffCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣHSD��Ӧ��״̬������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_EHSM_POWEROFF_CNF);

    /* ֪ͨHSM�ػ� */
    CNAS_HSD_SndHsmPowerOffReq();

    /* Ǩ�Ƶ� CNAS_HSD_POWER_OFF_STA_WAIT_HSM_POWEROFF_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_HSM_POWEROFF_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_OFF_STA_WAIT_HSM_POWEROFF_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_HSM_POWEROFF_CNF, TI_CNAS_HSD_WAIT_HSM_POWEROFF_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvTiWaitEhsmPowerOffCnfExpired_PowerOff_WaitEhsmPowerOffCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* �쳣��ӡ */
    CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_RcvTiWaitEhsmPowerOffCnfExpired_PowerOff_WaitEhsmPowerOffCnf:wait hsm power off expired");

    /* ֪ͨHSM�ػ� */
    CNAS_HSD_SndHsmPowerOffReq();

    /* Ǩ�Ƶ� CNAS_HSD_POWER_OFF_STA_WAIT_HSM_POWEROFF_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_HSM_POWEROFF_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_OFF_STA_WAIT_HSM_POWEROFF_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_HSM_POWEROFF_CNF, TI_CNAS_HSD_WAIT_HSM_POWEROFF_CNF_LEN);

    return VOS_TRUE;

}

/*lint -restore*/
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



