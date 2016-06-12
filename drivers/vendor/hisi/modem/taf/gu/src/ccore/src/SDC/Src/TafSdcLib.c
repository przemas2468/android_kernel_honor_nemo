

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "PsLib.h"

#include "PsCommonDef.h"
#include "UsimPsInterface.h"
#include "TafSdcLib.h"
#include "zlib.h"
#include "NVIM_Interface.h"

#include "NasUsimmApi.h"

#if (FEATURE_ON == FEATURE_PTM)
#include "NasErrorLog.h"
#endif

#include "TafStdlib.h"
#include "NasMntn.h"
#include "MnComm.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/*****************************************************************************
  2 ��������
*****************************************************************************/

/*lint -e767 �޸���:�޽� 107747;������:���ٻ�65952;ԭ��:Log��ӡ*/
#define THIS_FILE_ID PS_FILE_ID_TAF_SDC_LIB_C
/*lint +e767 �޸���:�޽� 107747;������:sunshaohua*/

/*****************************************************************************
  3 ���Ͷ���
*****************************************************************************/

/*****************************************************************************
  4 ��������
*****************************************************************************/

/*****************************************************************************
  5 ��������
*****************************************************************************/
/* ����AT^REJINFO�������ϱ����ƣ�����һ���ֽڵĵ���������BIT */
/* ����AT^PLMNSELERLST�������ϱ����ƣ�����һ���ֽڵĵ��������BIT */
/* ����^MIPICLK�������ϱ����ƣ����ڵ�һ���ֽڵ����һ��Bit */
/* ����^CTIME�������ϱ����ƣ����ڵ������ֽڵ��������Bit */
/* ����^CBURSTDTMF�������ϱ����ƣ����ڵڰ˸��ֽڵĵ����ڶ���Bit */
/* ����^CCONTDTMF�������ϱ����ƣ����ڵڰ˸��ֽڵ����һ��Bit */
/* ����^SRCHEDPLMNINFO�������ϱ����ƣ����ڵڶ����ֽڵĵڶ���Bit */
/* ����^CLOCINFO�������ϱ����ƣ����ڵڰ˸��ֽڵĵ�����3��Bit */
/* ����^CSID�������ϱ����ƣ����ڵڰ˸��ֽڵĵ�����4��Bit */
/* ���ص��עL4A_RPT_CMD_INDEX_ENUM_UINT32���ô�ʹ�õ�bitλ���ڴ˴������ٴ�ʹ�� */
TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8            g_aenTafSdcCurcRptCmdTable[] =
{
    TAF_SDC_RPT_CMD_MODE,        TAF_SDC_RPT_CMD_RSSI,        TAF_SDC_RPT_CMD_REJINFO,     TAF_SDC_RPT_CMD_SRVST,
    TAF_SDC_RPT_CMD_PLMNSELEINFO,TAF_SDC_RPT_CMD_SIMST,       TAF_SDC_RPT_CMD_TIME,        TAF_SDC_RPT_CMD_MIPICLK,
    TAF_SDC_RPT_CMD_ANLEVEL,     TAF_SDC_RPT_CMD_SRCHEDPLMNINFO, TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_SMMEMFULL,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_CTZV,
    TAF_SDC_RPT_CMD_CTZE,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_DSFLOWRPT,   TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_ORIG,        TAF_SDC_RPT_CMD_CONF,        TAF_SDC_RPT_CMD_CONN,        TAF_SDC_RPT_CMD_CEND,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_STIN,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_CTIME,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_CERSSI,      TAF_SDC_RPT_CMD_LWCLASH,     TAF_SDC_RPT_CMD_XLEMA,       TAF_SDC_RPT_CMD_ACINFO,
    TAF_SDC_RPT_CMD_PLMN,        TAF_SDC_RPT_CMD_CALLSTATE,   TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_XPASSINFO,
    TAF_SDC_RPT_CMD_CCONTDTMF,   TAF_SDC_RPT_CMD_CBURSTDTMF,  TAF_SDC_RPT_CMD_CLOCINFO,    TAF_SDC_RPT_CMD_CSID,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT
};

/* ����^CLOCINFO�������ϱ����ƣ����ڵڰ˸��ֽڵĵ�����3��Bit */
/* ����^CSID�������ϱ����ƣ����ڵڰ˸��ֽڵĵ�����4��Bit */
TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8            g_aenTafSdcUnsolicitedRptCmdTable[] =
{
    TAF_SDC_RPT_CMD_MODE,        TAF_SDC_RPT_CMD_RSSI,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_SRVST,
    TAF_SDC_RPT_CMD_CREG,        TAF_SDC_RPT_CMD_SIMST,       TAF_SDC_RPT_CMD_TIME,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_ANLEVEL,     TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_CTZV,
    TAF_SDC_RPT_CMD_CTZE,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_DSFLOWRPT,   TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_CUSD,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_CSSI,
    TAF_SDC_RPT_CMD_CSSU,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_CERSSI,      TAF_SDC_RPT_CMD_LWURC,       TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_CUUS1U,
    TAF_SDC_RPT_CMD_CUUS1I,      TAF_SDC_RPT_CMD_CGREG,       TAF_SDC_RPT_CMD_CEREG,       TAF_SDC_RPT_CMD_BUTT,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_CLOCINFO,    TAF_SDC_RPT_CMD_CSID,
    TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT,        TAF_SDC_RPT_CMD_BUTT
};


/*lint -save -e958 */

