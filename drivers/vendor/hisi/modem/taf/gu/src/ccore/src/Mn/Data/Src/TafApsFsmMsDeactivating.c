


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "TafTypeDef.h"
#include "TafApsApi.h"
#include "Taf_Aps.h"
#include "TafLog.h"
#include "Taf_Aps.h"
#include "TafApsCtx.h"
#include "TafApsFsmMainTbl.h"
#include "TafApsFsmMsDeactivating.h"
#include "TafApsFsmMsDeactivatingTbl.h"
#include "TafApsProcIpFilter.h"
#include "TafApsSndSm.h"
#include "TafApsSndAt.h"
#include "TafApsSndInternalMsg.h"
#include "TafFsm.h"
#if (FEATURE_ON == FEATURE_LTE)
#include "SmEsmInterface.h"
#include "MnApsMultiMode.h"
#include "ApsL4aInterface.h"
#include "TafApsSndL4a.h"
#include "MmcApsInterface.h"
#include "TafSmInterface.h"
#include "TafApsComFunc.h"
#include "TafApsSndEsm.h"
#endif
#include "ApsNdInterface.h"
#include "TafApsSndNd.h"
#include "MnApsComm.h"
#include "TafApsComFunc.h"
#include "TafSdcCtx.h"
#include "TafAppMma.h"
#include "TafApsSndEhsm.h"
#include "TafSdcLib.h"


#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/*lint -e958*/

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_TAF_APS_FSM_MSDEACTIVATING_C

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/

/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
*****************************************************************************/


/******************************************************************************
   5 函数实现
******************************************************************************/

VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpCtxStateReq = VOS_NULL_PTR;
    TAF_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;
    VOS_UINT8                           ucPdpId;
#if (FEATURE_ON == FEATURE_DSDS)
    TAF_MMA_SRV_ACQ_RAT_LIST_STRU       stSrvAcqRatList;

    PS_MEM_SET(&stSrvAcqRatList, 0, sizeof(TAF_MMA_SRV_ACQ_RAT_LIST_STRU));
#endif

    pstSetPdpCtxStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU *)TAF_PS_GET_MSG_CONTENT(pstMsg);
    enCurrRatType        = TAF_APS_GetCurrPdpEntityRatType();
    ucPdpId              = TAF_APS_GetCurrFsmEntityPdpId();

    /* 上报PDP去激活请求响应事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstSetPdpCtxStateReq->stCtrl), TAF_PS_CAUSE_SUCCESS);

#if (FEATURE_ON == FEATURE_DSDS)
    /* area lost no RF场景, 触发搜网*/
    if (VOS_FALSE == TAF_SDC_Get3GppRfAvailFlag())
    {
        stSrvAcqRatList.ucRatNum      = 1;
        stSrvAcqRatList.aenRatType[0] = TAF_MMA_SRV_ACQ_RAT_TYPE_GUL;

        TAF_MMA_SrvAcqReq(TAF_MMA_SRV_TYPE_PS_SUBSCRIB_TRAFFIC_CALL, &stSrvAcqRatList);
    }
#endif

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    switch (enCurrRatType)
    {
        case TAF_APS_RAT_TYPE_GSM:
        case TAF_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case TAF_APS_RAT_TYPE_LTE:
            if (VOS_FALSE == TAF_APS_IsPdnCntxtTeardownAllowed(TAF_APS_GetPdpEntInfoAddr(ucPdpId)))
            {
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
            }
            else
            {
                TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
                TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_LteMode();
            }
            break;
#endif

        case TAF_APS_RAT_TYPE_NULL:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND);

            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);

            break;

        default:
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_Init,enCurrRatType", enCurrRatType);
            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq = VOS_NULL_PTR;
    TAF_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;
    VOS_UINT8                           ucPdpId;
#if (FEATURE_ON == FEATURE_DSDS)
    TAF_MMA_SRV_ACQ_RAT_LIST_STRU       stSrvAcqRatList;

    PS_MEM_SET(&stSrvAcqRatList, 0, sizeof(TAF_MMA_SRV_ACQ_RAT_LIST_STRU));
#endif

    pstCallEndReq = (TAF_PS_CALL_END_REQ_STRU *)TAF_PS_GET_MSG_CONTENT(pstMsg);
    enCurrRatType = TAF_APS_GetCurrPdpEntityRatType();
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 上报PS呼叫挂断响应事件 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl), pstCallEndReq->ucCid, TAF_PS_CAUSE_SUCCESS);

#if (FEATURE_ON == FEATURE_DSDS)
    /* area lost no RF场景, 触发搜网*/
    if (VOS_FALSE == TAF_SDC_Get3GppRfAvailFlag())
    {
        stSrvAcqRatList.ucRatNum      = 1;
        stSrvAcqRatList.aenRatType[0] = TAF_MMA_SRV_ACQ_RAT_TYPE_GUL;

        TAF_MMA_SrvAcqReq(TAF_MMA_SRV_TYPE_PS_SUBSCRIB_TRAFFIC_CALL, &stSrvAcqRatList);
    }
#endif

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    switch (enCurrRatType)
    {
        case TAF_APS_RAT_TYPE_GSM:
        case TAF_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvAtPsCallEndReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case TAF_APS_RAT_TYPE_LTE:
            if (VOS_FALSE == TAF_APS_IsPdnCntxtTeardownAllowed(TAF_APS_GetPdpEntInfoAddr(ucPdpId)))
            {
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
            }
            else
            {
                TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode();
            }
            break;
#endif

        case TAF_APS_RAT_TYPE_NULL:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND);

            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);

            break;

        default:
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvAtPsCallEndReq_MsDeactivating_Init,enCurrRatType", enCurrRatType);
            break;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg = VOS_NULL_PTR;
    TAF_APS_RAT_TYPE_ENUM_UINT32            enCurrRatType;
    VOS_UINT8                               ucPdpId;

    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstMsg;
    enCurrRatType  = TAF_APS_GetCurrPdpEntityRatType();
    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    switch (enCurrRatType)
    {
        case TAF_APS_RAT_TYPE_GSM:
        case TAF_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode(pstInternalMsg->enCause);
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case TAF_APS_RAT_TYPE_LTE:
            if (VOS_FALSE == TAF_APS_IsPdnCntxtTeardownAllowed(TAF_APS_GetPdpEntInfoAddr(ucPdpId)))
            {
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
            }
            else
            {
                TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
                TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_LteMode();
            }
            break;
#endif

        case TAF_APS_RAT_TYPE_NULL:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND);

            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);

            break;

        default:
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_Init,enCurrRatType", enCurrRatType);
            break;
    }

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_RAT_TYPE_ENUM_UINT32        enCurrRatType;

    /*获取网络模式*/
    enCurrRatType                       = TAF_APS_GetCurrPdpEntityRatType();

    /* 保存子状态机的入口消息 */
    TAF_APS_SaveCurrSubFsmEntryMsg(ulEventType, pstMsg);

    switch (enCurrRatType)
    {
        case TAF_APS_RAT_TYPE_GSM:
        case TAF_APS_RAT_TYPE_WCDMA:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_GuMode();
            break;

#if (FEATURE_ON == FEATURE_LTE)
        case TAF_APS_RAT_TYPE_LTE:
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_MS_DEACTIVATING);
            TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_LteMode();
            break;
