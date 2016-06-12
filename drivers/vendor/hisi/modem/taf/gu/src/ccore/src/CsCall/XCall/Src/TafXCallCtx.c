

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "TafXCallCtx.h"
#include "TafSdcCtx.h"
#include "TafStdlib.h"
#include "TafXCallMntn.h"
#include "MnClient.h"
#include "siapppb.h"

#include "TafCdmaPlusTranslate.h"

#include "TafXCallProcNvim.h"
#include "TafLog.h"
#include "TafXCallSndApp.h"
#include "TafXCallSndXcc.h"

#include "TafXCallEccCipher.h"

#include "NasUsimmApi.h"
#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_TAF_X_CALL_CTX_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
TAF_XCALL_CTX_STRU                      g_stTafXcallCtx;

TAF_XCALL_CS_ERR_CODE_MAP_STRU          g_astTafXcallErrCodeMapTbl[] =
{
    {TAF_CS_CAUSE_SUCCESS,                                  TAF_XCC_CAUSE_SUCCESS},
    {TAF_CS_CAUSE_XCALL_NO_SERVICE,                         TAF_XCC_CAUSE_NO_SERVICE},
    {TAF_CS_CAUSE_XCALL_MAX_ACCESS_PROBES,                  TAF_XCC_CAUSE_MAX_ACCESS_PROBES},
    {TAF_CS_CAUSE_XCALL_REORDER,                            TAF_XCC_CAUSE_REORDER},
    {TAF_CS_CAUSE_XCALL_INTERCEPT,                          TAF_XCC_CAUSE_INTERCEPT},
    {TAF_CS_CAUSE_XCALL_ACCESS_DENYIED,                     TAF_XCC_CAUSE_ACCESS_DENYIED},
    {TAF_CS_CAUSE_XCALL_LOCK,                               TAF_XCC_CAUSE_LOCK},
    {TAF_CS_CAUSE_XCALL_ACCT_BLOCK,                         TAF_XCC_CAUSE_ACCT_BLOCK},
    {TAF_CS_CAUSE_XCALL_NDSS,                               TAF_XCC_CAUSE_NDSS},
    {TAF_CS_CAUSE_XCALL_REDIRECTION,                        TAF_XCC_CAUSE_REDIRECTION},
    {TAF_CS_CAUSE_XCALL_NOT_ACCEPT_BY_BS,                   TAF_XCC_CAUSE_NOT_ACCEPT_BY_BS},
    {TAF_CS_CAUSE_XCALL_ACCESS_IN_PROGRESS,                 TAF_XCC_CAUSE_ACCESS_IN_PROGRESS},
    {TAF_CS_CAUSE_XCALL_ACCESS_FAIL,                        TAF_XCC_CAUSE_ACCESS_FAIL},
    {TAF_CS_CAUSE_XCALL_ABORT,                              TAF_XCC_CAUSE_ABORT},


    {TAF_CS_CAUSE_XCALL_SIGNAL_FADE_IN_ACH,                 TAF_XCC_CAUSE_SIGNAL_FADE_IN_ACH},
    {TAF_CS_CAUSE_XCALL_CHANNEL_ASSIGN_TIMEOUT,             TAF_XCC_CAUSE_CHANNEL_ASSIGN_TIMEOUT},
    {TAF_CS_CAUSE_XCALL_BS_RELEASE,                         TAF_XCC_CAUSE_BS_RELEASE},
    {TAF_CS_CAUSE_XCALL_OTHER_SERVICE_IN_TCH,               TAF_XCC_CAUSE_OTHER_SERVICE_IN_TCH},
    {TAF_CS_CAUSE_XCALL_CCS_NOT_SUPPORT,                    TAF_XCC_CAUSE_CCS_NOT_SUPPORT},
    {TAF_CS_CAUSE_XCALL_MS_NORMAL_RELEASE,                  TAF_XCC_CAUSE_MS_NORMAL_RELEASE},
    {TAF_CS_CAUSE_XCALL_NW_NORMAL_RELEASE,                  TAF_XCC_CAUSE_NW_NORMAL_RELEASE},
    {TAF_CS_CAUSE_XCALL_SO_REJ,                             TAF_XCC_CAUSE_SO_REJ},
    {TAF_CS_CAUSE_XCALL_RELEASE_TIME_OUT,                   TAF_XCC_CAUSE_RELEASE_TIME_OUT},
    {TAF_CS_CAUSE_XCALL_CONNECT_ORDER_ACK_FAILURE,          TAF_XCC_CAUSE_CONNECT_ORDER_ACK_FAILURE},
    {TAF_CS_CAUSE_XCALL_INCOMING_RSP_TIME_OUT,              TAF_XCC_CAUSE_INCOMING_RSP_TIME_OUT},
    {TAF_CS_CAUSE_XCALL_L2_ACK_TIME_OUT,                    TAF_XCC_CAUSE_L2_ACK_TIME_OUT},
    {TAF_CS_CAUSE_XCALL_POWER_DOWN_IND,                     TAF_XCC_CAUSE_POWER_DOWN_IND},

    {TAF_CS_CAUSE_XCALL_CONNID_NOT_FOUND,                   TAF_XCC_CAUSE_CONNID_NOT_FOUND},
    {TAF_CS_CAUSE_XCALL_ACCESS_CNF_TIMEOUT,                 TAF_XCC_CAUSE_ACCESS_CNF_TIMEOUT},
    {TAF_CS_CAUSE_XCALL_ACCESS_IND_TIMEOUT,                 TAF_XCC_CAUSE_ACCESS_TCH_TIMEOUT},

    {TAF_CS_CAUSE_XCALL_UNKNOWN,                            TAF_XCC_CAUSE_UNKNOWN},
    {TAF_CS_CAUSE_XCALL_CALL_NOT_ALLOWED,                   TAF_XCC_CAUSE_CALL_NOT_ALLOWED},

    {TAF_CS_CAUSE_XCALL_XCALL_HANGUP,                       TAF_XCC_CAUSE_XCALL_HANGUP},

    {TAF_CS_CAUSE_XCALL_WAIT_L2_ACK_RELEASE,                TAF_XCC_CAUSE_WAIT_L2_ACK_CALL_RELEASE},
    {TAF_CS_CAUSE_XCALL_EMERGENCY_CALL_FLASHED,             TAF_XCC_CAUSE_EMERGENCY_CALL_FLASHED},
    {TAF_CS_CAUSE_XCALL_EMERGENCY_CALL_ABORT,               TAF_XCC_CAUSE_EMERGENCY_CALL_ABORT},

    {TAF_CS_CAUSE_XCALL_NDSS_REDIAL_IND,                    TAF_XCC_CAUSE_NDSS_REDIAL_IND},

    {TAF_CS_CAUSE_XCALL_MT_SMS_IN_TCH,                      TAF_XCC_CAUSE_MT_SMS_IN_TCH},

    {TAF_CS_CAUSE_XCALL_SIGNAL_FADE_IN_TCH,                 TAF_XCC_CAUSE_SIGNAL_FADE_IN_TCH},
};


/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -save -e958*/


TAF_XCALL_CTX_STRU* TAF_XCALL_GetCtxAddr(VOS_VOID)
{
    return &(g_stTafXcallCtx);
}


TAF_XCALL_CALL_ENTITY_STRU* TAF_XCALL_GetCallEntityAddr(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    return &(pstXcallCtx->astCallEntity[ucCallId - 1]);
}


TAF_XCALL_CONFIG_STRU* TAF_XCALL_GetConfigInfo(VOS_VOID)
{
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    return &(pstXcallCtx->stConfig);
}


VOS_VOID TAF_XCALL_UpdateDigitMode(
    TAF_XCALL_DIGIT_MODE_ENUM_UINT8     enDigitMode
)
{
    TAF_XCALL_CONFIG_STRU              *pstCallConfig = VOS_NULL_PTR;

    pstCallConfig                       = TAF_XCALL_GetConfigInfo();
    pstCallConfig->enDigitMode          = enDigitMode;

    return;
}


VOS_VOID TAF_XCALL_InitSpecCallEntity(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    PS_MEM_SET(pstCallEntity, 0, sizeof(TAF_XCALL_CALL_ENTITY_STRU));

    pstCallEntity->ulUsed         = VOS_FALSE;

    pstCallEntity->enSo           = TAF_XCALL_SO_BUTT;

    pstCallEntity->enCallState    = MN_CALL_S_IDLE;

    pstCallEntity->enLastReOrigErr = TAF_XCC_FAIL_LAYER_BUTT;

    /* Ĭ������Ϊpublic */
    pstCallEntity->enPrivacyMode   = TAF_CALL_PRIVACY_MODE_PUBLIC;

    /*lint -e717*/
    HI_INIT_LIST_HEAD(&pstCallEntity->stDtmfBuffer.stList);
    /*lint +e717*/
}


