

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "TafXCallProcApp.h"
#include "TafXCallSndApp.h"
#include "TafXCallSndCttf.h"
#include "TafXCallProcNvim.h"
#include "TafXCallEccCipher.h"
#include "MnComm.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_TAF_X_CALL_PROC_APP_ECC_TEST_MODE_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  3 ��������
*****************************************************************************/
#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT)
#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE)

VOS_VOID TAF_XCALL_RcvAppEccTestModeCfgReq(VOS_VOID *pstMsg)
{
    TAF_CALL_APP_SET_EC_TEST_MODE_REQ_STRU                 *pstSetTestModeReq = VOS_NULL_PTR;
    TAF_XCALL_ENCRYPT_VOICE_CTX_STRU                       *pstEncryptVoiceCtx = VOS_NULL_PTR;

    pstSetTestModeReq  = (TAF_CALL_APP_SET_EC_TEST_MODE_REQ_STRU *)pstMsg;

    pstEncryptVoiceCtx = TAF_XCALL_GetVoiceEncryptCtxAddr();

    pstEncryptVoiceCtx->stEncVoiceInfoCtx.enTestMode = pstSetTestModeReq->enEccTestModeStatus;

    TAF_XCALL_SendCttf1xSetEncryptVoiceModeNtf(pstSetTestModeReq->enEccTestModeStatus);

    TAF_XCALL_SndTestModeCfgCnf(&pstSetTestModeReq->stCtrl, VOS_TRUE);

    return;
}


VOS_VOID TAF_XCALL_RcvAppEccTestModeQryReq(VOS_VOID *pstMsg)
{
    TAF_CALL_APP_GET_EC_TEST_MODE_REQ_STRU                 *pstGetTestModeReq = VOS_NULL_PTR;
    TAF_CALL_APP_SET_EC_TEST_MODE_ENUM_UINT32               enTestMode;

    pstGetTestModeReq = (TAF_CALL_APP_GET_EC_TEST_MODE_REQ_STRU *)pstMsg;

    enTestMode = TAF_XCALL_GetVoiceEncryptTestMode();
    if (TAF_CALL_APP_SET_EC_TEST_MODE_ENABLE != enTestMode)
    {
        enTestMode = TAF_CALL_APP_SET_EC_TEST_MODE_DISABLE;
    }

    TAF_XCALL_SndTestModeQryCnf(&pstGetTestModeReq->stCtrl,
                                enTestMode,
                                VOS_TRUE);
    return;
}


VOS_VOID TAF_XCALL_RcvAppEccKmcKeyCfgReq(VOS_VOID *pstMsg)
{
    TAF_CALL_APP_SET_EC_KMC_REQ_STRU                       *pstSetKmcKeyReq = VOS_NULL_PTR;
    TAF_XCALL_ENCRYPT_VOICE_CTX_STRU                       *pstEncryptVoiceCtx = VOS_NULL_PTR;

    pstSetKmcKeyReq    = (TAF_CALL_APP_SET_EC_KMC_REQ_STRU *)pstMsg;

    pstEncryptVoiceCtx = TAF_XCALL_GetVoiceEncryptCtxAddr();

    if (TAF_CALL_ECC_KMC_PUBLIC_KEY_MAX_HEX_LENGTH != pstSetKmcKeyReq->stKmcData.usEccKmcLength )
    {
        TAF_XCALL_SndKmcKeyCfgCnf(&pstSetKmcKeyReq->stCtrl, VOS_FALSE);
        return;
    }

    TAF_XCALL_WriteVoiceEncryptPublicKeyAndVersionNumber(pstSetKmcKeyReq->stKmcData.aucEccKmcData,
                                                         pstSetKmcKeyReq->stKmcData.ucEccKmcVer);

    pstEncryptVoiceCtx->stEncVoiceInfoCtx.stEccPubKeyInfo.ucVerNum = pstSetKmcKeyReq->stKmcData.ucEccKmcVer;

    PS_MEM_CPY(pstEncryptVoiceCtx->stEncVoiceInfoCtx.stEccPubKeyInfo.aucPubKey,
               pstSetKmcKeyReq->stKmcData.aucEccKmcData,
               TAF_XCALL_MIN(pstSetKmcKeyReq->stKmcData.usEccKmcLength, TAF_XCALL_KMC_MSG_MAX_KMC_PUB_KEY_LEN));

    TAF_XCALL_SndKmcKeyCfgCnf(&pstSetKmcKeyReq->stCtrl, VOS_TRUE);
    return;
}