#endif

        default:
            TAF_WARNING_LOG1(WUEPS_PID_TAF, "TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_Init,enCurrRatType", enCurrRatType);
            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvInterServiceStatusChangeInd_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU           *pstServStaChangeInd = VOS_NULL_PTR;
    TAF_APS_ENTRY_MSG_STRU                                 *pstEntryMsg;
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU                  *pstInternalMsg;
#if (FEATURE_ON == FEATURE_IPV6)
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
#endif
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                               ucPdpId;
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg         = TAF_APS_GetCurrSubFsmMsgAddr();
    pstServStaChangeInd = (TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU*)pstMsg;
#if (FEATURE_ON == FEATURE_LTE)
    ucPdpId             = TAF_APS_GetCurrFsmEntityPdpId();
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity           = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 如果地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_RAT_TYPE_NULL != pstServStaChangeInd->enRatType)
      && (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        if (VOS_TRUE != pstPdpEntity->ulNdClientActiveFlg)
        {
            TAF_APS_SndNdPdpActInd(pstPdpEntity->ucNsapi,
                                   pstPdpEntity->PdpAddr.aucIpV6Addr);

            pstPdpEntity->ulNdClientActiveFlg = VOS_TRUE;
        }
    }
#endif


    /* 正常流程下，不应该有这个场景，此处仍然尝试去激活操作 */
    if ( (TAF_APS_RAT_TYPE_GSM   == pstServStaChangeInd->enRatType)
      || (TAF_APS_RAT_TYPE_WCDMA == pstServStaChangeInd->enRatType))
    {
        switch(pstEntryMsg->ulEventType)
        {
            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ):
                TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ):
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ):
                pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU*)pstEntryMsg->aucEntryMsgBuffer;
                TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode(pstInternalMsg->enCause);
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnfSuspend:Wrong Msg Entry");
                break;
        }
    }
#if (FEATURE_ON == FEATURE_LTE)
    else
    {
        if (TAF_APS_RAT_TYPE_LTE == pstServStaChangeInd->enRatType)
        {
            switch(pstEntryMsg->ulEventType)
            {
                case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ):
                    if (VOS_FALSE == TAF_APS_IsPdnCntxtTeardownAllowed(TAF_APS_GetPdpEntInfoAddr(ucPdpId)))
                    {
                        TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
                    }
                    else
                    {
                        TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_LteMode();
                    }
                    break;

                case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ):
                    if (VOS_FALSE == TAF_APS_IsPdnCntxtTeardownAllowed(TAF_APS_GetPdpEntInfoAddr(ucPdpId)))
                    {
                        TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
                    }
                    else
                    {
                        TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode();
                    }
                    break;

                case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ):
                    TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_LteMode();
                    break;

                default:
                    TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnfSuspend:Wrong Msg Entry");
                    break;
            }
        }
    }
#endif


#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if (TAF_APS_RAT_TYPE_EHRPD == pstServStaChangeInd->enRatType)
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg = TAF_APS_GetCurrSubFsmMsgAddr();

        if ((pstEntryMsg->ulEventType == TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ))
         || (pstEntryMsg->ulEventType == TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ)))
        {
            /* 主状态迁移至TAF_APS_STA_ACTIVE */
            TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);

            /* 退出子状态机 */
            TAF_APS_QuitCurrSubFsm();

            /* 重新将去激活请求发送出来，后续进入3GPP2状态机处理 */
            TAF_APS_SndInterFsmEntryMsg(pstEntryMsg);
        }
    }
    else if (TAF_APS_RAT_TYPE_1X == pstServStaChangeInd->enRatType)
    {
        /* 1X模式下，本地去激活, hrpd下由EHSM进行本地去激活 */
        /* 本地去激活，参考定时器超时的处理 */
        TAF_APS_ProcLocDeactBear_MsDeactivating_LteMode();
    }
    else
    {

    }
#endif


    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq   = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_OPERATION_CONFLICT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateInd_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND);
    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    TAF_APS_EPS_BEARER_INFO_IND_STRU   *pstBearerInfo;
    VOS_UINT8                           ucPdpId;

    pstBearerInfo = (TAF_APS_EPS_BEARER_INFO_IND_STRU*)pstMsg;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity  = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 处理EPS承载去激活指示 */
    if (SM_ESM_PDP_OPT_DEACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptDeactivate(pstPdpEntity, pstBearerInfo);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_Suspend: Wrong option.");
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvEsmPdpDeactivateInd_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(TAF_APS_GetPdpEntNsapi(ucPdpId));

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 如果SNDCP已经激活, 释放SNDCP资源 */
    if (APS_SNDCP_ACT == pstPdpEntity->PdpProcTrack.ucSNDCPActOrNot)
    {
        Aps_ReleaseSndcpResource(ucPdpId);
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
#endif


VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_Suspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity   = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 同步PDP状态&信息给ESM */
#if (FEATURE_ON == FEATURE_LTE)
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if (VOS_TRUE == TAF_SDC_IsConfigCLMode())
    {
        /* 通知EHSM清除LTE同步的PDN信息 */
        TAF_APS_SndEhsmLocDeactNtf(ucPdpId);
    }
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);
    }

    /* 向SM发送本地去激活请求 */
    TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);

    /* 本地去激活与当前缺省承载关联的专用承载 */
    TAF_APS_SndLocalAllSecPdpDeactivateInd(ucPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);

    /* 认为本地去激活，释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_OPERATION_CONFLICT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateCnf_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 释放SNDCP资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);
    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 如果去激活的是主PDP,挂接在下面的从PDP都需要本地去激活 */
        TAF_APS_SndLocalAllSecPdpDeactivateInd(ucPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);

    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvApsLocalPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);

    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvInterServiceStatusChangeInd_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU           *pstServStaChangeInd = VOS_NULL_PTR;

    /* 初始化, 获取消息内容 */
    pstServStaChangeInd = (TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU*)pstMsg;

    if (TAF_APS_RAT_TYPE_NULL == pstServStaChangeInd->enRatType)
    {
        /* 迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF_SUSPEND */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF_SUSPEND);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvInterServiceStatusChangeInd_MsDeactivating_WaitSmDeactivateCnf: Wrong RatType");
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitSmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示, 并等待SNDCP的响应 */
    if ( (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);
    }

    /* 向SM发送本地去激活请求 */
    TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);

    /* 本地去激活与当前缺省承载关联的专用承载 */
    TAF_APS_SndLocalAllSecPdpDeactivateInd(ucPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();
    return VOS_TRUE;

}




VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_OPERATION_CONFLICT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateCnf_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity                        = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示 */
    if ( (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);
    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvSmPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示 */
    if ( (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);

    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvApsLocalPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                            SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 如果已经激活SNDCP, 需要向SNDCP发送去激活指示 */
    if ( (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrackFlag)
      && (VOS_TRUE == g_PdpEntity[ucPdpId].PdpProcTrack.ucSNDCPActOrNot) )
    {
        /* 向SNDCP发送去激活指示失败, 直接上报PDP去激活成功事件, 释放资源 */
        Aps_ReleaseSndcpResource(ucPdpId);

        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);

    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    TAF_APS_EPS_BEARER_INFO_IND_STRU   *pstBearerInfo;
    VOS_UINT8                           ucPdpId;

    pstBearerInfo = (TAF_APS_EPS_BEARER_INFO_IND_STRU*)pstMsg;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity  = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 处理EPS承载去激活指示 */
    if (SM_ESM_PDP_OPT_DEACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptDeactivate(pstPdpEntity, pstBearerInfo);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitSmDeactivateCnfSuspend: Wrong option.");
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvEsmPdpDeactivateInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(TAF_APS_GetPdpEntNsapi(ucPdpId));

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 如果SNDCP已经激活, 释放SNDCP资源 */
    if (APS_SNDCP_ACT == pstPdpEntity->PdpProcTrack.ucSNDCPActOrNot)
    {
        Aps_ReleaseSndcpResource(ucPdpId);
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}
#endif


VOS_UINT32 TAF_APS_RcvInterServiceStaChangeInd_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU           *pstServStaChangeInd = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                                               ucPdpId;
    TAF_APS_ENTRY_MSG_STRU                                 *pstEntryMsg;

    ucPdpId               = TAF_APS_GetCurrFsmEntityPdpId();
#endif

    /* 初始化, 获取消息内容 */
    pstServStaChangeInd = (TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU*)pstMsg;

    if ( (TAF_APS_RAT_TYPE_GSM   == pstServStaChangeInd->enRatType)
      || (TAF_APS_RAT_TYPE_WCDMA == pstServStaChangeInd->enRatType))
    {

        /*GU模下 ,不需要处理,继续等SM的回复,状态回到
          TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF
        */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF);
    }
#if (FEATURE_ON == FEATURE_LTE)
    else if (TAF_APS_RAT_TYPE_LTE == pstServStaChangeInd->enRatType)
    {
        TAF_APS_SndSmPdpAbortReq(ucPdpId);

        /* 初始化, 获取消息内容 */
        pstEntryMsg                         = TAF_APS_GetCurrSubFsmMsgAddr();

        switch(pstEntryMsg->ulEventType)
        {
            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ):
                if (VOS_FALSE == TAF_APS_IsPdnCntxtTeardownAllowed(TAF_APS_GetPdpEntInfoAddr(ucPdpId)))
                {
                    TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
                }
                else
                {
                    TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_LteMode();
                }
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ):
                if (VOS_FALSE == TAF_APS_IsPdnCntxtTeardownAllowed(TAF_APS_GetPdpEntInfoAddr(ucPdpId)))
                {
                    TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef();
                }
                else
                {
                    TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode();
                }
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ):
                TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_LteMode();
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvMmcServiceStatusInd_MsDeactivating_WaitL4aDeactivateCnfSuspend:Wrong Msg Entry");
                break;

        }
    }
    else
    {
        ;
    }
#endif

    return VOS_TRUE;

}



VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitSmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