VOS_VOID TAF_XCALL_InitNvConfig( VOS_VOID )
{
    TAF_XCALL_ReadEmcCallRedialPeriod();

    TAF_XCALL_Read1xVoiceSoCfgNvim();
    TAF_XCALL_ReadStartAndStopContDtmfIntervalLen();

#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT)
    TAF_XCALL_ReadVoiceEncryptPublicKeyAndVersionNumber();

    TAF_XCALL_ReadVoiceEncryptCapCfgPara();

    TAF_XCALL_ReadVoiceEncryptSecInfoEraseSysTime();

    TAF_XCALL_ReadVoiceEncryptPassWordResetSysTime();

    TAF_XCALL_ReadVoiceEncryptTimerCfgInfo();
#endif

    TAF_XCALL_ReadEmcCallBackCfgNvim();

    return;
}


VOS_VOID TAF_XCALL_InitCtx(VOS_VOID)
{
    VOS_UINT8                           ucCallId;
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    /* ��ʼ������ʵ�� */
    for (ucCallId = 1; ucCallId <= TAF_XCALL_CALL_ENTITY_MAX_NUM; ucCallId++)
    {
        TAF_XCALL_InitSpecCallEntity(ucCallId);
    }

    /* ��ʼ��sups mgmt */
    PS_MEM_SET(&(pstXcallCtx->stSupsCmdMgmt), 0, sizeof(TAF_XCALL_SUPS_CMD_MGMT_STRU));

    /* ��ʼ��configure */
    PS_MEM_SET(&(pstXcallCtx->stConfig), 0, sizeof(TAF_XCALL_CONFIG_STRU));
    pstXcallCtx->stConfig.enDigitMode   = TAF_XCALL_DIGIT_MODE_DTMF;

    TAF_XCALL_InitAllTimers(pstXcallCtx->astTimerCtx);

#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT)
    TAF_XCALL_InitEncVoiceCtx();
#endif

    TAF_XCALL_InitNvConfig();
}


TAF_XCALL_TIMER_CTX_STRU*  TAF_XCALL_GetTimerCtxAddr( VOS_VOID )
{
    return TAF_XCALL_GetCtxAddr()->astTimerCtx;
}




VOS_UINT32  TAF_XCALL_AllocCallId(
    VOS_UINT8                      *pucCallId
)
{
    /* ���ҿ��е�CALL ID��������µĺ��� */
    VOS_UINT8                           i;
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    for (i = 0; i < TAF_XCALL_CALL_ENTITY_MAX_NUM; i++)
    {
        /* �����δ�����ʵ�壬���ص�Call id ��ֵΪʵ�������±�ֵ��1 */
        if (VOS_FALSE == pstXcallCtx->astCallEntity[i].ulUsed)
        {
            *pucCallId = i + 1;

            TAF_XCALL_InitSpecCallEntity(*pucCallId);
            pstXcallCtx->astCallEntity[i].ulUsed = VOS_TRUE;

            return VOS_TRUE;
        }
    }

    return VOS_FALSE;

}


