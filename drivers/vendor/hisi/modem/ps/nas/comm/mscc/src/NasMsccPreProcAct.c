

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "MsccMmcInterface.h"
#include "MmaMsccInterface.h"
#if (FEATURE_ON == FEATURE_IMS)
#include "ImsaMsccInterface.h"
#endif
#include "NasMsccPreProcAct.h"
#include "NasMsccSndMma.h"
#include "NasMsccSndImsa.h"
#include "NasMsccSndMmc.h"
#include "NasMsccCtx.h"
#include "NasMsccSndXsd.h"
#include "NasMsccSndHsd.h"
#include "hsd_mscc_pif.h"
#include "NasMsccComFunc.h"
#include "NasMsccMlplMsplApi.h"
#include "NasMsccSndInternalMsg.h"
#include "NasMntn.h"
#include "NasMsccFsmMain.h"
#include "NasMsccFsmMainTbl.h"
#include "NasMsccProcNvim.h"
#include "NasMsccFsmBsrTbl.h"
#include "NasMsccFsmSysAcqTbl.h"
#include "NasMsccMntn.h"

#include "NasMsccSysAcqStrategy.h"
#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_NAS_MSCC_PRE_PROC_ACT_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
#ifdef DMT
/* ����DMT���ԣ����ÿ���ʱ�Ƿ��Ͷ��������־��ģ�����ʧ�ܺͳ�ʱ�ĳ��� */
NAS_MSCC_CARD_READ_STUB_TYPE_ENUM_UINT8 genCardReadStubTypeForDmt = NAS_MSCC_CARD_READ_STUB_TYPE_BUTT;
#endif

/*****************************************************************************
  3 ��������
*****************************************************************************/


VOS_UINT32 NAS_MSCC_IsNetworkCapInfoChanged(
    NAS_MSCC_NETWORK_CAP_INFO_STRU      *pstNewNwCapInfo
)
{
    NAS_MSCC_NETWORK_CAP_INFO_STRU      *pstOldNwCapInfo = VOS_NULL_PTR;

    /* �Ȼ�ȡLTE������������Ϣ */
    pstOldNwCapInfo = NAS_MSCC_GetLteNwCapInfo();

    /* �¾������Ƿ��б仯 */
    if ((pstOldNwCapInfo->enLteCsCap   != pstNewNwCapInfo->enLteCsCap)
     || (pstOldNwCapInfo->enNwEmcBsCap != pstNewNwCapInfo->enNwEmcBsCap)
     || (pstOldNwCapInfo->enNwImsVoCap != pstNewNwCapInfo->enNwImsVoCap))
    {
        return VOS_TRUE;
    }

    /* �ٻ�ȡGU������������Ϣ */
    pstOldNwCapInfo = NAS_MSCC_GetGuNwCapInfo();

    /* �¾������Ƿ��б仯 */
    if ((pstOldNwCapInfo->enLteCsCap   != pstNewNwCapInfo->enLteCsCap)
     || (pstOldNwCapInfo->enNwEmcBsCap != pstNewNwCapInfo->enNwEmcBsCap)
     || (pstOldNwCapInfo->enNwImsVoCap != pstNewNwCapInfo->enNwImsVoCap))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_VOID NAS_MSCC_SetNetworkCapabilityInfo(
    NAS_MSCC_SYS_MODE_ENUM_UINT8        enSysMode,
    NAS_MSCC_NETWORK_CAP_INFO_STRU     *pstNwCapInfo
)
{
    if (NAS_MSCC_SYS_MODE_LTE == enSysMode)
    {
        NAS_MSCC_SetLteNwCapInfo(pstNwCapInfo);
    }
    else
    {
        NAS_MSCC_SetGuNwCapInfo(pstNwCapInfo);
    }
}


VOS_UINT32 NAS_MSCC_IsImsVoiceSupportedByNW(VOS_VOID)
{
    NAS_MSCC_NETWORK_CAP_INFO_STRU     *pstCurNwCapInfo = VOS_NULL_PTR;
    NAS_MSCC_SYS_INFO_STRU             *pstSysInfo      = VOS_NULL_PTR;

    pstSysInfo = NAS_MSCC_GetCurrSysInfo();

    if (NAS_MSCC_SYS_MODE_LTE == pstSysInfo->st3gppSysInfo.enSysMode)
    {
        pstCurNwCapInfo = NAS_MSCC_GetLteNwCapInfo();
    }
    else
    {
        pstCurNwCapInfo = NAS_MSCC_GetGuNwCapInfo();
    }

    if (NAS_MSCC_NW_IMS_VOICE_NOT_SUPPORTED == pstCurNwCapInfo->enNwImsVoCap)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_IsImsVoiceSupportedByUE(VOS_VOID)
{
    NAS_MSCC_IMS_CONFIG_PARA_STRU      *pstImsCfgInfo = VOS_NULL_PTR;

    pstImsCfgInfo = NAS_MSCC_GetImsCfgInfo();

    /* ��ǰIMS��RAT NV��֧�ֻ�������ѡ��ΪCS ONLY�����粻֧��IMSʱ ��ΪIMS VOICE������*/
    if ((VOS_FALSE                     == NAS_MSCC_GetImsSupportFlag())
     || (NAS_MSCC_VOICE_DOMAIN_CS_ONLY == pstImsCfgInfo->enVoiceDomain))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_IsCGIInfoChanged(
    NAS_MSCC_3GPP_SYS_INFO_STRU         *pstOldSysInfo,
    MMC_MSCC_SYS_INFO_IND_STRU          *pstMmcSysInfo
)
{
    /* check CGI is changed */
    if ((pstOldSysInfo->stPlmnId.ulMcc  != pstMmcSysInfo->stPlmnId.ulMcc)
     || (pstOldSysInfo->stPlmnId.ulMnc  != pstMmcSysInfo->stPlmnId.ulMnc)
     || (pstOldSysInfo->usLac           != pstMmcSysInfo->usLac)
     || (pstOldSysInfo->ucRac           != pstMmcSysInfo->ucRac)
     || (pstOldSysInfo->ulCellId        != pstMmcSysInfo->stCellId.aulCellId[0])
     || (pstOldSysInfo->ucRoamFlag      != pstMmcSysInfo->ucRoamFlag)
     || (pstOldSysInfo->enSysMode       != pstMmcSysInfo->enCurNetWork)
     || (pstOldSysInfo->enLmmAccessType != pstMmcSysInfo->enLmmAccessType))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 NAS_MSCC_ProcMmaUserReselReqIn1XMode_PreProc(VOS_VOID)
{
    /* ���1X�з���ֱ�ӷ��سɹ���������AP��Լ������ʱ����ȥ����DO&L���� */
    if (VOS_TRUE == NAS_MSCC_Is1xNormalService())
    {
        return VOS_TRUE;
    }

    /* �����ǰ״̬����power save״̬�������ں����������� */
    if (NAS_MSCC_FSM_POWER_OFF == NAS_MSCC_GetCurrFsmId())
    {
        return VOS_FALSE;
    }

    /* ��ǰ1Xû�м����Ҫ������ر�־ */
    if (VOS_FALSE == NAS_MSCC_Get1xActiveFlg())
    {
        NAS_MSCC_Set1xActiveFlg(VOS_TRUE);
    }

    NAS_MSCC_SndXsdSysAcqReq(MSCC_XSD_SYS_ACQ_TYPE_NORMAL);

    /*************************************************************************
     ������״̬�����������߼�:
     1. �����ǰL1״̬����deactive״̬�£�
        a. ��Ҫֹͣsleep ��ʱ��������scan��ʱ��;
        b. �����֧��L���򼤻�L��������
        c. �����֧��DO������Ҫ����DO��
        d. ���DO&L��֧�֣���Ҫ����MSCCϵͳ����״̬��������������;
     2. �����ǰ���ڽ���power save����(power off״̬����)������Ϣ��Ҫ��������棬��power save
        ������ɺ��ٴ����������̣�
     3. �����ǰ��L1״̬����active״̬�£���available��ʱ�������У���Ҫ����DO&L������

     3. ������L1��L2״̬��������Ҫ����
        a. �������ϵͳ�л�״̬���У�������ǰDO��L����������Ҫ����PS��������
        b. �����ϵͳ����״̬���У������������������У�����Ҫ����PS������
        c. �����BSR״̬���У��������ǰDO��L����������Ҫ����PS��������
        d. �ڿ���״̬���Լ�sys cfg״̬���У�����MMAĿǰ��ʵ�֣�����Ϣ�ǲ����·��ģ�
     *************************************************************************/
    return VOS_FALSE;
}
#endif


VOS_UINT32 NAS_MSCC_RcvMmcNetworkCapabilityInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_NETWORK_CAPABILITY_INFO_IND_STRU               *pstRcvMsg       = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_IMS)
    NAS_MSCC_SYS_INFO_STRU                                  *pstSysInfo      = VOS_NULL_PTR;
    NAS_MSCC_NETWORK_CAP_INFO_STRU                           stNewNwCapInfo;
    VOS_UINT32                                               ulNwCapInfoChanged;
#endif

    pstRcvMsg = (MMC_MSCC_NETWORK_CAPABILITY_INFO_IND_STRU *)pstMsg;

#if (FEATURE_ON == FEATURE_IMS)
    stNewNwCapInfo.enNwEmcBsCap = (NAS_MSCC_NW_EMC_BS_CAP_ENUM_UINT8)pstRcvMsg->enNwEmcBsCap;
    stNewNwCapInfo.enNwImsVoCap = (NAS_MSCC_NW_IMS_VOICE_CAP_ENUM_UINT8)pstRcvMsg->enNwImsVoCap;

    switch (pstRcvMsg->enLteCsCap)
    {
        case NAS_MSCC_PIF_LTE_CS_CAPBILITY_NO_ADDITION_INFO:
            stNewNwCapInfo.enLteCsCap = NAS_MSCC_LTE_CS_CAPBILITY_NO_ADDITION_INFO;
            break;

        case NAS_MSCC_PIF_LTE_CS_CAPBILITY_CSFB_NOT_PREFER:
            stNewNwCapInfo.enLteCsCap = NAS_MSCC_LTE_CS_CAPBILITY_CSFB_NOT_PREFER;
            break;

        case NAS_MSCC_PIF_LTE_CS_CAPBILITY_SMS_ONLY:
            stNewNwCapInfo.enLteCsCap = NAS_MSCC_LTE_CS_CAPBILITY_SMS_ONLY;
            break;

        case NAS_MSCC_PIF_LTE_CS_CAPBILITY_NOT_SUPPORTED:
            stNewNwCapInfo.enLteCsCap = NAS_MSCC_LTE_CS_CAPBILITY_NOT_SUPPORTED;
            break;

        default:
            stNewNwCapInfo.enLteCsCap = NAS_MSCC_LTE_CS_CAPBILITY_BUTT;
            break;
    }

    /* �жϵ�ǰ������������GU��L����������Ƿ����˸ı� */
    ulNwCapInfoChanged = NAS_MSCC_IsNetworkCapInfoChanged(&stNewNwCapInfo);

    if (VOS_TRUE == ulNwCapInfoChanged)
    {
        pstSysInfo = NAS_MSCC_GetCurrSysInfo();

        /* update informaton to the corresponding global variable according to RAT */
        NAS_MSCC_SetNetworkCapabilityInfo(pstSysInfo->st3gppSysInfo.enSysMode, &stNewNwCapInfo);
    }


#endif

    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaNetworkCapabilityInfoInd(pstRcvMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcSysInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_SYS_INFO_IND_STRU         *pstMmcSysInfo     = VOS_NULL_PTR;
    NAS_MSCC_3GPP_SYS_INFO_STRU         stOld3gppSysInfo;

    pstMmcSysInfo     = (MMC_MSCC_SYS_INFO_IND_STRU*)pstMsg;

    PS_MEM_SET(&stOld3gppSysInfo, 0x0, sizeof(NAS_MSCC_3GPP_SYS_INFO_STRU));
    PS_MEM_CPY(&stOld3gppSysInfo, NAS_MSCC_Get3gppSysInfoAddr(), sizeof(NAS_MSCC_3GPP_SYS_INFO_STRU));

    /* update 3gpp sys info */
    NAS_MSCC_Update3gppSysInfo(pstMmcSysInfo);

    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMma3gppSysInfoInd(pstMmcSysInfo);

#if (FEATURE_ON == FEATURE_IMS)
    if (VOS_TRUE == NAS_MSCC_GetImsSupportFlag())
    {
        if (VOS_TRUE == NAS_MSCC_IsCGIInfoChanged(&stOld3gppSysInfo, pstMmcSysInfo))
        {
            NAS_MSCC_SndImsaCampInfoChangeInd();
        }
    }
#endif

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcRegResultInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_REG_RESULT_IND_STRU       *pstRegResultInd   = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_IMS)
    VOS_UINT32                          ulUeSupportIms;
#endif

    pstRegResultInd = (MMC_MSCC_REG_RESULT_IND_STRU *)pstMsg;

    /* ת������Ϣ��MMA */
    NAS_MSCC_SndMmaRegResultInd(pstRegResultInd);

#if (FEATURE_ON == FEATURE_IMS)
    if ((NAS_MSCC_PIF_SRVDOMAIN_PS    == pstRegResultInd->enCnDomainId)
     || (NAS_MSCC_PIF_SRVDOMAIN_CS_PS == pstRegResultInd->enCnDomainId))
    {
        ulUeSupportIms = NAS_MSCC_IsImsVoiceSupportedByUE();

        /* PSע��ʧ��ֱ��֪ͨIMS������ */
        /* ��ǰIMS��RAT NV��֧�ֻ�������ѡ��ΪCS ONLY�����粻֧��IMSʱ ��ΪIMS VOICE������*/
        if ((VOS_FALSE == NAS_MSCC_IsImsVoiceSupportedByNW())
         || (VOS_FALSE == pstRegResultInd->ucRegRslt)
         || (VOS_FALSE == ulUeSupportIms))
        {

            /* ֪ͨMMC��ǰIMS voice������ */
            NAS_MSCC_SndMmcImsVoiceCapNotify(MSCC_MMC_IMS_VOICE_CAPABILITY_NOT_AVAIL, MSCC_MMC_PERSISTENT_BEARER_STATE_NOT_EXIST);
            NAS_MSCC_SetPersistentBearerState(NAS_MSCC_PERSISTENT_BEARER_STATE_NOT_EXIST);
            NAS_MSCC_SetImsVoiceCapability(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE);

            /* ֪ͨMMA��ǰIMS voice������ */
            NAS_MSCC_SndMmaImsVoiceCapInd(VOS_FALSE);

            return VOS_TRUE;
        }

    }
#else
    /* ֪ͨMMC��ǰIMS voice�Ƿ���� */
    NAS_MSCC_SndMmcImsVoiceCapNotify(MSCC_MMC_IMS_VOICE_CAPABILITY_NOT_AVAIL, MSCC_MMC_PERSISTENT_BEARER_STATE_NOT_EXIST);
    NAS_MSCC_SetPersistentBearerState(NAS_MSCC_PERSISTENT_BEARER_STATE_NOT_EXIST);
    NAS_MSCC_SetImsVoiceCapability(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE);

#endif

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcServiceStatusInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_SERVICE_STATUS_IND_STRU   *pstSrvStatusInd;
#if (FEATURE_ON == FEATURE_IMS)
    VOS_UINT8                           ucRoamingValid;

    VOS_UINT8                           ucCurr3gppRoamingFlg;
#endif
    NAS_MSCC_SERVICE_STATUS_ENUM_UINT8  enCurrPsSrvStatus;

    pstSrvStatusInd                     = (MMC_MSCC_SERVICE_STATUS_IND_STRU *)pstMsg;

#if (FEATURE_ON == FEATURE_IMS)
    ucRoamingValid                      = VOS_FALSE;
    ucCurr3gppRoamingFlg                = VOS_FALSE;
#endif

    /* ת������Ϣ��MMAģ�� */
    NAS_MSCC_SndMmaServiceStatusInd(pstSrvStatusInd);

    NAS_MSCC_SetSimCsRegStatus(pstSrvStatusInd->ucSimCsRegStatus);

    NAS_MSCC_SetSimPsRegStatus(pstSrvStatusInd->ucSimPsRegStatus);

    /*ע��״̬�ϱ�����: CS��PS����һ����ע������ʱ������3GPP�����α�ʾ */
    if (VOS_TRUE == pstSrvStatusInd->bitOpRegSta)
    {
        NAS_MSCC_UpdateRoamFlag(pstSrvStatusInd->enRegState);
    }

    /* ���´���ֻ���PS����״̬�ϱ��Ŵ��� */
    if ((VOS_TRUE                      == pstSrvStatusInd->bitOpSrvSta)
     && ((NAS_MSCC_PIF_SRVDOMAIN_PS    == pstSrvStatusInd->enCnDomainId)
      || (NAS_MSCC_PIF_SRVDOMAIN_CS_PS == pstSrvStatusInd->enCnDomainId)
      || (NAS_MSCC_PIF_SRVDOMAIN_NOT_REG_PLMN_SEARCHING == pstSrvStatusInd->enCnDomainId)))
    {
        enCurrPsSrvStatus = NAS_MSCC_ConvertMmcServiceStatusToMsccFormat(pstSrvStatusInd->enServiceStatus);

#if (FEATURE_ON == FEATURE_IMS)
        ucCurr3gppRoamingFlg = NAS_MSCC_GetCurr3gppRoamingFlg();

        if (NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE == enCurrPsSrvStatus)
        {
            ucRoamingValid              = VOS_TRUE;

        }

        /* ֻҪ����֧�־͸�IMSA�� */
        if (VOS_TRUE          == NAS_MSCC_GetImsSupportFlag())
        {
            NAS_MSCC_SndImsaSrvInfoNotify(enCurrPsSrvStatus, ucRoamingValid, ucCurr3gppRoamingFlg);

            /* ֹͣ�ȴ�IMSA��IMS voice�Ƿ������Ϣ��ʱ�� */
            NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY);

            /* ��PSע��ɹ�ʱ����������ʱ���ȴ�IMSע����ɺ��ϱ�IMS voice�Ƿ���� */
            NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY, NAS_MSCC_GetWaitImsVoiceAvailTimerLen());
        }
#endif

        /* ���浱ǰ3GPP��PS����״̬ */
        NAS_MSCC_SetCurr3gppPsServiceStatus(enCurrPsSrvStatus);

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        return NAS_MSCC_ProcPsServiceStatusIndInCLMode_PreProc(pstSrvStatusInd);
#endif
    }

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMmaSignReportReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_SIGN_REPORT_REQ_STRU      *pstSignReport = VOS_NULL_PTR;
    pstSignReport = (MMA_MSCC_SIGN_REPORT_REQ_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcSignalReportReq(pstSignReport);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaModeChangeReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_MODE_CHANGE_REQ_STRU      *pstModeChangeReq = VOS_NULL_PTR;
    pstModeChangeReq = (MMA_MSCC_MODE_CHANGE_REQ_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcModeChangeReq(pstModeChangeReq);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaAttachReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_ATTACH_REQ_STRU           *pstAttachReq = VOS_NULL_PTR;

    pstAttachReq = (MMA_MSCC_ATTACH_REQ_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcAttachReq(pstAttachReq);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaDetachReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_DETACH_REQ_STRU           *pstDetachReq = VOS_NULL_PTR;

    pstDetachReq = (MMA_MSCC_DETACH_REQ_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcDetachReq(pstDetachReq);

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_CSG)

VOS_UINT32 NAS_MSCC_RcvMmcCsgListSearchCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaCsgListSearchCnf((MMC_MSCC_CSG_LIST_SEARCH_CNF_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcCsgListRej_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_CSG_LIST_REJ_STRU         *pstCsgListRej = VOS_NULL_PTR;

    pstCsgListRej = (MMC_MSCC_CSG_LIST_REJ_STRU *)pstMsg;

    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaCsgListRej(pstCsgListRej);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcCsgListAbortCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_CSG_LIST_ABORT_CNF_STRU   *pstCsgListAbortCnf = VOS_NULL_PTR;

    pstCsgListAbortCnf = (MMC_MSCC_CSG_LIST_ABORT_CNF_STRU *)pstMsg;

    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaCsgListAbortCnf(pstCsgListAbortCnf);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaCsgListSearchReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CSG_LIST_SEARCH_REQ_STRU  *pstCsgListSearchReq = VOS_NULL_PTR;
    MMC_MSCC_CSG_LIST_REJ_STRU          stCsgListRej;

    PS_MEM_SET(&stCsgListRej, 0, sizeof(stCsgListRej));
    pstCsgListSearchReq = (MMA_MSCC_CSG_LIST_SEARCH_REQ_STRU *)pstMsg;

    /* if current rat setting support HRPD,we directly reply fail to upp layer */
    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD))
    {
        /* reply upp layer with reject result */
        NAS_MSCC_SndMmaCsgListRej(&stCsgListRej);

        return VOS_TRUE;
    }

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcCsgListSearchReq(pstCsgListSearchReq);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaCsgListAbortReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CSG_LIST_ABORT_REQ_STRU   *pstCsgListAbortReq = VOS_NULL_PTR;

    pstCsgListAbortReq = (MMA_MSCC_CSG_LIST_ABORT_REQ_STRU *)pstMsg;

    /* if current rat setting support HRPD,we directly reply fail to upp layer */
    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD))
    {
        NAS_MSCC_SndMmaCsgListAbortCnf(VOS_NULL_PTR);

        return VOS_TRUE;
    }

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcCsgListAbortReq(pstCsgListAbortReq);

    return VOS_TRUE;
}

#endif

VOS_UINT32 NAS_MSCC_RcvMmaPlmnListReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /* if current rat setting support HRPD,we directly reply fail to upp layer */
    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD))
    {
        /* reply upp layer with reject result */
        NAS_MSCC_SndMmaPlmnListRej((MMC_MSCC_PLMN_LIST_REJ_STRU*)pstMsg);

        return VOS_TRUE;
    }

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcPlmnListReq();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaPlmnListAbortReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_PLMN_LIST_ABORT_CNF_STRU   stMmcMsccPlmnListAbortCnf;

    PS_MEM_SET(&stMmcMsccPlmnListAbortCnf, 0x0, sizeof(stMmcMsccPlmnListAbortCnf));

    /* if current rat setting support HRPD,we directly reply fail to upp layer */
    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD))
    {
        NAS_MSCC_SndMmaPlmnListAbortCnf(&stMmcMsccPlmnListAbortCnf);

        return VOS_TRUE;
    }

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcPlmnListAbortReq();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaUserReselReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ���1X֧�֣������Զ����������XSD */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    VOS_UINT32                          ulRslt;

    ulRslt = VOS_TRUE;

    /* CL ģʽ */
    if (VOS_FALSE == NAS_MSCC_IsGULModeConfigured())
    {
        if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X))
        {
            /* ����Xģ�£��û�����������������Զ��������� */
            ulRslt = NAS_MSCC_ProcMmaUserReselReqIn1XMode_PreProc();
        }

        /* ƽ̨����֧��LTE����Ҫ������ģʽ����MMC */
        if (VOS_TRUE == NAS_MSCC_IsPlatformSupportLte())
        {
            NAS_MSCC_SndMmcPlmnUserReselReq((MMA_MSCC_PLMN_USER_RESEL_REQ_STRU *)pstMsg);
        }

        /* �����ȸ�MMA�ظ����������ɹ� */
        NAS_MSCC_SndMmaPlmnReselCnf(NAS_MSCC_PIF_USER_PLMN_SEARCH_RESULT_SUCCESS);

        return ulRslt;
    }