#if (FEATURE_ON == FEATURE_LTE)
    /* 同步PDP信息 */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                             SM_ESM_PDP_OPT_DEACTIVATE);
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 向SM发送本地去激活请求 */
    TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);

    /* 本地去激活与当前缺省承载关联的专用承载 */
    TAF_APS_SndLocalAllSecPdpDeactivateInd(ucPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);

    /* 认为本地去激活，释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}



VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                          pstCallOrigReq->stDialParaInfo.ucCid,
                          TAF_PS_CAUSE_OPERATION_CONFLICT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}



VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSndcpDeactivateRsp_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvInterServiceStatusChangeInd_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU           *pstServStaChangeInd = VOS_NULL_PTR;

    /* 初始化, 获取消息内容 */
    pstServStaChangeInd    = (TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU*)pstMsg;

    /* 挂起指示，此时应该是迁移子状态到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND */
    if (TAF_APS_RAT_TYPE_NULL == pstServStaChangeInd->enRatType)
    {
        /* 迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF_SUSPEND);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvInterServiceStatusChangeInd_MsDeactivating_WaitSndcpDeactivateRsp: Wrong RatType");
    }

    return VOS_TRUE;

}



VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitSndcpDeactivateRsp(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 认为本地去激活 */

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 认为本地去激活，释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl),
                                TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl),
                             TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                           pstCallOrigReq->stDialParaInfo.ucCid,
                           TAF_PS_CAUSE_OPERATION_CONFLICT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvSndcpDeactivateRsp_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvInterServiceStaChgInd_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU                  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST                              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                                 *pstEntryMsg;
    TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU           *pstServStaChangeInd;
    VOS_UINT8                               ucPdpId;


    ucPdpId         = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity    = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 初始化, 获取消息内容 */
    pstServStaChangeInd = (TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU*)pstMsg;

    if ( TAF_APS_RAT_TYPE_GSM == pstServStaChangeInd->enRatType )
    {
        /*G模下 ,不需要处理,继续等SM的回复,状态回到
          TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF
        */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SNDCP_DEACTIVATE_CNF);
    }

    /* WCDMA和LTE情况下，认为去激活已经成功，进行上报 */
    if ( ( TAF_APS_RAT_TYPE_WCDMA == pstServStaChangeInd->enRatType )
#if (FEATURE_ON == FEATURE_LTE)
       ||( TAF_APS_RAT_TYPE_LTE == pstServStaChangeInd->enRatType )
#endif
       )
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 认为本地去激活，释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 配置IP过滤器 */
        TAF_APS_IpfConfigUlFilter(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitSndcpDeactivateRspSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId        = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity   = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 认为本地去激活，释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                           pstCallOrigReq->stDialParaInfo.ucCid,
                           TAF_PS_CAUSE_OPERATION_CONFLICT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvEsmPdpReleaseCnf_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APP_ESM_PDP_RELEASE_CNF_STRU       *pstEsmReleaseCnf;

    pstEsmReleaseCnf = (APP_ESM_PDP_RELEASE_CNF_STRU*)pstMsg;

    if(APP_SUCCESS == pstEsmReleaseCnf->ulRslt)
    {
        return TAF_APS_RcvEsmPdpDeactivateCnf_MsDeactivating_WaitEsmDeactivateCnf(ulEventType, pstMsg);
    }
    else
    {
        return TAF_APS_RcvEsmPdpDeactivateRej_MsDeactivating_WaitEsmDeactivateCnf(ulEventType, pstMsg);
    }
}