VOS_UINT32 TAF_XCALL_IsAllowMoCallInCurrentStatus(
    MN_CALL_TYPE_ENUM_U8                enCallType
)
{
    /* Ŀǰ�����ز��Ѿ�ʵ��,����״̬���ж����ﲻ��Ҫ */

    /* �����Ϊ�������ҵ�ǰ����һ·������������,���еľ���״̬��XCC
       �����ж��ܷ��� */
    if (MN_CALL_TYPE_EMERGENCY == enCallType)
    {
        if (VOS_TRUE == TAF_XCALL_IsMtCallInstanceExist())
        {
#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT)
            if (TAF_XCALL_VOICE_ENCRYPT_STA_INACTIVE != TAF_XCALL_GetVoiceEncryptNegotiateState())
            {
                /* ������ܻ����ڣ��ܾ������� */
                return VOS_FALSE;
            }
#endif

            return VOS_TRUE;
        }
    }

    /* ��ǰ��������Ϊnormal callʱ�����к���ʵ�嶼����IDLE̬,���Է����º��� */
    if (VOS_TRUE == TAF_XCALL_ExistCallInstance())
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_XCALL_IsAvailCallId(
    VOS_UINT8                           ucCallId
)
{
    if ((0 == ucCallId)
     || (TAF_XCALL_CALL_ENTITY_MAX_NUM < ucCallId))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_XCALL_IsAvailVoiceCallSo(
    VOS_UINT16                          usSo
)
{
    if ((TAF_XCALL_SO_3_EVRC == usSo)
     || (TAF_XCALL_SO_68_EVRC_B == usSo)
     || (TAF_XCALL_SO_73_EVRC_NW == usSo))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID TAF_XCALL_CreateMoCallEntity(
    VOS_UINT8                           ucCallId,
    MN_CALL_APP_REQ_MSG_STRU           *pstCallPara
)
{
    /* ����CALL ID�������ʵ�� */
    MN_CALL_ORIG_PARAM_STRU            *pstOrig       = VOS_NULL_PTR;
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstOrig       = &pstCallPara->unParm.stOrig;
    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    /* ��¼����callҵ���û�����Ϣ(AT/STK) */
    TAF_XCALL_UpdateClientInfo(ucCallId,
                               pstCallPara->ulSenderPid,
                               pstCallPara->clientId,
                               pstCallPara->opId);
    /* ��ʼ�����в��� */
    pstCallEntity->enCallDir            = MN_CALL_DIR_MO;
    pstCallEntity->enCallState          = MN_CALL_S_DIALING;
    pstCallEntity->enCallType           = pstOrig->enCallType;
    pstCallEntity->enCause              = TAF_CS_CAUSE_SUCCESS;
    pstCallEntity->ucConnectId          = 0;
    pstCallEntity->ucConRef             = 0;

    #if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT)
    /***********************************************************************************************
     * ����ܻ�����֧�֣���SO����ΪEVRC
     **********************************************************************************************/
    if (VOS_TRUE == TAF_XCALL_GetVoiceEncryptCapCfgPara()->ucEccSrvCap)
    {
        pstCallEntity->enSo = TAF_XCALL_SO_3_EVRC;
    }
    else
    #endif
    {
        pstCallEntity->enSo = TAF_XCALL_GetConfigInfo()->enSoInitType;
    }

    pstCallEntity->ucIsAlertAnsByPass   = VOS_FALSE;


    /* ��ʼ�����к��� */
    PS_MEM_CPY(&pstCallEntity->stCalledNumber,
               &pstOrig->stDialNumber,
               sizeof(MN_CALL_CALLED_NUM_STRU));

    PS_MEM_CPY(&pstCallEntity->stCalledSubAddr,
               &pstOrig->stSubaddr,
               sizeof(MN_CALL_SUBADDR_STRU));

    /* ���к���Ӻ�ת�� */
    TAF_XCALL_TranlatePlusToCallNum(pstCallEntity);

    pstCallEntity->stRedirectNumber.ucNumLen    = 0;
    pstCallEntity->stConnectNumber.ucNumLen     = 0;

    /* ����SDC��CS���б�־ */
    TAF_XCALL_SetCsCallExistFlg(VOS_TRUE, pstCallPara->unParm.stOrig.enCallType);

    return;
}


VOS_VOID TAF_XCALL_CreateMtCallEntity(
    VOS_UINT8                           ucCallId,
    VOS_UINT32                          ulModuleId,
    XCC_XCALL_INCOMING_CALL_IND_STRU   *pstIncomingCallInd
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    /* �˴������ж�call id����Ч�ԣ����ݴ����call id�������ʵ�� */
    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    /* ��¼����callҵ���û�����Ϣ(AT/STK) */
    TAF_XCALL_UpdateClientInfo(ucCallId,
                               ulModuleId,
                               MN_CLIENT_ID_BROADCAST,
                               0);


    /* ����SO */
    pstCallEntity->enSo               = pstIncomingCallInd->enSo;

    /* ����By pass Flag */
    pstCallEntity->ucIsAlertAnsByPass = pstIncomingCallInd->ucIsAlertAnsByPass;

    /* ����Connect Reference */
    pstCallEntity->ucConRef           = pstIncomingCallInd->ucConRef;

    /* ����Connect Id */
    pstCallEntity->ucConnectId        = pstIncomingCallInd->ucConnectId;

    /* ���º���״̬ */
    pstCallEntity->enCallState        = MN_CALL_S_INCOMING;

    /* ���º������� */
    pstCallEntity->enCallType         = MN_CALL_TYPE_VOICE;

    /* ���·������Dir */
    pstCallEntity->enCallDir          = MN_CALL_DIR_MT;

    /* ����������� */
    TAF_XCALL_UpdateCallingNum(ucCallId, &(pstIncomingCallInd->stCallingPartyNum));

    /* ����privacy mode */
    if (TAF_CALL_PRIVACY_MODE_BUTT != (TAF_CALL_PRIVACY_MODE_ENUM_UINT8)pstIncomingCallInd->enPrivacyMode)
    {
        pstCallEntity->enPrivacyMode  = (TAF_CALL_PRIVACY_MODE_ENUM_UINT8)pstIncomingCallInd->enPrivacyMode;
    }

    /* ����SDC��CS���б�־ */
    TAF_XCALL_SetCsCallExistFlg(VOS_TRUE, MN_CALL_TYPE_VOICE);

    return;
}


VOS_VOID TAF_XCALL_FreeCallEntity(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_InitSpecCallEntity(ucCallId);

    if (VOS_FALSE == TAF_XCALL_ExistCallInstance())
    {
        /* ���SDC��CS���б�־ */
        TAF_XCALL_SetCsCallExistFlg(VOS_FALSE, MN_CALL_TYPE_BUTT);
    }

    return;
}


VOS_VOID TAF_XCALL_UpdateClientInfo(
    VOS_UINT8                           ucCallId,
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity       = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->stClientInfo.ulModuleId = ulModuleId;
    pstCallEntity->stClientInfo.usClientId = usClientId;
    pstCallEntity->stClientInfo.ucOpId     = ucOpId;

    return;
}


VOS_VOID TAF_XCALL_UpdateCallState(
    VOS_UINT8                           ucCallId,
    MN_CALL_STATE_ENUM_U8               enCallState
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity       = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->enCallState = enCallState;

    return;
}


VOS_VOID TAF_XCALL_UpdateCallType(
    VOS_UINT8                           ucCallId,
    MN_CALL_TYPE_ENUM_U8                enCallType
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity       = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->enCallType = enCallType;
}


VOS_VOID TAF_XCALL_UpdateCallDir(
    VOS_UINT8                           ucCallId,
    MN_CALL_DIR_ENUM_U8                 enCallDir
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity       = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->enCallDir = enCallDir;
}



VOS_VOID TAF_XCALL_UpdateSo(
    VOS_UINT8                           ucCallId,
    TAF_XCALL_SO_TYPE_ENUM_UINT16       enSo
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity       = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->enSo = enSo;
}


VOS_VOID TAF_XCALL_UpdateConnectId(
    VOS_UINT8                           ucCallId,
    VOS_UINT8                           ucConnectId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity              = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->ucConnectId = ucConnectId;
}


VOS_VOID TAF_XCALL_UpdateConnectRef(
    VOS_UINT8                           ucCallId,
    VOS_UINT8                           ucConRef
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity           = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->ucConRef = ucConRef;
}


VOS_VOID TAF_XCALL_UpdateAlertAnsByPassFlg(
    VOS_UINT8                           ucCallId,
    VOS_UINT8                           ucIsAlertAnsByPass
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity                     = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->ucIsAlertAnsByPass = ucIsAlertAnsByPass;
}


VOS_VOID TAF_XCALL_UpdateCcCause(
    VOS_UINT8                           ucCallId,
    TAF_CS_CAUSE_ENUM_UINT32            enCcCause
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity                     = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->enCause            = enCcCause;
}


VOS_VOID TAF_XCALL_UpdateDiscCallDir(
    VOS_UINT8                           ucCallId,
    VOS_UINT8                           ucIsUser
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity                     = TAF_XCALL_GetCallEntityAddr(ucCallId);

    if (VOS_TRUE == pstCallEntity->stDiscDir.ucDiscCallFlag)
    {
        return;
    }

    pstCallEntity->stDiscDir.ucIsUser       = ucIsUser;
    pstCallEntity->stDiscDir.ucDiscCallFlag = VOS_TRUE;

    return;
}


VOS_VOID TAF_XCALL_UpdateCallingNum(
    VOS_UINT8                                               ucCallId,
    XCC_XCALL_CALLING_PARTY_NUM_INFO_STRU                  *pstXccCallNum
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;
    VOS_UINT8                           aucDigit[XCC_XCALL_MAX_NUM_DIGIT_LEN + 1];

    PS_MEM_SET(aucDigit, 0, sizeof(aucDigit));

    pstCallEntity                     = TAF_XCALL_GetCallEntityAddr(ucCallId);

    /* num type:
        bit7 | bit6 bit5 bit4 | bit3 bit2 bit1 bit0
          1  |     num type   |    num plan
     */
    pstCallEntity->stCallNumber.enNumType = 0x80 | (VOS_UINT8)(pstXccCallNum->enNumType << 4) | (pstXccCallNum->enNumPlan& 0x0F);

    pstCallEntity->ucPi                   = pstXccCallNum->ucPi;
    pstCallEntity->ucSi                   = pstXccCallNum->ucSi;

    PS_MEM_CPY(aucDigit, pstXccCallNum->aucDigit, TAF_XCALL_MIN(pstXccCallNum->ucDigitNum, XCC_XCALL_MAX_NUM_DIGIT_LEN));

    (VOS_VOID)TAF_STD_ConvertAsciiNumberToBcd((VOS_CHAR *)aucDigit,
                                    pstCallEntity->stCallNumber.aucBcdNum,
                                    &(pstCallEntity->stCallNumber.ucNumLen));

    return;
}


TAF_XCALL_SUPS_CMD_MGMT_STRU* TAF_XCALL_GetSupsCmdMgmt(VOS_VOID)
{
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    return &(pstXcallCtx->stSupsCmdMgmt);

}


TAF_XCALL_CLIENT_INFO_STRU* TAF_XCALL_GetClientInfo(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    return &(pstCallEntity->stClientInfo);
}


MN_CALL_STATE_ENUM_U8 TAF_XCALL_GetCallState(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity                     = TAF_XCALL_GetCallEntityAddr(ucCallId);

    return pstCallEntity->enCallState;
}


TAF_XCALL_SO_TYPE_ENUM_UINT16 TAF_XCALL_GetSo(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity                     = TAF_XCALL_GetCallEntityAddr(ucCallId);

    return pstCallEntity->enSo;
}


MN_CALL_TYPE_ENUM_U8 TAF_XCALL_GetCallType(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity                     = TAF_XCALL_GetCallEntityAddr(ucCallId);

    return pstCallEntity->enCallType;
}



VOS_UINT8 TAF_XCALL_GetAlertAnsByPassFlg(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity                     = TAF_XCALL_GetCallEntityAddr(ucCallId);

    return pstCallEntity->ucIsAlertAnsByPass;
}


TAF_CS_CAUSE_ENUM_UINT32 TAF_XCALL_GetCcCause(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity                     = TAF_XCALL_GetCallEntityAddr(ucCallId);

    return pstCallEntity->enCause;
}


VOS_UINT32 TAF_XCALL_GetCallIdFromConnectId(
    VOS_UINT8                           ucConnectId,
    VOS_UINT8                          *pucCallId
)
{
    VOS_UINT8                           i;
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    for (i = 0; i < TAF_XCALL_CALL_ENTITY_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstXcallCtx->astCallEntity[i].ulUsed)
         && (ucConnectId == pstXcallCtx->astCallEntity[i].ucConnectId))
        {
            *pucCallId = (VOS_UINT8)(i + 1);

            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}



VOS_VOID TAF_XCALL_GetCallsByState(
    MN_CALL_STATE_ENUM_U8               enCallState,
    VOS_UINT32                         *pulNumOfCalls,
    VOS_UINT8                          *pucCallIds
)
{
    VOS_UINT8                           i;
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    *pulNumOfCalls = 0;

    for (i = 0; i < TAF_XCALL_CALL_ENTITY_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstXcallCtx->astCallEntity[i].ulUsed)
         && (enCallState == pstXcallCtx->astCallEntity[i].enCallState))
        {
            *pucCallIds++ = (VOS_UINT8)(i + 1);
            (*pulNumOfCalls)++;
        }
    }
}


VOS_VOID TAF_XCALL_GetNotIdleStateCalls(
    VOS_UINT32                         *pulNumOfCalls,
    VOS_UINT8                          *pucCallIds
)
{
    VOS_UINT8                           i;
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    *pulNumOfCalls = 0;

    for (i = 0; i < TAF_XCALL_CALL_ENTITY_MAX_NUM; i++)
    {
        if ((VOS_TRUE == pstXcallCtx->astCallEntity[i].ulUsed)
         && (MN_CALL_S_IDLE != pstXcallCtx->astCallEntity[i].enCallState))
        {
            *pucCallIds++ = (VOS_UINT8)(i + 1);
            (*pulNumOfCalls)++;
        }
    }
}


TAF_CS_CAUSE_ENUM_UINT32 TAF_XCALL_MapXccCauseToCsCause(
    TAF_XCC_CAUSE_ENUM_UINT32           enXccCause
)
{
    VOS_UINT32                          ulCnt;

    for (ulCnt = 0; ulCnt < (sizeof(g_astTafXcallErrCodeMapTbl)/sizeof(TAF_XCALL_CS_ERR_CODE_MAP_STRU)); ulCnt++)
    {
        if (g_astTafXcallErrCodeMapTbl[ulCnt].enXccCause == enXccCause)
        {
            return g_astTafXcallErrCodeMapTbl[ulCnt].enCsCause;
        }
    }

    return TAF_CS_CAUSE_UNKNOWN;
}


VOS_UINT32 TAF_XCALL_ExistCallInstance(VOS_VOID)
{
    VOS_UINT8                           i;
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    for (i = 0; i < TAF_XCALL_CALL_ENTITY_MAX_NUM; i++)
    {
        if (VOS_TRUE == pstXcallCtx->astCallEntity[i].ulUsed)
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_VOID TAF_XCALL_RegSsKeyEvent(
    VOS_UINT8                                               ucCallId,
    TAF_XCALL_SS_PROGRESS_EVT_ENUM_UINT8                    enEvt,
    TAF_XCALL_SS_SUBSEQ_OP_ENUM_UINT8                       enSubseqOp
)
{
    TAF_XCALL_SUPS_CMD_MGMT_STRU       *pstSupsCmdMgmt = VOS_NULL_PTR;
    VOS_UINT8                           i;

    pstSupsCmdMgmt = TAF_XCALL_GetSupsCmdMgmt();

    TAF_XCALL_MNTN_LogSupsCmdMgmt();

    if (pstSupsCmdMgmt->ucNumOfKeyEvt >= TAF_XCALL_MAX_SS_CMD_KEY_EVT_NUM)
    {
        return;
    }

    for (i = 0; i < pstSupsCmdMgmt->ucNumOfKeyEvt; i++)
    {
        /* ��call id��Ӧ���¼��Ѵ��� */
        if (pstSupsCmdMgmt->astKeyEvts[i].ucCallId == ucCallId)
        {
            pstSupsCmdMgmt->astKeyEvts[i].enEvt      = enEvt;
            pstSupsCmdMgmt->astKeyEvts[i].enSubseqOp = enSubseqOp;
            return;
        }
    }

    /* ���һ����ע�¼�, �������������д�¼����� */
    pstSupsCmdMgmt->astKeyEvts[pstSupsCmdMgmt->ucNumOfKeyEvt].ucCallId   = ucCallId;
    pstSupsCmdMgmt->astKeyEvts[pstSupsCmdMgmt->ucNumOfKeyEvt].enEvt      = enEvt;
    pstSupsCmdMgmt->astKeyEvts[pstSupsCmdMgmt->ucNumOfKeyEvt].enSubseqOp = enSubseqOp;

    pstSupsCmdMgmt->ucNumOfKeyEvt++;

    TAF_XCALL_MNTN_LogSupsCmdMgmt();

    return;
}


VOS_VOID TAF_XCALL_DeRegSsKeyEvent(
    VOS_UINT8                                               ucCallId,
    TAF_XCALL_SS_PROGRESS_EVT_ENUM_UINT8                    enEvt
)
{
    TAF_XCALL_SUPS_CMD_MGMT_STRU       *pstSupsCmdMgmt = VOS_NULL_PTR;
    VOS_UINT8                           ucNumOfKeyEvt;
    VOS_UINT8                           i;

    pstSupsCmdMgmt = TAF_XCALL_GetSupsCmdMgmt();

    ucNumOfKeyEvt  = pstSupsCmdMgmt->ucNumOfKeyEvt;

    TAF_XCALL_MNTN_LogSupsCmdMgmt();

    /* ���Ҷ�Ӧ��ע���¼� */
    for (i = 0; i < ucNumOfKeyEvt; i++)
    {
        if ((ucCallId == pstSupsCmdMgmt->astKeyEvts[i].ucCallId)
           &&(enEvt == pstSupsCmdMgmt->astKeyEvts[i].enEvt))
        {
            break;
        }
    }

    if (i >= ucNumOfKeyEvt)
    {
        return;
    }

    PS_MEM_CPY(&(pstSupsCmdMgmt->astKeyEvts[i]),
               &(pstSupsCmdMgmt->astKeyEvts[i + 1]),
               ((ucNumOfKeyEvt - i) - 1) * sizeof(TAF_XCALL_SUPS_CMD_KEY_EVT_STRU));

    pstSupsCmdMgmt->ucNumOfKeyEvt--;

    TAF_XCALL_MNTN_LogSupsCmdMgmt();
}


VOS_VOID TAF_XCALL_GetSsKeyEvent(
    VOS_UINT8                                               ucCallId,
    TAF_XCALL_SS_PROGRESS_EVT_ENUM_UINT8                   *penEvt,
    TAF_XCALL_SS_SUBSEQ_OP_ENUM_UINT8                      *penSubseqOp
)
{

    TAF_XCALL_SUPS_CMD_MGMT_STRU       *pstSupsCmdMgmt = VOS_NULL_PTR;
    VOS_UINT8                           ucNumOfKeyEvt;
    VOS_UINT8                           i;

    pstSupsCmdMgmt = TAF_XCALL_GetSupsCmdMgmt();

    ucNumOfKeyEvt  = pstSupsCmdMgmt->ucNumOfKeyEvt;

    TAF_XCALL_MNTN_LogSupsCmdMgmt();

    /* ���Ҷ�Ӧ��ע���¼� */
    for (i = 0; i < ucNumOfKeyEvt; i++)
    {
        if (ucCallId == pstSupsCmdMgmt->astKeyEvts[i].ucCallId)
        {
            *penEvt      = pstSupsCmdMgmt->astKeyEvts[i].enEvt;
            *penSubseqOp = pstSupsCmdMgmt->astKeyEvts[i].enSubseqOp;
            return;
        }
    }

    *penEvt      = TAF_XCALL_SS_PROG_EVT_BUTT;
    *penSubseqOp = TAF_XCALL_SS_SUBSEQ_OP_BUTT;

    return;
}


VOS_VOID TAF_XCALL_SetWaitForXCCDTMFCnfFlag(
    VOS_UINT8                           ucCallId,
    TAF_XCALL_WAIT_XCC_CNF_TYPE_UINT8   enWaitForXCCDTMFCnfFlag
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->enWaitForXCCDTMFCnfFlag = enWaitForXCCDTMFCnfFlag;

    return;
}


TAF_XCALL_WAIT_XCC_CNF_TYPE_UINT8 TAF_XCALL_GetWaitForXCCDTMFCnfFlag(
    VOS_UINT8       ucCallId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    return pstCallEntity->enWaitForXCCDTMFCnfFlag;
}



VOS_UINT32 TAF_XCALL_IsMtCallInstanceExist(VOS_VOID)
{
    VOS_UINT8                           i;
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    for (i = 0; i < TAF_XCALL_CALL_ENTITY_MAX_NUM; i++)
    {
        if ((VOS_TRUE       == pstXcallCtx->astCallEntity[i].ulUsed)
         && (MN_CALL_DIR_MT == pstXcallCtx->astCallEntity[i].enCallDir))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


MN_CALL_DIR_ENUM_U8 TAF_XCALL_GetCallDir(
    VOS_UINT8                           ucCallId
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;
    MN_CALL_DIR_ENUM_U8                 enCallDir;

    pstCallEntity       = TAF_XCALL_GetCallEntityAddr(ucCallId);

    enCallDir = pstCallEntity->enCallDir;

    return enCallDir;
}



#if 0

VOS_VOID TAF_XCALL_TranlateCallNumToPlus(
    TAF_XCALL_CALL_ENTITY_STRU                             *pstCallEntity
)
{
    VOS_UINT16                                              usCurMcc;
    MN_CALL_NUM_TYPE_ENUM_U8                                enCurNumType;
    VOS_UINT8                                               aucCurCallBcdNum[TAF_MT_MAX_BCD_NUM_LEN];
    VOS_UINT8                                               ucCurCallBcdNumLen;
    VOS_INT8                                                acCurCallAsciiNum[TAF_MT_MAX_ASCII_NUM_LEN + 1];
    VOS_UINT8                                               ucCurCallAsciiNumLen;
    TAF_SDC_1X_SYS_INFO_STRU                               *pstSdc1xSysInfo = VOS_NULL_PTR;

    /*��ȡmcc */
    pstSdc1xSysInfo      = TAF_SDC_Get1xSysInfo();
    usCurMcc             = pstSdc1xSysInfo->stServiceInfo.usMcc;

    /* ��ȡBCD���볤�� */
    ucCurCallBcdNumLen   = pstCallEntity->stCallNumber.ucNumLen;

    /* ��ȡ��������*/
    enCurNumType         = pstCallEntity->stCallNumber.enNumType;

    /*��ȡ���к��� */
    PS_MEM_SET(aucCurCallBcdNum, 0, sizeof(aucCurCallBcdNum));
    PS_MEM_CPY(aucCurCallBcdNum, pstCallEntity->stCallNumber.aucBcdNum, sizeof(aucCurCallBcdNum));

    /*��BCD����и�ʽ�ĺ���ת��ΪASCII��ʽ */
    PS_MEM_SET(acCurCallAsciiNum, 0, sizeof(acCurCallAsciiNum));
    TAF_STD_ConvertBcdNumberToAscii(aucCurCallBcdNum, ucCurCallBcdNumLen, acCurCallAsciiNum);

    /*����ģʽ�£��Ӻ�ת������ */
    ucCurCallAsciiNumLen  = (VOS_UINT8)VOS_StrLen(acCurCallAsciiNum);
    TAF_TranslateNumToPlus(usCurMcc, TAF_PLUS_TRANSLATE_DIAL_TYPE_VOICE, &enCurNumType, (VOS_UINT8 *)acCurCallAsciiNum, &ucCurCallAsciiNumLen);

    /*��ASCII���и�ʽ�ĺ���ת��ΪBCD�� */
    ucCurCallBcdNumLen    = 0;
    PS_MEM_SET(aucCurCallBcdNum, 0, sizeof(aucCurCallBcdNum));
    TAF_STD_ConvertAsciiNumberToBcd(acCurCallAsciiNum, aucCurCallBcdNum, &ucCurCallBcdNumLen);

    /*��ת����ĺ�����Ϣд�� */
    PS_MEM_CPY(pstCallEntity->stCallNumber.aucBcdNum, aucCurCallBcdNum, sizeof(aucCurCallBcdNum));
    pstCallEntity->stCallNumber.ucNumLen  = ucCurCallBcdNumLen;
    pstCallEntity->stCallNumber.enNumType = enCurNumType;

    return;
}
#endif

VOS_VOID TAF_XCALL_TranlatePlusToCallNum(
    TAF_XCALL_CALL_ENTITY_STRU                             *pstCallEntity
)
{
    VOS_INT32                                               lSid;
    MN_CALL_NUM_TYPE_ENUM_U8                                enCurNumType;
    VOS_UINT8                                               aucCurCalledBcdNum[TAF_MO_MAX_BCD_NUM_LEN];
    VOS_UINT8                                               aucCurCalledBcdNumLen;
    VOS_INT8                                                acCurCalledAsciiNum[TAF_MO_MAX_ASCII_NUM_LEN + 1];
    VOS_UINT8                                               ucCurCalledAsciiNumLen;

    /*��ȡmcc */
    lSid              = TAF_SDC_GetSdcCtx()->stNetworkInfo.lSid;

    /* ��ȡBCD���볤�� */
    aucCurCalledBcdNumLen = pstCallEntity->stCalledNumber.ucNumLen;

    /* ��ȡ��������*/
    enCurNumType      = pstCallEntity->stCalledNumber.enNumType;

    /*��ȡ���к��� */
    PS_MEM_SET(aucCurCalledBcdNum, 0, sizeof(aucCurCalledBcdNum));
    PS_MEM_CPY(aucCurCalledBcdNum, pstCallEntity->stCalledNumber.aucBcdNum, sizeof(aucCurCalledBcdNum));

    /*��BCD����и�ʽ�ĺ���ת��ΪASCII��ʽ */
    PS_MEM_SET(acCurCalledAsciiNum, 0, sizeof(acCurCalledAsciiNum));
    if (MN_ERR_NO_ERROR != TAF_STD_ConvertBcdNumberToAscii(aucCurCalledBcdNum, aucCurCalledBcdNumLen, (VOS_CHAR*)acCurCalledAsciiNum))
    {
        return;
    }

    /*����ģʽ�£��Ӻ�ת������ */
    ucCurCalledAsciiNumLen = (VOS_UINT8)VOS_StrLen((VOS_CHAR*)acCurCalledAsciiNum);
    TAF_TranslatePlusToNum(lSid, TAF_PLUS_TRANSLATE_DIAL_TYPE_VOICE, &enCurNumType, (VOS_UINT8 *)acCurCalledAsciiNum, &ucCurCalledAsciiNumLen);

    /*��ASCII���и�ʽ�ĺ���ת��ΪBCD�� */
    aucCurCalledBcdNumLen = 0;
    PS_MEM_SET(aucCurCalledBcdNum, 0, sizeof(aucCurCalledBcdNum));

    (VOS_VOID)TAF_STD_ConvertAsciiNumberToBcd((VOS_CHAR*)acCurCalledAsciiNum, aucCurCalledBcdNum, &aucCurCalledBcdNumLen);

    /*��ת����ĺ�����Ϣд�� */
    PS_MEM_CPY(pstCallEntity->stCalledNumber.aucBcdNum, aucCurCalledBcdNum, sizeof(aucCurCalledBcdNum));
    pstCallEntity->stCalledNumber.ucNumLen  = aucCurCalledBcdNumLen;
    pstCallEntity->stCalledNumber.enNumType = enCurNumType;

    return;
}


VOS_VOID TAF_XCALL_GetEccNumList(
    MN_CALL_ECC_NUM_INFO_STRU          *pstEccNumInfo
)
{
    VOS_UINT32                          i;
    TAF_SDC_CUSTOM_ECC_CTX_STRU        *pstCustomCallEmerNumCtx = VOS_NULL_PTR;
    SI_PB_ECC_DATA_STRU                *pstEccData= VOS_NULL_PTR;

    pstEccNumInfo->ulEccNumCount = 0;

    /* ���д��ں��룬���Ƚ����еĺ�������ϱ������� */
    if (TAF_SDC_USIM_STATUS_NO_PRESENT != TAF_SDC_GetCsimStatus())
    {

        pstEccData = (SI_PB_ECC_DATA_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(SI_PB_ECC_DATA_STRU));

        if (VOS_NULL_PTR != pstEccData)
        {
            /* ��ȡUSIM�еĽ������к��� */
            PS_MEM_SET(pstEccData, 0, sizeof(SI_PB_ECC_DATA_STRU));
            NAS_PBAPI_GetXeccNumber(pstEccData);

            for (i = 0; i < TAF_XCALL_MIN(pstEccData->ulReocrdNum, TAF_SDC_MAX_UCIM_ECC_NUM); i++)
            {

                if (pstEccNumInfo->ulEccNumCount >= MN_CALL_MAX_EMC_NUM)
                {
                    break;
                }

                pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].ulMcc              = 0xffffffff;
                pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].ucCategory         = 0;
                pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].ucValidSimPresent  = MN_CALL_ECC_NUM_VALID_SIM_PRESENT;

                pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].ucEccNumLen        = USIM_ECC_LEN;
                VOS_MemCpy(pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].aucEccNum,
                           pstEccData->astEccRecord[i].aucEccCode,
                           USIM_ECC_LEN);
                pstEccNumInfo->ulEccNumCount++;

            }

            /* �ڴ��ͷ� */
            PS_MEM_FREE(WUEPS_PID_TAF,pstEccData);
        }
    }

    /* �б�δ��,��APP���ƵĽ��������������Ϣ */
    pstCustomCallEmerNumCtx = TAF_SDC_GetCustomCallEccNumCtx();
    for (i = 0; i < TAF_XCALL_MIN(pstCustomCallEmerNumCtx->stCustomEccNumList.ucEccNumCount,TAF_SDC_MAX_CUSTOM_ECC_NUM); i++)
    {
        if (pstEccNumInfo->ulEccNumCount >= MN_CALL_MAX_EMC_NUM)
        {
            break;
        }

        pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].ulMcc
            = pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ulMcc;

        pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].ucCategory
            = pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ucCategory;

        pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].ucValidSimPresent
            = pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ucValidSimPresent;

        pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].ucEccNumLen
            = pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ucEccNumLen;

        VOS_MemCpy(pstEccNumInfo->astCustomEccNumList[pstEccNumInfo->ulEccNumCount].aucEccNum,
                   pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].aucEccNum,
                   pstCustomCallEmerNumCtx->stCustomEccNumList.astCustomEccNumList[i].ucEccNumLen);

        pstEccNumInfo->ulEccNumCount++;

    }


}

