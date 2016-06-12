

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "CnasHsdFsmPowerSave.h"
#include  "CnasHsdFsmPowerSaveTbl.h"
#include  "CnasHsdProcCard.h"
#include  "CnasHsdSndMscc.h"
#include  "CnasHsdSndCas.h"
#include  "CnasMntn.h"
#include  "CnasTimerMgmt.h"
#include  "CnasHsdCtx.h"
#include  "CnasHsdComFunc.h"
#include  "CnasHsdSndInternalMsg.h"
#include  "CnasHsdFsmSysAcq.h"
#include  "NasMntn.h"
#include  "CnasHsdSndHsm.h"


#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HSD_FSM_POWER_SAVE_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -save -e958*/


VOS_UINT32  CNAS_HSD_RcvMsccPowerSaveReq_PowerSave_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ���������Ϣ */
    CNAS_HSD_SaveCurEntryMsg(ulEventType, pstMsg);

    /* ֪ͨHSM����power save״̬ */
    CNAS_HSD_SndHsmPowerSaveReq();

    /* Ǩ�Ƶ� CNAS_HSD_POWER_SAVE_STA_WAIT_HSM_POWERSAVE_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_HSM_POWERSAVE_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_SAVE_STA_WAIT_HSM_POWERSAVE_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_HSM_POWERSAVE_CNF, TI_CNAS_HSD_WAIT_HSM_POWERSAVE_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvCasSuspendCnf_PowerSave_WaitCasSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣHSD��Ӧ��״̬������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_CAS_SUSPEND_CNF);

    /* ��������н��֪ͨ��һ����һ�յ�����Ϣ����в�һ״̬��Ǩ�ƣ�����MMA�ظ��ػ���Ϣ */
    CNAS_HSD_SndPowerSaveRslt();

    /* �ػ���ɣ��˳����״̬��*/
    CNAS_HSD_QuitFsmL2();

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvTiWaitCasSuspendCnfExpired_PowerSave_WaitCasSuspendCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_REBOOT_INFO_STRU               stCnasRebootInfo;

    /* ��λ�ײ� */
    PS_MEM_SET(&stCnasRebootInfo, 0x0, sizeof(CNAS_REBOOT_INFO_STRU));
    stCnasRebootInfo.enRebootScene = CNAS_REBOOT_SCENE_TI_HSD_WAIT_CAS_SUSPEND_CNF_EXPIRED;
    stCnasRebootInfo.ulPid         = UEPS_PID_HSD;
    stCnasRebootInfo.ulFsmId       = CNAS_HSD_GetCurrFsmId();
    stCnasRebootInfo.ulFsmState    = CNAS_HSD_GetFsmTopState();

    /* ��λ��ֱ���˳� */
    CNAS_SoftReboot(stCnasRebootInfo);

    /* �ػ���ɣ��˳����״̬��*/
    CNAS_HSD_QuitFsmL2();

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvHsmPowerSaveCnf_PowerSave_WaitHsmPowerSaveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֹͣHSD��Ӧ��״̬������ʱ�� */
    CNAS_HSD_StopTimer(TI_CNAS_HSD_WAIT_HSM_POWERSAVE_CNF);

    /* ֪ͨCAS����Suspend״̬ */
    CNAS_HSD_SndCasSuspendReq();

    /* Ǩ�Ƶ� CNAS_HSD_POWER_SAVE_STA_WAIT_CAS_SUSPEND_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_CAS_SUSPEND_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_SAVE_STA_WAIT_CAS_SUSPEND_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SUSPEND_CNF, TI_CNAS_HSD_WAIT_CAS_SUSPEND_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvTiWaitHsmPowerSaveCnfExpired_PowerSave_WaitHsmPowerSaveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֪ͨCAS����Suspend״̬ */
    CNAS_HSD_SndCasSuspendReq();

    /* Ǩ�Ƶ� CNAS_HSD_POWER_SAVE_STA_WAIT_CAS_SUSPEND_CNF״̬,������������ʱ��TI_CNAS_HSD_WAIT_CAS_SUSPEND_CNF */
    CNAS_HSD_SetCurrFsmState(CNAS_HSD_POWER_SAVE_STA_WAIT_CAS_SUSPEND_CNF);

    CNAS_HSD_StartTimer(TI_CNAS_HSD_WAIT_CAS_SUSPEND_CNF, TI_CNAS_HSD_WAIT_CAS_SUSPEND_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32  CNAS_HSD_RcvRrmStatusInd_PowerSave(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ���� */
    CNAS_HSD_SaveCacheMsg(ulEventType, pstMsg);

    return VOS_TRUE;
}

/*lint -restore*/
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