VOS_UINT32 TAF_APS_RcvEsmPdpDeactivateCnf_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvEsmPdpDeactivateRej_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    APP_ESM_PDP_RELEASE_CNF_STRU           *pstPdpRej;
    VOS_UINT8                               ucPdpId;

    /* 当前LTE下有可能会出现去激活失败，目前来说，如果收到L下的去激活失败，直接
       上报去激活成功，对应的状态清除 */

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取APS实体地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);


    pstPdpRej = (APP_ESM_PDP_RELEASE_CNF_STRU*)pstMsg;
    if (APP_ERR_SM_NW_LAST_PDN_DISCONN_NOT_ALLOWED == pstPdpRej->ulRslt)
    {
        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 如果是最后PDP去激活，L模会拒绝，根据此特定原因值进行特殊处理，此时
           该实体不会实际去激活，但是会刷新成默认承载，实体状态保持为激活状态 */


        /* 刷新成默认承载的CID */
        TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                    TAF_APS_DEFAULT_CID,
                                    TAF_APS_INVALID_MODULEID,
                                    TAF_APS_INVALID_CLIENTID,
                                    TAF_APS_INVALID_OPID);

        TAF_APS_SetPdpEntAttachBearerFlag(ucPdpId, VOS_TRUE);

        /* 同步PDP信息至ESM */
        MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_MODIFY);

        /* 同步PDP信息至SM */
        if (VOS_TRUE == pstPdpEntity->bitOpTransId)
        {
            MN_APS_SndSmBearerActivateInd(pstPdpEntity);
        }

        /* 需要将对应去激活PDP下的专有承载的信息同步给SM，
           因为这个时候SM已经将这些清除掉 */
        MN_APS_SndSmAllSecPdpActivateInd(pstPdpEntity);

        /* 主状态迁移至TAF_APS_STA_ACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }
    else
    {
        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 认为本地去激活 */

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                             SM_ESM_PDP_OPT_DEACTIVATE);

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if (VOS_TRUE == TAF_SDC_IsConfigCLMode())
    {
        /* 通知EHSM清除LTE同步的PDN信息 */
        TAF_APS_SndEhsmLocDeactNtf(ucPdpId);
    }
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    if (VOS_TRUE == pstPdpEntity->bitOpTransId)
    {
        /* 向SM发送本地去激活请求 */
        TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);
    }

    /* 本地去激活与当前缺省承载关联的专用承载 */
    TAF_APS_SndLocalAllSecPdpDeactivateInd(ucPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvInterServiceStatusChangeInd_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU           *pstServStaChangeInd = VOS_NULL_PTR;

    /* 初始化, 获取消息内容 */
    pstServStaChangeInd = (TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU*)pstMsg;

    if (TAF_APS_RAT_TYPE_NULL == pstServStaChangeInd->enRatType)
    {
        /* 迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF_SUSPEND */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF_SUSPEND);
    }
    else
    {
        /* 此时收到网络指示，认为是一条异常消息 */
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvInterServiceStatusChangeInd_MsDeactivating_WaitEsmDeactivateCnf: err msg");
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitEsmDeactivateCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    TAF_APS_EPS_BEARER_INFO_IND_STRU   *pstBearerInfo;

    pstBearerInfo = (TAF_APS_EPS_BEARER_INFO_IND_STRU*)pstMsg;
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);


    /* 根据EPS承载的操作类型(Activate, Deactivate, Modify),
       调用相应的处理函数 */
    if (SM_ESM_PDP_OPT_DEACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptDeactivate(pstPdpEntity, pstBearerInfo);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitL4aDeactivateCnfSuspend: Wrong option.");
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstPdpContextStateReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg             = (TAF_PS_MSG_STRU*)pstMsg;
    pstPdpContextStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 再激活或去激活该APS实体所用的CID,上报ERROR事件 */
    TAF_APS_SndSetPdpCtxStateCnf(&(pstPdpContextStateReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPppDailOrigReq_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg         = (TAF_PS_MSG_STRU*)pstMsg;
    pstPppDialOrigReq = (TAF_PS_PPP_DIAL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndPppDialOrigCnf(&(pstPppDialOrigReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallOrigReq_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg       = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallOrigReq  = (TAF_PS_CALL_ORIG_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 激活该APS实体所用的CID, 返回ERROR */
    TAF_APS_SndCallOrigCnf(&(pstCallOrigReq->stCtrl),
                           pstCallOrigReq->stDialParaInfo.ucCid,
                           TAF_PS_CAUSE_OPERATION_CONFLICT);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallEndReq_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取消息内容 */
    pstAppMsg               = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallEndReq           = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 继承原有断开流程 */
    TAF_APS_SndCallEndCnf(&(pstCallEndReq->stCtrl),
                          pstCallEndReq->ucCid,
                          TAF_PS_CAUSE_SUCCESS);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallModifyReq_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallModifyReq = (TAF_PS_CALL_MODIFY_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 修改该APS实体, 返回ERROR */
    TAF_APS_SndCallModifyCnf(&(pstCallModifyReq->stCtrl), TAF_PS_CAUSE_CID_INVALID);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallAnswerReq_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallAnswerReq = (TAF_PS_CALL_ANSWER_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 应答MT呼叫, 返回ERROR */
    TAF_APS_SndCallAnswerCnf(&(pstCallAnswerReq->stCtrl),
                            pstCallAnswerReq->stAnsInfo.ucCid,
                            TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_WaitL4aDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 初始化, 获取消息内容 */
    pstAppMsg        = (TAF_PS_MSG_STRU*)pstMsg;
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 正在去激活该APS实体, 挂断MT呼叫, 返回ERROR */
    TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl), TAF_ERR_ERROR);

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvEsmPdpReleaseCnf_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APP_ESM_PDP_RELEASE_CNF_STRU       *pstEsmReleaseCnf;

    pstEsmReleaseCnf = (APP_ESM_PDP_RELEASE_CNF_STRU*)pstMsg;

    if(APP_SUCCESS == pstEsmReleaseCnf->ulRslt)
    {
        return TAF_APS_RcvEsmPdpDeactivateCnf_MsDeactivating_WaitEsmDeactivateCnfSuspend(ulEventType, pstMsg);
    }
    else
    {
        return TAF_APS_RcvEsmPdpDeactivateRej_MsDeactivating_WaitEsmDeactivateCnfSuspend(ulEventType, pstMsg);
    }
}



VOS_UINT32 TAF_APS_RcvEsmPdpDeactivateCnf_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}



VOS_UINT32 TAF_APS_RcvEsmPdpDeactivateRej_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    APP_ESM_PDP_RELEASE_CNF_STRU           *pstPdpRej;
    VOS_UINT8                               ucPdpId;

    /* 当前LTE下有可能会出现去激活失败，目前来说，如果收到L下的去激活失败，直接
       上报去激活成功，对应的状态清除 */

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取APS实体地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);


    pstPdpRej = (APP_ESM_PDP_RELEASE_CNF_STRU*)pstMsg;

    /* 当前L4A与APS之间需要定义好错误码的值目前还未定义，待陈晓军这两天确认 */
    /* 如果是因为挂起导致的去激活失败，解挂后需要重新发起去激活操作，目前临时
       用一个值来代替 */
    if (APP_ERR_SM_SUSPENDED == pstPdpRej->ulRslt)
    {
        /*
          迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND，解挂后再次发起去激活操作
        */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_SUSPEND);

        return VOS_TRUE;
    }
    else if (APP_ERR_SM_NW_LAST_PDN_DISCONN_NOT_ALLOWED == pstPdpRej->ulRslt)
    {
        /* 如果是最后PDP去激活，L模会拒绝，根据此特定原因值进行特殊处理，此时
           该实体不会实际去激活，但是会刷新成默认承载，实体状态保持为激活状态 */

        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 刷新成默认承载的CID */
        TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                    TAF_APS_DEFAULT_CID,
                                    TAF_APS_INVALID_MODULEID,
                                    TAF_APS_INVALID_CLIENTID,
                                    TAF_APS_INVALID_OPID);

        TAF_APS_SetPdpEntAttachBearerFlag(ucPdpId, VOS_TRUE);

        /* 同步PDP信息至ESM */
        MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_MODIFY);

        /* 同步PDP信息至SM */
        if (VOS_TRUE == pstPdpEntity->bitOpTransId)
        {
            MN_APS_SndSmBearerActivateInd(pstPdpEntity);
        }

        /* 需要将对应去激活PDP下的专有承载的信息同步给SM，
           因为这个时候SM已经将这些清除掉 */
        MN_APS_SndSmAllSecPdpActivateInd(pstPdpEntity);

        /* 主状态迁移至TAF_APS_STA_ACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();
    }
    else
    {
        /* 停保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

        /* 获取当前的状态机入口消息和EVENTTYPE */
        pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
        pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

        /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
        if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
        {
            /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
            TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
        }
        else
        {
            if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
            {
                TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
            }
        }

        /* 释放资源 */
        Aps_ReleaseApsResource(ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();

    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvEsmPdpDeactivateInd_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    /* 初始化, 获取消息内容 */
    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 如果SNDCP已经激活, 释放SNDCP资源 */
    if (APS_SNDCP_ACT == pstPdpEntity->PdpProcTrack.ucSNDCPActOrNot)
    {
        Aps_ReleaseSndcpResource(ucPdpId);
    }

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvInterServiceStaChgInd_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU           *pstServStaChangeInd = VOS_NULL_PTR;
    TAF_APS_ENTRY_MSG_STRU                                 *pstEntryMsg;
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU                  *pstInternalMsg;

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg = TAF_APS_GetCurrSubFsmMsgAddr();

    /* 初始化, 获取消息内容 */
    pstServStaChangeInd = (TAF_APS_INTER_SERVICE_STATUS_CHANGE_IND_STRU*)pstMsg;

    /* 正常流程下，不应该有这个场景，此处仍然尝试去激活操作 */
    if ( (TAF_APS_RAT_TYPE_GSM   == pstServStaChangeInd->enRatType)
      || (TAF_APS_RAT_TYPE_WCDMA == pstServStaChangeInd->enRatType))
    {
        switch(pstEntryMsg->ulEventType)
        {
            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ):
                TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ):
                TAF_APS_RcvAtPsCallEndReq_MsDeactivating_GuMode(SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);
                break;

            case TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ):
                pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU*)pstEntryMsg->aucEntryMsgBuffer;
                TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode(pstInternalMsg->enCause);
                break;

            default:
                TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvInterServiceStaChgInd_MsDeactivating_WaitEsmDeactivateCnfSuspend:Wrong Msg Entry");
                break;
        }
    }
    if (TAF_APS_RAT_TYPE_LTE == pstServStaChangeInd->enRatType)
    {
        /* 迁移子状态机到TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF);
    }

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if (TAF_APS_RAT_TYPE_1X == pstServStaChangeInd->enRatType)
    {
        /* 本地去激活，参考定时器超时的处理 */
        TAF_APS_ProcLocDeactBear_MsDeactivating_LteMode();
    }