/* 1. Buffer����������XCallProcAppŲ��XCallCtx.c��
   2. Cont DTMF��BURST DTMF��Ϣ����һ������
*/

VOS_UINT8 TAF_XCALL_GetDtmfBufCnt(VOS_UINT8 ucCallId)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    return pstCallEntity->stDtmfBuffer.ucDtmfCnt;
}


VOS_VOID TAF_XCALL_IncrementDtmfBufCnt(VOS_UINT8   ucCallId)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->stDtmfBuffer.ucDtmfCnt++;
    return;
}


VOS_VOID TAF_XCALL_DecrementDtmfBufCnt(VOS_UINT8   ucCallId)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->stDtmfBuffer.ucDtmfCnt--;
    return;
}


HI_LIST_S* TAF_XCALL_GetDtmfBuffListHead(VOS_UINT8   ucCallId)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(ucCallId);

    return &(pstCallEntity->stDtmfBuffer.stList);
}



VOS_VOID TAF_XCALL_DelDtmfNode(
    TAF_XCALL_DTMF_NODE_STRU     *pstNode,
    VOS_UINT8                     ucCallId
)
{
    /* �ӹ��˱���ɾ���ڵ� */
    msp_list_del(&pstNode->stList);

    /* �ͷŽڵ��ڴ� */
    PS_MEM_FREE(WUEPS_PID_TAF, pstNode);

    TAF_XCALL_DecrementDtmfBufCnt(ucCallId);

    return;
}