VOS_VOID TAF_XCALL_RcvAppEccKmcKeyQryReq(VOS_VOID *pstMsg)
{
    TAF_CALL_APP_GET_EC_KMC_REQ_STRU                       *pstGetKmcKeyReq = VOS_NULL_PTR;
    TAF_XCALL_ENCRYPT_VOICE_CTX_STRU                       *pstEncryptVoiceCtx = VOS_NULL_PTR;

    pstGetKmcKeyReq    = (TAF_CALL_APP_GET_EC_KMC_REQ_STRU *)pstMsg;

    pstEncryptVoiceCtx = TAF_XCALL_GetVoiceEncryptCtxAddr();

    TAF_XCALL_SndKmcKeyQryCnf(&pstGetKmcKeyReq->stCtrl,
                              &pstEncryptVoiceCtx->stEncVoiceInfoCtx.stEccPubKeyInfo,
                              VOS_TRUE);
    return;
}


VOS_VOID TAF_XCALL_RcvAppEccRandomQryReq(VOS_VOID *pstMsg)
{
    VOS_UINT8                                               i;
    TAF_CALL_APP_GET_EC_RANDOM_REQ_STRU                    *pstGeRandomReq  = VOS_NULL_PTR;
    TAF_CALL_APP_EC_RANDOM_DATA_STRU                       *pstEccRandom    = VOS_NULL_PTR;
    TAF_CALL_APP_EC_RANDOM_DATA_STRU                       *pstEccRandomTmp = VOS_NULL_PTR;
    VOS_UINT8                                               aucRandom[TAF_XCALL_ECC_RANDOM_LEN];
    VOS_UINT32                                              ulResult = VOS_TRUE;

    pstGeRandomReq = (TAF_CALL_APP_GET_EC_RANDOM_REQ_STRU *)pstMsg;



    /* ������Ϣ */
    pstEccRandom = (TAF_CALL_APP_EC_RANDOM_DATA_STRU *)PS_MEM_ALLOC(WUEPS_PID_TAF,
                                                                    sizeof(TAF_CALL_APP_EC_RANDOM_DATA_STRU) * TAF_CALL_APP_EC_RANDOM_NUM);

    if (VOS_NULL_PTR == pstEccRandom)
    {
        TAF_XCALL_SndRandomQryCnf(&pstGeRandomReq->stCtrl,
                                  0,
                                  VOS_NULL_PTR,
                                  VOS_FALSE);
        return;
    }

    PS_MEM_SET((VOS_UINT8 *)pstEccRandom,
                0,
                sizeof(TAF_CALL_APP_EC_RANDOM_DATA_STRU) * TAF_CALL_APP_EC_RANDOM_NUM);

    pstEccRandomTmp = pstEccRandom;
    for (i = 0; i < TAF_CALL_APP_EC_RANDOM_NUM; i++)
    {
        PS_MEM_SET(aucRandom, 0, sizeof(aucRandom));

        if(VOS_TRUE != TAF_XCALL_CipherGetRandom(sizeof(aucRandom), aucRandom))
        {
            ulResult = VOS_FALSE;
            break;
        }

        TAF_XCALL_CipherHex2Ascii(aucRandom,
                                  sizeof(aucRandom),
                                  pstEccRandomTmp->aucEccData,
                                  TAF_CALL_APP_EC_RANDOM_NUM);

        pstEccRandomTmp++;
    }

    TAF_XCALL_SndRandomQryCnf(&pstGeRandomReq->stCtrl,
                              TAF_CALL_APP_EC_RANDOM_NUM,
                              pstEccRandom,
                              ulResult);

    PS_MEM_FREE(WUEPS_PID_TAF, pstEccRandom);

    return;
}

#endif /*FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE*/
#endif /*FEATURE_CHINA_TELECOM_VOICE_ENCRYPT*/
#endif /*FEATURE_UE_MODE_CDMA*/

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