/*****************************************************************************
  6 ��������
*****************************************************************************/

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32  TAF_SDC_IsPlatformSupportLte( VOS_VOID )
{
    VOS_UINT32                          i;
    TAF_SDC_PLATFORM_RAT_CAP_STRU      *pstPlatformRatCap = VOS_NULL_PTR;

    pstPlatformRatCap = TAF_SDC_GetPlatformRatCap();

    /* LTE�Ƿ���ƽ̨���뼼�������б��� */
    for (i = 0 ; i < pstPlatformRatCap->ucRatNum; i++)
    {
        if (TAF_SDC_PLATFORM_RAT_LTE == pstPlatformRatCap->aenRatList[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}
#endif


VOS_UINT32  TAF_SDC_IsPlatformSupportUtran( VOS_VOID )
{
    VOS_UINT32                          i;
    TAF_SDC_PLATFORM_RAT_CAP_STRU      *pstPlatformRatCap = VOS_NULL_PTR;

    pstPlatformRatCap = TAF_SDC_GetPlatformRatCap();

    /* UTRAN�Ƿ���ƽ̨���뼼�������б��� */
    for (i = 0 ; i < pstPlatformRatCap->ucRatNum; i++)
    {
        if ((TAF_SDC_PLATFORM_RAT_WCDMA == pstPlatformRatCap->aenRatList[i])
         || (TAF_SDC_PLATFORM_RAT_TDS == pstPlatformRatCap->aenRatList[i]))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32  TAF_SDC_IsPlatformSupportUtranFdd( VOS_VOID )
{
    VOS_UINT32                          i;
    TAF_SDC_PLATFORM_RAT_CAP_STRU      *pstPlatformRatCap = VOS_NULL_PTR;

    pstPlatformRatCap = TAF_SDC_GetPlatformRatCap();

    /* UTRAN�Ƿ���ƽ̨���뼼�������б��� */
    for (i = 0 ; i < pstPlatformRatCap->ucRatNum; i++)
    {
        if (TAF_SDC_PLATFORM_RAT_WCDMA == pstPlatformRatCap->aenRatList[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)

VOS_UINT32  TAF_SDC_IsPlatformSupportUtranTdd( VOS_VOID )
{
    VOS_UINT32                          i;
    TAF_SDC_PLATFORM_RAT_CAP_STRU      *pstPlatformRatCap = VOS_NULL_PTR;

    pstPlatformRatCap = TAF_SDC_GetPlatformRatCap();

    /* UTRAN�Ƿ���ƽ̨���뼼�������б��� */
    for (i = 0 ; i < pstPlatformRatCap->ucRatNum; i++)
    {
        if (TAF_SDC_PLATFORM_RAT_TDS == pstPlatformRatCap->aenRatList[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}
#endif



VOS_UINT32  TAF_SDC_IsPlatformSupportGsm( VOS_VOID )
{
    VOS_UINT32                          i;
    TAF_SDC_PLATFORM_RAT_CAP_STRU      *pstPlatformRatCap = VOS_NULL_PTR;

    pstPlatformRatCap = TAF_SDC_GetPlatformRatCap();

    /* GSM�Ƿ���ƽ̨���뼼�������б��� */
    for (i = 0 ; i < pstPlatformRatCap->ucRatNum; i++)
    {
        if (TAF_SDC_PLATFORM_RAT_GSM == pstPlatformRatCap->aenRatList[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 TAF_SDC_IsPlatformSupport3Gpp2(VOS_VOID)
{
    VOS_UINT32                          i;
    TAF_SDC_PLATFORM_RAT_CAP_STRU      *pstPlatformRatCap = VOS_NULL_PTR;

    pstPlatformRatCap = TAF_SDC_GetPlatformRatCap();

    /* CDMA�Ƿ���ƽ̨���뼼�������б��� */
    for (i = 0 ; i < pstPlatformRatCap->ucRatNum; i++)
    {
        if ((TAF_SDC_PLATFORM_RAT_1X   == pstPlatformRatCap->aenRatList[i])
         || (TAF_SDC_PLATFORM_RAT_HRPD == pstPlatformRatCap->aenRatList[i]))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SDC_IsPlatformSupport1x(VOS_VOID)
{
    VOS_UINT32                          i;
    TAF_SDC_PLATFORM_RAT_CAP_STRU      *pstPlatformRatCap = VOS_NULL_PTR;

    pstPlatformRatCap = TAF_SDC_GetPlatformRatCap();

    /* CDMA�Ƿ���ƽ̨���뼼�������б��� */
    for (i = 0 ; i < pstPlatformRatCap->ucRatNum; i++)
    {
        if (TAF_SDC_PLATFORM_RAT_1X == pstPlatformRatCap->aenRatList[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

VOS_UINT32 TAF_SDC_IsPlatformSupportHrpd(VOS_VOID)
{
    VOS_UINT32                          i;
    TAF_SDC_PLATFORM_RAT_CAP_STRU      *pstPlatformRatCap = VOS_NULL_PTR;

    pstPlatformRatCap = TAF_SDC_GetPlatformRatCap();

    /* CDMA�Ƿ���ƽ̨���뼼�������б��� */
    for (i = 0 ; i < pstPlatformRatCap->ucRatNum; i++)
    {
        if (TAF_SDC_PLATFORM_RAT_HRPD == pstPlatformRatCap->aenRatList[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SDC_IsPlatformSupportHybrid(VOS_VOID)
{
    VOS_UINT32                          ulSupport1X;
    VOS_UINT32                          ulSupportEVDO;

    ulSupport1X     = TAF_SDC_IsPlatformSupport1x();
    ulSupportEVDO   = TAF_SDC_IsPlatformSupportHrpd();

    if ((VOS_TRUE == ulSupport1X)
     && (VOS_TRUE == ulSupportEVDO))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
#endif


VOS_UINT32 TAF_SDC_IsPlatformSupport3Gpp(VOS_VOID)
{
    VOS_UINT32                                              i;
    TAF_SDC_PLATFORM_RAT_TYPE_ENUM_UINT8                    enRatType         = TAF_SDC_PLATFORM_RAT_BUTT;
    TAF_SDC_PLATFORM_RAT_CAP_STRU                          *pstPlatformRatCap = VOS_NULL_PTR;


    pstPlatformRatCap = TAF_SDC_GetPlatformRatCap();

    /* ��ƽ̨���뼼�������б����Ƿ���G��U��L */
    for (i = 0 ; i < pstPlatformRatCap->ucRatNum; i++)
    {
        enRatType = pstPlatformRatCap->aenRatList[i];

        if ((TAF_SDC_PLATFORM_RAT_GSM   == enRatType)
         || (TAF_SDC_PLATFORM_RAT_WCDMA == enRatType)
         || (TAF_SDC_PLATFORM_RAT_TDS   == enRatType)
         || (TAF_SDC_PLATFORM_RAT_LTE   == enRatType))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}



VOS_VOID  TAF_SDC_LogUnsolicitedRptInfo(
    TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8            enRptCmdIndex,
    TAF_SDC_CURC_RPT_CTRL_STRU                 *pstCurcRptCtrl,
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU          *pstUnsolicitedRptCtrl
)
{
    TAF_OAM_UNSOLICITED_RPT_INFO_STRU  *pstMsg = VOS_NULL_PTR;

    pstMsg = (TAF_OAM_UNSOLICITED_RPT_INFO_STRU*)PS_MEM_ALLOC(WUEPS_PID_TAF,
                                         sizeof(TAF_OAM_UNSOLICITED_RPT_INFO_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    PS_MEM_SET(pstMsg, 0x00, sizeof(TAF_OAM_UNSOLICITED_RPT_INFO_STRU));

    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_TAF;
    pstMsg->ulReceiverPid   = WUEPS_PID_TAF;
    pstMsg->ulLength        = sizeof(TAF_OAM_UNSOLICITED_RPT_INFO_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->ulMsgId         = TAF_OAM_UNSOLICITED_RPT_INFO_IND;
    pstMsg->ulRptCmdIndex   = enRptCmdIndex;
    PS_MEM_CPY(pstMsg->aucCurcRptCfg, pstCurcRptCtrl->aucRptCfg, sizeof(pstMsg->aucCurcRptCfg));
    PS_MEM_CPY(pstMsg->aucUnsolicitedRptCfg, pstUnsolicitedRptCtrl->aucRptCfg, sizeof(pstMsg->aucUnsolicitedRptCfg));

    DIAG_TraceReport(pstMsg);

    PS_MEM_FREE(WUEPS_PID_TAF, pstMsg);

    return;
}



VOS_UINT32 TAF_SDC_CheckRptCmdStatus(
    VOS_UINT8                                  *pucRptCfg,
    TAF_SDC_CMD_RPT_CTRL_TYPE_ENUM_UINT8        enRptCtrlType,
    TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8            enRptCmdIndex
)
{
    TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8   *pulRptCmdTblPtr       = VOS_NULL_PTR;
    VOS_UINT32                          ulRptCmdTblSize;
    VOS_UINT8                           ucTableIndex;
    VOS_UINT32                          ulOffset;
    VOS_UINT8                           ucBit;

    /* �����ϱ�������������Ĭ�������ϱ� */
    if (enRptCmdIndex >= TAF_SDC_RPT_CMD_BUTT)
    {
        return VOS_TRUE;
    }

    TAF_SDC_LogUnsolicitedRptInfo(enRptCmdIndex, TAF_SDC_GetCurcRptCtrl(), TAF_SDC_GetUnsolicitedRptCtrl());

    /* �����ϱ��ܿ�������д����Ĭ�������ϱ� */
    if (TAF_SDC_CMD_RPT_CTRL_BUTT == enRptCtrlType)
    {
        return VOS_TRUE;
    }

    if (TAF_SDC_CMD_RPT_CTRL_BY_CURC == enRptCtrlType)
    {
        pulRptCmdTblPtr = TAF_SDC_GET_CURC_RPT_CTRL_STATUS_MAP_TBL_PTR();
        ulRptCmdTblSize = TAF_SDC_GET_CURC_RPT_CTRL_STATUS_MAP_TBL_SIZE();
    }
    else
    {
        pulRptCmdTblPtr = TAF_SDC_GET_UNSOLICITED_RPT_CTRL_STATUS_MAP_TBL_PTR();
        ulRptCmdTblSize = TAF_SDC_GET_UNSOLICITED_RPT_CTRL_STATUS_MAP_TBL_SIZE();
    }

    for (ucTableIndex = 0; ucTableIndex < ulRptCmdTblSize; ucTableIndex++)
    {
        if (enRptCmdIndex == pulRptCmdTblPtr[ucTableIndex])
        {
            break;
        }
    }

    /* ��ȫ�ֱ����е�Bitλ�Ա� */
    if (ulRptCmdTblSize != ucTableIndex)
    {
        /* �����û����õ��ֽ�����Bitӳ������෴, ���ȷ�תBitλ */
        /*lint -e961*/
        ulOffset        = TAF_SDC_RPT_CFG_MAX_SIZE - (ucTableIndex / 8) - 1;
        /*lint +e961*/
        ucBit           = (VOS_UINT8)(ucTableIndex % 8);

        return (VOS_UINT32)((pucRptCfg[ulOffset] >> ucBit) & 0x1);
    }

    return VOS_TRUE;
}


VOS_VOID TAF_SDC_UpdateRptCmdStatus(
    TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8            enRptCmdIndex,
    VOS_UINT32                                  ulRptCmdStatus
)
{
    TAF_SDC_RPT_CMD_INDEX_ENUM_UINT8   *pulRptCmdTblPtr     = VOS_NULL_PTR;
    TAF_SDC_UNSOLICITED_RPT_CTRL_STRU  *pstUnsolicitedRpt   = VOS_NULL_PTR;
    VOS_UINT32                          ulRptCmdTblSize;
    VOS_UINT8                           ucTableIndex;
    VOS_UINT32                          ulOffset;
    VOS_UINT8                           ucBit;
    VOS_UINT8                           ucRptFlg;

    ucRptFlg    = 0x01;

    /* �����ϱ�������������Ĭ�������ϱ� */
    if (enRptCmdIndex >= TAF_SDC_RPT_CMD_BUTT)
    {
        return;
    }

    pulRptCmdTblPtr     = TAF_SDC_GET_UNSOLICITED_RPT_CTRL_STATUS_MAP_TBL_PTR();
    ulRptCmdTblSize     = TAF_SDC_GET_UNSOLICITED_RPT_CTRL_STATUS_MAP_TBL_SIZE();
    pstUnsolicitedRpt   = TAF_SDC_GetUnsolicitedRptCtrl();

    for (ucTableIndex = 0; ucTableIndex < ulRptCmdTblSize; ucTableIndex++)
    {
        if (enRptCmdIndex == pulRptCmdTblPtr[ucTableIndex])
        {
            break;
        }
    }

    /* ��ȫ�ֱ����е�Bitλ�Ա� */
    if (ulRptCmdTblSize != ucTableIndex)
    {
        /* �����û����õ��ֽ�����Bitӳ������෴, ���ȷ�תBitλ */
        /*lint -e961*/
        ulOffset        = TAF_SDC_RPT_CFG_MAX_SIZE - (ucTableIndex / 8) - 1;
        /*lint +e961*/
        ucBit           = (VOS_UINT8)(ucTableIndex % 8);

        /*lint -e701*/
        ucRptFlg = (VOS_UINT8)(ucRptFlg << ucBit);
        /*lint +e701*/

        /* �ر������ϱ�ʱ */
        if (VOS_FALSE == ulRptCmdStatus)
        {
            (pstUnsolicitedRpt->aucRptCfg[ulOffset]) &= (~ucRptFlg);
        }
        else
        {
            (pstUnsolicitedRpt->aucRptCfg[ulOffset]) |= ucRptFlg;
        }

    }

    return;
}



VOS_UINT32 TAF_SDC_ConvertOamSendPid(
    VOS_UINT32                          ulConvertedSendPid
)
{

    if ((I0_WUEPS_PID_USIM == ulConvertedSendPid)
     || (I1_WUEPS_PID_USIM == ulConvertedSendPid)
     || (I2_WUEPS_PID_USIM == ulConvertedSendPid))
    {
        return WUEPS_PID_USIM;
    }

    if ((I0_MAPS_STK_PID == ulConvertedSendPid)
     || (I1_MAPS_STK_PID == ulConvertedSendPid)
     || (I2_MAPS_STK_PID == ulConvertedSendPid))
    {
        return MAPS_STK_PID;
    }

    if ((I0_MAPS_PB_PID == ulConvertedSendPid)
     || (I1_MAPS_PB_PID == ulConvertedSendPid)
     || (I2_MAPS_PB_PID == ulConvertedSendPid))
    {
        return MAPS_PB_PID;
    }


    if ((I0_MAPS_PIH_PID == ulConvertedSendPid)
     || (I1_MAPS_PIH_PID == ulConvertedSendPid)
     || (I2_MAPS_PIH_PID == ulConvertedSendPid))
    {
        return MAPS_PIH_PID;
    }

    return ulConvertedSendPid;
}



VOS_UINT32 TAF_SDC_GetCsSrvExistFlg(VOS_VOID)
{
    /* ����е绰ҵ����ڣ���Ϊ��ǰ��CSҵ�� */
    if (VOS_TRUE == TAF_SDC_GetCsCallExistFlg())
    {
        return VOS_TRUE;
    }

    /* �����CS�����ҵ����ڣ���Ϊ��ǰ��CSҵ�� */
    if (VOS_TRUE == TAF_SDC_GetCsSmsSrvExistFlg())
    {
        return VOS_TRUE;
    }

    /* �����CS�򲹳�ҵ����ڣ���Ϊ��ǰ��CSҵ�� */
    if (VOS_TRUE == TAF_SDC_GetCsSsSrvExistFlg())
    {
        return VOS_TRUE;
    }

    /* �����������Ϊû��CSҵ�� */
    return VOS_FALSE;

}



VOS_UINT32 TAF_SDC_GetImsSupportFlag(VOS_VOID)
{
    /* ���֧��GSM IMS����Ϊ��ǰ֧��IMS */
    if (VOS_TRUE == TAF_SDC_GetGsmImsSupportFlag())
    {
        return VOS_TRUE;
    }

    /* ���֧��UTRAN IMS����Ϊ��ǰ֧��IMS */
    if (VOS_TRUE == TAF_SDC_GetUtranImsSupportFlag())
    {
        return VOS_TRUE;
    }

    /* ���֧��LTE IMS����Ϊ��ǰ֧��IMS */
    if (VOS_TRUE == TAF_SDC_GetLteImsSupportFlag())
    {
        return VOS_TRUE;
    }

    /* �����������Ϊ��ǰ��֧��IMS */
    return VOS_FALSE;

}



VOS_UINT32 TAF_SDC_IsImsiAvailableInCurrSimStatus(
    TAF_SDC_USIM_STATUS_ENUM_UINT8      enSimStatus
)
{
    switch(enSimStatus)
    {
        /* ��Щ״̬ʱ����ȡ��IMSI */
        case TAF_SDC_USIM_STATUS_VALID:
        case TAF_SDC_USIM_STATUS_CS_UNVALID:
        case TAF_SDC_USIM_STATUS_PS_UNVALID:
        case TAF_SDC_USIM_STATUS_CS_PS_UNVALID:
        case TAF_SDC_USIM_STATUS_NET_LCOK:
        case TAF_SDC_USIM_STATUS_ROMSIM:
        case TAF_SDC_USIM_STATUS_IMSI_LCOK:

            return VOS_TRUE;

        default:

            return VOS_FALSE;
    }
}

#if (FEATURE_ON == FEATURE_PTM)

VOS_UINT32 TAF_SDC_IsErrLogNeedRecord(VOS_UINT16 usLevel)
{
    /* Log���عرգ�����Ҫ�ϱ� */
    if (0 == TAF_SDC_GetErrlogCtrlFlag())
    {
        return VOS_FALSE;
    }

    if (VOS_TRUE == NAS_USIMMAPI_IsTestCard())
    {
        return VOS_FALSE;
    }

    /* ģ��log����usLevel�����û����õ�log�ϱ������usLevel��Ч������Ҫ�ϱ� */
    if ((TAF_SDC_GetErrlogAlmLevel() < usLevel)
     || (NAS_ERR_LOG_CTRL_LEVEL_NULL == usLevel))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID TAF_SDC_GetCardImsiFailRecord(
    VOS_UINT32                          ulGetCardImsiRslt,
    VOS_UINT8                          *pucImsi
)
{
    NAS_ERR_LOG_GET_CARD_IMSI_FAIL_STRU                     stGetCardImsiFail;
    VOS_UINT32                                              ulIsLogRecord;
    VOS_UINT32                                              ulLength;
    VOS_UINT32                                              ulResult;
    VOS_UINT16                                              usLevel;

    /* ��ѯ��ӦAlarm Id�Ƿ���Ҫ��¼�쳣��Ϣ */
    usLevel       = NAS_GetErrLogAlmLevel(NAS_ERR_LOG_ALM_GET_CARD_IMSI_FAIL);
    ulIsLogRecord = TAF_SDC_IsErrLogNeedRecord(usLevel);

    /* ����Ҫ��¼��û���쳣ʱ���������쳣��Ϣ */
    if ((VOS_FALSE == ulIsLogRecord)
     || (USIMM_API_SUCCESS == ulGetCardImsiRslt))
    {
        return;
    }

    ulLength = sizeof(NAS_ERR_LOG_GET_CARD_IMSI_FAIL_STRU);

    /* ���Cs����ʧ���쳣��Ϣ */
    PS_MEM_SET(&stGetCardImsiFail, 0x00, ulLength);

    NAS_COMM_BULID_ERRLOG_HEADER_INFO(&stGetCardImsiFail.stHeader,
                                      VOS_GetModemIDFromPid(WUEPS_PID_TAF),
                                      NAS_ERR_LOG_ALM_GET_CARD_IMSI_FAIL,
                                      usLevel,
                                      VOS_GetSlice(),
                                      (ulLength - sizeof(OM_ERR_LOG_HEADER_STRU)));



    /* ����API���صĽ�� */
    stGetCardImsiFail.ulGetCardImsiRslt = ulGetCardImsiRslt;

    /* ����API���ص�IMSI */
    if (VOS_NULL_PTR != pucImsi)
    {
        PS_MEM_CPY(stGetCardImsiFail.aucImsi, pucImsi, NAS_MNTN_IMSI_LENGTH_IN_USIM);
    }

    /* USIM��Ϣ */
    stGetCardImsiFail.stUsimInfo.enSimType      = TAF_SDC_GetUsimType();
    stGetCardImsiFail.stUsimInfo.enSimStatus    = TAF_SDC_GetUsimStatus();

    /* CSIM��Ϣ */
    stGetCardImsiFail.stCsimInfo.enSimType      = TAF_SDC_GetCsimType();
    stGetCardImsiFail.stCsimInfo.enSimStatus    = TAF_SDC_GetCsimStatus();

    /* �����ϱ�NAS_ERR_LOG_ALM_GET_CARD_IMSI_FAIL */
    TAF_SndAcpuOmFaultErrLogInd(&stGetCardImsiFail, ulLength, NAS_ERR_LOG_ALM_GET_CARD_IMSI_FAIL);

    /*
       ���쳣��Ϣд��Buffer��
       ʵ��д����ַ�������Ҫд��Ĳ������ӡ�쳣
     */
    ulResult = TAF_SDC_PutErrLogRingBuf((VOS_CHAR *)&stGetCardImsiFail, ulLength);
    if (ulResult != ulLength)
    {
        NAS_ERROR_LOG(WUEPS_PID_TAF, "TAF_SDC_GetCardImsiFailRecord(): Push buffer error.");
    }

    NAS_COM_MntnPutRingbuf(NAS_ERR_LOG_ALM_GET_CARD_IMSI_FAIL,
                           WUEPS_PID_TAF,
                           (VOS_UINT8 *)&stGetCardImsiFail,
                           sizeof(stGetCardImsiFail));
    return;
}
#endif


VOS_UINT32  TAF_CalcTime(
                VOS_UINT32  ulFirTime,
                VOS_UINT32  ulSecTime,
                VOS_UINT32 *pulTotalTime
)
{
    if (ulFirTime > ulSecTime)
    {
        *pulTotalTime = 0;
        return VOS_ERR;
    }

    *pulTotalTime = ulSecTime - ulFirTime;

    return VOS_OK;
}


VOS_UINT32 TAF_SDC_IsLteOnlyMode(
    TAF_SDC_RAT_PRIO_STRU              *pstRatOrder
)
{
    if ( (1                        == pstRatOrder->ucRatNum)
      && (TAF_SDC_SYS_MODE_LTE     == pstRatOrder->aenRatPrio[0]) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SDC_IsUsimStausValid(VOS_VOID)
{
    VOS_UINT8                           ucSimCsRegStatus;
    VOS_UINT8                           ucSimPsRegStatus;
#if (FEATURE_ON == FEATURE_ECALL)
    TAF_SDC_CALL_MODE_ENUM_UINT8        enCallMode;
#endif
#if (FEATURE_ON == FEATURE_LTE)
    TAF_SDC_RAT_PRIO_STRU              *pstRatPrio = VOS_NULL_PTR;

    pstRatPrio       = TAF_SDC_GetMsPrioRatList();
#endif

#if (FEATURE_ON == FEATURE_ECALL)
    enCallMode = TAF_SDC_GetCurCallMode();
#endif

    ucSimCsRegStatus = TAF_SDC_GetSimCsRegStatus();
    ucSimPsRegStatus = TAF_SDC_GetSimPsRegStatus();

    if ((VOS_FALSE == ucSimPsRegStatus)
     && (VOS_FALSE == ucSimCsRegStatus))
    {
        NAS_NORMAL_LOG(WUEPS_PID_TAF, "TAF_SDC_IsUsimStausValid(): SIM CS and PS reg status invalid");
        return VOS_FALSE;
    }

    /* �޿�ʱ����VOS_FALSE����Ч */
    if (TAF_SDC_USIM_STATUS_NO_PRESENT == TAF_SDC_GetSimStatus())
    {
        NAS_NORMAL_LOG(WUEPS_PID_TAF, "TAF_SDC_IsUsimStausValid(): SIM Not Present");
        return VOS_FALSE;
    }

    if (( VOS_FALSE == TAF_SDC_GetPsAttachAllowFlg())
     && ( VOS_FALSE == ucSimCsRegStatus))
    {
        NAS_NORMAL_LOG(WUEPS_PID_TAF, "TAF_SDC_IsUsimStausValid(): SIM PS attach not allowed and CS reg status invalid");
        return VOS_FALSE;
    }

    if (( VOS_FALSE == TAF_SDC_GetCsAttachAllowFlg())
     && ( VOS_FALSE == ucSimPsRegStatus)
#if (FEATURE_ON == FEATURE_ECALL)
     && (TAF_SDC_ECALL_ONLY != enCallMode)
#endif
       )
    {
        NAS_NORMAL_LOG(WUEPS_PID_TAF, "TAF_SDC_IsUsimStausValid(): SIM CS attach not allowed and PS reg status invalid");
        return VOS_FALSE;
    }

#if (FEATURE_ON == FEATURE_LTE)

    /* LTE��ģ�����PS����Ч��ʱ��Ҳ��Ϊ��ʱ����Ч */
    if ((VOS_TRUE == TAF_SDC_IsLteOnlyMode(pstRatPrio))
     && (VOS_FALSE == ucSimPsRegStatus))
    {
        NAS_NORMAL_LOG(WUEPS_PID_TAF, "TAF_SDC_IsUsimStausValid(): L only mode and SIM PS reg status invalid");
        return VOS_FALSE;
    }
#endif
    return VOS_TRUE;

}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 TAF_SDC_IsImsVoiceSupportedInUE(VOS_VOID)
{
    VOS_UINT32                          ulImsSupportFlag;

    ulImsSupportFlag    = TAF_SDC_GetImsSupportFlag();

    /* ��ǰIMS��RAT NV��֧�ֻ�������ѡ��ΪCS ONLY�����粻֧��IMSʱ ��ΪIMS VOICE������*/
    if ((TAF_SDC_VOICE_DOMAIN_CS_ONLY       == TAF_SDC_GetVoiceDomain())
     || (VOS_FALSE                          == ulImsSupportFlag))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 TAF_SDC_IsImsVoiceSupportedInNW(VOS_VOID)
{
    TAF_SDC_NETWORK_CAP_INFO_STRU      *pstCurNwCapInfo   = VOS_NULL_PTR;

    if (TAF_SDC_SYS_MODE_LTE == TAF_SDC_GetSysMode())
    {
        pstCurNwCapInfo = TAF_SDC_GetLteNwCapInfo();
    }
    else
    {
        pstCurNwCapInfo = TAF_SDC_GetGuNwCapInfo();
    }

    if (TAF_SDC_NW_IMS_VOICE_NOT_SUPPORTED == pstCurNwCapInfo->enNwImsVoCap)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}



#endif


VOS_VOID TAF_SDC_PlmnId2Bcd(
    TAF_SDC_PLMN_ID_STRU               *pstPlmnId
)
{
    VOS_UINT8                           ucI;
    VOS_UINT32                          ulTmp;
    VOS_UINT8                           aucTmp[4];

    ulTmp            = pstPlmnId->ulMcc;
    pstPlmnId->ulMcc = 0;
    for ( ucI = 0 ; ucI < 3 ; ucI++ )
    {
        aucTmp[ucI]     = ulTmp & 0x0f;
        ulTmp         >>=  8;
    }
    pstPlmnId->ulMcc = ((VOS_UINT32)aucTmp[0] << 8)
                      |((VOS_UINT32)aucTmp[1] << 4)
                      | aucTmp[2];

    ulTmp            = pstPlmnId->ulMnc;
    pstPlmnId->ulMnc = 0;
    for ( ucI = 0 ; ucI < 3 ; ucI++ )
    {
        aucTmp[ucI]     = ulTmp & 0x0f;
        ulTmp         >>=  8;
    }

    if(0xf == aucTmp[2])
    {
        pstPlmnId->ulMnc = ((VOS_UINT32)aucTmp[0] << 4)
                          |((VOS_UINT32)aucTmp[1] )
                          |0xf00;
    }
    else
    {
        pstPlmnId->ulMnc = ((VOS_UINT32)aucTmp[0] << 8)
                          |((VOS_UINT32)aucTmp[1] << 4)
                          | aucTmp[2];
    }
    return;
}


VOS_UINT32 TAF_SDC_IsConfigCLInterWorkCOutside(VOS_VOID)
{
    if (VOS_FALSE == TAF_SDC_GetLCEnableFlg())
    {
        return VOS_FALSE;
    }

    if ( TAF_NVIM_LC_RAT_COMBINED_CL != TAF_SDC_GetLCRatCombined() )
    {
        return VOS_FALSE;
    }

    /* ��ǰ����ΪCL������ģʽ��BALONG��modem��֧��CDMA */
    if (TAF_NVIM_LC_INTER_WORK == TAF_SDC_GetLCWorkCfg())
    {
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        if (VOS_FALSE == TAF_SDC_IsPlatformSupport3Gpp2())
#endif
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 TAF_SDC_IsConfigCLMode(VOS_VOID)
{
    TAF_SDC_RAT_PRIO_STRU              *pstRatPrio = VOS_NULL_PTR;
    VOS_UINT32                          i;

    pstRatPrio = TAF_SDC_GetMsPrioRatList();

    for (i = 0; i < pstRatPrio->ucRatNum; i++)
    {
        if ((TAF_SDC_SYS_MODE_CDMA_1X == pstRatPrio->aenRatPrio[i])
         || (TAF_SDC_SYS_MODE_EVDO == pstRatPrio->aenRatPrio[i]))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SDC_Is1xOnlyMode(VOS_VOID)
{
    TAF_SDC_RAT_PRIO_STRU              *pstRatPrio      = VOS_NULL_PTR;
    pstRatPrio                          = TAF_SDC_GetMsPrioRatList();

    if ( (1                        == pstRatPrio->ucRatNum)
      && (TAF_SDC_SYS_MODE_CDMA_1X == pstRatPrio->aenRatPrio[0]) )
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SDC_Get1xConcurrentSupportedFlag(VOS_VOID)
{
    TAF_SDC_1X_SYS_INFO_STRU           *pst1xSysInfo;

    pst1xSysInfo = TAF_SDC_Get1xSysInfo();

    if ((VOS_TRUE             == pst1xSysInfo->stServiceInfo.ucConcurrentSupported)
     && (TAF_SDC_REV_IN_USE_7 <= pst1xSysInfo->ucPrevInUse))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}

#endif





/*lint -restore */


VOS_UINT32 TAF_SDC_CheckDtmfKey(
    VOS_CHAR                           *pckey
)
{
    /* ת��Ϊ��д�ַ� */
    if ( (*pckey >= 'a') && (*pckey <= 'z'))
    {
        *pckey = *pckey - 0x20;
    }

    if (!((('0' <= *pckey) && ('9' >= *pckey))
         || ('*' == *pckey) || ('#' == *pckey)
         || ('A' == *pckey) || ('B' == *pckey)
         || ('C' == *pckey) || ('D' == *pckey)))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_SDC_IsCsNormalService(VOS_VOID)
{
    if (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == TAF_SDC_GetCsServiceStatus())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SDC_IsBcchPlmnIdInDestSimPlmnList (
    TAF_SDC_PLMN_ID_STRU               *pstBcchPlmnId,
    VOS_UINT8                           ucSimPlmnNum,
    TAF_SDC_PLMN_ID_STRU               *pstSimPlmnIdList
)
{
    VOS_UINT32                          i;

    for ( i = 0; i < ucSimPlmnNum; i++ )
    {
        if ( VOS_TRUE == TAF_SDC_CompareBcchPlmnwithSimPlmn(pstBcchPlmnId,
                                                            pstSimPlmnIdList + i))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}






VOS_UINT32 TAF_SDC_ExactlyCompareBcchPlmnwithSimPlmn(
    TAF_SDC_PLMN_ID_STRU                *pstBccPlmnId,
    TAF_SDC_PLMN_ID_STRU                *pstSimPlmnId
)
{
    VOS_UINT32                          ulCmpRlst;

    if (pstBccPlmnId->ulMcc == pstSimPlmnId->ulMcc)
    {
        if (pstBccPlmnId->ulMnc == pstSimPlmnId->ulMnc)
        {
            return VOS_TRUE;
        }
        else if ((pstBccPlmnId->ulMnc & TAF_SDC_MCC_LOW_TWO_BYTE_VALUE_MASK)
              == (pstSimPlmnId->ulMnc & TAF_SDC_MCC_LOW_TWO_BYTE_VALUE_MASK))
        {
            /* ȡMNC�����λ ,�����λ��0��F����2λMnc��ͬ����ΪPlmn��� */
            ulCmpRlst = TAF_SDC_CompareBcchMnc3BytewithSimMnc3Byte(
                                    pstBccPlmnId,
                                    pstSimPlmnId,
                                    VOS_TRUE);

            return ulCmpRlst;
        }
        else
        {
        }
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_SDC_CompareBcchPlmnwithSimPlmn(
    TAF_SDC_PLMN_ID_STRU                *pstBccPlmnId,
    TAF_SDC_PLMN_ID_STRU                *pstSimPlmnId
)
{
    VOS_UINT32                          ulCmpRlst;

    ulCmpRlst   = VOS_FALSE;

    /* �����Ҫ��ȷ�Ƚ�MNC*/
    if (VOS_TRUE == TAF_SDC_GetPlmnExactlyCompareFlg())
    {
        ulCmpRlst = TAF_SDC_ExactlyCompareBcchPlmnwithSimPlmn(pstBccPlmnId, pstSimPlmnId);

        return ulCmpRlst;
    }

    /* HPLMN Matching Criteria for mobiles which support PCS1900 for NA:
    1) The MS shall compare using all 3 digits the SIM-MCC with the BCCH-MCC.
    If the values do not match, then the HPLMN match fails*/
    if (pstBccPlmnId->ulMcc == pstSimPlmnId->ulMcc)
    {
        /*The MS shall compare using all 3 digits the SIM-MNC with the BCCH-MNC.
          If the values match, then the HPLMN match succeeds*/
        if (pstBccPlmnId->ulMnc == pstSimPlmnId->ulMnc)
        {
            ulCmpRlst = VOS_TRUE;
        }
        else if ((pstBccPlmnId->ulMnc & TAF_SDC_MCC_LOW_TWO_BYTE_VALUE_MASK)
              == (pstSimPlmnId->ulMnc & TAF_SDC_MCC_LOW_TWO_BYTE_VALUE_MASK))
        {
            /* ȡMNC�����λ ,�����λ��0��F����2λMnc��ͬ����ΪPlmn��� */
            ulCmpRlst = TAF_SDC_CompareBcchMnc3BytewithSimMnc3Byte(
                                        pstBccPlmnId,
                                        pstSimPlmnId,
                                        VOS_FALSE);
        }
        else
        {
        }

    }

    return ulCmpRlst;
}


VOS_UINT32 TAF_SDC_CompareBcchPlmnBytewithSimPlmnByte_WithWildcard(
    VOS_UINT8                           ucHexBcchByte,
    VOS_UINT8                           ucHexSimByte,
    VOS_CHAR                            cWildcard
)
{
    VOS_UINT8                           ucAsciiBcchByte;
    VOS_UINT8                           ucAsciiSimByte;
    VOS_CHAR                            cUpperWildCard;

    /* ���������Ϊ�����ֽ�����ͬ��:
     * 1.�����ֽ����
     * 2.ĳ�ֽ���ͨ��������Ƚ��ֽ������֣����ֽ��е�ͨ�����16��������ucWildcard��ASCII�룬
     *   ����: 0x0D ��Ҫת����'D',0x02--'2', 0X12�Ǵ����ʽ
     */
    TAF_SDC_HexToAsciiOfPlmnByte(ucHexBcchByte, &ucAsciiBcchByte);
    TAF_SDC_HexToAsciiOfPlmnByte(ucHexSimByte, &ucAsciiSimByte);

    cUpperWildCard = VOS_CharToUpper(cWildcard);

    if ( (ucHexBcchByte == ucHexSimByte)
       || ((cUpperWildCard == ucAsciiBcchByte)
        && (isdigit(ucAsciiSimByte)))
       || ((cUpperWildCard == ucAsciiSimByte)
        && (isdigit(ucAsciiBcchByte))))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SDC_CompareBcchMccwithSimMcc_WithWildcard(
    VOS_UINT32                          ulBcchMcc,
    VOS_UINT32                          ulSimMcc,
    VOS_CHAR                            cWildcard,
    VOS_UINT8                           ucExactlyCmpFlg
)
{
    VOS_UINT8                           ucBcchByte;
    VOS_UINT8                           ucSimByte;
    VOS_UINT32                          ulCmpRlst;
    VOS_UINT32                          ulIdx;
    VOS_UINT32                          ulShiftNum;

    /* ���MCC�Ƿ���ͬ */
    for (ulIdx = 0; ulIdx < TAF_SDC_PLMN_MCC_LEN; ulIdx++)
    {
        ulShiftNum = ulIdx * 8;
        ucBcchByte = (VOS_UINT8)((ulBcchMcc & ((VOS_UINT32)TAF_SDC_MCC_MNC_BYTE_VALUE_MASK << ulShiftNum)) >> ulShiftNum);
        ucSimByte  = (VOS_UINT8)((ulSimMcc & ((VOS_UINT32)TAF_SDC_MCC_MNC_BYTE_VALUE_MASK << ulShiftNum)) >> ulShiftNum);

        ulCmpRlst  = TAF_SDC_CompareBcchPlmnBytewithSimPlmnByte_WithWildcard(
                                        ucBcchByte,
                                        ucSimByte,
                                        cWildcard);

        /* ���ĳ�ֽڲ��ȣ���MCC����ͬ */
        if (VOS_FALSE == ulCmpRlst)
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;

}


VOS_UINT32 TAF_SDC_CompareBcchMnc3BytewithSimMnc3Byte(
    TAF_SDC_PLMN_ID_STRU               *pstBccPlmnId,
    TAF_SDC_PLMN_ID_STRU               *pstSimPlmnId,
    VOS_UINT8                           ucExactlyCmpFlg
)
{
    VOS_UINT8                           ucBcchMnc3Byte;
    VOS_UINT8                           ucSimMnc3Byte;

    ucBcchMnc3Byte = (VOS_UINT8)((pstBccPlmnId->ulMnc & TAF_SDC_MCC_THIRD_BYTE_VALUE_MASK) >> 16);
    ucSimMnc3Byte  = (VOS_UINT8)((pstSimPlmnId->ulMnc & TAF_SDC_MCC_THIRD_BYTE_VALUE_MASK) >> 16);

    /* PLMN ���� NA PCS1900,�����Ĺ�������310��316֮�䣬Ҫ�������⴦�� */
    if ((TAF_SDC_PCS1900_MCC_LOW_VALUE == (pstBccPlmnId->ulMcc & TAF_SDC_MCC_LOW_TWO_BYTE_VALUE_MASK))
     && (TAF_SDC_PCS1900_MCC_HIGH_VALUE_RANGE >= (pstSimPlmnId->ulMcc & TAF_SDC_MCC_THIRD_BYTE_VALUE_MASK)))
    {
        if ( ((TAF_SDC_PCS1900_MNC_THIRD_BYTE_VALUE_F == ucBcchMnc3Byte)
            && (TAF_SDC_PCS1900_MNC_THIRD_BYTE_VALUE_0 == ucSimMnc3Byte))
          || ((TAF_SDC_PCS1900_MNC_THIRD_BYTE_VALUE_0 == ucBcchMnc3Byte)
           && (TAF_SDC_PCS1900_MNC_THIRD_BYTE_VALUE_F == ucSimMnc3Byte)) )
        {
            return VOS_TRUE;
        }
    }
    else
    {
        /* The MS shall compare using just the 1st 2 digits of the SIM-MNC
        with the BCCH-MNC. If the values match, then the HPLMN match succeeds,
        otherwise the HPLMN match fails.*/
        if ( (VOS_FALSE == ucExactlyCmpFlg)
          && (TAF_SDC_LOW_BYTE_INVALID == ucBcchMnc3Byte) )
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;

}


VOS_UINT32 TAF_SDC_CompareBcchMncLow2ByteWithSimMnclowLow2Byte_WithWildcard(
    VOS_UINT32                          ulBcchMnc,
    VOS_UINT32                          ulSimMnc,
    VOS_CHAR                            cWildcard,
    VOS_UINT8                           ucExactlyCmpFlg
)
{
    VOS_UINT8                           ucBcchByte;
    VOS_UINT8                           ucSimByte;
    VOS_UINT32                          ulCmpRlst;
    VOS_UINT32                          ulIdx;
    VOS_UINT32                          ulShiftNum;

    /*���MCC�Ƿ���ͬ*/
    for (ulIdx = 0; ulIdx < TAF_SDC_PLMN_MNC_LEN-1; ulIdx++)
    {
        ulShiftNum = ulIdx * 8;
        ucBcchByte = (VOS_UINT8)((ulBcchMnc & ((VOS_UINT32)TAF_SDC_MCC_MNC_BYTE_VALUE_MASK<<ulShiftNum)) >> ulShiftNum);
        ucSimByte  = (VOS_UINT8)((ulSimMnc & ((VOS_UINT32)TAF_SDC_MCC_MNC_BYTE_VALUE_MASK<<ulShiftNum)) >> ulShiftNum);

        ulCmpRlst  = TAF_SDC_CompareBcchPlmnBytewithSimPlmnByte_WithWildcard(
                                        ucBcchByte,
                                        ucSimByte,
                                        cWildcard);

        /*���ĳ�ֽڲ��ȣ���MNC��2���ֽڲ���ͬ*/
        if (VOS_FALSE == ulCmpRlst)
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;

}


VOS_VOID  TAF_SDC_ConvertSimPlmnToNasPLMN(
    TAF_SDC_SIM_FORMAT_PLMN_ID          *pstSimPlmn,
    TAF_SDC_PLMN_ID_STRU                *pstNasPlmn
)
{
    VOS_UINT32                          ulTempData;
    TAF_SDC_PLMN_ID_STRU                stPlmn;

    /*
       PLMN BCD ���� ��ʽ

       MCC digit 2,  MCC digit 1, ----OCTET 1
       MNC digit 3,  MCC digit 3, ----OCTET 2
       MNC digit 2,  MNC digit 1, ----OCTET 3
    */
    ulTempData = TAF_SDC_OCTET_LOW_FOUR_BITS & (pstSimPlmn->aucSimPlmn[0]);
    stPlmn.ulMcc = ulTempData;
    ulTempData = (TAF_SDC_OCTET_HIGH_FOUR_BITS & (pstSimPlmn->aucSimPlmn[0])) >> TAF_SDC_OCTET_MOVE_FOUR_BITS;
    stPlmn.ulMcc |= ulTempData << TAF_SDC_OCTET_MOVE_EIGHT_BITS;

    ulTempData = TAF_SDC_OCTET_LOW_FOUR_BITS & (pstSimPlmn->aucSimPlmn[1]);
    stPlmn.ulMcc |= ulTempData << TAF_SDC_OCTET_MOVE_SIXTEEN_BITS;
    ulTempData = (TAF_SDC_OCTET_HIGH_FOUR_BITS & (pstSimPlmn->aucSimPlmn[1])) >> TAF_SDC_OCTET_MOVE_FOUR_BITS;

    stPlmn.ulMnc = ulTempData << TAF_SDC_OCTET_MOVE_SIXTEEN_BITS;
    ulTempData = TAF_SDC_OCTET_LOW_FOUR_BITS & (pstSimPlmn->aucSimPlmn[2]);
    stPlmn.ulMnc |= ulTempData;
    ulTempData = (TAF_SDC_OCTET_HIGH_FOUR_BITS & (pstSimPlmn->aucSimPlmn[2])) >> TAF_SDC_OCTET_MOVE_FOUR_BITS;
    stPlmn.ulMnc |= ulTempData << TAF_SDC_OCTET_MOVE_EIGHT_BITS;

    *pstNasPlmn = stPlmn;

    return;
}


VOS_UINT32 TAF_SDC_CompareBcchMncwithSimMnc_WithWildcard(
    TAF_SDC_PLMN_ID_STRU               *pstBccPlmnId,
    TAF_SDC_PLMN_ID_STRU               *pstSimPlmnId,
    VOS_CHAR                            cWildcard,
    VOS_UINT8                           ucExactlyCmpFlg
)
{
    VOS_UINT8                           ucBcchMnc3Byte;
    VOS_UINT8                           ucSimMnc3Byte;
    VOS_UINT32                          ulMncLow2ByteCmpRlst;
    VOS_UINT32                          ulMnc3ByteCmpRlst;

    /* ���MNC�ĵ�2���ֽ��Ƿ���ͬ */
    ulMncLow2ByteCmpRlst = TAF_SDC_CompareBcchMncLow2ByteWithSimMnclowLow2Byte_WithWildcard(
                                        pstBccPlmnId->ulMnc,
                                        pstSimPlmnId->ulMnc,
                                        cWildcard,
                                        ucExactlyCmpFlg);

    /* MNC�ĵ�2���ֽڲ�ͬ����MNC��ͬ */
    if (VOS_FALSE == ulMncLow2ByteCmpRlst)
    {
        return VOS_FALSE;
    }

    /* MNC�ĵ�2���ֽ���ͬ���ټ��MNC�ĵ�3���ֽ��Ƿ���ͬ */
    ucBcchMnc3Byte = (VOS_UINT8)((pstBccPlmnId->ulMnc & TAF_SDC_MCC_THIRD_BYTE_VALUE_MASK) >> 16);
    ucSimMnc3Byte  = (VOS_UINT8)((pstSimPlmnId->ulMnc & TAF_SDC_MCC_THIRD_BYTE_VALUE_MASK) >> 16);

    ulMnc3ByteCmpRlst = TAF_SDC_CompareBcchPlmnBytewithSimPlmnByte_WithWildcard(
                                    ucBcchMnc3Byte,
                                    ucSimMnc3Byte,
                                    cWildcard);

    /* MNC�ĵ�3���ֽ�Ҳ��ȣ���MNC��ͬ */
    if (VOS_TRUE == ulMnc3ByteCmpRlst)
    {
        return VOS_TRUE;
    }
    else
    {
        /* MNC���λ����ͬ����Ҫ�ж��Ƿ���Ҫ���⴦��*/
        ulMnc3ByteCmpRlst = TAF_SDC_CompareBcchMnc3BytewithSimMnc3Byte(
                                        pstBccPlmnId,
                                        pstSimPlmnId,
                                        ucExactlyCmpFlg);

        return ulMnc3ByteCmpRlst;
    }


}



VOS_UINT32 TAF_SDC_IsValidWildcard(
    VOS_CHAR                            cWildcard
)
{
    VOS_CHAR                            cUpperWildCard;

    cUpperWildCard = VOS_CharToUpper(cWildcard);

    /* ͨ���ֻ���� 'A-F','a-f',ȡֵ */
    if ( ('A' <= cUpperWildCard)
      && (cUpperWildCard <= 'F'))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;

}

VOS_VOID TAF_SDC_HexToAsciiOfPlmnByte(
    VOS_UINT8                            ucSrc,
    VOS_UINT8                           *pucDst
)
{
    if (10 > ucSrc)
    {
        /* 0x0-0x9 ---'0'-'9' */
        *pucDst = ucSrc + 0x30;
    }
    else if (16 > ucSrc)
    {
        /* 0xA-0xF ---'A'-'F' */
        *pucDst = ucSrc + 'A' - 10;
    }
    else
    {
        *pucDst = ucSrc;
    }

}



VOS_UINT32 TAF_SDC_ExactlyCompareBcchPlmnwithSimPlmn_WithWildcard(
    TAF_SDC_PLMN_ID_STRU                *pstBccPlmnId,
    TAF_SDC_PLMN_ID_STRU                *pstSimPlmnId,
    VOS_CHAR                             cWildcard
)
{
    VOS_UINT32                          ulMccCmpRlst;
    VOS_UINT32                          ulMncCmpRlst;

    /* ͨ����Ƿ���Ч */
    if (VOS_FALSE == TAF_SDC_IsValidWildcard(cWildcard))
    {
        return VOS_FALSE;
    }

    /* ���MCC�Ƿ���ͬ */
    ulMccCmpRlst = TAF_SDC_CompareBcchMccwithSimMcc_WithWildcard(
                                        pstBccPlmnId->ulMcc,
                                        pstSimPlmnId->ulMcc,
                                        cWildcard,
                                        VOS_TRUE);
    if (VOS_FALSE == ulMccCmpRlst)
    {
        return VOS_FALSE;
    }



    /* MCC��ȣ��ټ��MNC�Ƿ���ͬ */
    ulMncCmpRlst = TAF_SDC_CompareBcchMncwithSimMnc_WithWildcard(
                                        pstBccPlmnId,
                                        pstSimPlmnId,
                                        cWildcard,
                                        VOS_TRUE);

    return ulMncCmpRlst;


}


VOS_VOID TAF_SDC_ConvertPlmnId2NasStyle(
    TAF_SDC_PLMN_ID_STRU               *pstPlmnId
)
{
    VOS_UINT8                           ucI;
    VOS_UINT8                           aucTmp[4];
    VOS_UINT32                          ulTmp;

    ulTmp           = pstPlmnId->ulMcc;
    pstPlmnId->ulMcc  = 0;
    for ( ucI = 0 ; ucI < 3 ; ucI++ )
    {
        aucTmp[ucI]     = ulTmp & 0x0f;
        ulTmp         >>=  4;
    }
    pstPlmnId->ulMcc  =  aucTmp[2]
                     |((VOS_UINT32)aucTmp[1] << 8)
                     |((VOS_UINT32)aucTmp[0] << 16);

    ulTmp           = pstPlmnId->ulMnc;
    pstPlmnId->ulMnc  = 0;
    for ( ucI = 0 ; ucI < 3 ; ucI++ )
    {
        aucTmp[ucI]     = ulTmp & 0x0f;
        ulTmp         >>=  4;
    }

    if(0xf == aucTmp[2])
    {
        pstPlmnId->ulMnc =  aucTmp[1]
                         |((VOS_UINT32)aucTmp[0] << 8)
                         |0x0f0000;
    }
    else
    {
        pstPlmnId->ulMnc =   aucTmp[2]
                     |((VOS_UINT32)aucTmp[1] << 8)
                     |((VOS_UINT32)aucTmp[0] << 16);
    }
    return;
}


TAF_SDC_FILE_TYPE_ENUM_UINT8  TAF_SDC_GetOplFileType(VOS_VOID)
{
    VOS_UINT8                           ucSimType;
    TAF_SDC_OPL_FILE_INFO_STRU         *pstUsimOplInfo = VOS_NULL_PTR;
    TAF_SDC_OPL_FILE_INFO_STRU         *pstSimOplInfo = VOS_NULL_PTR;

    pstUsimOplInfo = TAF_SDC_GetUsimOplFileInfo();
    pstSimOplInfo  = TAF_SDC_GetSimOplFileInfo();
    ucSimType      = 0;

    (VOS_VOID)NAS_USIMMAPI_GetCardType(VOS_NULL_PTR, &ucSimType);

    if ((USIMM_CARD_SIM == ucSimType)
     && (0 != pstSimOplInfo->ulOplRecordNum))
    {
        return TAF_SDC_FILE_TYPE_SIM;
    }

    if (USIMM_CARD_USIM == ucSimType)
    {
        if (0 != pstUsimOplInfo->ulOplRecordNum)
        {
            return TAF_SDC_FILE_TYPE_USIM;
        }

        if (0 != pstSimOplInfo->ulOplRecordNum)
        {
            return TAF_SDC_FILE_TYPE_SIM;
        }
    }

    return TAF_SDC_FILE_TYPE_BUTT;
}


TAF_SDC_FILE_TYPE_ENUM_UINT8  TAF_SDC_GetPnnFileType(VOS_VOID)
{
    VOS_UINT8                           ucSimType;
    TAF_SDC_PNN_FILE_INFO_STRU         *pstUsimPnnInfo = VOS_NULL_PTR;
    TAF_SDC_PNN_FILE_INFO_STRU         *pstSimPnnInfo = VOS_NULL_PTR;

    ucSimType      = 0;
    pstUsimPnnInfo = TAF_SDC_GetUsimPnnFileInfo();
    pstSimPnnInfo  = TAF_SDC_GetSimPnnFileInfo();

    (VOS_VOID)NAS_USIMMAPI_GetCardType(VOS_NULL_PTR, &ucSimType);

    if ((USIMM_CARD_SIM == ucSimType)
     && (0 != pstSimPnnInfo->ulPnnRecordNum))
    {
        return TAF_SDC_FILE_TYPE_SIM;
    }

    if (USIMM_CARD_USIM == ucSimType)
    {
        if (0 != pstUsimPnnInfo->ulPnnRecordNum)
        {
            return TAF_SDC_FILE_TYPE_USIM;
        }

        if (0 != pstSimPnnInfo->ulPnnRecordNum)
        {
            return TAF_SDC_FILE_TYPE_SIM;
        }
    }

    return TAF_SDC_FILE_TYPE_BUTT;
}


TAF_SDC_SPDI_FILE_INFO_STRU* TAF_SDC_GetSpdiFileInfo(VOS_VOID)
{
    VOS_UINT8                           ucSimType;
    TAF_SDC_SPDI_FILE_INFO_STRU        *pstUsimSpdiInfo = VOS_NULL_PTR;
    TAF_SDC_SPDI_FILE_INFO_STRU        *pstSimSpdiInfo = VOS_NULL_PTR;

    ucSimType       = 0;
    pstUsimSpdiInfo = TAF_SDC_GetUsimSpdiFileInfo();
    pstSimSpdiInfo  = TAF_SDC_GetSimSpdiFileInfo();

    (VOS_VOID)NAS_USIMMAPI_GetCardType(VOS_NULL_PTR, &ucSimType);

    if (USIMM_CARD_SIM == ucSimType)
    {
        return pstSimSpdiInfo;
    }

    if (USIMM_CARD_USIM == ucSimType)
    {
        if (0 != pstUsimSpdiInfo->ucServiceProviderPlmnNum)
        {
            return pstUsimSpdiInfo;
        }

        if (0 != pstSimSpdiInfo->ucServiceProviderPlmnNum)
        {
            return pstSimSpdiInfo;
        }
    }

    return pstUsimSpdiInfo;
}


VOS_UINT32  TAF_SDC_IsOperNameTypeSupport(
    TAF_SDC_OPER_NAME_TYPE_ENUM_UINT8   enOperNameType
)
{
    TAF_SDC_OPER_NAME_TYPE_PRIO_STRU   *pstOperNamePrio = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    pstOperNamePrio = TAF_SDC_GetOperNamePrio();

    for (ucIndex = 0; ucIndex < pstOperNamePrio->ucOperNamePrioNum; ucIndex++)
    {
        if (enOperNameType == pstOperNamePrio->aucOperNameTypePrio[ucIndex])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 TAF_SDC_CompareBcchPlmnwithSimPlmn_WithWildcard(
    TAF_SDC_PLMN_ID_STRU                *pstBccPlmnId,
    TAF_SDC_PLMN_ID_STRU                *pstSimPlmnId,
    VOS_CHAR                             cWildcard
)
{
    VOS_UINT32                          ulCmpRlst;
    VOS_UINT32                          ulMccCmpRlst;
    VOS_UINT32                          ulMncCmpRlst;

    /* �����Ҫ��ȷ�Ƚ�*/
    if (VOS_TRUE == TAF_SDC_GetPlmnExactlyCompareFlg())
    {
        ulCmpRlst = TAF_SDC_ExactlyCompareBcchPlmnwithSimPlmn_WithWildcard(pstBccPlmnId, pstSimPlmnId, cWildcard);

        return ulCmpRlst;
    }

    /* ͨ����Ƿ���Ч */
    if (VOS_FALSE == TAF_SDC_IsValidWildcard(cWildcard))
    {
        return VOS_FALSE;
    }

    /* ���MCC�Ƿ���ͬ */
    ulMccCmpRlst = TAF_SDC_CompareBcchMccwithSimMcc_WithWildcard(
                                        pstBccPlmnId->ulMcc,
                                        pstSimPlmnId->ulMcc,
                                        cWildcard,
                                        VOS_FALSE);
    if (VOS_FALSE == ulMccCmpRlst)
    {
        return VOS_FALSE;
    }



    /* MCC��ȣ��ټ��MNC�Ƿ���ͬ */
    ulMncCmpRlst = TAF_SDC_CompareBcchMncwithSimMnc_WithWildcard(
                                        pstBccPlmnId,
                                        pstSimPlmnId,
                                        cWildcard,
                                        VOS_FALSE);

    return ulMncCmpRlst;

}


TAF_SDC_PLMN_ID_STRU  TAF_SDC_GetImsiHomePlmn(
    VOS_UINT8                          *pucImsi,
    VOS_UINT8                           ucUsimMncLenth
)
{
    TAF_SDC_PLMN_ID_STRU                stHplmn;

    PS_MEM_SET(&stHplmn, 0, sizeof(TAF_SDC_PLMN_ID_STRU));

    stHplmn.ulMcc |= (pucImsi[1] & TAF_SDC_OCTET_HIGH_FOUR_BITS) >> TAF_SDC_OCTET_MOVE_FOUR_BITS;
    stHplmn.ulMcc |= (pucImsi[2] & TAF_SDC_OCTET_LOW_FOUR_BITS) << TAF_SDC_OCTET_MOVE_EIGHT_BITS;
    stHplmn.ulMcc |= (pucImsi[2] & TAF_SDC_OCTET_HIGH_FOUR_BITS) << TAF_SDC_OCTET_MOVE_TWELVE_BITS;
    stHplmn.ulMnc |= (pucImsi[3] & TAF_SDC_OCTET_LOW_FOUR_BITS);
    stHplmn.ulMnc |= (pucImsi[3] & TAF_SDC_OCTET_HIGH_FOUR_BITS) << TAF_SDC_OCTET_MOVE_FOUR_BITS;

    if ((TAF_SDC_MNC_LENGTH_TWO_BYTES_IN_IMSI == ucUsimMncLenth))
    {
        stHplmn.ulMnc |= (TAF_SDC_LOW_BYTE_INVALID) << TAF_SDC_OCTET_MOVE_SIXTEEN_BITS;
    }
    else
    {
        stHplmn.ulMnc |= (pucImsi[4] & TAF_SDC_OCTET_LOW_FOUR_BITS) << TAF_SDC_OCTET_MOVE_SIXTEEN_BITS;
    }

    /* �������ƶ��������⴦����Ҫ����,���2λMNC��HPLMN��Hplmn�б���,
      �ƶ�MCC=460,MNC=02��07�й��ƶ������⴦��
    */
    if (( 0x000604 == stHplmn.ulMcc )
     && ( (0x200   == (stHplmn.ulMnc & 0xFFFF ) )
       || (0x700   == (stHplmn.ulMnc & 0xFFFF))))
    {
        stHplmn.ulMnc &= 0xFF0000;
    }

    return stHplmn;
}



VOS_UINT32 TAF_SDC_IsImsSrvExist(VOID)
{
    VOS_UINT8                           ucImsVoiceFlag;
    VOS_UINT8                           ucImsSmsFlag;
    VOS_UINT8                           ucImsSsFlag;

    ucImsVoiceFlag  = TAF_SDC_GetImsCallExistFlg();
    ucImsSmsFlag    = TAF_SDC_GetImsSmsSrvExistFlg();
    ucImsSsFlag     = TAF_SDC_GetImsSsSrvExistFlg();

    if ((VOS_TRUE == ucImsVoiceFlag)
     || (VOS_TRUE == ucImsSmsFlag)
     || (VOS_TRUE == ucImsSsFlag))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;

}



VOS_UINT32 TAF_SDC_IsSysCfgRatSupportHrpdAndLte(VOS_VOID)
{
    /* syscfg set both support HRPD and LTE, we consider it's CL mode */
    VOS_UINT32                          ulLteSupport;

    ulLteSupport = TAF_SDC_IsSyscfgSupportSpecRat(TAF_SDC_SYS_MODE_LTE);

    if ((VOS_TRUE == TAF_SDC_IsSyscfgSupportSpecRat(TAF_SDC_SYS_MODE_EVDO))
     && (VOS_TRUE == ulLteSupport))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_SDC_IsPlmnIdValid(
    TAF_SDC_PLMN_ID_STRU               *pstPlmnId
)
{
    VOS_UINT32                          ulResult;

    /*
    The coding of this field is the responsibility of each administration but
    BCD coding shall be used. The MNC shall consist of 2 or 3 digits.
    If a network operator decides to use only two digits in the MNC,
    bits 5 to 8 of octet 7 shall be coded as "1111".
    MCC digit 2     MCC digit 1
    MNC digit 3     MCC digit 3
    MNC digit 2     MNC digit 1
    */

    ulResult = VOS_TRUE;

    if ( ((pstPlmnId->ulMcc & TAF_SDC_MCC_MNC_BYTE_VALUE_MASK) > TAF_SDC_MAX_DIGIT_VALUE)
      || (((pstPlmnId->ulMcc >> 8) & TAF_SDC_MCC_MNC_BYTE_VALUE_MASK) > TAF_SDC_MAX_DIGIT_VALUE)
      || (((pstPlmnId->ulMcc >> 16) & TAF_SDC_MCC_MNC_BYTE_VALUE_MASK) > TAF_SDC_MAX_DIGIT_VALUE))
    {
        ulResult = VOS_FALSE;
    }

    if ((((pstPlmnId->ulMnc) & TAF_SDC_MCC_MNC_BYTE_VALUE_MASK) > TAF_SDC_MAX_DIGIT_VALUE)
     || (((pstPlmnId->ulMnc >> 8) & TAF_SDC_MCC_MNC_BYTE_VALUE_MASK) > TAF_SDC_MAX_DIGIT_VALUE))
    {
        ulResult = VOS_FALSE;
    }

    if ((((pstPlmnId->ulMnc >> 16) & TAF_SDC_MCC_MNC_BYTE_VALUE_MASK) > TAF_SDC_MAX_DIGIT_VALUE)
     && (((pstPlmnId->ulMnc >> 16) & TAF_SDC_MCC_MNC_BYTE_VALUE_MASK) != TAF_SDC_LOW_BYTE_INVALID))
    {
        ulResult = VOS_FALSE;
    }

    return ulResult;
}




VOS_UINT32 TAF_SDC_IsPlatformRatCapNvimValid(
    PLATAFORM_RAT_CAPABILITY_STRU      *pstNvPlatformRatCap
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          j;

    /* ���ƽ̨֧�ֵĽ��뼼������Ϊ0��������Ч */
    if (0 == pstNvPlatformRatCap->usRatNum)
    {
        return VOS_FALSE;
    }

    for (i = 0; i < pstNvPlatformRatCap->usRatNum; i++)
    {
        /* ���ƽ̨֧�ֵĽ��뼼�����ظ���������Ч  */
        for (j = i + 1 ; j < pstNvPlatformRatCap->usRatNum; j++)
        {
            if (pstNvPlatformRatCap->aenRatList[i] == pstNvPlatformRatCap->aenRatList[j])
            {
                return VOS_FALSE;
            }
        }

        /* ���ƽ̨֧�ֵĽ��뼼�����ڻ����BUTT��������Ч */
        if (pstNvPlatformRatCap->aenRatList[i] >= PLATFORM_RAT_BUTT)
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}


TAF_SDC_PLATFORM_RAT_TYPE_ENUM_UINT8 TAF_SDC_ConvertNvPlatformRatToTafType(
    PLATFORM_RAT_TYPE_ENUM_UINT16       enNvPlatformRatCap
)
{
    TAF_SDC_PLATFORM_RAT_TYPE_ENUM_UINT8                    enTafSdcPlatformRat;

    switch (enNvPlatformRatCap)
    {
        case PLATFORM_RAT_GSM:
            enTafSdcPlatformRat = TAF_SDC_PLATFORM_RAT_GSM;
            break;

        case PLATFORM_RAT_WCDMA:
            enTafSdcPlatformRat = TAF_SDC_PLATFORM_RAT_WCDMA;
            break;

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
        case PLATFORM_RAT_TDS:
            enTafSdcPlatformRat = TAF_SDC_PLATFORM_RAT_TDS;
            break;
#endif

#if   (FEATURE_ON == FEATURE_LTE)
        case PLATFORM_RAT_LTE:
            enTafSdcPlatformRat = TAF_SDC_PLATFORM_RAT_LTE;
            break;
#endif
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        case PLATFORM_RAT_1X:
            enTafSdcPlatformRat = TAF_SDC_PLATFORM_RAT_1X;
            break;

        case PLATFORM_RAT_HRPD:
            enTafSdcPlatformRat = TAF_SDC_PLATFORM_RAT_HRPD;
            break;

#endif
        default:
            enTafSdcPlatformRat = TAF_SDC_PLATFORM_RAT_BUTT;
            break;
    }

    return enTafSdcPlatformRat;
}



VOS_UINT32 TAF_SDC_IsSyscfgSupportSpecRat(
    TAF_SDC_SYS_MODE_ENUM_UINT8         enSysMode
)
{
    VOS_UINT8                           i;
    TAF_SDC_RAT_PRIO_STRU              *pstRatPrioList;

    pstRatPrioList = TAF_SDC_GetMsPrioRatList();

    for (i = 0; i < pstRatPrioList->ucRatNum; i++)
    {
        if (enSysMode == pstRatPrioList->aenRatPrio[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}




VOS_UINT8 TAF_SDC_IsCurrentNormalServiceStatus(void)
{
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   ulPsServiceStatus;
    TAF_SDC_SERVICE_STATUS_ENUM_UINT8   ulCsServiceStatus;

    ulCsServiceStatus = TAF_SDC_GetCsServiceStatus();
    ulPsServiceStatus = TAF_SDC_GetPsServiceStatus();

    /* �����ܵķ���״̬ */
    if ((TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == ulCsServiceStatus)
     || (TAF_SDC_SERVICE_STATUS_NORMAL_SERVICE == ulPsServiceStatus))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }

}



VOS_UINT8 TAF_SDC_IsCurrentNormalRegStatus(void)
{
    TAF_SDC_REG_STATUS_ENUM_UINT8   ucPsRegStatus;
    TAF_SDC_REG_STATUS_ENUM_UINT8   ulCsRegStatus;

    ulCsRegStatus = TAF_SDC_GetCsRegStatus();
    ucPsRegStatus = TAF_SDC_GetPsRegStatus();

    /* �����ܵķ���״̬ */
    if ((TAF_SDC_REG_REGISTERED_HOME_NETWORK == ulCsRegStatus)
     || (TAF_SDC_REG_REGISTERED_ROAM == ulCsRegStatus))
    {
        return VOS_TRUE;
    }
    else if ((TAF_SDC_REG_REGISTERED_HOME_NETWORK == ucPsRegStatus)
          || (TAF_SDC_REG_REGISTERED_ROAM == ucPsRegStatus))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }

}



VOS_UINT8 TAF_SDC_IsCurrentAppNormalRegStatus(void)
{
    TAF_SDC_REG_STATUS_ENUM_UINT8   ucPsRegStatus;
    TAF_SDC_REG_STATUS_ENUM_UINT8   ulCsRegStatus;

    ulCsRegStatus = TAF_SDC_GetAppCsRegStatus();
    ucPsRegStatus = TAF_SDC_GetAppPsRegStatus();

    /* �����ܵķ���״̬ */
    if ((TAF_SDC_REG_REGISTERED_HOME_NETWORK == ulCsRegStatus)
     || (TAF_SDC_REG_REGISTERED_ROAM == ulCsRegStatus))
    {
        NAS_NORMAL_LOG1(WUEPS_PID_TAF, "TAF_SDC_IsCurrentAppNormalRegStatus(): App CS normal reg status ", ulCsRegStatus);
        return VOS_TRUE;
    }
    else if ((TAF_SDC_REG_REGISTERED_HOME_NETWORK == ucPsRegStatus)
          || (TAF_SDC_REG_REGISTERED_ROAM == ucPsRegStatus))
    {
        NAS_NORMAL_LOG1(WUEPS_PID_TAF, "TAF_SDC_IsCurrentAppNormalRegStatus(): App PS normal reg status ", ucPsRegStatus);
        return VOS_TRUE;
    }
    else
    {
        NAS_NORMAL_LOG2(WUEPS_PID_TAF, "TAF_SDC_IsCurrentAppNormalRegStatus(): App non-normal reg status CS, PS", ulCsRegStatus, ucPsRegStatus);
        return VOS_FALSE;
    }

}


VOS_VOID TAF_MMA_UpdateRoamFlag(
    TAF_SDC_REG_STATUS_ENUM_UINT8      enRegState
)
{
    switch (enRegState)
    {
        case TAF_SDC_REG_REGISTERED_HOME_NETWORK:
            TAF_SDC_SetRoamFlag(VOS_FALSE);
            break;

        case TAF_SDC_REG_REGISTERED_ROAM:
            TAF_SDC_SetRoamFlag(VOS_TRUE);
            break;

        default:
            break;
    }

    return;

}


VOS_UINT32 TAF_SDC_IsAppSepcDomainRegStaChg(
    TAF_SDC_SERVICE_DOMAIN_ENUM_UINT8   enCnDomainId,
    TAF_SDC_REG_STATUS_ENUM_UINT8       enRegState
)
{
    TAF_SDC_REG_STATUS_ENUM_UINT8       enAppCsRegState;
    TAF_SDC_REG_STATUS_ENUM_UINT8       enAppPsRegState;

    enAppCsRegState = TAF_SDC_GetAppCsRegStatus();
    enAppPsRegState = TAF_SDC_GetAppPsRegStatus();


    if (TAF_SDC_SERVICE_DOMAIN_CS== enCnDomainId)
    {
        /* cs��ע��״̬�仯ʱ�ϱ� */
        if (enAppCsRegState != enRegState)
        {
            return VOS_TRUE;
        }
    }
    else if (TAF_SDC_SERVICE_DOMAIN_PS == enCnDomainId)
    {
        /* ps��ע��״̬�仯ʱ�ϱ� */
        if (enAppPsRegState != enRegState)
        {
            return VOS_TRUE;
        }
    }
    else
    {
        return VOS_FALSE;
    }

    return VOS_FALSE;
}



VOS_UINT32 TAF_SDC_IsNeedReportRegStatusWhenNonnormalService(
    TAF_SDC_SERVICE_DOMAIN_ENUM_UINT8   enCnDomainId,
    TAF_SDC_REG_STATUS_ENUM_UINT8       enRegState
)
{
    TAF_SDC_NONNORMAL_REG_STATUS_MERGE_CFG_STRU            *pstNonnormalRegStatusMergeCfg = VOS_NULL_PTR;
    TAF_SDC_REG_STATUS_ENUM_UINT8                           enAppCsRegState;
    TAF_SDC_REG_STATUS_ENUM_UINT8                           enAppPsRegState;

    enAppCsRegState                 = TAF_SDC_GetAppCsRegStatus();
    enAppPsRegState                 = TAF_SDC_GetAppPsRegStatus();
    pstNonnormalRegStatusMergeCfg   = TAF_SDC_GetNonnormalRegStatusMergeCfg();

    if (VOS_TRUE == pstNonnormalRegStatusMergeCfg->ucMergeFlg)
    {
        if (TAF_SDC_CREG_TYPE_BREVITE == TAF_SDC_GetCregType())
        {
            /* CS���µ��ж� */
            if (TAF_SDC_SERVICE_DOMAIN_CS == enCnDomainId)
            {
                if (((NAS_MSCC_PIF_REG_NOT_REGISTERED_NOT_SEARCH == enAppCsRegState )
                  || (NAS_MSCC_PIF_REG_NOT_REGISTERED_SEARCHING == enAppCsRegState))
                && ((NAS_MSCC_PIF_REG_NOT_REGISTERED_SEARCHING == enRegState)
                 || (NAS_MSCC_PIF_REG_NOT_REGISTERED_NOT_SEARCH == enRegState)))
                {
                    return VOS_FALSE;
                }
            }
        }

        if (TAF_SDC_CREG_TYPE_BREVITE == TAF_SDC_GetCgregType())
        {

            /* PS���µ��ж� */
            if (TAF_SDC_SERVICE_DOMAIN_PS == enCnDomainId)
            {
                if (((NAS_MSCC_PIF_REG_NOT_REGISTERED_NOT_SEARCH == enAppPsRegState )
                  || (NAS_MSCC_PIF_REG_NOT_REGISTERED_SEARCHING == enAppPsRegState))
                && ((NAS_MSCC_PIF_REG_NOT_REGISTERED_SEARCHING == enRegState)
                 || (NAS_MSCC_PIF_REG_NOT_REGISTERED_NOT_SEARCH == enRegState)))
                {
                    return VOS_FALSE;
                }
            }
        }
    }
    return VOS_TRUE;
}


VOS_UINT32 TAF_SDC_IsSpecRatInRatList(
    TAF_MMA_RAT_TYPE_ENUM_UINT8         enRatType,
    TAF_MMA_RAT_ORDER_STRU             *pstRatList
)
{
    VOS_UINT8                           i;

    if (VOS_NULL_PTR == pstRatList)
    {
        return VOS_FALSE;
    }

    for (i = 0; i < TAF_SDC_MIN(pstRatList->ucRatOrderNum, TAF_PH_MAX_GUL_RAT_NUM); i++)
    {
        if (enRatType == pstRatList->aenRatOrder[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 TAF_SDC_CovertCimisNumToBcd(VOS_UINT32 ulSrcValude, VOS_UINT32 ulCodeMode)
{
    VOS_UINT32                          ulTempl   = 0;
    VOS_UINT32                          ulDstValue = 0;

    if (ulCodeMode >= 3)
    {
        ulTempl = ulSrcValude / 100;        /*��λ*/
        if (ulTempl + 1 <= 9)
        {
            ulDstValue += (ulTempl + 1) * 100;
        }
    }

    if (ulCodeMode >= 2)
    {
        ulTempl = (ulSrcValude / 10)  % 10; /*ʮλ*/
        if ( ulTempl+1 <= 9)
        {
            ulDstValue += (ulTempl+1 ) * 10;
        }
    }

    if (ulCodeMode >= 1)
    {
        ulTempl = ulSrcValude % 10;         /*��λ*/
        if (ulTempl +1 <= 9)
        {
            ulDstValue += ulTempl +1;
        }
    }

    return ulDstValue;
}


VOS_UINT32 TAF_SDC_ConvertCImsiBcdCode(
    VOS_UINT8                           *pucCIMSI,
    VOS_UINT8                           *pucStrCIMSI)
{
    VOS_UINT32                          ulMCC     = 0; /* ���Ҵ��� */
    VOS_UINT32                          ulMNC     = 0; /* ������� */
    VOS_UINT32                          ulMSIN_A  = 0; /* �ƶ��û���ݵ�0 - 3λ*/
    VOS_UINT32                          ulMSIN_B  = 0; /* �ƶ��û���ݵ�4 - 10λ*/
    VOS_UINT32                          ulMSIN_B1 = 0; /* �ƶ��û���ݵ�4 - 6λ*/
    VOS_UINT32                          ulMSIN_B2 = 0; /* �ƶ��û���ݵ�7λ*/
    VOS_UINT32                          ulMSIN_B3 = 0; /* �ƶ��û���ݵ�8 - 10λ*/
/*
    6F22��ʽ��
    00 AA AA BB BB BB CC 80 DD DD

    00����CLASS=0
    AA AA����MINǰ3λ����
    BB BB BB����MIN��7λ����
    CC����MNC����
    80����IMSI_M_PROGRAMMED=1��IMSI_M_ADD_NUM=0

    DD DD����MCC����
    ����CDMA2000Э����6F22�ĸ�ʽ����1���ֽں͵�7���ֽڶ��ø㶨���������Ǳ�־λ�����á�

    ����AA AA����
    1����MIN��ǰ3λΪ�������ݣ��ָ��3����λ�����ֱ�����ΪD1��D2��D3
    2����������NUM = D1 * 100 + D2 * 10 + D3 - 111����D1��D2��D3�г���0������10���㣩
    3����NUMת��Ϊ2�ֽ�С�˸�ʽ16���ƣ�λ��������ǰ����0����AA AA�������

    ����BB BB BB����
    1����MIN��4-6λΪ���ݣ�����AA AA�ļ��㲽��1��2���NUM����NUMת��Ϊ10bit��2���ƣ�������0��
    2����MIN��7λ����ȡ���������ձ�׼10����ת2���Ƶļ��㷽���������4bit��2���ƣ�������0��(�����������0������10����)
    3����MIN��8-10λΪ���ݣ�����AA AA�ļ��㲽��1��2���NUM����NUMת��Ϊ10bit��2���ƣ�������0��
    4����ǰ3���ļ�������Ĺ�24bit��2������ƴ������ת����3�ֽ�С�˸�ʽ16��������BB BB BB�������

    ����CC����
    1����MNC�ָ��2����λ�����ֱ�����ΪD1��D2
    2����������NUM = D1 * 10 + D2 - 11����D1��D2�г���0������10���㣩
    3����NUMת��Ϊ1�ֽ�16���ơ�CC�������

    ����DD DD����
    1����MCCΪ���ݣ�����AA AA�ļ��㷽���������2�ֽ�С�˸�ʽ16���ƣ���ΪDD DD

    ���ӣ�
    ��֪IMSI�ţ�460036591141482
    ��IMSI�У�
    MCC��460
    MNC��03
    MIN��6591141482

    ����AA AA
    MINǰ3λ659��NUM = 6 * 100 + 5 * 10 + 9 - 111 = 548 ת����2�ֽ�С�˸�ʽ16����Ϊ��24 02

    ����BB BB BB
    MIN��4-6λ114��NUM = 1 * 100 + 1 * 10 + 4 - 111 = 3 ת����10bit��2����Ϊ��0000000011
    MIN��7λ1��ת��Ϊ4bit��2����Ϊ��0001
    MIN��8-10λ482��NUM = 4 * 100 + 8 * 10 + 2 - 111 = 371 ת����10bit��2����Ϊ��0101110011
    24bit��2������Ϊ��000000001100010101110011��ת����3�ֽ�С�˸�ʽ16����Ϊ��73 C5 00

    ����CC
    MNCΪ03��NUM = 10 * 10 + 3 - 11 = 92��ת����1�ֽ�16����Ϊ5C

    ����DD DD
    MCCΪ460��NUM = 4 * 100 + 6 * 10 + 10 - 111 = 359 ת����2�ֽ�С�˸�ʽ16����Ϊ��67 01

    ���˵õ�6F22�ļ�����ӦΪ
    00 24 02 73 C5 00 5C 80 67 01
    460036591141482
*/
    pucStrCIMSI[0] = 0x08;

    ulMCC     = pucCIMSI[8];
    ulMCC     = 0x0000FFFF & ( ulMCC | (pucCIMSI[9] << 8));
    ulMCC     = TAF_SDC_CovertCimisNumToBcd(ulMCC,3);       /* ���Ҵ��� */
    pucStrCIMSI[1] = 0x09 | TAF_STD_ConvertDeciDigitToBcd((VOS_UINT8)(ulMCC/100), VOS_TRUE);
    pucStrCIMSI[2] = TAF_STD_ConvertDeciDigitToBcd((VOS_UINT8)(ulMCC%100), VOS_TRUE);

    ulMNC     = pucCIMSI[6];
    ulMNC     = TAF_SDC_CovertCimisNumToBcd(ulMNC,2);       /* ������� */
    pucStrCIMSI[3] = TAF_STD_ConvertDeciDigitToBcd((VOS_UINT8)(ulMNC), VOS_TRUE);

    ulMSIN_A  = pucCIMSI[1];
    ulMSIN_A  = 0x0000FFFF & ( ulMSIN_A | (pucCIMSI[2] << 8));
    ulMSIN_A  = TAF_SDC_CovertCimisNumToBcd(ulMSIN_A, 3);   /* �ƶ��û���ݵ�1 - 3λ*/
    pucStrCIMSI[4] = TAF_STD_ConvertDeciDigitToBcd((VOS_UINT8)(ulMSIN_A/10), VOS_TRUE);
    pucStrCIMSI[5] = TAF_STD_ConvertDeciDigitToBcd((VOS_UINT8)(ulMSIN_A%10), VOS_FALSE);

    ulMSIN_B  = pucCIMSI[3];
    ulMSIN_B  = 0x0000FFFF & (ulMSIN_B | (pucCIMSI[4] << 8));
    ulMSIN_B  = 0x00FFFFFF & (ulMSIN_B | (pucCIMSI[5] << 16));

    ulMSIN_B1 = ulMSIN_B >> 14;
    ulMSIN_B1 = TAF_SDC_CovertCimisNumToBcd(ulMSIN_B1, 3); /* �ƶ��û���ݵ�4 - 6λ */
    pucStrCIMSI[5] |= (TAF_STD_ConvertDeciDigitToBcd((VOS_UINT8)(ulMSIN_B1/100), VOS_TRUE) & 0xF0);
    pucStrCIMSI[6]  = TAF_STD_ConvertDeciDigitToBcd((VOS_UINT8)(ulMSIN_B1%100), VOS_TRUE);

    ulMSIN_B2 = (ulMSIN_B & 0x3c00)>>10;         /* �ƶ��û���ݵ�7λ */
    ulMSIN_B2 = ulMSIN_B2 % 10;

    pucStrCIMSI[7]  = (ulMSIN_B2 & 0x0F);

    ulMSIN_B3 = ulMSIN_B & 0x3FF;
    ulMSIN_B3 = TAF_SDC_CovertCimisNumToBcd(ulMSIN_B3, 3);  /* �ƶ��û���ݵ�8 - 10λ */
    pucStrCIMSI[7]|= TAF_STD_ConvertDeciDigitToBcd((VOS_UINT8)(ulMSIN_B3/100), VOS_TRUE) & 0xF0;
    pucStrCIMSI[8] = TAF_STD_ConvertDeciDigitToBcd((VOS_UINT8)(ulMSIN_B3%100), VOS_TRUE);

    return VOS_OK;
}
#endif

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