TAF_CALL_SEND_BURST_DTMF_CNF_RESULT_ENUM_U8 TAF_XCALL_AddBurstDtmfInfo(
    TAF_CALL_BURST_DTMF_REQ_MSG_STRU   *pstBurstDtmfInfo
)
{
    /*lint -e429 -e830*/
    TAF_XCALL_DTMF_NODE_STRU           *pstNode       = VOS_NULL_PTR;
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(pstBurstDtmfInfo->stBurstDTMFPara.ucCallId);

    /* ������˱�ڵ��ڴ� */
    pstNode = (TAF_XCALL_DTMF_NODE_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(TAF_XCALL_DTMF_NODE_STRU));
    if (VOS_NULL_PTR == pstNode)
    {
        return TAF_CALL_SEND_BURST_DTMF_CNF_RESULT_ALLOC_MEM_FAIL;
    }

    PS_MEM_CPY(&pstNode->unDtmf.stBurstDtmf, &pstBurstDtmfInfo->stBurstDTMFPara, sizeof(TAF_CALL_BURST_DTMF_PARA_STRU));
    pstNode->enMsgType = TAF_CALL_APP_SEND_BURST_DTMF_REQ;

    /* ���ڵ����ӵ�DTMF���������� */
    msp_list_add_tail(&pstNode->stList, &pstCallEntity->stDtmfBuffer.stList);

    return TAF_CALL_SEND_BURST_DTMF_CNF_RESULT_SUCCESS;
    /*lint +e429 +e830*/

}