#endif

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcPlmnUserReselReq((MMA_MSCC_PLMN_USER_RESEL_REQ_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaUserSpecPlmnSearch_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_PLMN_SPECIAL_REQ_STRU     *pstPlmnSpecialReq  = VOS_NULL_PTR;

    pstPlmnSpecialReq = (MMA_MSCC_PLMN_SPECIAL_REQ_STRU *)pstMsg;

    /* GUL mode or current active mode is LTE, transfer user resel
       request to MMC module */
    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD))
    {
        /* reply upp layer with failed result */
        NAS_MSCC_SndMmaPlmnSpecialSelRej((MMC_MSCC_PLMN_SPECIAL_SEL_REJ_STRU*)pstMsg);

        return VOS_TRUE;
    }

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcPlmnSpecialReq(pstPlmnSpecialReq);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMmaSpecPlmnSearchAbortReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* GUL mode or current active mode is LTE, transfer user resel
       request to MMC module */
    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD))
    {
        NAS_MSCC_SndMmaSpecPlmnSearchAbortCnf((MMC_MSCC_SPEC_PLMN_SEARCH_ABORT_CNF_STRU*)pstMsg);

        return VOS_TRUE;
    }

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcSpecPlmnSearchAbortReq();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaOmMaintainInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_OM_MAINTAIN_INFO_IND_STRU *pstMaintainInfoInd = VOS_NULL_PTR;

    pstMaintainInfoInd = (MMA_MSCC_OM_MAINTAIN_INFO_IND_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcOmMaintainInfoInd(pstMaintainInfoInd);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaUpdateUplmnNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcUpdateUplmnNotify();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaEOPlmnSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_EOPLMN_SET_REQ_STRU       *pstEOPlmnSetReq = VOS_NULL_PTR;

    pstEOPlmnSetReq = (MMA_MSCC_EOPLMN_SET_REQ_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcEOPlmnSetReq(pstEOPlmnSetReq);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaNetScanReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_NET_SCAN_REQ_STRU         *pstNetScanReq = VOS_NULL_PTR;

    pstNetScanReq = (MMA_MSCC_NET_SCAN_REQ_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcNetScanMsgReq(pstNetScanReq);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaAbortNetScanReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcAbortNetScanMsgReq();

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmaAcqReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_ACQ_REQ_STRU              *pstAcqReq = VOS_NULL_PTR;

    pstAcqReq = (MMA_MSCC_ACQ_REQ_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcAcqReq(pstAcqReq);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaRegReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_REG_REQ_STRU              *pstRegReq = VOS_NULL_PTR;

    pstRegReq = (MMA_MSCC_REG_REQ_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcRegReq(pstRegReq);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcPhoneRssiInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SndMmaRssiInd((MMC_MSCC_RSSI_IND_STRU*)pstMsg);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcStaMmInfo_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SndMmaMmInfo((MMC_MSCC_MM_INFO_IND_STRU*)pstMsg);

    return VOS_TRUE;

}



VOS_UINT32 NAS_MSCC_RcvMmcPlmnSelectStartInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_PLMN_SELECTION_START_IND_STRU                  *pstPlmnStartInd    = VOS_NULL_PTR;

    pstPlmnStartInd = (MMC_MSCC_PLMN_SELECTION_START_IND_STRU *)pstMsg;

    NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

    if (NAS_MSCC_PIF_PLMN_SELECTION_START == pstPlmnStartInd->enPlmnSeleStartFlag)
    {
        /* GULģʽ����ԭ���߼�֪ͨMMA������ʼ,ģʽΪ3GPP */
        if (VOS_TRUE == NAS_MSCC_IsGULModeConfigured())
        {
            NAS_MSCC_SndMmaSystemAcquireStartInd(MSCC_MMA_ACQ_SYS_TYPE_3GPP);

            return VOS_TRUE;
        }

        /* CLģʽ�²��ڲ���������,֪ͨMMA������ʼ,ģʽΪDO_LTE��������CL�������̿�ʼʱ֪ͨ */
        if (NAS_MSCC_FSM_SYS_ACQ != NAS_MSCC_GetCurrFsmId())
        {
            NAS_MSCC_SndMmaSystemAcquireStartInd(MSCC_MMA_ACQ_SYS_TYPE_DO_LTE);
        }
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcEOPlmnSetCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_EOPLMN_SET_CNF_STRU                  *pstMmcMsg       = VOS_NULL_PTR;

    pstMmcMsg   = (MMC_MSCC_EOPLMN_SET_CNF_STRU *)pstMsg;

    NAS_MSCC_SndMmaEOPlmnSetCnf(pstMmcMsg);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcWCipherInfo_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_UMTS_CIPHER_INFO_IND_STRU  *pstCipherInfoInd;

    pstCipherInfoInd = (MMC_MSCC_UMTS_CIPHER_INFO_IND_STRU*)pstMsg;

    NAS_MSCC_SndMmaWCipherInfo(pstCipherInfoInd);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMmcGCipherInfo_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_GPRS_CIPHER_INFO_IND_STRU  *pstCipherInfoInd;
    pstCipherInfoInd = (MMC_MSCC_GPRS_CIPHER_INFO_IND_STRU*)pstMsg;

    NAS_MSCC_SndMmaGCipherInfo(pstCipherInfoInd);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcAcInfoChangeInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_AC_INFO_CHANGE_IND_STRU   *pstAcInfoChangeInd;
    pstAcInfoChangeInd = (MMC_MSCC_AC_INFO_CHANGE_IND_STRU *)pstMsg;

    NAS_MSCC_SndMmaAcInfoChangeInd(pstAcInfoChangeInd);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcCampOnInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_CAMP_ON_IND_STRU_STRU      *pstCampOnInd = VOS_NULL_PTR;

    pstCampOnInd = (MMC_MSCC_CAMP_ON_IND_STRU_STRU*)pstMsg;

    NAS_MSCC_SndMmaCampOnInd(pstCampOnInd);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcRfAvailableInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_PIF_RF_AVAILABLE_IND_STRU      *pstRfAvail = VOS_NULL_PTR;

    pstRfAvail = (NAS_MSCC_PIF_RF_AVAILABLE_IND_STRU*)pstMsg;

    /* the current rat is the master mode, just transfer it to mma */

    NAS_MSCC_SndMmaRfAvailInd(pstRfAvail);

    /* �������غ�ģʽ�£�ֻ͸������Ϣ������������ */
    if (NAS_MSCC_PIF_EMC_CALLBACK_MODE_ENABLE == NAS_MSCC_GetEmcCallExistFlag())
    {
        return VOS_TRUE;
    }

    /* ��ǰMMC���û�б�����Ļ�����Ҫ��״̬�������п��ܴ������� */
    if (VOS_FALSE == NAS_MSCC_Get3gppActiveFlg())
    {
        return VOS_FALSE;
    }


    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcStaDataTranAttri_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_DATATRAN_ATTRI_IND_STRU    *pstDataTranAttri;

    pstDataTranAttri = (MMC_MSCC_DATATRAN_ATTRI_IND_STRU*)pstMsg;

    NAS_MSCC_SndMmaDataTranAttri(pstDataTranAttri);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcStaCoverageAreaInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_COVERAGE_AREA_IND_STRU    *pCoverageAreaInd = VOS_NULL_PTR;

    pCoverageAreaInd = (MMC_MSCC_COVERAGE_AREA_IND_STRU *)pstMsg;

    NAS_MSCC_SndMmaCoverageInd(pCoverageAreaInd);

    if (NAS_MSCC_FSM_BETTER_SYSTEM_RESELECTION == NAS_MSCC_GetCurrFsmId())
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcNetScanCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_NET_SCAN_CNF_STRU         *pstMmcNetScanCnf = VOS_NULL_PTR;

    pstMmcNetScanCnf = (MMC_MSCC_NET_SCAN_CNF_STRU *)pstMsg;

    NAS_MSCC_SndMmaNetScanCnf(pstMmcNetScanCnf);
    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcAbortNetScanCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_ABORT_NET_SCAN_CNF_STRU    *pstMmcAbortNetScanCnf = VOS_NULL_PTR;

    pstMmcAbortNetScanCnf = (MMC_MSCC_ABORT_NET_SCAN_CNF_STRU *)pstMsg;

    NAS_MSCC_SndMmaAbortNetScanCnf(pstMmcAbortNetScanCnf);
    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcEplmnInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_EPLMN_INFO_IND_STRU       *pstEplmnInfoIndMsg  = VOS_NULL_PTR;

    pstEplmnInfoIndMsg   = (MMC_MSCC_EPLMN_INFO_IND_STRU *)pstMsg;

    NAS_MSCC_SndMmaEplmnInfoInd(pstEplmnInfoIndMsg);


    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMmcAcqCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    NAS_MSCC_FSM_ID_ENUM_UINT32          enCurrFsmId;
    NAS_MSCC_FSM_CTX_STRU               *pstCurFsm;
#endif
    MMC_MSCC_ACQ_CNF_STRU               *pstMmcMsccAcqCnf;



#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    enCurrFsmId = NAS_MSCC_GetCurrFsmId();
    /* in CL system acquire fsm, then enter fsm to proc this msg */
    if (NAS_MSCC_FSM_SYS_ACQ == enCurrFsmId)
    {
        /* Get the current FSM address*/
        pstCurFsm   = NAS_MSCC_GetCurFsmAddr();

        if (NAS_MSCC_SYSACQ_STA_WAIT_INIT_LOC_INFO_IND == pstCurFsm->ulState)
        {
             return VOS_FALSE;
        }
    }
#endif

    pstMmcMsccAcqCnf = (MMC_MSCC_ACQ_CNF_STRU *)pstMsg;

    NAS_MSCC_SndMmaAcqCnf(pstMmcMsccAcqCnf);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaPowerSaveReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SetPowerSaveReplyFlg(VOS_TRUE);

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcPowerSaveReq();

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcMsgLmmCellSignInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_LMM_CELL_SIGN_INFO_REPORT_IND_STRU            *pstLmmCellSignInfoInd = VOS_NULL_PTR;

    pstLmmCellSignInfoInd = (MMC_MSCC_LMM_CELL_SIGN_INFO_REPORT_IND_STRU *)pstMsg;

    /* ת������Ϣ��MMAģ�� */
    NAS_MSCC_SndMmaLmmCellSignInfoReportInd(pstLmmCellSignInfoInd);

    return VOS_TRUE;
}










VOS_UINT32 NAS_MSCC_RcvMmcRegCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SndMmaRegCnf((MMC_MSCC_REG_CNF_STRU*)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcPowerSaveCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_SERVICE_STATUS_IND_STRU    stServiceSta;
    MMC_MSCC_POWER_SAVE_CNF_STRU       *pstPowerSaveCnf;

    pstPowerSaveCnf = (MMC_MSCC_POWER_SAVE_CNF_STRU*)pstMsg;

    PS_MEM_SET(&stServiceSta, 0X0, sizeof(MMC_MSCC_SERVICE_STATUS_IND_STRU));

    if (NAS_MSCC_PIF_POWER_SAVE_SUCCESS == pstPowerSaveCnf->enResult)
    {
        NAS_MSCC_Set3gppActiveFlg(VOS_FALSE);
        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        /* notify mma no service */
        NAS_MSCC_SetCurr3gppPsServiceStatus(NAS_MSCC_SERVICE_STATUS_NO_SERVICE);

        stServiceSta.bitOpSrvSta      = VOS_TRUE;
        stServiceSta.enServiceStatus  = NAS_MSCC_PIF_SERVICE_STATUS_NO_SERVICE;
        stServiceSta.enCnDomainId     = NAS_MSCC_PIF_SRVDOMAIN_CS_PS;
        stServiceSta.ucSimCsRegStatus = NAS_MSCC_GetSimCsRegStatus();
        stServiceSta.ucSimPsRegStatus = NAS_MSCC_GetSimPsRegStatus();

        NAS_MSCC_SndMmaServiceStatusInd(&stServiceSta);

        /* need reply to upplayer */
        if (VOS_TRUE == NAS_MSCC_GetPowerSaveReplyFlg())
        {
            NAS_MSCC_SndMmaPowerSaveCnf((MMC_MSCC_POWER_SAVE_CNF_STRU*)pstMsg);

            NAS_MSCC_SetPowerSaveReplyFlg(VOS_FALSE);
        }

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        if (VOS_FALSE == NAS_MSCC_GetSimPsRegStatus())
        {
            NAS_MSCC_SndHsdDisableLteNtf();
        }
#endif
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_RcvMmcUsimAuthFailInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    MMC_MSCC_USIM_AUTH_FAIL_IND_STRU    *pstAuthFailInd  = VOS_NULL_PTR;

    pstAuthFailInd            = (MMC_MSCC_USIM_AUTH_FAIL_IND_STRU *)pstMsg;

    NAS_MSCC_SndMmaUsimAuthFailInd(pstAuthFailInd);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcCsServiceConnStatusInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_CS_SERVICE_CONN_STATUS_IND_STRU                *pstCsServiceConnStatusInd = VOS_NULL_PTR;


    pstCsServiceConnStatusInd = (MMC_MSCC_CS_SERVICE_CONN_STATUS_IND_STRU *)pstMsg;

    NAS_MSCC_SndMmaCsServiceConnStatusInd(pstCsServiceConnStatusInd);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcServRejInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_SERV_REJ_IND_STRU          *pstServRejInd = VOS_NULL_PTR;


    pstServRejInd = (MMC_MSCC_SERV_REJ_IND_STRU *) pstMsg;

    (VOS_VOID)NAS_MSCC_SndMmaServRejRsltInd(pstServRejInd);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcAttachCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_ATTACH_CNF_STRU            *pstAttachCnf = VOS_NULL_PTR;

    pstAttachCnf = (MMC_MSCC_ATTACH_CNF_STRU*)pstMsg;

    NAS_MSCC_SndMmaAttachCnf(pstAttachCnf);

    /* ����״̬������ */
    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcDetachInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_DETACH_IND_STRU            *pstDetachInd = VOS_NULL_PTR;
    pstDetachInd = (MMC_MSCC_DETACH_IND_STRU *) pstMsg;

    NAS_MSCC_SndMmaDetachInd(pstDetachInd);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcDetachCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
       /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaDetachCnf((MMC_MSCC_DETACH_CNF_STRU*)pstMsg);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcPlmnListCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaPlmnListCnf((MMC_MSCC_PLMN_LIST_CNF_STRU*)pstMsg);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcPlmnListRej_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
       /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaPlmnListRej((MMC_MSCC_PLMN_LIST_REJ_STRU*)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcPlmnSpecialSelCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
       /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaPlmnSpecialSelCnf((MMC_MSCC_PLMN_SPECIAL_SEL_CNF_STRU*)pstMsg);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMmcPlmnListAbortCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
       /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaPlmnListAbortCnf((MMC_MSCC_PLMN_LIST_ABORT_CNF_STRU*)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcSpecPlmnSearchAbortCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
       /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaSpecPlmnSearchAbortCnf((MMC_MSCC_SPEC_PLMN_SEARCH_ABORT_CNF_STRU*)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcPlmnSpecialSelRej_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaPlmnSpecialSelRej((MMC_MSCC_PLMN_SPECIAL_SEL_REJ_STRU*)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcPlmnReselCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaPlmnReselCnf(((MMC_MSCC_PLMN_RESEL_CNF_STRU*)pstMsg)->enResult);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmaCFPlmnSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcCFPlmnSetReq((MMA_MSCC_CFPLMN_SET_REQ_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaCFPlmnQueryReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcCFPlmnQueryReq((MMA_MSCC_CFPLMN_QUERY_REQ_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcCFPlmnSetCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaCFPlmnSetCnf((MMC_MSCC_CFPLMN_SET_CNF_STRU*)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcCFPlmnQueryCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaCFPlmnQueryCnf((MMC_MSCC_CFPLMN_QUERY_CNF_STRU*)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaSdtConnedInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcSdtConnedInd((MMA_MSCC_OM_MAINTAIN_INFO_IND_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaPrefPlmnSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ������prefplmn��Ϣת����mmc */
    NAS_MSCC_SndMmcPrefPlmnSetReq((MMA_MSCC_PREF_PLMN_SET_REQ_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaPrefPlmnQueryReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ����ѯprefplmn��Ϣת����mmc */
    NAS_MSCC_SndMmcPrefPlmnQueryReq((MMA_MSCC_PREF_PLMN_QUERY_REQ_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaDplmnSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_DPLMN_SET_REQ_STRU        *pstRcvMsg = VOS_NULL_PTR;

    pstRcvMsg = (MMA_MSCC_DPLMN_SET_REQ_STRU *)pstMsg;

    /* ��DPLMN SET REQ��Ϣת����MMC */
    NAS_MSCC_SndMmcDplmnSetReq(pstRcvMsg);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMmcPrefPlmnQueryCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ����ѯprefplmnӦ����Ϣת����mma */
    NAS_MSCC_SndMmaPrefPlmnQueryCnf((MMC_MSCC_PREF_PLMN_QUERY_CNF_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcPrefPlmnSetCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ������prefplmnӦ����Ϣת����mma */
    NAS_MSCC_SndMmaPrefPlmnSetCnf((MMC_MSCC_PREF_PLMN_SET_CNF_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcPrefPlmnInfoInd_PreProc(
    VOS_UINT32                                               ulEventType,
    struct MsgCB                                            *pstMsg
)
{
    MMC_MSCC_PREF_PLMN_INFO_IND_STRU                        *pstPrefPlmnInd      = VOS_NULL_PTR;
    NAS_MSCC_PIF_SIM_EHPLMN_INFO_STRU                       *pstMsccEhPlmnInfo   = VOS_NULL_PTR;
    NAS_MSCC_PIF_SIM_USERPLMN_INFO_STRU                     *pstMsccUserPlmnInfo = VOS_NULL_PTR;
    NAS_MSCC_PIF_SIM_OPERPLMN_INFO_STRU                     *pstMsccOperPlmnInfo = VOS_NULL_PTR;

    pstPrefPlmnInd = (MMC_MSCC_PREF_PLMN_INFO_IND_STRU *)pstMsg;


    if (NAS_MSCC_PREF_PLMN_UPLMN == pstPrefPlmnInd->enPrefPlmnType)
    {
        pstMsccUserPlmnInfo = NAS_MSCC_GetUserPlmnInfoAddr();

        PS_MEM_CPY(pstMsccUserPlmnInfo, &(pstPrefPlmnInd->stUplmnInfo),
                   sizeof(NAS_MSCC_PIF_SIM_USERPLMN_INFO_STRU));
    }

    if (NAS_MSCC_PREF_PLMN_HPLMN == pstPrefPlmnInd->enPrefPlmnType)
    {
        pstMsccEhPlmnInfo   = NAS_MSCC_GetEhPlmnInfoAddr();

        PS_MEM_CPY(pstMsccEhPlmnInfo,   &(pstPrefPlmnInd->stEhplmnInfo),
                   sizeof(NAS_MSCC_PIF_SIM_EHPLMN_INFO_STRU));
    }

    if (NAS_MSCC_PREF_PLMN_OPLMN == pstPrefPlmnInd->enPrefPlmnType)
    {
        pstMsccOperPlmnInfo = NAS_MSCC_GetOperPlmnInfoAddr();

        PS_MEM_CPY(pstMsccOperPlmnInfo, &(pstPrefPlmnInd->stOplmnInfo),
                   sizeof(NAS_MSCC_PIF_SIM_OPERPLMN_INFO_STRU));
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmaGetGeoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_NO_CARD_INIT_SEARCH_LOC_STRU                  *pstNoCardInitSearchLocInfo = VOS_NULL_PTR;

    NAS_TRACE_HIGH("NAS_MSCC_RcvMmaGetGeoReq_PreProc():Mscc Start Get Geo Proc");

    pstNoCardInitSearchLocInfo = NAS_MSCC_GetNoCardInitSearchLocInfoAddr();

    pstNoCardInitSearchLocInfo->ucIsLocInfoUsed  = VOS_TRUE;

    /* ƽ̨����֧��LTE������GUL��GET_GEO_REQ */
    if (VOS_TRUE == NAS_MSCC_IsPlatformSupportLte())
    {
        NAS_MSCC_SndMmcGetGeoReq((MMA_MSCC_GET_GEO_REQ_STRU *)pstMsg);

        NAS_MSCC_SetNoCardInitSearchWaitType(NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_GUL);
    }
    else
    {
        /* ƽ̨����֧��1x������Xģ���� */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        if (VOS_TRUE == NAS_MSCC_IsPlatformSupportCdma1X())
        {
            /* ���ƽ̨������֧��GUL��֧��1X����xsd�����޿�����,��ȡ1x��λ����Ϣ */
            NAS_MSCC_SndXsdSysAcqReq(MSCC_XSD_SYS_ACQ_TYPE_INIT_SEARCH);

            NAS_MSCC_SetNoCardInitSearchWaitType(NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_1X);
        }
        else
#endif
        {
            /* ƽ̨����GUL��1x����֧�֣�ֱ�ӻظ�mma���޿�����λ����Ϣ��Ч */
            NAS_MSCC_ResetNoCardInitSearchInfo();

            NAS_MSCC_SndMmaGetGeoCnf(pstNoCardInitSearchLocInfo);
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcGetGeoCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_NO_CARD_INIT_SEARCH_LOC_STRU                  *pstNoCardInitSearchLocInfo = VOS_NULL_PTR;
    MMC_MSCC_GET_GEO_CNF_STRU                              *pstMsccMmaGetGeoCnf        = VOS_NULL_PTR;

    pstMsccMmaGetGeoCnf        = (MMC_MSCC_GET_GEO_CNF_STRU *)pstMsg;

    /* ���޿����ѵ�lteλ����Ϣ���и��� */
    NAS_MSCC_UpdateGulNoCardInitSearchLocInfo(pstMsccMmaGetGeoCnf->stPlmnId.ulMcc,
                                              pstMsccMmaGetGeoCnf->stPlmnId.ulMnc);

    pstNoCardInitSearchLocInfo = NAS_MSCC_GetNoCardInitSearchLocInfoAddr();
    /* GUL���ѳɹ�������λ����Ϣ��λ����Ϣδ��ʹ�ã�����ΪFALSE */
    pstNoCardInitSearchLocInfo->ucIsLocInfoUsed = VOS_FALSE;

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if (VOS_TRUE == NAS_MSCC_IsPlatformSupportCdma1X())
    {
        /* ���ƽ̨����֧��1x����ȡLTE��GEOλ����Ϣ֮����xsd�����޿����� */
        NAS_MSCC_SndXsdSysAcqReq(MSCC_XSD_SYS_ACQ_TYPE_INIT_SEARCH);

        NAS_MSCC_SetNoCardInitSearchWaitType(NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_1X);
    }
    else
#endif
    {
        /* ����ȡ����λ����Ϣ�ظ�ת����mma */
        NAS_MSCC_SndMmaGetGeoCnf(pstNoCardInitSearchLocInfo);

        NAS_MSCC_SetNoCardInitSearchWaitType(NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_BUTT);
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaStopGetGeoReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_ENUM_UINT32      enNoCardInitSearchWaitType;

    enNoCardInitSearchWaitType = NAS_MSCC_GetNoCardInitSearchWaitType();

    /* ���ݵ�ǰ�ȴ����ѵ�״̬��ѡ���Ǵ��gul�ĳ��ѣ�����1x�ĳ��ѣ�1x�ĳ��Ѵ�Ͻ���power_save */
    if (NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_GUL == enNoCardInitSearchWaitType)
    {
        /* ��ֹͣ��ȡ����λ����Ϣ����ת����mmc */
        NAS_MSCC_SndMmcStopGetGeoReq((MMA_MSCC_STOP_GET_GEO_REQ_STRU *)pstMsg);
    }
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    else if (NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_1X == enNoCardInitSearchWaitType)
    {
        /* ����power_save,���1x��ȡ����λ����Ϣ */
        NAS_MSCC_SndXsdPowerSaveReq(NAS_MSCC_PIF_POWER_SAVE_TYPE_SLEEP);
    }
#endif
    else
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvMmaStopGetGeoReq_PreProc:ERROR!!");
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcStopGetGeoCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ���޿����ѵȴ���������Ϊ��Ч */
    NAS_MSCC_SetNoCardInitSearchWaitType(NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_BUTT);

    /* ��ֹͣ��ȡ����λ����Ϣ�ظ�ת����mma */
    NAS_MSCC_SndMmaStopGetGeoCnf((MMC_MSCC_STOP_GET_GEO_CNF_STRU *)pstMsg);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcDplmnSetCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* MSCC��DPLMN SET CNF��Ϣת����MMA */
    NAS_MSCC_SndMmaDplmnSetCnf((MMC_MSCC_DPLMN_SET_CNF_STRU *)pstMsg);

    return VOS_TRUE;
}



#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)


UINT32 NAS_MSCC_RcvMmaCdmaMoCallStartNtf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CDMA_MO_CALL_START_NTF_STRU         *pstCdmaMoCallStartNtf = VOS_NULL_PTR;

    pstCdmaMoCallStartNtf = (MMA_MSCC_CDMA_MO_CALL_START_NTF_STRU*)pstMsg;

    /* ת������Ϣ��hxd ��xsdģ�� */
    if (NAS_MSCC_PIF_CDMA_CALL_TYPE_DO_NORMAL_DATA_CALL == pstCdmaMoCallStartNtf->enCallType)
    {
        NAS_MSCC_SndHsdCdmaMoCallStartNtf(pstCdmaMoCallStartNtf);
    }
    else
    {
        NAS_MSCC_Set1xSysAcqMoCallFlag(VOS_TRUE);

        NAS_MSCC_SndXsdCdmaMoCallStartNtf(pstCdmaMoCallStartNtf);

        /* ��ǰ�����������������Ĭ��1X RAT��֧�ֵģ�������ж�1X�Ƿ񼤻� */
        if (VOS_FALSE == NAS_MSCC_Get1xActiveFlg())
        {
            NAS_MSCC_Set1xCampOnFlag(VOS_FALSE);

            NAS_MSCC_Set1xActiveFlg(VOS_TRUE);

            NAS_MSCC_SndXsdSysAcqReq(MSCC_XSD_SYS_ACQ_TYPE_NORMAL);
        }
    }

    return VOS_FALSE;
}



UINT32 NAS_MSCC_RcvMmaCdmaMoCallRedialSysAcqNtf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CDMA_MO_CALL_REDIAL_SYS_ACQ_NTF_STRU         *pstCdmaMoCallRedialSysAcqNtf = VOS_NULL_PTR;

    pstCdmaMoCallRedialSysAcqNtf = (MMA_MSCC_CDMA_MO_CALL_REDIAL_SYS_ACQ_NTF_STRU*)pstMsg;

    /* ת������Ϣ��hxd ��xsdģ�� */
    if (NAS_MSCC_PIF_CDMA_CALL_TYPE_DO_NORMAL_DATA_CALL == pstCdmaMoCallRedialSysAcqNtf->enCallType)
    {
        /* ������ԭ���߼��У���������:
           1�������ǰ1X���񲻴��ڣ���ֱ�ӷ��ؿ���ʧ��
           2�������ǰMSCC������L1״̬������ֱ�ӷ���ʧ��
           �޸ĳ�:
           1�������ǰ1X���񲻴��ڣ���ֱ�ӷ��ؿ���ʧ��
           2�������ǰMSCC������L1״̬�����򻺴����Ϣ
        */
        /* HRPD��֧�ֵ�ʱ��ֱ�ӷ��� */
        if ( VOS_FALSE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD) )
        {
            NAS_MSCC_SndMmaDataCallRedialSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL, VOS_FALSE);

            return VOS_TRUE;
        }

        /* ֻ�д���1X�����ʱ�򣬲Ÿ�HSD���ز�����ָʾ(����) */
        if (NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE != NAS_MSCC_GetCurr1xServiceStatus())
        {
            NAS_MSCC_SndMmaDataCallRedialSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL, VOS_FALSE);

            return VOS_TRUE;
        }

        /* ��L1 MAIN״̬��, ˵����������,��Ҫ�Ȼ�����,Ŀǰ����ֻ��SYSCFG��������Ҫ������ */
        if (NAS_MSCC_FSM_L1_MAIN == NAS_MSCC_GetCurrFsmId())
        {
            if (VOS_TRUE == NAS_MSCC_IsNeedSndHsdCdmaMoCallRedialSysAcqNtf_PreProc())
            {
                /* ���available��ʱ��������,ֹͣ�ö�ʱ�� */
                NAS_MSCC_StopTimer(TI_NAS_MSCC_AVAILABLE_TIMER);

                NAS_MSCC_SndHsdCdmaMoCallRedialSysAcqNtf(pstCdmaMoCallRedialSysAcqNtf);

                NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

                NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);
            }
            else
            {
                NAS_MSCC_SndMmaDataCallRedialSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL, VOS_FALSE);
            }
        }
        else
        {
            return VOS_FALSE;
        }

        /* TO DO:����Ҫ���͸�HSD��ʱ����yanzhiji����ظ���APS��Ϣ���߼� */
    }
    else
    {
        NAS_MSCC_SndXsdCdmaMoCallRedialSysAcqNtf(pstCdmaMoCallRedialSysAcqNtf);
    }

    return VOS_TRUE;
}



UINT32 NAS_MSCC_RcvMmaCdmaMoCallSuccessNtf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CDMA_MO_CALL_SUCCESS_NTF_STRU   *pstCdmaMoCallSuccessNtf = VOS_NULL_PTR;

    pstCdmaMoCallSuccessNtf = (MMA_MSCC_CDMA_MO_CALL_SUCCESS_NTF_STRU *)pstMsg;

    /* ת������Ϣ��hxd ��xsdģ�� */
    if (NAS_MSCC_PIF_CDMA_CALL_TYPE_DO_NORMAL_DATA_CALL == pstCdmaMoCallSuccessNtf->enCallType)
    {
        NAS_MSCC_SndHsdCdmaMoCallSuccessNtf(pstCdmaMoCallSuccessNtf);
    }
    else
    {
        NAS_MSCC_SndXsdCdmaMoCallSuccessNtf(pstCdmaMoCallSuccessNtf);
    }

    if ((VOS_TRUE == NAS_MSCC_GetEmcCallBackEnableFlag())
     && (NAS_MSCC_PIF_CDMA_CALL_TYPE_1X_EMC_VOICE_CALL == pstCdmaMoCallSuccessNtf->enCallType))
    {
        NAS_MSCC_SetEmcCallExistFlag(VOS_TRUE);

        return VOS_FALSE;
    }

    return VOS_TRUE;
}





VOS_UINT32 NAS_MSCC_RcvMmaCdmaMoCallEndNtf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CDMA_MO_CALL_END_NTF_STRU   *pstCdmaMoCallEndNtf = VOS_NULL_PTR;

    pstCdmaMoCallEndNtf = (MMA_MSCC_CDMA_MO_CALL_END_NTF_STRU *)pstMsg;

    /* ת������Ϣ��hxd ��xsdģ�� */
    if (NAS_MSCC_PIF_CDMA_CALL_TYPE_DO_NORMAL_DATA_CALL == pstCdmaMoCallEndNtf->enCallType)
    {
        NAS_MSCC_SndHsdCdmaMoCallEndNtf(pstCdmaMoCallEndNtf);
    }
    else
    {
        NAS_MSCC_Set1xSysAcqMoCallFlag(VOS_FALSE);

        NAS_MSCC_SndXsdCdmaMoCallEndNtf(pstCdmaMoCallEndNtf);
    }

    return VOS_TRUE;
}



UINT32 NAS_MSCC_RcvMmaCfreqLockNtf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CFREQ_LOCK_NTF_STRU   *pstCfreqLockNtf = VOS_NULL_PTR;
    VOS_UINT32                      ulIsSupport1x;

    pstCfreqLockNtf = (MMA_MSCC_CFREQ_LOCK_NTF_STRU *)pstMsg;

    /* �ַ�����Ϣ��hsd��xsdģ�� */
    ulIsSupport1x = NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X);

    if (VOS_FALSE == ulIsSupport1x)
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvMmaCfreqLockNtf_PreProc:1x unSupport!");
    }

    NAS_MSCC_SndXsdCfreqLockNtf(pstCfreqLockNtf, ulIsSupport1x);

    NAS_MSCC_SndHsdCfreqLockNtf(pstCfreqLockNtf);

    return VOS_TRUE;
}



UINT32 NAS_MSCC_RcvMmaCdmacsqSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CDMACSQ_SET_REQ_STRU   *pstCdmacsqSetReq = VOS_NULL_PTR;

    pstCdmacsqSetReq = (MMA_MSCC_CDMACSQ_SET_REQ_STRU *)pstMsg;

    /* ת������Ϣ��hxd ��xsdģ�� */
    /*if (NAS_MSCC_PIF_CDMA_CALL_TYPE_DO_NORMAL_DATA_CALL == pstCdmacsqSetReq->enCallType)
    {
        NAS_MSCC_SndHsdCdmaMoCallEndNtf(pstCdmacsqSetReq);
    }
    else
    {
        NAS_MSCC_SndXsdCdmacsqSetReq(pstCdmacsqSetReq);
    }*/
    NAS_MSCC_SndXsdCdmacsqSetReq(pstCdmacsqSetReq);
    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvXsdCdmacsqSetCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SndMmaCdmacsqSetCnf((XSD_MSCC_CDMACSQ_SET_CNF_STRU *)pstMsg);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvXsdCdmacsqSignalQualityInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SndMmaCdmacsqSignalQualityInd((XSD_MSCC_CDMACSQ_IND_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_VOID NAS_MSCC_ProcCLAssociatedInfoNtfWith1xLocInfo_PreProc(
    XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU               *pst1xSysSrvInfoInd
)
{
    NAS_MSCC_PIF_PLMN_PRIORITY_CLASS_ENUM_UINT8             enLteSysPriClass;
    VOS_UINT8                                               ucPreAllowSrchLteFlg;
    VOS_UINT8                                               ucCurAllowSrchLteFlg;

    ucPreAllowSrchLteFlg = NAS_MSCC_GetAllowSrchLteFlg();

    /* ����1X��Ϣ��ȡLTE�Ƿ����������Լ�����������pri class */
    ucCurAllowSrchLteFlg = NAS_MSCC_GetAllowedSrchLtePriClassWith1xLocInfo(pst1xSysSrvInfoInd, &enLteSysPriClass);

    NAS_MSCC_SetAllowSrchLteFlg(ucCurAllowSrchLteFlg);

    NAS_MSCC_SndMmcCLAssociatedInfoNtf(enLteSysPriClass, ucCurAllowSrchLteFlg);

    /* ����LTE�������ѵ����,��ʱ֪ͨMMCֱ�ӹر�LTE,��1Xλ����Ϣ�����仯ʱ,MSPL�е����ȼ�
       �����仯ʱ,LTE��Ϊ������������֪ͨmmc����LTE;

       [ע��]:������ֻ��Բ��ܽ���CL��ģ�����ĳ���;����ܽ���CL��ģ����,LTE����������ʱ,MMC
       ֪ͨ��MSCC����ʧ��,MSCC��ر�LTE��HRPD,����Ҫ�������ӹر�LTE���߼���
    */
    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        return;
    }

    if (NAS_MSCC_FSM_L1_MAIN != NAS_MSCC_GetCurrFsmId())
    {
        return;
    }

    if ((VOS_TRUE  == NAS_MSCC_Get3gppActiveFlg())
     && (VOS_FALSE == ucCurAllowSrchLteFlg))
    {
        NAS_MSCC_SndMmcPowerSaveReq();
    }

    if ((VOS_FALSE == NAS_MSCC_Get3gppActiveFlg())
     && (VOS_TRUE  == ucCurAllowSrchLteFlg)
     && (VOS_FALSE == ucPreAllowSrchLteFlg))
    {
        NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                     NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                     VOS_NULL_PTR,
                                     VOS_NULL_PTR);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);
    }

    return;
}



VOS_VOID NAS_MSCC_ProcNoCardInitSearchWhenRcv1xSidNidInd_PreProc(
    MSCC_XSD_SID_NID_IND_STRU          *pst1xSidNidInd
)
{
    NAS_MSCC_NO_CARD_INIT_SEARCH_LOC_STRU                  *pstNoCardInitSearchLocInfo = VOS_NULL_PTR;

    pstNoCardInitSearchLocInfo = NAS_MSCC_GetNoCardInitSearchLocInfoAddr();

    NAS_MSCC_Update1xNoCardInitSearchLocInfo((VOS_INT32)pst1xSidNidInd->usSid,
                                             (VOS_INT32)pst1xSidNidInd->usNid);

    NAS_MSCC_SetNoCardInitSearchWaitType(NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_BUTT);

    /* Xģ���ѳɹ�������λ����Ϣ��λ����Ϣδʹ�ã�����ΪVOS_FALSE */
    pstNoCardInitSearchLocInfo->ucIsLocInfoUsed = VOS_FALSE;

    /* 1x�������,�ظ�mma�޿�����λ����Ϣ */
    NAS_MSCC_SndMmaGetGeoCnf(pstNoCardInitSearchLocInfo);

    return;
}


VOS_VOID NAS_MSCC_ProcNoCardInitSearchWhenRcvXsdPowerSaveCnf_PreProc(VOS_VOID)
{
    NAS_MSCC_NO_CARD_INIT_SEARCH_LOC_STRU                  *pstNoCardInitSearchLocInfo = VOS_NULL_PTR;

    pstNoCardInitSearchLocInfo = NAS_MSCC_GetNoCardInitSearchLocInfoAddr();

    NAS_MSCC_Update1xNoCardInitSearchLocMcc(NAS_MSCC_INVALID_MCC);

    NAS_MSCC_Update1xNoCardInitSearchLocInfo(NAS_MSCC_INVALID_SID,
                                             NAS_MSCC_INVALID_NID);

    NAS_MSCC_SetNoCardInitSearchWaitType(NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_BUTT);

    /* 1x���ѱ����,�ظ�mma�޿�����λ����Ϣ */
    /* ����Ҫ�ظ�stop get geo cnf��mma�ˣ�mma��stop get geo cnf״̬���£��յ�get geo cnf�жԳ崦�� */
    NAS_MSCC_SndMmaGetGeoCnf(pstNoCardInitSearchLocInfo);

    return;
}


VOS_VOID NAS_MSCC_ProcNoCardInitSearchWhenRcv1xSystemServiceInfoInd_PreProc(
    XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU               *pst1xSysSrvInfoInd
)
{
    NAS_MSCC_NO_CARD_INIT_SEARCH_LOC_STRU                  *pstNoCardInitSearchLocInfo = VOS_NULL_PTR;

    if (NAS_MSCC_PIF_SERVICE_STATUS_NO_SERVICE == pst1xSysSrvInfoInd->stSysSrvInfo.enServiceStatus)
    {
        pstNoCardInitSearchLocInfo = NAS_MSCC_GetNoCardInitSearchLocInfoAddr();

        NAS_MSCC_Update1xNoCardInitSearchLocMcc(NAS_MSCC_INVALID_MCC);

        NAS_MSCC_Update1xNoCardInitSearchLocInfo(NAS_MSCC_INVALID_SID,
                                                 NAS_MSCC_INVALID_NID);

        NAS_MSCC_SetNoCardInitSearchWaitType(NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_BUTT);

        /* 1x����ʧ��,�ظ�mma�޿�����λ����Ϣ */
        NAS_MSCC_SndMmaGetGeoCnf(pstNoCardInitSearchLocInfo);
    }

    return;
}

 
VOS_UINT32 NAS_MSCC_RcvXsd1xSystemServiceInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU               *pst1xSysSrvInfoInd;
    MSCC_HSD_1X_SYS_INFO_STRU                               st1xSysInfo;
    NAS_MSCC_SERVICE_STATUS_ENUM_UINT8                      en1xServiceStatus;
    VOS_UINT8                                               ucIs1xLocationInfoChanged;
    NAS_MSCC_1X_SYS_INFO_STRU                              *pstOld1xSysInfo;
    VOS_UINT8                                               ucIs1xAvail;
    NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_ENUM_UINT32      enNoCardInitSearchWaitType;

    pst1xSysSrvInfoInd  = (XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU *)pstMsg;
    pstOld1xSysInfo     = NAS_MSCC_Get1xSysInfoAddr();

    enNoCardInitSearchWaitType = NAS_MSCC_GetNoCardInitSearchWaitType();

    if ((NAS_MSCC_FSM_L1_MAIN == NAS_MSCC_GetCurrFsmId())
     && (NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_1X == enNoCardInitSearchWaitType))
    {
        /* �����յ�ID_XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND,���1x�޷���˵��1x����ʧ�ܣ���Ҫ����1xλ����ϢΪ��Чֵ */
        NAS_MSCC_ProcNoCardInitSearchWhenRcv1xSystemServiceInfoInd_PreProc(pst1xSysSrvInfoInd);

        return VOS_TRUE;
    }

    if ((NAS_MSCC_PIF_SERVICE_STATUS_NORMAL_SERVICE  == pst1xSysSrvInfoInd->stSysSrvInfo.enServiceStatus)
     || (NAS_MSCC_PIF_SERVICE_STATUS_LIMITED_SERVICE == pst1xSysSrvInfoInd->stSysSrvInfo.enServiceStatus))
    {
        ucIs1xAvail = VOS_TRUE;
    }
    else
    {
        ucIs1xAvail = VOS_FALSE;
    }

    NAS_MSCC_ProcCLAssociatedInfoNtfWith1xLocInfo_PreProc(pst1xSysSrvInfoInd);

    st1xSysInfo.usBandClass = pst1xSysSrvInfoInd->stSysSrvInfo.usBandClass;
    st1xSysInfo.usChannel   = pst1xSysSrvInfoInd->stSysSrvInfo.usFreq;
    st1xSysInfo.usSid       = pst1xSysSrvInfoInd->stSysSrvInfo.usSid;
    st1xSysInfo.usNid       = pst1xSysSrvInfoInd->stSysSrvInfo.usNid;

    /* Send the 1x Sys Change Ind to HSD if EVDO is supported */
    if (VOS_TRUE == NAS_MSCC_IsPlatformSupportCdmaEVDO())
    {
        NAS_MSCC_SndHsd1xSysChgInd(ucIs1xAvail, &st1xSysInfo);
    }

    if ((pstOld1xSysInfo->ulMcc != pst1xSysSrvInfoInd->stSysSrvInfo.stServiceInfo.ulMcc)
     || (pstOld1xSysInfo->ulMnc != pst1xSysSrvInfoInd->stSysSrvInfo.stServiceInfo.usMnc)
     || (pstOld1xSysInfo->usSid != pst1xSysSrvInfoInd->stSysSrvInfo.usSid)
     || (pstOld1xSysInfo->usNid != pst1xSysSrvInfoInd->stSysSrvInfo.usNid))
    {
        ucIs1xLocationInfoChanged = VOS_TRUE;
    }
    else
    {
        ucIs1xLocationInfoChanged = VOS_FALSE;
    }

    en1xServiceStatus = NAS_MSCC_Convert1xServiceStatusToMsccFormat(pst1xSysSrvInfoInd->stSysSrvInfo.enServiceStatus);

    NAS_MSCC_SetCurr1xServiceStatus(en1xServiceStatus);

    NAS_MSCC_Update1xSysInfo(pst1xSysSrvInfoInd);

    NAS_MSCC_SndMma1xSystemServiceInfoInd(pst1xSysSrvInfoInd);

    if ((VOS_TRUE == ucIs1xLocationInfoChanged)
     && (VOS_TRUE == ucIs1xAvail))
    {
        /* Update the 1x location info to NVIM if 1x has normal service and location info has changed */
        NAS_MSCC_UpdateLocationInfoToNvim();
    }

    /* in CL system acquire fsm, then enter fsm to proc this msg */
    if (NAS_MSCC_FSM_SYS_ACQ == NAS_MSCC_GetCurrFsmId())
    {
        if (NAS_MSCC_SYSACQ_STA_WAIT_INIT_LOC_INFO_IND == NAS_MSCC_GetFsmTopState())
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvXsd1xSystemTimeInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SndMma1xSystemTimeInd((XSD_MSCC_1X_SYSTEM_TIME_IND_STRU *)pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvXsdSidNidInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MSCC_XSD_SID_NID_IND_STRU                              *pstRcvMsg = VOS_NULL_PTR;
    NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_ENUM_UINT32      enNoCardInitSearchWaitType;

    enNoCardInitSearchWaitType = NAS_MSCC_GetNoCardInitSearchWaitType();

    pstRcvMsg = (MSCC_XSD_SID_NID_IND_STRU *)pstMsg;

    if ((NAS_MSCC_FSM_L1_MAIN == NAS_MSCC_GetCurrFsmId())
     && (NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_1X == enNoCardInitSearchWaitType))
    {
        /* �����յ�ID_XSD_MSCC_1X_SID_NID_IND,����sid,nid,����MMA_GET_GEO_CNF */
        NAS_MSCC_ProcNoCardInitSearchWhenRcv1xSidNidInd_PreProc(pstRcvMsg);

        return VOS_TRUE;
    }

    NAS_MSCC_SndMmaSidNidInd(pstRcvMsg->usSid,
                             pstRcvMsg->usNid);
    return VOS_TRUE;
}


MSCC_MMA_HRPD_SESSION_RELEASE_TYPE_ENUM_UINT32 NAS_MSCC_CovertSessionType_PreProc(
    HSD_MSCC_SESSION_RELEASE_TYPE_ENUM_UINT32               enSessionRelType
)
{
    MSCC_MMA_HRPD_SESSION_RELEASE_TYPE_ENUM_UINT32          enCurrSessionRelType;

    if (HSD_MSCC_SESSION_RELEASE_TYPE_0 == enSessionRelType)
    {
        enCurrSessionRelType = MSCC_MMA_SESSION_RELEASE_TYPE_0;
    }
    else if (HSD_MSCC_SESSION_RELEASE_TYPE_A == enSessionRelType)
    {
        enCurrSessionRelType = MSCC_MMA_SESSION_RELEASE_TYPE_A;
    }
    else
    {
        enCurrSessionRelType = MSCC_MMA_SESSION_RELEASE_TYPE_BUTT;
    }

    return enCurrSessionRelType;
}


VOS_UINT32 NAS_MSCC_RcvHsdSessionNegRsltInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_SESSION_NEG_RSLT_IND_STRU                     *pstSessionInfo = VOS_NULL_PTR;
    MSCC_MMA_HRPD_SESSION_RELEASE_TYPE_ENUM_UINT32          enSessionRelType;

    pstSessionInfo = (HSD_MSCC_SESSION_NEG_RSLT_IND_STRU *)pstMsg;

    if (VOS_TRUE == NAS_MSCC_IsNeedDiscardHsdServiceStatusInd_PreProc(pstSessionInfo->enRslt))
    {
        NAS_MSCC_MNTN_LogDiscardServStaInfo(NAS_MSCC_DISCARD_HSD_SERVICE_STATUS_IND);

        return VOS_TRUE;
    }

    if (HSD_MSCC_SESSION_NEG_RESULT_SUCC == pstSessionInfo->enRslt)
    {
        NAS_MSCC_StopTimer(TI_NAS_MSCC_SCAN_TIMER);

        enSessionRelType = NAS_MSCC_CovertSessionType_PreProc(pstSessionInfo->enCurrSessionRelType);

        NAS_MSCC_SndMmaHrpdServiceStatusInd(MSCC_MMA_HRPD_SERVICE_STATUS_NORMAL_SERVICE,
                                            pstSessionInfo->ucIsNewSession,
                                            pstSessionInfo->ucIsEhrpdSupport,
                                            enSessionRelType);

        NAS_MSCC_SetCurrHrpdServiceStatus(NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE);

        /* �ж��Ƿ���Ҫ������������BSR Timer */
        if (VOS_TRUE == NAS_MSCC_IsNeedStartBsrTimer())
        {
            if (VOS_TRUE == NAS_MSCC_IsNeedRestartBsrTimer())
            {
                NAS_MSCC_StopTimer(TI_NAS_MSCC_BSR_TIMER);
                NAS_MSCC_StartBsrTimer();
            }
            else
            {
                if (NAS_MSCC_TIMER_STATUS_RUNING != NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_BSR_TIMER))
                {
                    NAS_MSCC_StartBsrTimer();
                }
            }
        }
    }
    else
    {
        NAS_MSCC_SndMmaHrpdServiceStatusInd(MSCC_MMA_HRPD_SERVICE_STATUS_NO_SERVICE,
                                            pstSessionInfo->ucIsNewSession,
                                            pstSessionInfo->ucIsEhrpdSupport,
                                            MSCC_MMA_SESSION_RELEASE_TYPE_BUTT);

        NAS_MSCC_SetCurrHrpdServiceStatus(NAS_MSCC_SERVICE_STATUS_NO_SERVICE);
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvHsdOverheadMsgInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_OVERHEAD_MSG_IND_STRU     *pstHsdMsccOhm            = VOS_NULL_PTR;
    NAS_MSCC_HRPD_SYS_INFO_STRU         stOldHrpdSysInd;

    pstHsdMsccOhm = (HSD_MSCC_OVERHEAD_MSG_IND_STRU *)pstMsg;

    PS_MEM_SET(&stOldHrpdSysInd, 0x0, sizeof(NAS_MSCC_HRPD_SYS_INFO_STRU));
    PS_MEM_CPY(&stOldHrpdSysInd, NAS_MSCC_GetHrpdSysInfoAddr(), sizeof(NAS_MSCC_HRPD_SYS_INFO_STRU));

    NAS_MSCC_UpdateHrpdSysInfo(pstHsdMsccOhm);

    NAS_MSCC_SndMmaHrpdOverheadMsgInd((HSD_MSCC_OVERHEAD_MSG_IND_STRU *)pstMsg);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvHsdDataCallRedialAcqCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_DATA_CALL_REDIAL_SYS_ACQ_CNF_STRU             *pstDataCallSysAcqCnf;

    pstDataCallSysAcqCnf = (HSD_MSCC_DATA_CALL_REDIAL_SYS_ACQ_CNF_STRU *)pstMsg;

    NAS_MSCC_SndMmaDataCallRedialSysAcqInd(pstDataCallSysAcqCnf->enRslt, pstDataCallSysAcqCnf->ucIsEhrpdSupport);



    return VOS_TRUE;

}


VOS_UINT32 NAS_MSCC_RcvHsdHrpdCasStatusInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_CAS_STATUS_IND_STRU       *pstHsdMsccCasStatusInd   = VOS_NULL_PTR;

    pstHsdMsccCasStatusInd = (HSD_MSCC_CAS_STATUS_IND_STRU *)pstMsg;

    if (HSD_MSCC_HRPD_CAS_STATUS_ENUM_CONN == pstHsdMsccCasStatusInd->enCasStatus)
    {
        NAS_MSCC_SetHrpdConnExistFlg(VOS_TRUE);
    }
    else
    {
        NAS_MSCC_SetHrpdConnExistFlg(VOS_FALSE);
    }

    /* send cas status to xsd */
    NAS_MSCC_SndXsdHrpdCasStatusInd((HSD_MSCC_CAS_STATUS_IND_STRU *)pstMsg);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_ProcBsrTimerExpired_PreProc(VOS_VOID)
{
    NAS_MSCC_TIMER_STATUS_ENUM_UINT8    enMsccSleepTimerStatus;
    NAS_MSCC_TIMER_STATUS_ENUM_UINT8    enMsccAvailTimerStatus;
    NAS_MSCC_FSM_ID_ENUM_UINT32         enCurrFsmId;
    VOS_UINT8                           ucHrpdConnExistFlg;
    VOS_UINT8                           ucLteEpsConnExistFlg;
    NAS_MSCC_MNTN_BSR_CTRL_INFO_STRU    stBsrCtrlInfo;

    enMsccSleepTimerStatus  = NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_SLEEP_TIMER);
    enMsccAvailTimerStatus  = NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_AVAILABLE_TIMER);
    ucHrpdConnExistFlg      = NAS_MSCC_GetHrpdConnExistFlg();
    ucLteEpsConnExistFlg    = NAS_MSCC_GetLteEpsConnExistFlg();
    enCurrFsmId             = NAS_MSCC_GetCurrFsmId();

    /* log ��ӡ */
    stBsrCtrlInfo.en3gppPsServiceStatus     = NAS_MSCC_GetCurr3gppPsServiceStatus();
    stBsrCtrlInfo.ucSimPsRegStatus          = NAS_MSCC_GetSimPsRegStatus();
    stBsrCtrlInfo.enHrpdPsServiceStatus     = NAS_MSCC_GetCurrHrpdServiceStatus();
    stBsrCtrlInfo.enCsimCardStatus          = NAS_MSCC_GetCsimCardStatus();
    stBsrCtrlInfo.enUsimCardStatus          = NAS_MSCC_GetUsimCardStatus();
    stBsrCtrlInfo.ucMlplMsplValidFlag       = NAS_MSCC_GetMlplMsplValidFlag();
    stBsrCtrlInfo.ulIsHrpdLteBothSupported  = NAS_MSCC_IsHrpdAndLteBothSupported();
    stBsrCtrlInfo.ucHrpdConnExistFlg        = ucHrpdConnExistFlg;
    stBsrCtrlInfo.ucLteEpsConnExistFlg      = ucLteEpsConnExistFlg;
    stBsrCtrlInfo.enMsccSleepTimerStatus    = enMsccSleepTimerStatus;
    stBsrCtrlInfo.enMsccAvailTimerStatus    = enMsccAvailTimerStatus;
    stBsrCtrlInfo.enCurrFsmId               = enCurrFsmId;

    NAS_MSCC_MNTN_LogBsrCtrlInfo(&stBsrCtrlInfo);


    /* �����������ض�ʱ�������У�����Ҫ����BSR FSM */
    if ((NAS_MSCC_TIMER_STATUS_RUNING == enMsccAvailTimerStatus)
     || (NAS_MSCC_TIMER_STATUS_RUNING == enMsccSleepTimerStatus))
    {
        return VOS_TRUE;
    }

    /* ���MSCC����״̬��ִ�й����У�����BSR Retry��ʱ�����ȴ� */
    /* ���������ҵ������У�����BSR Retry��ʱ�����ȴ� */
    if ((NAS_MSCC_FSM_L1_MAIN != enCurrFsmId)
     || (VOS_TRUE == ucLteEpsConnExistFlg)
     || (VOS_TRUE == ucHrpdConnExistFlg))
    {
        (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH,
                                      TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH_LEN);

        return VOS_TRUE;
    }

    /* �����ǰ���粻��������ȼ����磬��Ҫ����BSR FSM */
    if (VOS_TRUE == NAS_MSCC_IsBgSearchHighPriSysAllowed())
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

VOS_UINT32 NAS_MSCC_RcvTiBsrTimerExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{


    return NAS_MSCC_ProcBsrTimerExpired_PreProc();
}


VOS_UINT32 NAS_MSCC_RcvTiPeriodTryingHighPrioNetworkSearchTimerExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    return NAS_MSCC_ProcBsrTimerExpired_PreProc();
}


VOS_UINT32 NAS_MSCC_RcvXsdSysAcqInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_SYSTEM_ACQUIRE_IND_STRU   *pstXsdSysAcqCnf = VOS_NULL_PTR;
    NAS_MSCC_SERVICE_STATUS_ENUM_UINT8  enHrpdSrvst;

    pstXsdSysAcqCnf = (XSD_MSCC_SYSTEM_ACQUIRE_IND_STRU *)pstMsg;

    /* power off�����л���SCAN��ʱ����ʱ����������յ�XSD�ϱ��������,ֱ���ϱ�MMA����ʧ�� */
    if (NAS_MSCC_FSM_POWER_OFF == NAS_MSCC_GetCurrFsmId())
    {
        NAS_MSCC_SndMmaSystemAcquireEndInd(NAS_MSCC_GetSystemAcquireFinalResult(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL),
                                           MSCC_MMA_ACQ_SYS_TYPE_1X,
                                           MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);
        return VOS_TRUE;
    }

    /* Get the final result to send to MMA */
    NAS_MSCC_SndMmaSystemAcquireEndInd(NAS_MSCC_GetSystemAcquireFinalResult(pstXsdSysAcqCnf->enRslt),
                                       MSCC_MMA_ACQ_SYS_TYPE_1X,
                                       MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);

    if (NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC == pstXsdSysAcqCnf->enRslt)
    {
        NAS_MSCC_StopTimer(TI_NAS_MSCC_SCAN_TIMER);

        return VOS_TRUE;
    }

    /* if bothe 1x,lte and hrpd are no service, start scan timer */
    if (NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL == pstXsdSysAcqCnf->enRslt)
    {


        enHrpdSrvst = NAS_MSCC_GetCurrHrpdServiceStatus();

        if ((NAS_MSCC_SERVICE_STATUS_NO_SERVICE == NAS_MSCC_GetCurr3gppPsServiceStatus())
         && (NAS_MSCC_SERVICE_STATUS_NO_SERVICE == enHrpdSrvst))
        {
            /* we start the timer only in CL mode */
            if (VOS_FALSE == NAS_MSCC_IsGULModeConfigured())
            {
                (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());
            }
        }
    }

    return VOS_TRUE;

}



VOS_UINT32 NAS_MSCC_RcvXsdSysAcqStartInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_Set1xCampOnFlag(VOS_FALSE);

    NAS_MSCC_SndMmaSystemAcquireStartInd(MSCC_MMA_ACQ_SYS_TYPE_1X);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvHsdSysAcqStartInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

    /* �����֧��LTE,��ΪHRPD�����Ĳ�������;֪ͨ����ʼ��ϵͳģʽΪDO */
    if (VOS_FALSE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_LTE))
    {
        NAS_MSCC_SndMmaSystemAcquireStartInd(MSCC_MMA_ACQ_SYS_TYPE_DO);

        return VOS_TRUE;
    }

    /* CLģʽ�²��ڲ���������,֪ͨMMA������ʼ,ģʽΪDO_LTE��������CL�������̿�ʼʱ֪ͨ */
    if (NAS_MSCC_FSM_SYS_ACQ != NAS_MSCC_GetCurrFsmId())
    {
        NAS_MSCC_SndMmaSystemAcquireStartInd(MSCC_MMA_ACQ_SYS_TYPE_DO_LTE);
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_IsNeedDiscardHsdServiceStatusInd_PreProc(
    HSD_MSCC_SESSION_NEG_RSLT_ENUM_UINT32       enRslt
)
{
    NAS_MSCC_FSM_CTX_STRU              *pstCurFsm  = VOS_NULL_PTR;

    pstCurFsm = NAS_MSCC_GetCurFsmAddr();
    if (NAS_MSCC_FSM_BETTER_SYSTEM_RESELECTION == pstCurFsm->enFsmId)
    {
        if ((NAS_MSCC_BSR_STA_WAIT_MMC_POWER_SAVE_CNF            == pstCurFsm->ulState)
         || (NAS_MSCC_BSR_STA_WAIT_HSD_POWER_SAVE_CNF_REACQ_LTE  == pstCurFsm->ulState)
         || (NAS_MSCC_BSR_STA_WAIT_MMC_PLMN_SEARCH_CNF_REACQ_LTE == pstCurFsm->ulState))
        {
            return VOS_TRUE;
        }

        if (HSD_MSCC_SESSION_NEG_RESULT_SUCC != enRslt)
        {
            if (NAS_MSCC_BSR_STA_WAIT_HSD_SYSTEM_ACQUIRE_CNF == pstCurFsm->ulState)
            {
                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;
}


UINT32 NAS_MSCC_RcvMmaHandsetInfoQryReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_HANDSET_INFO_QRY_REQ_STRU   *pstInfoQryReq = VOS_NULL_PTR;

    pstInfoQryReq = (MMA_MSCC_HANDSET_INFO_QRY_REQ_STRU *)pstMsg;

    /* ��XSD���Ͳ�ѯ���� */
    NAS_MSCC_SndXsdHandsetInfoQryReq((VOS_UINT32)pstInfoQryReq->enInfoType);

    return VOS_TRUE;
}


UINT32 NAS_MSCC_RcvXsdHandsetInfoQryCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_HANDSET_INFO_QRY_CNF_STRU *pstCnfMsg  = VOS_NULL_PTR;
    MSCC_MMA_HANDSET_INFO_QRY_CNF_STRU  stInfoCnf;

    PS_MEM_SET(&stInfoCnf, 0x00, sizeof(MSCC_MMA_HANDSET_INFO_QRY_CNF_STRU));

    pstCnfMsg = (XSD_MSCC_HANDSET_INFO_QRY_CNF_STRU *)pstMsg;

    stInfoCnf.enInfoType     = (MMA_MSCC_HANDSET_INFO_TYPE_ENUM_UINT32)pstCnfMsg->ulInfoType;
    stInfoCnf.ucCasState     = pstCnfMsg->ucCasState;
    stInfoCnf.ucCasSubSta    = pstCnfMsg->ucCasSubSta;
    stInfoCnf.enHighVer      = (MSCC_MMA_1X_CAS_P_REV_ENUM_UINT8)pstCnfMsg->ucHighVer;

    /* ��MMA���Ͳ�ѯ�ظ� */
    NAS_MSCC_SndMmaHandsetInfoQryCnf(&stInfoCnf);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmaPsRatTypeNtf_Main_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 1x�з���ʱ�Ѿ�������available��ʱ����������L */

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaQuitCallBackNtf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SndXsdEndEmcCallBackNtf();

    return VOS_TRUE;
}

VOS_UINT32 NAS_MSCC_RcvMmaSetCSidListReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MSCC_XSD_SET_CSIDLIST_REQ_STRU     *pstSndMsg = VOS_NULL_PTR;
    MMA_MSCC_SET_CSIDLIST_REQ_STRU     *pstRcvMsg = VOS_NULL_PTR ;

    pstRcvMsg = (MMA_MSCC_SET_CSIDLIST_REQ_STRU *)pstMsg;
    /* �����ڴ�  */
    pstSndMsg = (MSCC_XSD_SET_CSIDLIST_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_MSCC,
                           sizeof(MSCC_XSD_SET_CSIDLIST_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstSndMsg)
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvMmaSetCSidListReq_PreProc:ERROR: Memory Alloc Error for pMsg");
        return VOS_TRUE;
    }

    PS_MEM_SET((VOS_INT8*)pstSndMsg + VOS_MSG_HEAD_LENGTH, 0x00,
                     sizeof(MSCC_XSD_SET_CSIDLIST_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSndMsg->stMsgHeader.ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstSndMsg->stMsgHeader.ulReceiverPid    = UEPS_PID_XSD;
    pstSndMsg->stMsgHeader.ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstSndMsg->stMsgHeader.ulSenderPid      = UEPS_PID_MSCC;
    pstSndMsg->stMsgHeader.ulLength         = sizeof(MSCC_XSD_SET_CSIDLIST_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstSndMsg->stMsgHeader.ulMsgName        = ID_MSCC_XSD_SET_CSIDLIST_REQ;
    PS_MEM_CPY(&pstSndMsg->stSidWhiteList, &pstRcvMsg->stSidWhiteList, sizeof(NAS_MSCC_PIF_OPER_LOCK_SYS_WHITE_STRU));

    /* ����VOS����ԭ�� */
    PS_SEND_MSG(UEPS_PID_MSCC, pstSndMsg);
    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvXsdSetCSidListCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MSCC_MMA_SET_CSIDLIST_CNF_STRU     *pstSndMsg = VOS_NULL_PTR;
    XSD_MSCC_SET_CSIDLIST_CNF_STRU     *pstRcvMsg = VOS_NULL_PTR;

    pstRcvMsg = (XSD_MSCC_SET_CSIDLIST_CNF_STRU *)pstMsg;
        /* ������Ϣ�� */
    pstSndMsg = (MSCC_MMA_SET_CSIDLIST_CNF_STRU *)PS_ALLOC_MSG(UEPS_PID_MSCC,
                        sizeof(MSCC_MMA_SET_CSIDLIST_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstSndMsg)
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvXsdSetCSidListCnf_PreProc():ERROR:Memory Alloc Error!");
        return VOS_TRUE;
    }

    PS_MEM_SET(((VOS_UINT8*)pstSndMsg) + VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(MSCC_MMA_SET_CSIDLIST_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* �����Ϣͷ */
    pstSndMsg->stMsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstSndMsg->stMsgHeader.ulSenderPid       = UEPS_PID_MSCC;
    pstSndMsg->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstSndMsg->stMsgHeader.ulReceiverPid     = WUEPS_PID_MMA;
    pstSndMsg->stMsgHeader.ulLength          = sizeof(MSCC_MMA_SET_CSIDLIST_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstSndMsg->stMsgHeader.ulMsgName         = ID_MSCC_MMA_SET_CSIDLIST_CNF;
    pstSndMsg->ulRslt                        = pstRcvMsg->ulRslt;

    NAS_MSCC_LogMsgInfo((MSG_HEADER_STRU*)pstSndMsg);

    /* ������Ϣ */
    PS_SEND_MSG(UEPS_PID_MSCC, pstSndMsg);
    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvXsdEmcCallBackNtf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_EMC_CALLBACK_IND_STRU                         *pstEmcCallBackInd = VOS_NULL_PTR;
    NAS_MSCC_PIF_EMC_CALLBACK_MODE_ENUM_UINT32              enCallBackMode;
    VOS_UINT8                                               ucPreEmcCallExistFlag;

    pstEmcCallBackInd     = (XSD_MSCC_EMC_CALLBACK_IND_STRU *)pstMsg;
    enCallBackMode        = pstEmcCallBackInd->enCallBackState;
    ucPreEmcCallExistFlag = NAS_MSCC_GetEmcCallExistFlag();

    /* ֪ͨMMA�������غ�ģʽ */
    NAS_MSCC_SndMmaEmcCallBackInd(enCallBackMode);

    /* ����������غ�ģʽ����ر�HRPD��LTE */
    if ( (NAS_MSCC_PIF_EMC_CALLBACK_MODE_ENABLE != enCallBackMode)
      && (VOS_TRUE == ucPreEmcCallExistFlag) )
    {
        /* ��״̬���������� */
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvXsdSyncServiceAvailableInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_SYNC_SERVICE_AVAILABLE_IND_STRU               *pst1xSyncSrvAvailableInd = VOS_NULL_PTR;
    MSCC_MMA_SYNC_SERVICE_AVAIL_IND_STRU                    stSyncSrvAvailInd;

    pst1xSyncSrvAvailableInd = (XSD_MSCC_SYNC_SERVICE_AVAILABLE_IND_STRU *)pstMsg;

    PS_MEM_SET(&stSyncSrvAvailInd, 0x00, sizeof(stSyncSrvAvailInd));
    stSyncSrvAvailInd.usBandClass     = pst1xSyncSrvAvailableInd->usBandClass;
    stSyncSrvAvailInd.usFreq          = pst1xSyncSrvAvailableInd->usFreq;
    stSyncSrvAvailInd.usSid           = pst1xSyncSrvAvailableInd->usSid;
    stSyncSrvAvailInd.usNid           = pst1xSyncSrvAvailableInd->usNid;
    stSyncSrvAvailInd.enServiceStatus = pst1xSyncSrvAvailableInd->enServiceStatus;
    stSyncSrvAvailInd.ucRoamingInd    = pst1xSyncSrvAvailableInd->ucRoamingInd;

    NAS_MSCC_SndMmaSyncServiceAvailInd(&stSyncSrvAvailInd);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvXsdCurUeStatusInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_UE_STATE_IND_STRU         *pstUeStatusInd;

    pstUeStatusInd = (XSD_MSCC_UE_STATE_IND_STRU*)pstMsg;

    NAS_MSCC_SndMmaUeStatusInd(pstUeStatusInd->ucUeMainState,
                               pstUeStatusInd->ucUeSubState);

    return VOS_TRUE;
}


UINT32 NAS_MSCC_RcvMmaHdrcsqSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_HDR_CSQ_SET_REQ_STRU   *pstHdrcsqSetReq = VOS_NULL_PTR;

    pstHdrcsqSetReq = (MMA_MSCC_HDR_CSQ_SET_REQ_STRU *)pstMsg;

    NAS_MSCC_SndHsdHdrcsqSetReq(pstHdrcsqSetReq);
    return VOS_TRUE;
}


UINT32 NAS_MSCC_RcvHsdHdrcsqSetCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_HDR_CSQ_SET_CNF_STRU      *pstHdrcsqSetCnf= VOS_NULL_PTR;

    pstHdrcsqSetCnf = (HSD_MSCC_HDR_CSQ_SET_CNF_STRU *)pstMsg;

    NAS_MSCC_SndMmaHdrcsqSetCnf(pstHdrcsqSetCnf);

    return VOS_TRUE;
}


UINT32 NAS_MSCC_RcvHsdHdrcsqInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_HDR_CSQ_IND_STRU          *pstHdrcsqInd = VOS_NULL_PTR;

    pstHdrcsqInd = (HSD_MSCC_HDR_CSQ_IND_STRU *)pstMsg;

    NAS_MSCC_SndMmaHdrcsqSignalQualityInd(pstHdrcsqInd);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_IsNeedSndHsdCdmaMoCallRedialSysAcqNtf_PreProc(VOS_VOID)
{
    VOS_UINT8                           ucHrdpActiveFlg;
    VOS_UINT8                           uc3gppActiveFlg;


    /* DO����active״̬�Լ�L��DO������deactive̬ʱ���Է��Ϳ������������HSD */
    ucHrdpActiveFlg = NAS_MSCC_GetHrpdActiveFlg();
    uc3gppActiveFlg = NAS_MSCC_Get3gppActiveFlg();

    if (VOS_TRUE == ucHrdpActiveFlg)
    {
        return VOS_TRUE;
    }

    if ((VOS_FALSE == ucHrdpActiveFlg)
     && (VOS_FALSE == uc3gppActiveFlg))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}







#endif


#if (FEATURE_ON == FEATURE_CL_INTERWORK)

VOS_UINT32 NAS_MSCC_RcvMmcAcqInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_ACQ_IND_STRU               *pstMmcMmaAcqInd = VOS_NULL_PTR;

    pstMmcMmaAcqInd = (MMC_MSCC_ACQ_IND_STRU *)pstMsg;

    NAS_MSCC_SndMmaAcqInd(pstMmcMmaAcqInd);

    return VOS_TRUE;
}
#endif



#if ((FEATURE_ON == FEATURE_MULTI_MODEM) || (FEATURE_ON == FEATURE_UE_MODE_CDMA))


VOS_UINT32 NAS_MSCC_RcvMmcPsServiceConnStatusInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_PS_SERVICE_CONN_STATUS_IND_STRU                *pstPsServiceConnStatusInd = VOS_NULL_PTR;

    pstPsServiceConnStatusInd = (MMC_MSCC_PS_SERVICE_CONN_STATUS_IND_STRU *)pstMsg;

    if (VOS_TRUE == pstPsServiceConnStatusInd->ucPsServiceConnStatusFlag)
    {
        NAS_MSCC_SetLteEpsConnExistFlg(VOS_TRUE);
    }
    else
    {
        NAS_MSCC_SetLteEpsConnExistFlg(VOS_FALSE);
    }

    NAS_MSCC_SndMmaPsServiceConnStatusInd(pstPsServiceConnStatusInd);

    return VOS_TRUE;
}
#endif

#if (FEATURE_ON == FEATURE_MULTI_MODEM)

VOS_UINT32 NAS_MSCC_RcvMmaOtherModemInfoNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_OTHER_MODEM_INFO_NOTIFY_STRU                  *pstOtherModemInfoNtf = VOS_NULL_PTR;

    pstOtherModemInfoNtf = (MMA_MSCC_OTHER_MODEM_INFO_NOTIFY_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcOtherModemInfoNotify(pstOtherModemInfoNtf);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaOtherModemDplmnNplmnInfoNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_OTHER_MODEM_DPLMN_NPLMN_INFO_NOTIFY_STRU       *pstOhterModemDplmnNplmnInfoNtf = VOS_NULL_PTR;

    pstOhterModemDplmnNplmnInfoNtf = (MMA_MSCC_OTHER_MODEM_DPLMN_NPLMN_INFO_NOTIFY_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcOtherModemDplmnNplmnInfoNotify(pstOhterModemDplmnNplmnInfoNtf);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaNcellInfoNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_NCELL_INFO_NOTIFY_STRU    *pstNcellInfoNtf = VOS_NULL_PTR;

    pstNcellInfoNtf = (MMA_MSCC_NCELL_INFO_NOTIFY_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcNcellInfoNotify(pstNcellInfoNtf);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaPsTransferNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_PS_TRANSFER_NOTIFY_STRU   *pstPsTransferNtf = VOS_NULL_PTR;

    pstPsTransferNtf = (MMA_MSCC_PS_TRANSFER_NOTIFY_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcPsTransferNotify(pstPsTransferNtf);

    return VOS_TRUE;
}
#endif


VOS_UINT32 NAS_MSCC_RcvMmaSrvAcqReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_SRV_ACQ_REQ_STRU          *pstSrvAcqReq = VOS_NULL_PTR;
    NAS_MSCC_PIF_SRV_ACQ_RAT_LIST_STRU *pstRatList    = VOS_NULL_PTR;
    VOS_UINT32                          i;
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    VOS_UINT32                          ulEnterFsmFlg;
    VOS_UINT32                          ulState;
#endif

    pstSrvAcqReq = (MMA_MSCC_SRV_ACQ_REQ_STRU *)pstMsg;

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    ulEnterFsmFlg = VOS_FALSE;
#endif
    pstRatList    = &(pstSrvAcqReq->stRatList);

    for (i = 0; i < pstRatList->ucRatNum; i++)
    {
#if (FEATURE_ON == FEATURE_DSDS)
        /* ĿǰCLģʽ��û������ʹ�ó����������ȱ��� */
        if (NAS_MSCC_PIF_SRV_ACQ_RAT_TYPE_GUL == pstRatList->aenRatType[i])
        {
            /* Set the Camp On flag to FALSE */
            NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

            NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

            /* ת������Ϣ��MMCģ�� */
            NAS_MSCC_SndMmcSrvAcqReq(pstSrvAcqReq);
        }
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        if (NAS_MSCC_PIF_SRV_ACQ_RAT_TYPE_1X == pstRatList->aenRatType[i])
        {
            ulState = NAS_MSCC_GetFsmTopState();

            if ( (NAS_MSCC_FSM_L1_MAIN == NAS_MSCC_GetCurrFsmId())
              && (NAS_MSCC_L1_STA_DEACTIVE == ulState) )
            {
                /* MSCC����deactive״̬�Ļ�����Ҫ��״̬������ */
                ulEnterFsmFlg = VOS_TRUE;
            }
            else
            {
                NAS_MSCC_Set1xCampOnFlag(VOS_FALSE);

                NAS_MSCC_Set1xActiveFlg(VOS_TRUE);

                /* ת������Ϣ��1Xģ�� */
                NAS_MSCC_SndXsdSrvAcqReq(pstSrvAcqReq);
            }
        }

        /* Ŀǰû������ʹ�ó����������ȱ��� */
        if (NAS_MSCC_PIF_SRV_ACQ_RAT_TYPE_HRPD == pstRatList->aenRatType[i])
        {
            NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

            NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);

            /* ת������Ϣ��HSDģ�� */
            NAS_MSCC_SndHsdSrvAcqReq(pstSrvAcqReq);
        }
#endif
    }

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if (VOS_TRUE == ulEnterFsmFlg)
    {
        return VOS_FALSE;
    }
#endif

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaBeginSessionNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_BEGIN_SESSION_NOTIFY_STRU *pstBeginSessionNtf = VOS_NULL_PTR;
    NAS_MSCC_PIF_SRV_ACQ_RAT_LIST_STRU *pstRatList = VOS_NULL_PTR;
    VOS_UINT32                          i;

    pstBeginSessionNtf = (MMA_MSCC_BEGIN_SESSION_NOTIFY_STRU *)pstMsg;

    pstRatList = &(pstBeginSessionNtf->stRatList);

    for (i = 0; i < pstRatList->ucRatNum; i++)
    {
#if (FEATURE_ON == FEATURE_DSDS)
        if (NAS_MSCC_PIF_SRV_ACQ_RAT_TYPE_GUL == pstRatList->aenRatType[i])
        {
            /* ת������Ϣ��MMCģ�� */
            NAS_MSCC_SndMmcBeginSessionNotify(pstBeginSessionNtf);
        }
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        if (NAS_MSCC_PIF_SRV_ACQ_RAT_TYPE_1X == pstRatList->aenRatType[i])
        {
            /* ת������Ϣ��1Xģ�� */
            NAS_MSCC_SndXsdBeginSessionNotify(pstBeginSessionNtf);
        }

        if (NAS_MSCC_PIF_SRV_ACQ_RAT_TYPE_HRPD == pstRatList->aenRatType[i])
        {
            /* ��������û��HSD�Ĵ��뿪����Ŀǰû��Ӧ�ó��� */

            /* ת������Ϣ��HSDģ�� */
            NAS_MSCC_SndHsdBeginSessionNotify(pstBeginSessionNtf);
        }
#endif
    }


    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaEndSessionNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_END_SESSION_NOTIFY_STRU   *pstEndSessionNtf = VOS_NULL_PTR;
    NAS_MSCC_PIF_SRV_ACQ_RAT_LIST_STRU *pstRatList = VOS_NULL_PTR;
    VOS_UINT32                          i;

    pstEndSessionNtf = (MMA_MSCC_END_SESSION_NOTIFY_STRU *)pstMsg;

    pstRatList = &(pstEndSessionNtf->stRatList);

    for (i = 0; i < pstRatList->ucRatNum; i++)
    {
#if (FEATURE_ON == FEATURE_DSDS)
        if (NAS_MSCC_PIF_SRV_ACQ_RAT_TYPE_GUL == pstRatList->aenRatType[i])
        {
            /* ת������Ϣ��MMCģ�� */
            NAS_MSCC_SndMmcEndSessionNotify(pstEndSessionNtf);
        }
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        if (NAS_MSCC_PIF_SRV_ACQ_RAT_TYPE_1X == pstRatList->aenRatType[i])
        {
            /* ת������Ϣ��1Xģ�� */
            NAS_MSCC_SndXsdEndSessionNotify(pstEndSessionNtf);
        }

        if (NAS_MSCC_PIF_SRV_ACQ_RAT_TYPE_HRPD == pstRatList->aenRatType[i])
        {
            /* ת������Ϣ��HSDģ�� */
            NAS_MSCC_SndHsdEndSessionNotify(pstEndSessionNtf);
        }
#endif
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcSrvAcqCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_SRV_ACQ_CNF_STRU           *pstSrvAcqCnf = VOS_NULL_PTR;

    pstSrvAcqCnf = (MMC_MSCC_SRV_ACQ_CNF_STRU *)pstMsg;

    /* Ŀǰ������1X��LTEһ���ѵó���������CDMA֧��IMS��CLģʽʱ������ͬʱ��1X��LTE��
       ֻҪ��һ�����뼼���ɹ�����͸�MMA�ظ��ɹ���Ŀǰû��ʹ�ó��� */

    if (NAS_MSCC_PIF_SRV_ACQ_RESULT_SUCCESS == pstSrvAcqCnf->enResult)
    {
        NAS_MSCC_Set3gppCampOnFlag(VOS_TRUE);
    }

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmaSrvAcqCnf(pstSrvAcqCnf);

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 NAS_MSCC_RcvXsdSrvAcqCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_SRV_ACQ_CNF_STRU          *pstSrvAcqCnf = VOS_NULL_PTR;

    pstSrvAcqCnf = (XSD_MSCC_SRV_ACQ_CNF_STRU *)pstMsg;

    /* Ŀǰ������1X��LTEһ���ѵó���������CDMA֧��IMS��CLģʽʱ������ͬʱ��1X��LTE��
       ֻҪ��һ�����뼼���ɹ�����͸�MMA�ظ��ɹ���Ŀǰû��ʹ�ó��� */

    if (NAS_MSCC_PIF_SRV_ACQ_RESULT_SUCCESS == pstSrvAcqCnf->enResult)
    {
        NAS_MSCC_Set1xCampOnFlag(VOS_TRUE);
    }

    /* ת������Ϣ��MMCģ�飬����MMC��MSCC�Ľӿڶ���ת������NAS_MSCC_PIF_SRV_ACQ_CNF_STRU
       �������ֱ��ǿ��ת�����и��� */
    NAS_MSCC_SndMmaSrvAcqCnf((MMC_MSCC_SRV_ACQ_CNF_STRU *)pstSrvAcqCnf);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvHsdSrvAcqCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_SRV_ACQ_CNF_STRU          *pstSrvAcqCnf = VOS_NULL_PTR;

    pstSrvAcqCnf = (XSD_MSCC_SRV_ACQ_CNF_STRU *)pstMsg;

    if (NAS_MSCC_PIF_SRV_ACQ_RESULT_SUCCESS == pstSrvAcqCnf->enResult)
    {
        NAS_MSCC_SetHrpdCampOnFlag(VOS_TRUE);
    }

    /* ת������Ϣ��MMCģ�飬����MMC��MSCC�Ľӿڶ���ת������NAS_MSCC_PIF_SRV_ACQ_CNF_STRU
       �������ֱ��ǿ��ת�����и��� */
    NAS_MSCC_SndMmaSrvAcqCnf((MMC_MSCC_SRV_ACQ_CNF_STRU *)pstSrvAcqCnf);

    return VOS_TRUE;
}
#endif



#if (FEATURE_ON == FEATURE_IMS)


VOS_UINT32 NAS_MSCC_RcvMmaImsSrvInfoNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_IMS_SRV_INFO_NOTIFY_STRU  *pstImsSrvInfoNtf = VOS_NULL_PTR;

    pstImsSrvInfoNtf = (MMA_MSCC_IMS_SRV_INFO_NOTIFY_STRU *)pstMsg;

    /* ת������Ϣ��MMCģ�� */
    NAS_MSCC_SndMmcImsSrvInfoNotify(pstImsSrvInfoNtf);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvImsaImsVoiceCapNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    IMSA_MSCC_IMS_VOICE_CAP_NOTIFY_STRU                    *pstImsVoiceCap = VOS_NULL_PTR;
    VOS_UINT8                                               ucImsVoiceAvail;
    MSCC_IMSA_IMS_VOICE_CAP_ENUM_UINT8                      enOldImsVoiceCap;

    enOldImsVoiceCap = NAS_MSCC_GetImsVoiceCapability();

    pstImsVoiceCap          = (IMSA_MSCC_IMS_VOICE_CAP_NOTIFY_STRU *)pstMsg;



    /* ֹͣ�ȴ�IMSA��IMS voice�Ƿ������Ϣ��ʱ�� */
    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_IMSA_IMS_VOICE_CAP_NOTIFY);

    /* ����IMS voice�Ƿ������Ϣ��SDC�� */
    if (MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE == pstImsVoiceCap->enImsVoiceCap)
    {
        ucImsVoiceAvail = VOS_TRUE;
    }
    else
    {
        ucImsVoiceAvail = VOS_FALSE;
    }

    /* ֪ͨMMC��ǰIMS voice�Ƿ���� */
    NAS_MSCC_SndMmcImsVoiceCapNotify(ucImsVoiceAvail, pstImsVoiceCap->ucIsExistPersistentBearer);
    NAS_MSCC_SetPersistentBearerState(pstImsVoiceCap->ucIsExistPersistentBearer);

    NAS_MSCC_SetImsVoiceCapability(pstImsVoiceCap->enImsVoiceCap);

    if (enOldImsVoiceCap != pstImsVoiceCap->enImsVoiceCap)
    {
        /* ֪ͨMMA��ǰIMS VOICE�Ƿ���� */
        NAS_MSCC_SndMmaImsVoiceCapInd(ucImsVoiceAvail);
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvTiWaitImsaImsVoiceCapNtfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucImsVoiceAvailFlg;
    VOS_UINT8                           ucIsExistPersistentBearer;

    ucImsVoiceAvailFlg                  = VOS_FALSE;
    ucIsExistPersistentBearer           = VOS_FALSE;

    if (MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE == NAS_MSCC_GetImsVoiceCapability())
    {
        ucImsVoiceAvailFlg              = VOS_TRUE;
    }

    if (NAS_MSCC_PERSISTENT_BEARER_STATE_EXIST == NAS_MSCC_GetPersistentBearerState())
    {
        ucIsExistPersistentBearer       = VOS_TRUE;
    }

    NAS_MSCC_SndMmcImsVoiceCapNotify(ucImsVoiceAvailFlg, ucIsExistPersistentBearer);

    NAS_MSCC_SndMmaImsVoiceCapInd(ucImsVoiceAvailFlg);


    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaImsStartReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֪ͨIMSA����IMSЭ��ջ */
    NAS_MSCC_SndImsaStartReq(MSCC_IMSA_START_TYPE_IMS_SWITCH_ON);

    /* Start timer TI_NAS_MSCC_WAIT_IMSA_START_CNF */
    (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_IMSA_START_CNF, TI_NAS_MSCC_WAIT_IMSA_START_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaImsStopReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* ֪ͨIMSA�ر�IMSЭ��ջ */
    NAS_MSCC_SndImsaStopReq(MSCC_IMSA_STOP_TYPE_IMS_SWITCH_OFF);

    /* Start timer TI_NAS_IMSA_WAIT_IMSA_POWER_OFF_CNF */
    (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_IMSA_POWER_OFF_CNF, TI_NAS_MSCC_WAIT_IMSA_POWER_OFF_CNF_LEN);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaVoiceDomainChangeInd_PreProc(
    VOS_UINT32                                              ulEventType,
    struct MsgCB                                           *pstMsg
)
{
    MMA_MSCC_VOICE_DOMAIN_CHANGE_IND_STRU                  *pstVoiceDomainInd = VOS_NULL_PTR;
    NAS_MSCC_IMS_CONFIG_PARA_STRU                          *pstImsCfgInfo = VOS_NULL_PTR;

    pstImsCfgInfo     = NAS_MSCC_GetImsCfgInfo();
    pstVoiceDomainInd = (MMA_MSCC_VOICE_DOMAIN_CHANGE_IND_STRU *)pstMsg;

    /* ����VOICE DOMAIN��ֵ */
    pstImsCfgInfo->enVoiceDomain = pstVoiceDomainInd->enVoiceDomain;

    /* ֪ͨIMSA�仯���VOICE DOMAINֵ */
    NAS_MSCC_SndImsaVoiceDomainChangeInd(pstVoiceDomainInd->enVoiceDomain);

    /* ֪ͨMMC�仯���VOICE DOMAINֵ */
    NAS_MSCC_SndMmcVoiceDomainChangeInd(pstVoiceDomainInd->enVoiceDomain);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvImsaStartCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_IMS_CONFIG_PARA_STRU      *pstImsCfgInfo = VOS_NULL_PTR;
    NAS_MSCC_FSM_ID_ENUM_UINT32         enFsmId;
    NAS_MSCC_SERVICE_STATUS_ENUM_UINT8  enPsSrvStatus;
    VOS_UINT8                           ucRoamingValid;
    VOS_UINT8                           ucCurr3gppRoamingFlg;

    pstImsCfgInfo = NAS_MSCC_GetImsCfgInfo();
    enFsmId       = NAS_MSCC_GetCurrFsmId();

    enPsSrvStatus                       = NAS_MSCC_SERVICE_STATUS_NO_SERVICE;
    ucRoamingValid                      = VOS_FALSE;
    ucCurr3gppRoamingFlg                = VOS_FALSE;

    /* �жϵ�ǰ״̬����״̬��������ڿ�����״̬���棬����Ϊ��IMS SWITCH�����Ŀ������� */
    if (NAS_MSCC_FSM_SWITCH_ON != enFsmId)
    {
        /* ����IMSҵ������ */
        pstImsCfgInfo->stImsRatSupport.ucLteImsSupportFlag = VOS_TRUE;
        pstImsCfgInfo->stImsRatSupport.ucLteEmsSupportFlag = VOS_TRUE;

        /* Stop the timer */
        NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_IMSA_START_CNF);

        /* ֪ͨMMC��ǰIMS�Ŀ���״̬ */
        NAS_MSCC_SndMmcImsSwitchStateInd(NAS_MSCC_PIF_IMS_SWITCH_STATE_ON);

        /* ֪ͨIMSA פ����Ϣ */
        NAS_MSCC_SndImsaCampInfoChangeInd();

        enPsSrvStatus  = NAS_MSCC_GetCurr3gppPsServiceStatus();

        if (NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE == enPsSrvStatus)
        {
            ucRoamingValid              = VOS_TRUE;
            ucCurr3gppRoamingFlg        = NAS_MSCC_GetCurr3gppRoamingFlg();
        }

        /* ֪ͨIMSA Service Change Ind*/
        NAS_MSCC_SndImsaSrvInfoNotify(enPsSrvStatus, ucRoamingValid, ucCurr3gppRoamingFlg);


        /* ��IMSA�ظ�START CNF��Ϣ */
        NAS_MSCC_SndMmaImsStartCnf(NAS_MSCC_PIF_IMS_START_RESULT_SUCC);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_RcvImsaStopCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_IMS_CONFIG_PARA_STRU      *pstImsCfgInfo = VOS_NULL_PTR;
    NAS_MSCC_FSM_ID_ENUM_UINT32         enFsmId;

    pstImsCfgInfo = NAS_MSCC_GetImsCfgInfo();
    enFsmId       = NAS_MSCC_GetCurrFsmId();

    /* �жϵ�ǰ״̬����״̬��������ڹػ���״̬���棬����Ϊ��IMS SWITCH�����Ĺػ����� */
    if (NAS_MSCC_FSM_POWER_OFF != enFsmId)
    {
        /* ����IMSҵ������ */
        pstImsCfgInfo->stImsRatSupport.ucLteImsSupportFlag = VOS_FALSE;
        pstImsCfgInfo->stImsRatSupport.ucLteEmsSupportFlag = VOS_FALSE;

        /* Stop the timer */
        NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_IMSA_POWER_OFF_CNF);

        /* ֪ͨMMC��ǰIMS�Ŀ���״̬ */
        NAS_MSCC_SndMmcImsSwitchStateInd(NAS_MSCC_PIF_IMS_SWITCH_STATE_OFF);

        /* ֪ͨMMC��ǰIMS voice������ */
        NAS_MSCC_SndMmcImsVoiceCapNotify(MSCC_MMC_IMS_VOICE_CAPABILITY_NOT_AVAIL, MSCC_MMC_PERSISTENT_BEARER_STATE_NOT_EXIST);
        NAS_MSCC_SetPersistentBearerState(NAS_MSCC_PERSISTENT_BEARER_STATE_NOT_EXIST);
        NAS_MSCC_SetImsVoiceCapability(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE);

        /* ֪ͨMMA��ǰIMS VOICE������ */
        NAS_MSCC_SndMmaImsVoiceCapInd(VOS_FALSE);

        /* ��IMSA�ظ�STOP CNF��Ϣ */
        NAS_MSCC_SndMmaImsStopCnf(NAS_MSCC_PIF_IMS_STOP_RESULT_SUCC);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_RcvTiWaitImsaStartCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_IMS_CONFIG_PARA_STRU      *pstImsCfgInfo = VOS_NULL_PTR;
    NAS_MSCC_FSM_ID_ENUM_UINT32         enFsmId;

    pstImsCfgInfo = NAS_MSCC_GetImsCfgInfo();
    enFsmId       = NAS_MSCC_GetCurrFsmId();

    /* �жϵ�ǰ״̬����״̬��������ڿ�����״̬���棬����Ϊ��IMS SWITCH�����Ŀ������� */
    if (NAS_MSCC_FSM_SWITCH_ON != enFsmId)
    {
        /* ����IMSҵ������ */
        pstImsCfgInfo->stImsRatSupport.ucLteImsSupportFlag = VOS_FALSE;
        pstImsCfgInfo->stImsRatSupport.ucLteEmsSupportFlag = VOS_FALSE;

        /* ��MMA�ظ�START CNF��Ϣ */
        NAS_MSCC_SndMmaImsStartCnf(NAS_MSCC_PIF_IMS_START_RESULT_TIMEOUT);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_RcvTiWaitImsaPowerOffCnfExpired_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_IMS_CONFIG_PARA_STRU      *pstImsCfgInfo = VOS_NULL_PTR;
    NAS_MSCC_FSM_ID_ENUM_UINT32         enFsmId;

    pstImsCfgInfo = NAS_MSCC_GetImsCfgInfo();
    enFsmId       = NAS_MSCC_GetCurrFsmId();

    /* �жϵ�ǰ״̬����״̬��������ڹػ���״̬���棬����Ϊ��IMS SWITCH�����Ĺػ����� */
    if (NAS_MSCC_FSM_POWER_OFF != enFsmId)
    {
        /* ����IMSҵ������ */
        pstImsCfgInfo->stImsRatSupport.ucLteImsSupportFlag = VOS_FALSE;
        pstImsCfgInfo->stImsRatSupport.ucLteEmsSupportFlag = VOS_FALSE;

        /* ֪ͨMMC��ǰIMS�Ŀ���״̬ */
        NAS_MSCC_SndMmcImsSwitchStateInd(NAS_MSCC_PIF_IMS_SWITCH_STATE_OFF);

        /* ֪ͨMMC��ǰIMS voice������ */
        NAS_MSCC_SndMmcImsVoiceCapNotify(MSCC_MMC_IMS_VOICE_CAPABILITY_NOT_AVAIL, MSCC_MMC_PERSISTENT_BEARER_STATE_NOT_EXIST);
        NAS_MSCC_SetPersistentBearerState(NAS_MSCC_PERSISTENT_BEARER_STATE_NOT_EXIST);
        NAS_MSCC_SetImsVoiceCapability(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE);


        /* ֪ͨMMA��ǰIMS VOICE������ */
        NAS_MSCC_SndMmaImsVoiceCapInd(VOS_FALSE);

        /* ��IMSA�ظ�STOP CNF��Ϣ */
        NAS_MSCC_SndMmaImsStopCnf(NAS_MSCC_PIF_IMS_STOP_RESULT_TIMEOUT);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}


#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 NAS_MSCC_RcvXsdPowerSaveCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU                st1xSysSrvInfoInd;
    XSD_MSCC_POWER_SAVE_CNF_STRU                           *pstPowerSaveCnf;
    MSCC_HSD_1X_SYS_INFO_STRU                               st1xSysInfo;

    NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_ENUM_UINT32      enNoCardInitSearchWaitType;

    pstPowerSaveCnf = (XSD_MSCC_POWER_SAVE_CNF_STRU*)pstMsg;

    enNoCardInitSearchWaitType = NAS_MSCC_GetNoCardInitSearchWaitType();

    if ((NAS_MSCC_FSM_L1_MAIN == NAS_MSCC_GetCurrFsmId())
     && (NAS_MSCC_NO_CARD_INIT_SEARCH_WAIT_TYPE_1X == enNoCardInitSearchWaitType))
    {
        /* �޿������յ�ID_XSD_MSCC_POWER_SAVE_CNF,˵��GU������ɣ�1x���ѱ���ϣ��ظ�MMA_GET_GEO_CNF��MMA_STOP_GET_GEO_CNF */
        NAS_MSCC_ProcNoCardInitSearchWhenRcvXsdPowerSaveCnf_PreProc();

        return VOS_TRUE;
    }

    if (NAS_MSCC_PIF_POWER_SAVE_SUCCESS == pstPowerSaveCnf->enResult)
    {
        NAS_MSCC_Set1xActiveFlg(VOS_FALSE);
        NAS_MSCC_Set1xCampOnFlag(VOS_FALSE);

        /* notify mma no service */
        NAS_MSCC_SetCurr1xServiceStatus(NAS_MSCC_SERVICE_STATUS_NO_SERVICE);

        PS_MEM_SET(&st1xSysSrvInfoInd, 0x0, sizeof(XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU));
        st1xSysSrvInfoInd.stSysSrvInfo.enServiceStatus       = NAS_MSCC_PIF_SERVICE_STATUS_NO_SERVICE;
        st1xSysSrvInfoInd.stSysSrvInfo.enBaseStationInfoIncl = PS_TRUE;

        NAS_MSCC_SndMma1xSystemServiceInfoInd(&st1xSysSrvInfoInd);

        /* power save 1Xϵͳ�����HRPDϵͳ֧�֣�֪ͨһ��HSD */
        if (VOS_TRUE == NAS_MSCC_IsPlatformSupportCdmaEVDO())
        {
            PS_MEM_SET(&st1xSysInfo, 0, sizeof(MSCC_HSD_1X_SYS_INFO_STRU));

            NAS_MSCC_SndHsd1xSysChgInd(VOS_FALSE, &st1xSysInfo);
        }
        /* 1X�ر�ʱ���LTE��������, ��ʱ��Ҫ����LTE������any����,
           ���LTE֮ǰ���ڲ������ѱ�power save��, �ر�1X��SYSCFG
           ���̻����´�����һ��LTE */
        if (VOS_FALSE == NAS_MSCC_GetAllowSrchLteFlg())
        {
            NAS_MSCC_SetAllowSrchLteFlg(VOS_TRUE);

            NAS_MSCC_SndMmcCLAssociatedInfoNtf(NAS_MSCC_PIF_PLMN_PRIORITY_ANY, VOS_TRUE);
        }
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_RcvHsdPowerSaveCnf_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_POWER_SAVE_CNF_STRU       *pstPowerSaveCnf;

    pstPowerSaveCnf = (HSD_MSCC_POWER_SAVE_CNF_STRU*)pstMsg;

    if (NAS_MSCC_PIF_POWER_SAVE_SUCCESS == pstPowerSaveCnf->enResult)
    {
        NAS_MSCC_SetHrpdActiveFlg(VOS_FALSE);
        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

        /* notify mma no service */
        NAS_MSCC_SetCurrHrpdServiceStatus(NAS_MSCC_SERVICE_STATUS_NO_SERVICE);

        NAS_MSCC_SndMmaHrpdServiceStatusInd(MSCC_MMA_HRPD_SERVICE_STATUS_NO_SERVICE,
                                            VOS_FALSE,
                                            VOS_FALSE,
                                            MSCC_MMA_SESSION_RELEASE_TYPE_BUTT);
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_IsNeedDiscardMmcServiceStatusInd_PreProc(
    NAS_MSCC_PIF_SERVICE_STATUS_ENUM_UINT32      enServiceStatus
)
{
    NAS_MSCC_FSM_CTX_STRU              *pstCurFsm = VOS_NULL_PTR;

    pstCurFsm = NAS_MSCC_GetCurFsmAddr();

    if (NAS_MSCC_FSM_BETTER_SYSTEM_RESELECTION == pstCurFsm->enFsmId)
    {
        if ((NAS_MSCC_BSR_STA_WAIT_HSD_POWER_SAVE_CNF                == pstCurFsm->ulState)
         || (NAS_MSCC_BSR_STA_WAIT_MMC_POWER_SAVE_CNF_REACQ_HRPD     == pstCurFsm->ulState)
         || (NAS_MSCC_BSR_STA_WAIT_HSD_SYSTEM_ACQUIRE_CNF_REACQ_HRPD == pstCurFsm->ulState))
        {
            return VOS_TRUE;
        }

        if (NAS_MSCC_PIF_SERVICE_STATUS_NORMAL_SERVICE != enServiceStatus)
        {
            if (NAS_MSCC_BSR_STA_WAIT_MMC_PLMN_SEARCH_CNF == pstCurFsm->ulState)
            {
                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_ProcPsServiceStatusIndInCLMode_PreProc(
    MMC_MSCC_SERVICE_STATUS_IND_STRU   *pstSrvStatusInd
)
{
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8 enUsimCardStatus;
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8 enCsimCardStatus;

    /* �����Ƿ�CLģʽ,ͳһ����NV */
    if ((VOS_TRUE                                   == NAS_MSCC_IsLtePlmnIdChanged())
     && (NAS_MSCC_PIF_SERVICE_STATUS_NORMAL_SERVICE == pstSrvStatusInd->enServiceStatus))
    {
        /* Update the location info to NVIM if the service status is normal service and the location info is changed */
        NAS_MSCC_UpdateLocationInfoToNvim();
    }

    /* SYSCFGEX��ͬʱ֧��HRPD+LTE�򲻴������Ϣ */
    if (VOS_FALSE == NAS_MSCC_IsHrpdAndLteBothSupported())
    {
        return VOS_TRUE;
    }

    /* MLPL��MSPL��Ч�򲻴������Ϣ */
    if (VOS_FALSE== NAS_MSCC_GetMlplMsplValidFlag())
    {
        return VOS_TRUE;
    }

    enUsimCardStatus   = NAS_MSCC_GetUsimCardStatus();
    enCsimCardStatus   = NAS_MSCC_GetCsimCardStatus();

    /* ��CSIM+USIMӦ���򲻴������Ϣ */
    if ((NAS_MSCC_PIF_CARD_STATUS_USIM_PRESENT != enUsimCardStatus)
     || (NAS_MSCC_PIF_CARD_STATUS_CSIM_PRESENT != enCsimCardStatus))
    {
        return VOS_TRUE;
    }

    /* BG�����������ж��Ƿ���Ҫ�����÷���״ָ̬ʾ��Ϣ */
    if (VOS_TRUE == NAS_MSCC_IsNeedDiscardMmcServiceStatusInd_PreProc(pstSrvStatusInd->enServiceStatus))
    {
        NAS_MSCC_MNTN_LogDiscardServStaInfo(NAS_MSCC_DISCARD_MMC_SERVICE_STATUS_IND);

        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MSCC_IsNeedStartBsrTimer())
    {
        if (VOS_TRUE == NAS_MSCC_IsNeedRestartBsrTimer())
        {
            NAS_MSCC_StopTimer(TI_NAS_MSCC_BSR_TIMER);
            NAS_MSCC_StartBsrTimer();
        }
        else
        {
            if (NAS_MSCC_TIMER_STATUS_RUNING != NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_BSR_TIMER))
            {
                NAS_MSCC_StartBsrTimer();
            }
        }
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvHsdRfAvailableInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_PIF_RF_AVAILABLE_IND_STRU      *pstRfAvail = VOS_NULL_PTR;

    pstRfAvail = (NAS_MSCC_PIF_RF_AVAILABLE_IND_STRU*)pstMsg;

    NAS_MSCC_SndMmaRfAvailInd(pstRfAvail);

    /* �������غ�ģʽ�£�ֻ͸������Ϣ������������ */
    if (NAS_MSCC_PIF_EMC_CALLBACK_MODE_ENABLE == NAS_MSCC_GetEmcCallExistFlag())
    {
        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MSCC_Is3gppNormalService())
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvHsdRfAvailableInd_PreProc discard due to lte normal service");
        return VOS_TRUE;
    }

    /* if HRPD is deactive, mscc should enter active state to do */
    if (VOS_FALSE == NAS_MSCC_GetHrpdActiveFlg())
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvXsdRfAvailableInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_PIF_RF_AVAILABLE_IND_STRU      *pstRfAvail = VOS_NULL_PTR;

    pstRfAvail = (NAS_MSCC_PIF_RF_AVAILABLE_IND_STRU*)pstMsg;

    NAS_MSCC_SndMmaRfAvailInd(pstRfAvail);

    if (VOS_FALSE == NAS_MSCC_Get1xActiveFlg())
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


#endif


#ifdef DMT

VOS_UINT32 NAS_MSCC_RcvMsccMofiyMlplRecordReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MLPL_STRU                                     *pstMlplInfo;
    NAS_MSCC_MSCC_MODIFY_MLPL_RECORD_REQ_STRU              *pstMdyMlplRecReq;
    VOS_UINT8                                               ucMlplRecIndex;
    VOS_UINT8                                               ucMlplRecTagIndex;
    VOS_UINT8                                               i;

    pstMdyMlplRecReq = (NAS_MSCC_MSCC_MODIFY_MLPL_RECORD_REQ_STRU *)pstMsg;
    pstMlplInfo      = NAS_MSCC_GetMlplInfoAddr();

    pstMlplInfo->ucIsContainDefRec = pstMdyMlplRecReq->ucIsContainDefRec;

    for ( i = 0; i < pstMdyMlplRecReq->ucRecNum; i++)
    {
        ucMlplRecIndex = pstMdyMlplRecReq->astMlplRecord[i].ucMlplRecIndex;

        pstMlplInfo->apstMlplRec[ucMlplRecIndex]->ucIsRecValid   = pstMdyMlplRecReq->astMlplRecord[i].stMlplRec.ucIsRecValid;
        pstMlplInfo->apstMlplRec[ucMlplRecIndex]->ucLocParamType = pstMdyMlplRecReq->astMlplRecord[i].stMlplRec.ucLocParamType;
        pstMlplInfo->apstMlplRec[ucMlplRecIndex]->ucMsplIndex = pstMdyMlplRecReq->astMlplRecord[i].stMlplRec.ucMsplIndex;

        pstMlplInfo->apstMlplRec[ucMlplRecIndex]->stLocParamVal.usMcc
                        = pstMdyMlplRecReq->astMlplRecord[i].stMlplRec.usMcc;

        pstMlplInfo->apstMlplRec[ucMlplRecIndex]->stLocParamVal.usMnc
                        = pstMdyMlplRecReq->astMlplRecord[i].stMlplRec.usMnc;

        ucMlplRecTagIndex = pstMdyMlplRecReq->astMlplRecord[i].stMlplRec.ucSysLocTagIndex;

        pstMlplInfo->apstMlplRec[ucMlplRecIndex]->stLocParamVal.astSysLocTag[ucMlplRecTagIndex]
                    = pstMdyMlplRecReq->astMlplRecord[i].stMlplRec.stSysLocTag;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMsccMofiyMsplIdReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MSPL_STRU                                     *pstMsplInfo;
    NAS_MSCC_MSCC_MODIFY_MSPL_ID_REQ_STRU                  *pstMdyMsplIdReq;
    VOS_UINT8                                               ucMsplIdIndex;
    VOS_UINT8                                               ucMsplRecIndex;
    VOS_UINT8                                               i;

    pstMdyMsplIdReq = (NAS_MSCC_MSCC_MODIFY_MSPL_ID_REQ_STRU *)pstMsg;
    pstMsplInfo     = NAS_MSCC_GetMsplInfoAddr();

    for ( i = 0; i < pstMdyMsplIdReq->ucRecNum; i++)
    {
        ucMsplIdIndex = pstMdyMsplIdReq->astMsplId[i].ucMlplIdIndex;

        pstMsplInfo->apstMsplId[ucMsplIdIndex]->ucIsMsplIdValid = pstMdyMsplIdReq->astMsplId[i].stMsplId.ucIsMsplIdValid;
        pstMsplInfo->apstMsplId[ucMsplIdIndex]->ucMsplId        = pstMdyMsplIdReq->astMsplId[i].stMsplId.ucMsplId;

        ucMsplRecIndex = pstMdyMsplIdReq->astMsplId[i].stMsplId.ucMsplRecIndex;

        pstMsplInfo->apstMsplId[ucMsplIdIndex]->astMsplRec[ucMsplRecIndex]  = pstMdyMsplIdReq->astMsplId[i].stMsplId.stMsplRec;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvSetCardReadStubTypeReq_PreProc(
    VOS_UINT32                                              ulEventType,
    struct MsgCB                                           *pstMsg
)
{
    NAS_MSCC_SET_CARD_READ_STUB_TYPE_REQ_STRU              *pstStubReqMsg;

    pstStubReqMsg = (NAS_MSCC_SET_CARD_READ_STUB_TYPE_REQ_STRU *)pstMsg;

    genCardReadStubTypeForDmt = pstStubReqMsg->enStubType;

    return VOS_TRUE;
}

#endif


VOS_UINT32 NAS_MSCC_RcvMmaAutoReselSetReq_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_AUTO_RESEL_SET_STRU       *pstAutoReselSetReq;

    pstAutoReselSetReq = (MMA_MSCC_AUTO_RESEL_SET_STRU *)pstMsg;

    NAS_MSCC_SndMmcAutoReselSetReq(pstAutoReselSetReq->ucActiveFlg);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcSrchedPlmnInfo_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
       /* ת����Ϣ��MMA */
    NAS_MSCC_SndMmaSrchedPlmnInfo((MMC_MSCC_SRCHED_PLMN_INFO_IND_STRU*)pstMsg);

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 NAS_MSCC_RcvHsdInterSysEndInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_INTERSYS_END_IND_STRU     *pstInterSysEndInd       = VOS_NULL_PTR;

    NAS_MSCC_FSM_ID_ENUM_UINT32         enCurFsmId;

    pstInterSysEndInd = (HSD_MSCC_INTERSYS_END_IND_STRU*)pstMsg;
    enCurFsmId        = NAS_MSCC_GetCurrFsmId();

    /* CLģʽ�£�LTE2HRPD��hsd�յ�CAS_CNAS_HRPD_IRAT_FROM_LTE��Ϣ����mscc���͸���Ϣ
        ��ʱ����Ϣ������дVOS_RATMODE_BUTT
    */
    if (VOS_RATMODE_BUTT == pstInterSysEndInd->enCurrRatMode)
    {
        NAS_MSCC_Set3gppActiveFlg(VOS_FALSE);
        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);

        NAS_MSCC_SndMmcInterSysHrpdInd();
    }
    /* CLģʽ�£�HRPD2LTE, ʧ�ܻ��˵�HRPD���յ�CAS_CNAS_RESUME_ID��Ϣ����mscc����END_IND��������дVOS_RATMODE_HRPD.
    */
    else if (VOS_RATMODE_HRPD == pstInterSysEndInd->enCurrRatMode)
    {

    }
    /* CLģʽ�£�HRPD2LTE, �ɹ���hsd�յ�CAS_CNAS_HRPD_IRAT_TO_LTE��Ϣ����mscc����END_IND��������дVOS_RATMODE_LTE */
    else if (VOS_RATMODE_LTE == pstInterSysEndInd->enCurrRatMode)
    {
        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);
        NAS_MSCC_SetHrpdActiveFlg(VOS_FALSE);
    }
    else  /* ����ֵ���쳣����ӡ��¼ */
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvHsdInterSysEndInd_PreProc:The IE enCurrRatMode value is error!");
    }

    /* ֻ����ϵͳ�����ﴦ����ϵͳ����ָʾ */
    if (NAS_MSCC_FSM_CL_INTERSYS == enCurFsmId)
    {
        return VOS_FALSE;
    }

    NAS_WARNING_LOG(UEPS_PID_MSCC,
            "NAS_MSCC_RcvHsdInterSysStartInd_PreProc(): receive intersyss end ind while not in intersys fsm");

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvHsdInterSysStartInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_INTERSYS_START_IND_STRU                       *pstInterSysStartInd;
    NAS_MSCC_FSM_ID_ENUM_UINT32                             enCurFsmId;

    pstInterSysStartInd = (HSD_MSCC_INTERSYS_START_IND_STRU *)pstMsg;
    enCurFsmId          = NAS_MSCC_GetCurrFsmId();

    /* ͸����mmc C2L ��ϵͳ�л��Ŀ�ʼָʾ */
    NAS_MSCC_SncMmcCLInterSysStartNtf(pstInterSysStartInd->enInterSysCause);

    if ((NAS_MSCC_FSM_L1_MAIN                   == enCurFsmId)
     || (NAS_MSCC_FSM_BETTER_SYSTEM_RESELECTION == enCurFsmId))
    {
        return VOS_FALSE;
    }

    /* mscc�ڷ�main״̬���л���intersys cause��Ϊ ��ѡ���յ�hsd��ϵͳ��ʼָʾ��ֱ�Ӷ����������� */
    NAS_WARNING_LOG(UEPS_PID_MSCC,
        "NAS_MSCC_RcvHsdInterSysStartInd_PreProc(): when receive hsd intersyss start ind not in L1 main or bsr fsm, just discard it");

    return VOS_TRUE;
}
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 NAS_MSCC_RcvHsdHrpdNoSrv_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SndMmaHrpdServiceStatusInd(MSCC_MMA_HRPD_SERVICE_STATUS_NO_SERVICE,
                                        VOS_FALSE,
                                        VOS_FALSE,
                                        MSCC_MMA_SESSION_RELEASE_TYPE_BUTT);

    /* ��MMA�ϱ��޷��񣬺�MMA���ַ���״̬һ�£��˴�Ҳ�����޷��� */
    NAS_MSCC_SetCurrHrpdServiceStatus(NAS_MSCC_SERVICE_STATUS_NO_SERVICE);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvXsd1xNoSrv_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU                st1xSysSrvInfoInd;

    PS_MEM_SET(&st1xSysSrvInfoInd, 0x0, sizeof(XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU));
    st1xSysSrvInfoInd.stSysSrvInfo.enServiceStatus = NAS_MSCC_PIF_SERVICE_STATUS_NO_SERVICE;

    NAS_MSCC_SndMma1xSystemServiceInfoInd(&st1xSysSrvInfoInd);

    /* ��MMA�ϱ��޷��񣬺�MMA���ַ���״̬һ�£��˴�Ҳ�����޷��� */
    NAS_MSCC_SetCurr1xServiceStatus(NAS_MSCC_SERVICE_STATUS_NO_SERVICE);

    return VOS_TRUE;
}
#endif


#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 NAS_MSCC_RcvMmcInterSysStartInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_FSM_ID_ENUM_UINT32                             enCurFsmId;
    MMC_MSCC_INTERSYS_START_IND_STRU                       *pstInterSysStartInd;

    enCurFsmId          = NAS_MSCC_GetCurrFsmId();
    pstInterSysStartInd = (MMC_MSCC_INTERSYS_START_IND_STRU *)pstMsg;

    /* ��ϵͳ�������̻�bsr��active��״̬��Ҫ����mmc��ϵͳ��ʼָʾ */
    if ((NAS_MSCC_FSM_L1_MAIN                   != enCurFsmId)
     && (NAS_MSCC_FSM_SYS_ACQ                   != enCurFsmId)
     && (NAS_MSCC_FSM_BETTER_SYSTEM_RESELECTION != enCurFsmId))
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC,
        "NAS_MSCC_RcvMmcInterSysStartInd_PreProc(): when receive intersys start ind not in syscfg or bsr or  L1 main fsm,just discard it");

        return VOS_TRUE;
    }

    /* ֻ��intersys casuse Ϊ��ѡ���ض������Ҫ����mmc��ϵͳ��ʼָʾ */
    if ((NAS_MSCC_PIF_INTERSYS_CAUSE_RESELECT != pstInterSysStartInd->enInterSysCause)
     && (NAS_MSCC_PIF_INTERSYS_CAUSE_REDIR    != pstInterSysStartInd->enInterSysCause))
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC,
        "NAS_MSCC_RcvMmcInterSysStartInd_PreProc(): when receive intersys start ind with not reselect or redir,just discard it");

        return VOS_TRUE;
    }

    /* ֻ����ϵͳ����ΪL2C����Ҫ����mmc��ϵͳ��ʼָʾ */
    if ((VOS_RATMODE_LTE  != pstInterSysStartInd->enOrignRatMode)
     || (VOS_RATMODE_HRPD != pstInterSysStartInd->enDestRatMode))
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC,
        "NAS_MSCC_RcvMmcInterSysStartInd_PreProc(): when receive intersys start ind with not L2C,just discard it");

        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_RcvMmcInterSysEndInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_FSM_ID_ENUM_UINT32         enCurFsmId;

    enCurFsmId = NAS_MSCC_GetCurrFsmId();

    /* ֻ����ϵͳ�����ﴦ����ϵͳ����ָʾ */
    if (NAS_MSCC_FSM_CL_INTERSYS == enCurFsmId)
    {
        return VOS_FALSE;
    }

    NAS_WARNING_LOG(UEPS_PID_MSCC,
            "NAS_MSCC_RcvMmcInterSysEndInd_PreProc(): receive intersyss end ind while not in intersys fsm");

    return VOS_TRUE;
}

#endif



#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