#endif

    else
    {
         TAF_WARNING_LOG(WUEPS_PID_TAF, "TAF_APS_RcvInterServiceStaChgInd_MsDeactivating_WaitEsmDeactivateCnfSuspend: Wrong Msg");
    }

    return VOS_TRUE;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 TAF_APS_RcvEhsmLteHandoverToEhrpdInd_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    EHSM_APS_LTE_HANDOVER_TO_EHRPD_IND_STRU                *pstLteHandoverInd;
    TAF_APS_ENTRY_MSG_STRU                                 *pstEntryMsg;
    VOS_UINT8                                               ucPdpId;

    pstLteHandoverInd = (EHSM_APS_LTE_HANDOVER_TO_EHRPD_IND_STRU *)pstMsg;
    ucPdpId           = TAF_APS_GetCurrFsmEntityPdpId();

    if (EHSM_APS_PDN_OPTION_MODIFIED == pstLteHandoverInd->enPdnOption)
    {
        TAF_APS_ProcModifyReq_LteHandoverToEhrpdScene(ucPdpId, pstLteHandoverInd);
    }

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg = TAF_APS_GetCurrSubFsmMsgAddr();

    /* TO DO: 其它入口消息 */
    if (pstEntryMsg->ulEventType == TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_CALL_END_REQ))
    {
        /* 主状态迁移至TAF_APS_STA_ACTIVE */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);

        /* 退出子状态机 */
        TAF_APS_QuitCurrSubFsm();

        /* 重新将去激活请求发送出来，后续进入3GPP2状态机处理 */
        TAF_APS_SndInterFsmEntryMsg(pstEntryMsg);
    }

    /* 发送内部消息，其它正在激活过程中的状态机需要处理此消息 */
    TAF_APS_SndInterAttachBearerActivateInd();

    return VOS_TRUE;
}
#endif


VOS_UINT32 TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucPdpId;
    APS_PDP_CONTEXT_ENTITY_ST          *pstPdpEntity;
    TAF_APS_EPS_BEARER_INFO_IND_STRU   *pstBearerInfo;

    pstBearerInfo = (TAF_APS_EPS_BEARER_INFO_IND_STRU*)pstMsg;
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);


    /* 根据EPS承载的操作类型(Activate, Deactivate, Modify),
       调用相应的处理函数 */
    if (SM_ESM_PDP_OPT_DEACTIVATE == pstBearerInfo->enPdpOption)
    {
        MN_APS_ProcEsmBearerInfoIndOptDeactivate(pstPdpEntity, pstBearerInfo);
    }
    else
    {
        TAF_WARNING_LOG(WUEPS_PID_TAF,"TAF_APS_RcvEsmSmEpsBearerInfoInd_MsDeactivating_WaitEsmDeactivateCnfSuspend: Wrong option.");
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_APS_RcvTiMsDeactivatingExpired_MsDeactivating_WaitEsmDeactivateCnfSuspend(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg;
    VOS_UINT8                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 认为本地去激活 */

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(g_PdpEntity[ucPdpId].ucNsapi);

    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                             SM_ESM_PDP_OPT_DEACTIVATE);

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    if (VOS_TRUE == pstPdpEntity->bitOpTransId)
    {
        /* 向SM发送本地去激活请求 */
        TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);
    }

    /* 本地去激活与当前缺省承载关联的专用承载 */
    TAF_APS_SndLocalAllSecPdpDeactivateInd(ucPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return VOS_TRUE;

}

#endif


VOS_VOID TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode(
    SM_TAF_CAUSE_ENUM_UINT16            enCause
)
{
    VOS_UINT8                           ucErrCode;
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;

    /* 初始化 */
    ucErrCode = TAF_ERR_NO_ERROR;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId   = TAF_APS_GetCurrFsmEntityPdpId();

    /* 发起PDP去激活请求 */
    Aps_PsCallEnd(enCause, ucPdpId, &ucErrCode);

    if (TAF_ERR_NO_ERROR != ucErrCode)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_GuMode: End call failed!");
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF);

        /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                               ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);
        }
    }

    return;
}


VOS_VOID TAF_APS_RcvAtPsCallEndReq_MsDeactivating_GuMode(
    SM_TAF_CAUSE_ENUM_UINT16            enCause
)
{
    VOS_UINT8                           ucErrCode;
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;

    /* 初始化 */
    ucErrCode = TAF_ERR_NO_ERROR;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId   = TAF_APS_GetCurrFsmEntityPdpId();

    /* 发起PDP去激活请求 */
    Aps_PsCallEnd(enCause, ucPdpId, &ucErrCode);

    if (TAF_ERR_NO_ERROR != ucErrCode)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvAtCallEndReq_MsActivating_GuMode: End call failed!");
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF);

        /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                               ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);
        }
    }

    return;
}


VOS_VOID TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode(
    SM_TAF_CAUSE_ENUM_UINT16            enCause
)
{
    VOS_UINT8                           ucErrCode;
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;

    /* 初始化 */
    ucErrCode = TAF_ERR_NO_ERROR;

    /* 获取当前状态机的APS实体索引 */
    ucPdpId   = TAF_APS_GetCurrFsmEntityPdpId();

    /* 发起PDP去激活请求 */
    Aps_PsCallEnd(enCause, ucPdpId, &ucErrCode);

    if (TAF_ERR_NO_ERROR != ucErrCode)
    {
        TAF_ERROR_LOG(WUEPS_PID_TAF,
            "TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_GuMode: End call failed!");
    }
    else
    {
        /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF */
        TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_SM_DEACTIVATE_CNF);

        /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
        enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                               ucPdpId);
        if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
        {
            /* 起保护定时器 */
            TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                               TI_TAF_APS_MS_DEACTIVATING_LEN,
                               ucPdpId);
        }
    }

    return;
}