TAF_CALL_SEND_BURST_DTMF_CNF_RESULT_ENUM_U8 TAF_XCALL_SaveBurstDtmfInfo(
    TAF_CALL_BURST_DTMF_REQ_MSG_STRU   *pstBurstDtmfInfo
)
{
    TAF_CALL_SEND_BURST_DTMF_CNF_RESULT_ENUM_U8             enResult;
    VOS_UINT8                                               ucDtmfCnt;

    /* �жϻ����Ƿ��Ѿ����� */
    ucDtmfCnt = TAF_XCALL_GetDtmfBufCnt(pstBurstDtmfInfo->stBurstDTMFPara.ucCallId);
    if (TAF_XCALL_DTMF_BUF_MAX_CNT <= ucDtmfCnt)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_XCALL_SaveBurstDtmfInfo: DTMF Buffer is full ");
        return TAF_CALL_SEND_BURST_DTMF_CNF_RESULT_BUFFER_FULL;
    }

    /* ��BURST DTMF��Ϣ���浽������ */
    enResult = TAF_XCALL_AddBurstDtmfInfo(pstBurstDtmfInfo);
    if (TAF_CALL_SEND_BURST_DTMF_CNF_RESULT_SUCCESS != enResult)
    {
        return enResult;
    }

    /* ��DTMF�Ļ��������һ */
    TAF_XCALL_IncrementDtmfBufCnt(pstBurstDtmfInfo->stBurstDTMFPara.ucCallId);

    return TAF_CALL_SEND_BURST_DTMF_CNF_RESULT_SUCCESS;
}


VOS_VOID TAF_XCALL_CleanDtmfBuff(VOS_UINT8   ucCallId)
{
    HI_LIST_S                          *pstMe       = VOS_NULL_PTR;
    HI_LIST_S                          *pstTmp      = VOS_NULL_PTR;
    HI_LIST_S                          *pstHead     = VOS_NULL_PTR;
    TAF_XCALL_DTMF_NODE_STRU           *pstNode     = VOS_NULL_PTR;
    TAF_XCALL_CALL_ENTITY_STRU         *pstXcallCtx = VOS_NULL_PTR;

   /* ���û�л��棬��ֱ���˳� */
    if (0 == TAF_XCALL_GetDtmfBufCnt(ucCallId))
    {
        return;
    }

    /* �ͷŻ����ڴ� */
    /*lint -e961*/
    /* ѭ���������˱����ͷŹ��˱�����нڵ� */
    pstHead = TAF_XCALL_GetDtmfBuffListHead(ucCallId);
    msp_list_for_each_safe(pstMe, pstTmp, pstHead)
    /*lint +e961*/
    {
        /*lint -e961*/
        pstNode = msp_list_entry(pstMe, TAF_XCALL_DTMF_NODE_STRU, stList);
        /*lint +e961*/

        if (TAF_CALL_APP_SEND_BURST_DTMF_REQ == pstNode->enMsgType)
        {
            pstXcallCtx = TAF_XCALL_GetCallEntityAddr(pstNode->unDtmf.stBurstDtmf.ucCallId);

            /* ����ʱ����Ҫ�������е�����BURST DTMF��Ϣ�ظ���ʽ��Ӧ */
            TAF_XCALL_SndBurstDTMFRslt(pstXcallCtx->stClientInfo.usClientId,
                                       pstXcallCtx->stClientInfo.ucOpId,
                                       VOS_ERR);
        }
        else if (TAF_CALL_APP_SEND_CONT_DTMF_REQ == pstNode->enMsgType)
        {
            pstXcallCtx = TAF_XCALL_GetCallEntityAddr(pstNode->unDtmf.stContDtmf.ucCallId);

            /* ����ʱ����Ҫ�������е�����CONT DTMF��Ϣ�ظ���ʽ��Ӧ */
            TAF_XCALL_SndContDTMFRslt(pstXcallCtx->stClientInfo.usClientId,
                                      pstXcallCtx->stClientInfo.ucOpId,
                                      VOS_ERR);
        }
        else /* Ŀǰֻ����CONT DTMF��BURST DTMF,�������ʹ��� */
        {
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_XCALL_CleanDtmfBuff: Node MsgType Error ", pstNode->enMsgType);
        }

        /* �ӹ��˱���ɾ���ڵ� */
        TAF_XCALL_DelDtmfNode(pstNode, ucCallId);
    }

    return;
}


VOS_VOID TAF_XCALL_ProcDtmfBuffer(VOS_UINT8 ucCallId)
{
    TAF_XCALL_DTMF_NODE_STRU           *pstNode             = VOS_NULL_PTR;
    TAF_CALL_BURST_DTMF_PARA_STRU       stBurstDtmfCurInfo;
    TAF_CALL_CONT_DTMF_PARA_STRU        stContDtmfCurInfo;
    TAF_XCALL_CALL_ENTITY_STRU         *pstXcallCtx;
    HI_LIST_S                          *pstMe       = VOS_NULL_PTR;
    HI_LIST_S                          *pstTmp      = VOS_NULL_PTR;
    HI_LIST_S                          *pstHead     = VOS_NULL_PTR;

    PS_MEM_SET(&stBurstDtmfCurInfo, 0x0, sizeof(TAF_CALL_BURST_DTMF_PARA_STRU));
    PS_MEM_SET(&stContDtmfCurInfo, 0x0, sizeof(TAF_CALL_CONT_DTMF_PARA_STRU));

    /* ���û�л��棬��ֱ���˳� */
    if (0 == TAF_XCALL_GetDtmfBufCnt(ucCallId))
    {
        return;
    }

    /* ����账��DTMF���棬��״̬��ΪACTIVE�����建�� */
    if (MN_CALL_S_ACTIVE != TAF_XCALL_GetCallState(ucCallId))
    {
        /* ��DTMF���� */
        TAF_XCALL_CleanDtmfBuff(ucCallId);
        return;
     }

    /*lint -e961*/
    /* ѭ���������˱����ͷŹ��˱�����нڵ� */
    pstHead = TAF_XCALL_GetDtmfBuffListHead(ucCallId);

    msp_list_for_each_safe(pstMe, pstTmp, pstHead)
    /*lint +e961*/
    {
        /*lint -e961*/
        pstNode     = msp_list_entry(pstMe, TAF_XCALL_DTMF_NODE_STRU, stList);
        /*lint +e961*/

        /* ��һ��NODE������BURST DTMF��CONT DTMF���������ͽ��벻ͬ�����֧ */
        if (TAF_CALL_APP_SEND_BURST_DTMF_REQ == pstNode->enMsgType)
        {
            /* �������callid ��һ�£���������� */
            if(ucCallId != pstNode->unDtmf.stBurstDtmf.ucCallId)
            {
                /* ͬһCallʵ���µĻ��������б������Ϣ��call id�ض���ͬ�������ͬ�������쳣 */
                TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_XCALL_ProcDtmfBuffer: Call ID is not equal ");

                pstXcallCtx = TAF_XCALL_GetCallEntityAddr(pstNode->unDtmf.stBurstDtmf.ucCallId);
                TAF_XCALL_SndBurstDTMFRslt(pstXcallCtx->stClientInfo.usClientId,
                                          pstXcallCtx->stClientInfo.ucOpId,
                                          VOS_ERR);

                TAF_XCALL_DelDtmfNode(pstNode, pstNode->unDtmf.stBurstDtmf.ucCallId);
                continue;
            }

            PS_MEM_CPY(&stBurstDtmfCurInfo, &pstNode->unDtmf.stBurstDtmf, sizeof(TAF_CALL_BURST_DTMF_PARA_STRU));

            /* �ӹ��˱���ɾ���ڵ� */
            TAF_XCALL_DelDtmfNode(pstNode, pstNode->unDtmf.stBurstDtmf.ucCallId);

            if (TAF_CALL_SEND_BURST_DTMF_CNF_RESULT_SUCCESS == TAF_XCALL_SndXccBurstDTMFReq(&stBurstDtmfCurInfo))
            {
                /* ���ͳɹ������ñ�ʶλΪVOS_TRUE,ֱ���˳�ѭ�� */
                TAF_XCALL_SetWaitForXCCDTMFCnfFlag(stBurstDtmfCurInfo.ucCallId, TAF_XCALL_WAIT_XCC_CNF_BURST_DTMF);
                break;
            }

            /* ����ʧ�ܣ���call id��Ӧ��ʵ���л�ȡ��Ϣ���ظ�ʧ�ܵĽ��������������һ������ */
            pstXcallCtx = TAF_XCALL_GetCallEntityAddr(stBurstDtmfCurInfo.ucCallId);

            TAF_XCALL_SndBurstDTMFRslt(pstXcallCtx->stClientInfo.usClientId,
                                       pstXcallCtx->stClientInfo.ucOpId,
                                       VOS_ERR);
        }
        else if (TAF_CALL_APP_SEND_CONT_DTMF_REQ == pstNode->enMsgType)
        {
            /* �������callid ��һ�£���������� */
            if (ucCallId != pstNode->unDtmf.stContDtmf.ucCallId)
            {
                /* ͬһCallʵ���µĻ��������б������Ϣ��call id�ض���ͬ�������ͬ�������쳣 */
                TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_XCALL_ProcDtmfBuffer: Call ID is not equal ");

                pstXcallCtx = TAF_XCALL_GetCallEntityAddr(pstNode->unDtmf.stContDtmf.ucCallId);
                TAF_XCALL_SndContDTMFRslt(pstXcallCtx->stClientInfo.usClientId,
                                          pstXcallCtx->stClientInfo.ucOpId,
                                          VOS_ERR);

                TAF_XCALL_DelDtmfNode(pstNode, pstNode->unDtmf.stContDtmf.ucCallId);

                continue;
            }

            PS_MEM_CPY(&stContDtmfCurInfo, &pstNode->unDtmf.stContDtmf, sizeof(TAF_CALL_CONT_DTMF_PARA_STRU));

            /* �ӹ��˱���ɾ���ڵ� */
            TAF_XCALL_DelDtmfNode(pstNode, pstNode->unDtmf.stContDtmf.ucCallId);

            if (TAF_CALL_SEND_CONT_DTMF_CNF_RESULT_SUCCESS == TAF_XCALL_SndXccContDTMFReq(&stContDtmfCurInfo))
            {
                /* ���ͳɹ������ñ�ʶλΪVOS_TRUE,ֱ���˳�ѭ�� */
                if (TAF_CALL_CONT_DTMF_START == stContDtmfCurInfo.enSwitch)
                {
                    TAF_XCALL_SetWaitForXCCDTMFCnfFlag(stContDtmfCurInfo.ucCallId, TAF_XCALL_WAIT_XCC_CNF_START_CONT_DTMF);
                }
                else
                {
                    TAF_XCALL_SetWaitForXCCDTMFCnfFlag(stContDtmfCurInfo.ucCallId, TAF_XCALL_WAIT_XCC_CNF_STOP_CONT_DTMF);
                }

                break;
            }

            /* ����ʧ�ܣ���call id��Ӧ��ʵ���л�ȡ��Ϣ���ظ�ʧ�ܵĽ��������������һ������ */
            pstXcallCtx = TAF_XCALL_GetCallEntityAddr(stContDtmfCurInfo.ucCallId);

            TAF_XCALL_SndContDTMFRslt(pstXcallCtx->stClientInfo.usClientId,
                                      pstXcallCtx->stClientInfo.ucOpId,
                                      VOS_ERR);
        }
        else
        {
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_XCALL_ProcBurstDtmfBuffer: Node MsgType Error ", pstNode->enMsgType);

            TAF_XCALL_DelDtmfNode(pstNode, pstNode->unDtmf.stContDtmf.ucCallId);
        }
    }

    return;
}


TAF_CALL_SEND_CONT_DTMF_CNF_RESULT_ENUM_U8 TAF_XCALL_AddContDtmfInfo(
    TAF_CALL_CONT_DTMF_REQ_MSG_STRU    *pstContDtmfInfo
)
{
    /*lint -e429 -e830*/
    TAF_XCALL_DTMF_NODE_STRU           *pstNode       = VOS_NULL_PTR;
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    pstCallEntity = TAF_XCALL_GetCallEntityAddr(pstContDtmfInfo->stContDTMFPara.ucCallId);

    /* ������˱�ڵ��ڴ� */
    pstNode = (TAF_XCALL_DTMF_NODE_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF, sizeof(TAF_XCALL_DTMF_NODE_STRU));
    if (VOS_NULL_PTR == pstNode)
    {
        return TAF_CALL_SEND_CONT_DTMF_CNF_RESULT_ALLOC_MEM_FAIL;
    }

    PS_MEM_CPY(&pstNode->unDtmf.stContDtmf, &pstContDtmfInfo->stContDTMFPara, sizeof(TAF_CALL_CONT_DTMF_PARA_STRU));
    pstNode->enMsgType = TAF_CALL_APP_SEND_CONT_DTMF_REQ;

    /* ���ڵ����ӵ� DTMF���������� */
    msp_list_add_tail(&pstNode->stList, &pstCallEntity->stDtmfBuffer.stList);

    return TAF_CALL_SEND_CONT_DTMF_CNF_RESULT_SUCCESS;
    /*lint +e429 +e830*/
}


TAF_CALL_SEND_CONT_DTMF_CNF_RESULT_ENUM_U8 TAF_XCALL_SaveContDtmfInfo(
    TAF_CALL_CONT_DTMF_REQ_MSG_STRU    *pstContDtmfInfo
)
{
    TAF_CALL_SEND_CONT_DTMF_CNF_RESULT_ENUM_U8              enResult;
    VOS_UINT8                                               ucDtmfCnt;

    /* �жϻ����Ƿ��Ѿ����� */
    ucDtmfCnt = TAF_XCALL_GetDtmfBufCnt(pstContDtmfInfo->stContDTMFPara.ucCallId);
    if (TAF_XCALL_DTMF_BUF_MAX_CNT <= ucDtmfCnt)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF, "TAF_XCALL_SaveContDtmfInfo: DTMF Buffer is full ");
        return TAF_CALL_SEND_CONT_DTMF_CNF_RESULT_BUFFER_FULL;
    }

    /* ��CONT DTMF��Ϣ���浽������ */
    enResult = TAF_XCALL_AddContDtmfInfo(pstContDtmfInfo);
    if (TAF_CALL_SEND_CONT_DTMF_CNF_RESULT_SUCCESS != enResult)
    {
        return enResult;
    }

    /* ��CONT DTMF�Ļ��������һ */
    TAF_XCALL_IncrementDtmfBufCnt(pstContDtmfInfo->stContDTMFPara.ucCallId);

    return TAF_CALL_SEND_CONT_DTMF_CNF_RESULT_SUCCESS;
}



VOS_UINT32 TAF_XCALL_GetStartAndStopContDtmfIntervalLen(VOS_VOID)
{
    return TAF_XCALL_GetConfigInfo()->ulStartAndStopContDtmfIntervalLen;
}



VOS_UINT32 TAF_XCALL_GetAllUsedCallId(
    VOS_UINT8                          *pucCallIds
)
{
    TAF_XCALL_CTX_STRU                 *pstXcallCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulCallIdNum;
    VOS_UINT32                          i;

    ulCallIdNum = 0;

    pstXcallCtx = TAF_XCALL_GetCtxAddr();

    for (i = 0; i < TAF_XCALL_CALL_ENTITY_MAX_NUM; i++)
    {
        if (VOS_TRUE == pstXcallCtx->astCallEntity[i].ulUsed)
        {
            *pucCallIds++ = (VOS_UINT8)(i + 1);
            ulCallIdNum++;
        }
    }

    return ulCallIdNum;
}


VOS_VOID TAF_XCALL_SetCsCallExistFlg(
    VOS_UINT8                           ucCallExitFlg,
    MN_CALL_TYPE_ENUM_U8                enCallType
)
{
    /* �޵绰���϶��޽����� */
    if (VOS_FALSE == ucCallExitFlg)
    {
        TAF_SDC_SetCsCallExistFlg(VOS_FALSE);
        TAF_SDC_SetCsEccExistFlg(VOS_FALSE);

        TAF_INFO_LOG(WUEPS_PID_TAF, "TAF_XCALL_SetCsCallExistFlg:Set Call And Ecc Call Exist Flg To False!");

        return;
    }

    /* �е绰������call type���ý�������� */
    TAF_SDC_SetCsCallExistFlg(VOS_TRUE);

    if (MN_CALL_TYPE_EMERGENCY == enCallType)
    {
        TAF_SDC_SetCsEccExistFlg(VOS_TRUE);
    }
    else
    {
        TAF_SDC_SetCsEccExistFlg(VOS_FALSE);
    }

    TAF_INFO_LOG1(WUEPS_PID_TAF, "TAF_XCALL_SetCsCallExistFlg:CallType:", (VOS_UINT32)enCallType);

    return;
}


#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT)

VOS_VOID TAF_XCALL_InitEncVoiceCtx(VOS_VOID)
{
    TAF_XCALL_InitEncVoiceKeyNegotiateCtx();

    TAF_XCALL_InitEncVoiceRemoteCtrlCtx();

    TAF_XCALL_InitEncVoiceKmcUpdateCtx();

    TAF_XCALL_InitEncVoiceOpIdMgmtCtx();

    TAF_XCALL_InitEncVoiceParaInfoCtx();

    TAF_XCALL_CipherEccInit();
}