VOS_VOID TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_GuMode(VOS_VOID)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangupReq;

    /* 获取缓存消息内容 */
    pstEntryMsg      = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg        = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstCallHangupReq = (TAF_PS_CALL_HANGUP_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取当前状态机的APS实体索引 */
    ucPdpId          = TAF_APS_GetCurrFsmEntityPdpId();

    /* 检查PS域卡状态, 如果无效, 直接返回ERROR */
    if (VOS_FALSE == TAF_APS_GetCurrPdpEntitySimRegStatus())
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_NET_ACTIVATING,
                          ucPdpId);

        /* 上报PDP操作结果 */
        TAF_APS_SndCallHangupCnf(&(pstCallHangupReq->stCtrl),
                                 TAF_PS_CAUSE_SIM_INVALID);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();

        return;
    }

    /* 网络激活手动应答中",则该消息实际为: APP拒绝网络发起的激活
       发送内部激活消息，状态不迁移，在本状态机中处理
    */
    Aps_ActIndRejRsp(ucPdpId);

    /* 停止激活流程定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_NET_ACTIVATING,
                      ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
    TAF_APS_QuitCurrSubFsm();

    return;
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_VOID TAF_APS_RcvAtSetPdpContextStateReq_MsDeactivating_LteMode(VOS_VOID)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpCtxStateReq;

    /* 获取缓存消息内容 */
    pstEntryMsg          = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg            = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstSetPdpCtxStateReq = (TAF_PS_SET_PDP_STATE_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取当前状态机的APS实体索引 */
    ucPdpId              = TAF_APS_GetCurrFsmEntityPdpId();

    /* 向ESM发送去激活EPS承载 */
    TAF_APS_SndEsmSetCgactMsgDistr(ucPdpId, pstSetPdpCtxStateReq);

    /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF);

    /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                           ucPdpId);
    if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
    {
        /* 起保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                           TI_TAF_APS_MS_DEACTIVATING_LEN,
                           ucPdpId);
    }

    return;
}


VOS_VOID TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode(VOS_VOID)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    TAF_APS_ENTRY_MSG_STRU             *pstEntryMsg;
    TAF_PS_MSG_STRU                    *pstAppMsg;
    TAF_PS_CALL_END_REQ_STRU           *pstCallEndReq;

    /* 获取缓存消息内容 */
    pstEntryMsg   = TAF_APS_GetCurrSubFsmMsgAddr();
    pstAppMsg     = (TAF_PS_MSG_STRU*)(pstEntryMsg->aucEntryMsgBuffer);
    pstCallEndReq = (TAF_PS_CALL_END_REQ_STRU*)(pstAppMsg->aucContent);

    /* 获取当前状态机的APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 向ESM发送ID_APP_ESM_PDP_RELEASE_REQ_REQ消息, 去激活EPS承载 */
    TAF_APS_SndEsmPdpReleaseReq(pstCallEndReq);

    /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF);

    /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                           ucPdpId);
    if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
    {
        /* 起保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                           TI_TAF_APS_MS_DEACTIVATING_LEN,
                           ucPdpId);
    }

    return;
}


VOS_VOID TAF_APS_RcvApsInternalPdpDeavtivateReq_MsDeactivating_LteMode(VOS_VOID)
{
    VOS_UINT8                           ucPdpId;
    TAF_APS_TIMER_STATUS_ENUM_U8        enTimerStatus;
    TAF_PS_CALL_END_REQ_STRU            stCallEndReq;

    PS_MEM_SET(&stCallEndReq, 0, sizeof(TAF_PS_CALL_END_REQ_STRU));
    /* 获取当前状态机的APS实体索引 */
    ucPdpId                           = TAF_APS_GetCurrFsmEntityPdpId();

    /* 构造断开拨号参数 */
    stCallEndReq.ucCid             = TAF_APS_GetPdpEntCurrCid(ucPdpId);
    stCallEndReq.stCtrl.ulModuleId = TAF_APS_GetPdpEntModuleId(ucPdpId, stCallEndReq.ucCid);
    stCallEndReq.stCtrl.usClientId = TAF_APS_GetPdpEntClientId(ucPdpId, stCallEndReq.ucCid );
    stCallEndReq.stCtrl.ucOpId     = TAF_APS_GetPdpEntOpId(ucPdpId, stCallEndReq.ucCid);

    /* 向ESM发送ID_APP_ESM_PDP_RELEASE_REQ_REQ消息, 去激活EPS承载 */
    TAF_APS_SndEsmPdpReleaseReq(&stCallEndReq);

    /* 子状态迁移至TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF */
    TAF_APS_SetCurrPdpEntitySubFsmState(TAF_APS_MS_DEACTIVATING_SUBSTA_WAIT_ESM_DEACTIVATE_CNF);

    /* 检查定时器是否正在运行, 如果正在运行, 则不需要重新启动 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                           ucPdpId);
    if (TAF_APS_TIMER_STATUS_STOP == enTimerStatus)
    {
        /* 起保护定时器 */
        TAF_APS_StartTimer(TI_TAF_APS_MS_DEACTIVATING,
                           TI_TAF_APS_MS_DEACTIVATING_LEN,
                           ucPdpId);
    }

    return;
}

VOS_VOID TAF_APS_RcvAtPsCallHangupReq_MsDeactivating_LteMode( VOS_VOID )
{
    VOS_UINT8                           ucPdpId;

    /*获取PDP ID*/
    ucPdpId                             = TAF_APS_GetCurrFsmEntityPdpId();

    /* 检查PS域卡状态, 如果无效, 直接返回ERROR */
    if (VOS_FALSE == TAF_APS_GetCurrPdpEntitySimRegStatus())
    {
        /* 停止激活流程定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_NET_ACTIVATING,
                          ucPdpId);

        /* 主状态迁移至TAF_APS_STA_INACTIVE, 退出子状态机 */
        TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);
        TAF_APS_QuitCurrSubFsm();

        return;
    }

    /*向LTE发送挂断请求，L模保证挂断不会失败*/
    TAF_APS_SndEsmPdpManagerRsp(APS_ESM_BEARER_ACT_REJ, 0);

    /* 停保护定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_NET_ACTIVATING, ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return;
}


VOS_VOID TAF_APS_RcvAtPsCallEndReq_MsDeactivating_LteMode_ReserveDef(VOS_VOID)
{
    VOS_UINT8                               ucPdpId;
    TAF_APS_ENTRY_MSG_STRU                 *pstEntryMsg     = VOS_NULL_PTR;
    APS_PDP_CONTEXT_ENTITY_ST              *pstPdpEntity    = VOS_NULL_PTR;
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU  *pstInternalMsg  = VOS_NULL_PTR;
    TAF_APS_TIMER_STATUS_ENUM_U8            enTimerStatus;

    /* 获取缓存消息内容 */
    pstEntryMsg   = TAF_APS_GetCurrSubFsmMsgAddr();

    /* 获取当前状态机的APS实体索引 */
    ucPdpId       = TAF_APS_GetCurrFsmEntityPdpId();

    /* 获取APS实体地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(pstPdpEntity->ucNsapi);

    /* 检查定时器是否正在运行, 如果正在运行, 则需要停止定时器 */
    enTimerStatus = TAF_APS_GetTimerStatus(TI_TAF_APS_MS_DEACTIVATING,
                                           ucPdpId);
    if (TAF_APS_TIMER_STATUS_RUNING == enTimerStatus)
    {
        /* 停止保护定时器 */
        TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING,
                          ucPdpId);
    }

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 刷新成默认承载的CID */
    TAF_APS_SetPdpEntClientInfo(ucPdpId,
                                TAF_APS_DEFAULT_CID,
                                TAF_APS_INVALID_MODULEID,
                                TAF_APS_INVALID_CLIENTID,
                                TAF_APS_INVALID_OPID);

    /* 同步PDP信息至ESM */
    MN_APS_SndEsmPdpInfoInd(pstPdpEntity, SM_ESM_PDP_OPT_MODIFY);

    /* 同步PDP信息至SM */
    if (VOS_TRUE == pstPdpEntity->bitOpTransId)
    {
        MN_APS_SndSmBearerActivateInd(pstPdpEntity);
    }

    /* 主状态迁移至TAF_APS_STA_ACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_ACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

    return;
}


VOS_VOID TAF_APS_ProcLocDeactBear_MsDeactivating_LteMode(VOS_VOID)
{
    TAF_APS_ENTRY_MSG_STRU                                 *pstEntryMsg;
    TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU                  *pstInternalMsg;
    APS_PDP_CONTEXT_ENTITY_ST                              *pstPdpEntity;
    VOS_UINT8                                               ucPdpId;

    ucPdpId      = TAF_APS_GetCurrFsmEntityPdpId();
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    /* 停止定时器 */
    TAF_APS_StopTimer(TI_TAF_APS_MS_DEACTIVATING, ucPdpId);

    /* 停止流量统计 */
    TAF_APS_StopDsFlowStats(TAF_APS_GetPdpEntNsapi(ucPdpId));

    /* 同步PDP状态&信息给ESM */
    MN_APS_SndEsmPdpInfoInd(TAF_APS_GetPdpEntInfoAddr(ucPdpId),
                             SM_ESM_PDP_OPT_DEACTIVATE);

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if (TAF_APS_RAT_TYPE_1X == TAF_APS_GetCurrPdpEntityRatType())
    {
        /* 通知EHSM清除LTE同步的PDN信息 */
        TAF_APS_SndEhsmLocDeactNtf(ucPdpId);
    }
#endif

#if (FEATURE_ON == FEATURE_IPV6)
    /* 如果是缺省承载且地址类型是IPv6, 需要同步给ND Client */
    if ( (TAF_APS_CheckPrimaryPdp(ucPdpId))
      && (TAF_APS_CheckPdpAddrTypeIpv6(ucPdpId)) )
    {
        TAF_APS_SndNdPdpDeactInd(TAF_APS_GetPdpEntNsapi(ucPdpId));
    }
#endif

    /* 获取当前的状态机入口消息和EVENTTYPE */
    pstEntryMsg    = TAF_APS_GetCurrSubFsmMsgAddr();
    pstInternalMsg = (TAF_APS_INTER_PDP_DEACTIVATE_REQ_STRU *)pstEntryMsg->aucEntryMsgBuffer;

    /* 如果内部触发的PDP去激活，不用上报去激活的事件 */
    if (pstEntryMsg->ulEventType != TAF_BuildEventType(WUEPS_PID_TAF, ID_MSG_TAF_PS_APS_INTERNAL_PDP_DEACTIVATE_REQ))
    {
        /* 上报ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF事件 */
        TAF_APS_SndPdpDeActivateCnf(ucPdpId, TAF_APS_GetPdpEntCurrCid(ucPdpId));
    }
    else
    {
        if (VOS_TRUE == pstPdpEntity->ucInterDeactNotifyFlg)
        {
            TAF_APS_SndPdpDeActivateInd(ucPdpId, TAF_APS_MapSmCause(pstInternalMsg->enCause));
        }
    }

    /* 获取该PDPID的PDP实体内容地址 */
    pstPdpEntity = TAF_APS_GetPdpEntInfoAddr(ucPdpId);

    if (VOS_TRUE == pstPdpEntity->bitOpTransId)
    {
        /* 向SM发送本地去激活请求 */
        TAF_APS_SndSmPdpLocalDeactivateReq(ucPdpId);
    }

    /* 本地去激活与当前缺省承载关联的专用承载 */
    TAF_APS_SndLocalAllSecPdpDeactivateInd(ucPdpId, SM_TAF_CAUSE_SM_NW_REGULAR_DEACTIVATION);

    /* 释放资源 */
    Aps_ReleaseApsResource(ucPdpId);

    /* 配置IP过滤器 */
    TAF_APS_IpfConfigUlFilter(ucPdpId);

    /* 主状态迁移至TAF_APS_STA_INACTIVE */
    TAF_APS_SetCurrPdpEntityMainFsmState(TAF_APS_STA_INACTIVE);

    /* 退出子状态机 */
    TAF_APS_QuitCurrSubFsm();

}

#endif

/*lint +e958*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