VOS_VOID TAF_XCALL_InitEncVoiceKeyNegotiateCtx(VOS_VOID)
{
    TAF_XCALL_KS_NEGOTIATE_CTX         *pstKsNegoCtx = VOS_NULL_PTR;

    pstKsNegoCtx = TAF_XCALL_GetKsNegotiateCtxAddr();

    PS_MEM_SET(pstKsNegoCtx, 0, sizeof(TAF_XCALL_KS_NEGOTIATE_CTX));

    pstKsNegoCtx->enVoiceEncState      = TAF_XCALL_VOICE_ENCRYPT_STA_INACTIVE;

    pstKsNegoCtx->stCcaCtrl.usClientId = MN_GetRealClientId(MN_CLIENT_ID_BROADCAST, WUEPS_PID_TAF);
    pstKsNegoCtx->stCcaCtrl.ulModuleId = WUEPS_PID_AT;
    pstKsNegoCtx->stCcaCtrl.ucOpId     = 0;
}


VOS_VOID TAF_XCALL_InitEncVoiceRemoteCtrlCtx(VOS_VOID)
{
    TAF_XCALL_REMOTE_CTRL_CTX         *pstRemoteCtrlCtx = VOS_NULL_PTR;

    pstRemoteCtrlCtx = TAF_XCALL_GetRemoteCtrlCtxAddr();

    PS_MEM_SET(pstRemoteCtrlCtx, 0, sizeof(TAF_XCALL_REMOTE_CTRL_CTX));

    pstRemoteCtrlCtx->enRemoteCtrlState    = TAF_XCALL_REMOTE_CTRL_STA_INACTIVE;
    pstRemoteCtrlCtx->enRemoteCtrlCmdType  = TAF_XCALL_KMC_MSG_ROMOTE_CTRL_CMD_TYPE_BUTT;

    pstRemoteCtrlCtx->stCcaCtrl.usClientId = MN_GetRealClientId(MN_CLIENT_ID_BROADCAST, WUEPS_PID_TAF);
    pstRemoteCtrlCtx->stCcaCtrl.ulModuleId = WUEPS_PID_AT;
    pstRemoteCtrlCtx->stCcaCtrl.ucOpId     = 0;
}


VOS_VOID TAF_XCALL_InitEncVoiceOpIdMgmtCtx(VOS_VOID)
{
    TAF_XCALL_XSMS_SEND_REQ_OPID_MGMT_CTX                  *pstOpIdMgmtCtx = VOS_NULL_PTR;

    pstOpIdMgmtCtx = TAF_XCALL_GetOpIdMgmtCtxAddr();

    PS_MEM_SET(pstOpIdMgmtCtx, 0, sizeof(TAF_XCALL_XSMS_SEND_REQ_OPID_MGMT_CTX));
}


VOS_VOID TAF_XCALL_InitEncVoiceParaInfoCtx(VOS_VOID)
{
    TAF_XCALL_ENCRYPT_VOICE_PARA_INFO_CTX                  *pstEncVoiceParaInfo = VOS_NULL_PTR;

    pstEncVoiceParaInfo = TAF_XCALL_GetEncVoiceInfoCtxAddr();

    PS_MEM_SET(pstEncVoiceParaInfo, 0, sizeof(TAF_XCALL_ENCRYPT_VOICE_PARA_INFO_CTX));

    pstEncVoiceParaInfo->enTestMode = TAF_CALL_APP_SET_EC_TEST_MODE_DISABLE;
}


TAF_XCALL_ENCRYPT_VOICE_CTX_STRU* TAF_XCALL_GetVoiceEncryptCtxAddr(VOS_VOID)
{
    return &(TAF_XCALL_GetCtxAddr()->stEncVoiceCtx);
}


TAF_XCALL_XSMS_SEND_REQ_OPID_MGMT_CTX* TAF_XCALL_GetOpIdMgmtCtxAddr(VOS_VOID)
{
    return &(TAF_XCALL_GetVoiceEncryptCtxAddr()->stOpIdMgmtCtx);
}



TAF_XCALL_REMOTE_CTRL_CTX* TAF_XCALL_GetRemoteCtrlCtxAddr(VOS_VOID)
{
    return &(TAF_XCALL_GetVoiceEncryptCtxAddr()->stRemoteCtrlCtx);
}


TAF_XCALL_KS_NEGOTIATE_CTX* TAF_XCALL_GetKsNegotiateCtxAddr(VOS_VOID)
{
    return &(TAF_XCALL_GetVoiceEncryptCtxAddr()->stNegotiateCtx);
}


TAF_XCALL_ENCRYPT_VOICE_PARA_INFO_CTX* TAF_XCALL_GetEncVoiceInfoCtxAddr(VOS_VOID)
{
    return &(TAF_XCALL_GetVoiceEncryptCtxAddr()->stEncVoiceInfoCtx);
}



TAF_XCALL_VOICE_ENCRYPT_STA_ENUM_UINT32 TAF_XCALL_GetVoiceEncryptNegotiateState(VOS_VOID)
{
    return TAF_XCALL_GetVoiceEncryptCtxAddr()->stNegotiateCtx.enVoiceEncState;
}


TAF_XCALL_ECC_PUB_KEY_INFO_STRU* TAF_XCALL_GetVoiceEncryptPublicKeyAndVersionNumber( VOS_VOID )
{
    return &(TAF_XCALL_GetCtxAddr()->stEncVoiceCtx.stEncVoiceInfoCtx.stEccPubKeyInfo);
}


TAF_XCALL_ECC_SRV_CAP_INFO_STRU* TAF_XCALL_GetVoiceEncryptCapCfgPara( VOS_VOID )
{
    return &(TAF_XCALL_GetCtxAddr()->stEncVoiceCtx.stEncVoiceInfoCtx.stEccSrvCapInfo);
}


TAF_STD_TIME_ZONE_TYPE_STRU* TAF_XCALL_GetVoiceEncryptSecInfoEraseSysTime( VOS_VOID )
{
    return &(TAF_XCALL_GetCtxAddr()->stEncVoiceCtx.stEncVoiceInfoCtx.stLastInfoEraseTime);
}


TAF_STD_TIME_ZONE_TYPE_STRU* TAF_XCALL_GetVoiceEncryptPassWordResetSysTime( VOS_VOID )
{
    return &(TAF_XCALL_GetCtxAddr()->stEncVoiceCtx.stEncVoiceInfoCtx.stLastPassworResetTime);
}


VOS_UINT16 TAF_XCALL_GetVoiceEncryptReqDelaySendTimerLen( VOS_VOID )
{
    return TAF_XCALL_GetCtxAddr()->stEncVoiceCtx.stEncVoiceInfoCtx.usDelayEncVoiceReqTimerLen;
}


VOS_UINT16 TAF_XCALL_GetRemoteCtrlWaitApAnswerTimerLen(VOS_VOID)
{
    return TAF_XCALL_GetCtxAddr()->stEncVoiceCtx.stEncVoiceInfoCtx.usRemoteCtrlAnswerTimerLen;
}


TAF_CALL_APP_SET_EC_TEST_MODE_ENUM_UINT32 TAF_XCALL_GetVoiceEncryptTestMode( VOS_VOID )
{
    return TAF_XCALL_GetCtxAddr()->stEncVoiceCtx.stEncVoiceInfoCtx.enTestMode;
}


TAF_XCALL_PUBLIC_KEY_UPDATE_CTX* TAF_XCALL_GetPubKeyUpdateCtxAddr( VOS_VOID )
{
    return &(TAF_XCALL_GetCtxAddr()->stEncVoiceCtx.stPubKeyUpdateCtx);
}


VOS_VOID TAF_XCALL_InitEncVoiceKmcUpdateCtx(VOS_VOID)
{
    TAF_XCALL_PUBLIC_KEY_UPDATE_CTX    *pstPubKeyUpdateCtx = VOS_NULL_PTR;

    pstPubKeyUpdateCtx = TAF_XCALL_GetPubKeyUpdateCtxAddr();

    PS_MEM_SET(pstPubKeyUpdateCtx, 0, sizeof(TAF_XCALL_PUBLIC_KEY_UPDATE_CTX));

    pstPubKeyUpdateCtx->enK0UpdateState = TAF_XCALL_K0_UPATE_STA_INACTIVE;
}


#endif


VOS_VOID TAF_XCALL_UpdatePrivacyMode(
    VOS_UINT8                           ucCallId,
    TAF_CALL_PRIVACY_MODE_ENUM_UINT8    enPrivacyMode
)
{
    TAF_XCALL_CALL_ENTITY_STRU         *pstCallEntity = VOS_NULL_PTR;

    if (TAF_CALL_PRIVACY_MODE_BUTT == enPrivacyMode)
    {
        return;
    }

    pstCallEntity                       = TAF_XCALL_GetCallEntityAddr(ucCallId);

    pstCallEntity->enPrivacyMode        = enPrivacyMode;

    return;
}

/*lint -restore*/
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */





