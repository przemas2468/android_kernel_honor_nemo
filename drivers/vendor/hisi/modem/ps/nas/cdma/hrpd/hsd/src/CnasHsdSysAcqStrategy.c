


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/


#include "PsCommonDef.h"
#include "stdlib.h"
#include "CnasHsdSysAcqStrategy.h"
#include "CnasHsdMntn.h"
#include "CnasCcb.h"
#include "CnasPrlApi.h"
#include "CnasHsdAvoidStrategy.h"
#include "CnasHsdFsmSysAcq.h"
#include "hsd_mscc_pif.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_CNAS_HSD_SYS_ACQ_STRATEGY_C

/*lint -save -e958*/

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/* ��������������Ӧ�������б��ʼ��������������״̬����ʼ��ʱʹ�� */
CNAS_HSD_SCAN_CHANNEL_LIST_BUILD_TBL_STRU           g_astCnasHsdScanChanListBuildTbl[] =
{
    {CNAS_HSD_SYS_ACQ_SCENE_SWITCH_ON,              CNAS_HSD_BuildScanChanList_SwitchOn},
    {CNAS_HSD_SYS_ACQ_SCENE_SYSTEM_LOST,            CNAS_HSD_BuildScanChanList_SystemLost},
    {CNAS_HSD_SYS_ACQ_SCENE_REDIRECTION,            CNAS_HSD_BuildScanChanList_Redirection},
    {CNAS_HSD_SYS_ACQ_SCENE_OOC_MRU0,               CNAS_HSD_BuildScanChanList_OocMru0},
    {CNAS_HSD_SYS_ACQ_SCENE_DATA_CALL_QUICK,        CNAS_HSD_BuildScanChanList_DataCallQuick},
    {CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH,           CNAS_HSD_BuildScanChanList_PilotSearch},
    {CNAS_HSD_SYS_ACQ_SCENE_HIGH_PRIORITY,          CNAS_HSD_BuildScanChanList_HighPriority},
    {CNAS_HSD_SYS_ACQ_SCENE_MSCC_OTHER,             CNAS_HSD_BuildScanChanList_SystemLost},
    {CNAS_HSD_SYS_ACQ_SCENE_CFREQ_LOCK,             CNAS_HSD_BuildScanChanList_CFreqLock},
    {CNAS_HSD_SYS_ACQ_SCENE_OOC_SWITCH_ON,          CNAS_HSD_BuildScanChanList_SwitchOn},
};

/* �ڶ��������²���MRU0�����к� */
VOS_UINT8                                           g_aucCnasHsdSystemLostMru0Pos[CNAS_HSD_MRU0_NUM_4_IN_SCAN_LIST] = {2,5,9,14};

/* �������������»�ȡɨ��Ƶ���Ӧ����Table */
CNAS_HSD_GET_SCAN_CHANNEL_LIST_TBL_STRU             g_astCnasHsdGetScanChanListTbl[] =
{
    {CNAS_HSD_SYS_ACQ_SCENE_SWITCH_ON,              CNAS_HSD_GetScanChanListFromHrpdList_SwitchOn},
    {CNAS_HSD_SYS_ACQ_SCENE_SYSTEM_LOST,            CNAS_HSD_GetScanChanListFromHrpdList_SystemLost},
    {CNAS_HSD_SYS_ACQ_SCENE_REDIRECTION,            CNAS_HSD_GetScanChanListFromHrpdList_Redirection},
    {CNAS_HSD_SYS_ACQ_SCENE_OOC_MRU0,               CNAS_HSD_GetScanChanListFromMru0_Ooc},
    {CNAS_HSD_SYS_ACQ_SCENE_DATA_CALL_QUICK,        CNAS_HSD_GetScanChanListFromHrpdList_DataCallQuick},
    {CNAS_HSD_SYS_ACQ_SCENE_PILOT_SEARCH,           CNAS_HSD_GetScanChanListFromHrpdList_SwitchOn},
    {CNAS_HSD_SYS_ACQ_SCENE_HIGH_PRIORITY,          CNAS_HSD_GetScanChanListFromHrpdList_HighPriority},
    {CNAS_HSD_SYS_ACQ_SCENE_MSCC_OTHER,             CNAS_HSD_GetScanChanListFromHrpdList_SystemLost},
    {CNAS_HSD_SYS_ACQ_SCENE_OOC_SWITCH_ON,          CNAS_HSD_GetScanChanListFromHrpdList_SwitchOn},
};


VOS_UINT32 CNAS_HSD_IsAcqedHrpdSysMatchTheSysRecs
(
    CNAS_PRL_HRPD_SYSTEM_STRU                              *pstHrpdSys,
    VOS_UINT8                                              *pucPrio,
    VOS_UINT16                                             *pusHrpdSysListMatchIndex
)
{
    VOS_UINT16                                              i;
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstHrpdSysList;

    pstHrpdSysList                      = CNAS_HSD_GetSysRecListAddr();

    CNAS_HSD_GetAcqedHrpdSysMatchIndexList(pstHrpdSys);

    for (i = 0; i < pstHrpdSysList->usHrpdSysNum; i++)
    {
        if (VOS_TRUE == CNAS_HSD_IsAcqedHrpdSysInMatchIndexList(pstHrpdSysList->pstHrpdSysItem[i].usSysIndex))
        {
            *pucPrio                    = pstHrpdSysList->pstHrpdSysItem[i].ucPrioLevel;
            *pusHrpdSysListMatchIndex   = i;
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 CNAS_HSD_RefreshScanListByMatchLvl
(
    VOS_UINT16                          usScanListPos,
    VOS_UINT8                           ucPrio
)
{

    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstScanList;
    CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU   stHsdChannelInfo;
    VOS_UINT16                          i;
    VOS_UINT16                          usIndex;

    usIndex                             = 0;
    pstScanList                         = CNAS_HSD_GetScanFreqListAddr();

    /* �����ǰ��PRIO���Ѿ��е�ǰ��band channel����Ҫ�޸�scan list*/
    for (i = usScanListPos + 1; i < pstScanList->usAvailFreqNum; i++)
    {
        if (pstScanList->pstScanFreqItem[i].ucPrioLevel == ucPrio)
        {
            break;
        }
    }

    /* ��ǰlevel�����һ��Ƶ��ƥ������һ���ȼ�����ֱ��פ�� */
    if (i == (usScanListPos + 1))
    {
        return VOS_TRUE;
    }

    usIndex = i - 1;

    PS_MEM_SET(&stHsdChannelInfo, 0, sizeof(CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU));

    /* ���õ�CNF�ĵ�ǰλ�õ�band channel �������� */
    PS_MEM_CPY(&stHsdChannelInfo,
               &pstScanList->pstScanFreqItem[usScanListPos],
               sizeof(CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU));

    stHsdChannelInfo.ucPrioLevel    = ucPrio;

    /* mem move */
    PS_MEM_MOVE(&pstScanList->pstScanFreqItem[usScanListPos],
                &pstScanList->pstScanFreqItem[usScanListPos + 1],
                (usIndex - usScanListPos) * (sizeof(CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU)));

    /* ��ֵ���һ�� */
    PS_MEM_CPY(&pstScanList->pstScanFreqItem[usIndex],
               &stHsdChannelInfo,
               sizeof(CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU));

    /* ��ǰ��Index ��1 */
    pstScanList->usCurrIndex--;

    return VOS_FALSE;
}


VOS_UINT32 CNAS_HSD_IsCurrFreqStay
(
    CNAS_PRL_HRPD_SYSTEM_STRU          *pstHrpdSys
)
{
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstScanList;
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysList;
    VOS_UINT8                           ucMatchPrio;
    VOS_UINT8                           ucCurrPrio;
    VOS_UINT16                          usMatchedSysIndex;
    VOS_UINT16                          usCurrIndex;
    CNAS_PRL_FREQENCY_CHANNEL_STRU      stCFreqLock;
    CNAS_HSD_CFREQ_LOCK_SET_PARA_STRU  *pstCFreqLockPara;
    CNAS_HSD_1X_SYSTEM_INFO_STRU       *pst1xSys;
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32 enModeType;

    enModeType                          = CNAS_HSD_GetModeType();
    pst1xSys                            = CNAS_HSD_Get1xSystemInfo();
    pstScanList                         = CNAS_HSD_GetScanFreqListAddr();
    pstHrpdSysList                      = CNAS_HSD_GetSysRecListAddr();
    usCurrIndex                         = pstScanList->usCurrIndex - 1;
    ucCurrPrio                          = pstScanList->pstScanFreqItem[usCurrIndex].ucPrioLevel;

    if (VOS_TRUE == CNAS_HSD_GetFreqLockMode())
    {
        pstCFreqLockPara                = CNAS_HSD_GetFreqLockAddr();

        stCFreqLock.enBandClass         = pstCFreqLockPara->usHrpdBandClass;
        stCFreqLock.usChannel           = pstCFreqLockPara->usHrpdFreq;

        if (VOS_TRUE == CNAS_PRL_CmpFreqValue(&pstHrpdSys->stFreq, &stCFreqLock))
        {
            return VOS_TRUE;
        }
        else
        {
            return VOS_FALSE;
        }
    }

    /* ϵͳ��ƥ������ */
    if (VOS_TRUE == CNAS_HSD_IsAcqedHrpdSysMatchTheSysRecs(pstHrpdSys, &ucMatchPrio, &usMatchedSysIndex))
    {
        if (CNAS_PRL_PREF_NEG_SYS_NEG == pstHrpdSysList->pstHrpdSysItem[usMatchedSysIndex].enPrefNegSys)
        {
            return VOS_FALSE;
        }

        if (CNAS_HSD_FREQ_TYPE_REDIRECTION == pstScanList->pstScanFreqItem[usCurrIndex].enFreqType)
        {
            return VOS_TRUE;
        }

        /* ���HYBRID���ҵ�ǰ1x�����ڣ�ֱ��פ�� */
        if ((CNAS_HSD_1X_SYSTEM_UNAVAILABLE == pst1xSys->enStatus)
        &&  (CNAS_HSD_SYS_MODE_HYBRID == enModeType))
        {
            return VOS_TRUE;
        }

        /* ���ƥ�䵽�����ȼ��ȵ�ǰSCANList�ĸ߻�����ȶ��Ҳ���NEG��ֱ��פ����
            ������ڵ�ǰ�޸�SCANLIST�� */
        if (ucCurrPrio >= ucMatchPrio)
        {
            return VOS_TRUE;
        }
        else
        {
            return CNAS_HSD_RefreshScanListByMatchLvl(usCurrIndex, ucMatchPrio);
        }
    }
    else
    {
        return VOS_FALSE;
    }
}


VOS_VOID CNAS_HSD_RefreshScanListIndex(VOS_UINT16 usFreqNumUsed)
{
    VOS_UINT16                          usCurIndex;

    usCurIndex = CNAS_HSD_GetCurScanFreqIndex();

    usCurIndex += usFreqNumUsed;

    CNAS_HSD_SetCurScanFreqIndex(usCurIndex);

    return;
}


VOS_VOID CNAS_HSD_BuildHrpdScanList(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32  enSysAcqScene
)
{
    VOS_UINT32                          i;
    pCNasHsdScanChanListBuildFunc       funSysScanListBuild    = VOS_NULL_PTR;

    /* ���ϵͳ��������� */
    CNAS_HSD_InitSysRecList(CNAS_HSD_INIT_CTX_BUILD_FREQ, CNAS_HSD_GetSysRecListAddr());
    CNAS_HSD_InitScanChanList(CNAS_HSD_INIT_CTX_BUILD_FREQ, CNAS_HSD_GetScanFreqListAddr());

    /* ��������Ƶ */
    if (VOS_TRUE == CNAS_HSD_GetFreqLockMode())
    {
        enSysAcqScene = CNAS_HSD_SYS_ACQ_SCENE_CFREQ_LOCK;
    }

    /* ���ݳ�����g_astCnasHsdAcqSysListBuildTbl���������б��캯�� */
    /* ���ݳ�����Ƶ����Ϣ��SYS RECS ����SCAN LIST */
    for (i = 0; i < (sizeof(g_astCnasHsdScanChanListBuildTbl)/sizeof(CNAS_HSD_SCAN_CHANNEL_LIST_BUILD_TBL_STRU)); i++)
    {
        if (enSysAcqScene == g_astCnasHsdScanChanListBuildTbl[i].enSysAcqScene)
        {
            funSysScanListBuild = g_astCnasHsdScanChanListBuildTbl[i].pfunScanChanListBuild;
            break;
        }
    }

    /* ���ҳɹ�ʱ���ó�ʼ���������й��� */
    if (VOS_NULL_PTR != funSysScanListBuild)
    {
        funSysScanListBuild(enSysAcqScene);
    }

    /* �洢��ǰƥ���1Xϵͳ*/
    PS_MEM_CPY(CNAS_HSD_GetScanListMatched1xSystemInfo(),
               CNAS_HSD_Get1xSystemInfo(),
               sizeof(CNAS_HSD_1X_SYSTEM_INFO_STRU));


    return;
}


VOS_VOID CNAS_HSD_BuildScanChanList_SwitchOn(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene
)
{
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32                      enModeType;
    CNAS_HSD_1X_SYSTEM_INFO_STRU                           *pst1xSysInfo;
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstSysList;
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                      *pstScanList;

    pstScanList     = CNAS_HSD_GetScanFreqListAddr();
    pstSysList      = CNAS_HSD_GetSysRecListAddr();
    enModeType      = CNAS_HSD_GetModeType();
    pst1xSysInfo    = CNAS_HSD_Get1xSystemInfo();

    /* ����EVDO LIST */
    if (VOS_FALSE == CNAS_HSD_CreateHrpdSysList(enModeType, pst1xSysInfo, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_SwitchOn: create hrpd list fail!");
        return;
    }

    CNAS_HSD_LogSysRecList(pstSysList, enSysAcqScene);

    /* ����SWITCH ON SCAN LIST */
    if (VOS_FALSE == CNAS_HSD_CreateScanFreqList(enSysAcqScene, pstScanList, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_SwitchOn: create freq list fail!");
        return;
    }

    CNAS_HSD_LogScanFreqList(pstScanList, enSysAcqScene);

    return;
}


VOS_VOID CNAS_HSD_BuildScanChanList_PilotSearch(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene
)
{
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32                      enModeType;
    CNAS_HSD_1X_SYSTEM_INFO_STRU                           *pst1xSysInfo;
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstSysList;
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                      *pstScanList;

    pstScanList     = CNAS_HSD_GetScanFreqListAddr();
    pstSysList      = CNAS_HSD_GetSysRecListAddr();
    enModeType      = CNAS_HSD_GetModeType();
    pst1xSysInfo    = CNAS_HSD_Get1xSystemInfo();

    /* ����EVDO LIST */
    if (VOS_FALSE == CNAS_HSD_CreateHrpdSysList(enModeType, pst1xSysInfo, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_PilotSearch: create hrpd list fail!");
        return;
    }

    CNAS_HSD_LogSysRecList(pstSysList, enSysAcqScene);

    /* ����SWITCH ON SCAN LIST */
    if (VOS_FALSE == CNAS_HSD_CreateScanFreqList(enSysAcqScene, pstScanList, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_PilotSearch: create freq list fail!");
        return;
    }

    CNAS_HSD_LogScanFreqList(pstScanList, enSysAcqScene);

    return;
}

VOS_UINT32 CNAS_HSD_CreateHrpdSysList(
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32                      enModeType,
    CNAS_HSD_1X_SYSTEM_INFO_STRU                           *pst1xSysInfo,
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstSysList
)
{
    VOS_UINT32                                              ulResult;
    CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU              *pstGeoListInfo = VOS_NULL_PTR;

    if (CNAS_HSD_SYS_MODE_HYBRID == enModeType)
    {
        if (CNAS_HSD_1X_SYSTEM_UNAVAILABLE == pst1xSysInfo->enStatus)
        {
            ulResult = CNAS_HSD_CreateAllHrpdSysRecs(CNAS_PRL_CHECK_ASSN_INCL, CNAS_PRL_SORT_NEEDED, pstSysList);
        }
        else
        {
                /* ���ݵ�ǰ��1Xϵͳ��ȡ��ƥ���GEO��������XSD�ṩ�ӿ� */
            pstGeoListInfo  = (CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU*)VOS_MemAlloc(UEPS_PID_HSD, DYNAMIC_MEM_PT, sizeof(CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU));

            if (VOS_NULL_PTR == pstGeoListInfo)
            {
                CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_CreateHrpdSysList: pstGeoListInfo NULL_PTR!");
                return VOS_FALSE;
            }

            PS_MEM_SET(pstGeoListInfo, 0, sizeof(CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU));

            ulResult = CNAS_HSD_Create1xMatchedHrpdSysRecs(&pst1xSysInfo->st1xSys, pstSysList,pstGeoListInfo);

            VOS_MemFree(UEPS_PID_HSD, pstGeoListInfo);

        }
    }
    else
    {
        ulResult = CNAS_HSD_CreateAllHrpdSysRecs(CNAS_PRL_NO_CHECK_ASSN_INCL, CNAS_PRL_SORT_NEEDED, pstSysList);
    }

    return ulResult;
}

VOS_UINT32 CNAS_HSD_GetScanChanelList(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32  enSysAcqScene,
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysRecList,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstHrpdScanFreqList
)
{
    pCNasHsdGetScanChanListFunc         funSysGetScanList    = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          i;

    ulResult                  = VOS_FALSE;

    for (i = 0; i < (sizeof(g_astCnasHsdGetScanChanListTbl) / sizeof(CNAS_HSD_GET_SCAN_CHANNEL_LIST_TBL_STRU)); i++)
    {
        if (enSysAcqScene     == g_astCnasHsdGetScanChanListTbl[i].enSysAcqScene)
        {
            funSysGetScanList = g_astCnasHsdGetScanChanListTbl[i].pfunGetScanChanList;
            break;
        }
    }

    /* ���ҳɹ�ʱ���ó�ʼ���������й��� */
    if (VOS_NULL_PTR != funSysGetScanList)
    {
        ulResult     = funSysGetScanList(pstHrpdSysRecList, pstHrpdScanFreqList);
    }

    return ulResult;
}


VOS_UINT32 CNAS_HSD_CreateScanFreqList(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32  enSysAcqScene,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstScanList,
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstSysList
)
{
    VOS_UINT16                          usTotalFreqNum;

    usTotalFreqNum = CNAS_HSD_CalcFreqNumForScanList(enSysAcqScene, pstSysList);

    if (0 == usTotalFreqNum)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_CreateScanFreqList: calc total freq number fail!");
        return VOS_FALSE;
    }

    if (VOS_FALSE == CNAS_HSD_MallocMemForScanList(pstScanList, usTotalFreqNum))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_CreateScanFreqList: malloc memory fail!");
        return VOS_FALSE;
    }

    /* ͨ��HRPD LIST ����SCAN LIST */
    return CNAS_HSD_GetScanChanelList(enSysAcqScene, pstSysList, pstScanList);
}


VOS_UINT16 CNAS_HSD_CalcFreqNumForScanList(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32  enSysAcqScene,
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstSysList
)
{
    VOS_UINT16                          usTotalFreqNum;

    usTotalFreqNum                      = 0;

    if (CNAS_HSD_SYS_ACQ_SCENE_REDIRECTION == enSysAcqScene)
    {
        /* ��ȡREDIRECTION��Ƶ������:�ض���ֻ������෢���������������б��м��� */
        usTotalFreqNum = CNAS_HSD_GetRedirectionFreqNum();
        return usTotalFreqNum;
    }

    /* �����ȼ�������ֻ���������ȼ���Ƶ����� */
    if (CNAS_HSD_SYS_ACQ_SCENE_HIGH_PRIORITY == enSysAcqScene)
    {
        usTotalFreqNum = CNAS_HSD_GetHighPriorityFreqNum();
        return usTotalFreqNum;
    }

    if (CNAS_HSD_SYS_ACQ_SCENE_OOC_MRU0 == enSysAcqScene)
    {
        return CNAS_HSD_MRU0_NUM_1_IN_SCAN_LIST;
    }

    if ((CNAS_HSD_SYS_ACQ_SCENE_SYSTEM_LOST     == enSysAcqScene)
      ||(CNAS_HSD_SYS_ACQ_SCENE_MSCC_OTHER      == enSysAcqScene))
    {
        usTotalFreqNum                  += CNAS_HSD_MRU0_NUM_4_IN_SCAN_LIST;
    }

    /* MRU ����Ƶ����� */
    usTotalFreqNum                      += CNAS_HSD_GetMruListNum();

    /* Hrpd LIST����Ƶ����� */
    usTotalFreqNum                      += CNAS_HSD_CalcMatchHrpdSystemFreqNum(pstSysList);

    return usTotalFreqNum;
}


VOS_UINT32 CNAS_HSD_MallocMemForScanList(
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstScanList,
    VOS_UINT16                          usTotalFreqNum
)
{
    if (VOS_NULL_PTR != pstScanList->pstScanFreqItem)
    {
        PS_MEM_FREE(UEPS_PID_HSD, pstScanList->pstScanFreqItem);

        pstScanList->usTotalFreqNum  = 0;
        pstScanList->usAvailFreqNum  = 0;
        pstScanList->usCurrIndex     = 0;
        pstScanList->ucLowPrioLevel  = CNAS_PRL_PRIO_LEVEL_1;
    }

    /* ����ռ� */
    pstScanList->pstScanFreqItem = (CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU*)PS_MEM_ALLOC(UEPS_PID_HSD,
                                    usTotalFreqNum * sizeof(CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU));

    if (VOS_NULL_PTR == pstScanList->pstScanFreqItem)
    {
        return VOS_FALSE;
    }

    /* ���� */
    PS_MEM_SET(pstScanList->pstScanFreqItem, 0, usTotalFreqNum * sizeof(CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU));

    pstScanList->usTotalFreqNum  = usTotalFreqNum;
    pstScanList->usAvailFreqNum  = 0;
    pstScanList->usCurrIndex     = 0;
    pstScanList->ucLowPrioLevel  = CNAS_PRL_PRIO_LEVEL_1;

    return VOS_TRUE;
}

VOS_UINT32 CNAS_HSD_Create1xMatchedHrpdSysRecs(
    CNAS_PRL_1X_SYSTEM_STRU                                *pst1xSys,
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstSysList,
    CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU              *pstGeoListInfo
)
{
    VOS_UINT32                                              i;
    VOS_UINT16                                              usHrpdSysNum;
    VOS_UINT16                                              usRecHrpdSysIndex;
    VOS_UINT16                                              usAvilHrpdSysNum;
    VOS_UINT8                                               aucPivotDeltPrio[CNAS_PRL_MAX_MATCHING_GEO_NUM] = {0};

    usRecHrpdSysIndex   = 0;

    /* ���ݵ�ǰ��1Xϵͳ��ȡ��ƥ���GEO��������XSD�ṩ�ӿ� */
    if (VOS_ERR == CNAS_HSD_Get1XMostMatchedGeoList(pst1xSys, pstGeoListInfo))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Create1xMatchedHrpdSysRecs: CNAS_HSD_Get1XMostMatchedGeoListfail!");
        return VOS_FALSE;
    }

    if (0 == pstGeoListInfo->ucGeoNum)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Create1xMatchedHrpdSysRecs: get 1x sys geo list fail!");
        return VOS_FALSE;
    }

    /* ������ڶ��GEO,��Ҫ���ÿ����ƥ���1x֧������ȼ� */
    if (1 < pstGeoListInfo->ucGeoNum)
    {
        if (VOS_ERR == CNAS_HSD_GetPivot1xSysPriosInGeos(pstGeoListInfo, aucPivotDeltPrio))
        {
            CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Create1xMatchedHrpdSysRecs: CNAS_HSD_GetPivot1xSysPriosInGeos fail!");

            return VOS_FALSE;
        }
    }

    usHrpdSysNum = 0;

    /* �õ�HRPD SYS REC ������ */
    for (i  = 0; i < pstGeoListInfo->ucGeoNum; i++)
    {
        usHrpdSysNum += CNAS_HSD_GetHybridHrpdSysRecNum(&pstGeoListInfo->astGeoInfoList[i]);
    }

    if (0 == usHrpdSysNum)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Create1xMatchedHrpdSysRecs: get hrpd sys rec num fail!");
        return VOS_FALSE;
    }

    /* ΪSYS REC ����ռ� */
    if (VOS_FALSE == CNAS_HSD_MallocMemForSysRec(usHrpdSysNum, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Create1xMatchedHrpdSysRecs: hrpd sys rec malloc memory fail!");
        return VOS_FALSE;
    }

    usAvilHrpdSysNum = usHrpdSysNum;

    /* ��ȡHRPD����Ϣ */
    /* ��Ӷ��GEO��ʱ��ÿ��GEO����ʼ���ȼ��� 1 + ����֧��Ĳ�ֵ */
    for (i  = 0; i < pstGeoListInfo->ucGeoNum; i++)
    {
        /* �õ�HRPD���б���� */
        usRecHrpdSysIndex    += CNAS_HSD_GetHybridHrpdSysRecList(&pstGeoListInfo->astGeoInfoList[i],
                                                                 &pstSysList->pstHrpdSysItem[usRecHrpdSysIndex],
                                                                 usAvilHrpdSysNum,
                                                                 aucPivotDeltPrio[i]);
        usAvilHrpdSysNum  = usHrpdSysNum - usRecHrpdSysIndex;

        if ((0 == usAvilHrpdSysNum) || (usHrpdSysNum <= usRecHrpdSysIndex))
        {
            CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_Create1xMatchedHrpdSysRecs: hrpd sys num full!");
            break;
        }
    }

    if (VOS_NULL == usRecHrpdSysIndex)
    {
        pstSysList->usHrpdSysNum = VOS_NULL;
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Create1xMatchedHrpdSysRecs: get hrpd sys rec list fail!");
        return VOS_FALSE;
    }

    pstSysList->usHrpdSysNum = usRecHrpdSysIndex;

    /* ��Hrpdϵͳ��Ϣ���� */
    CNAS_HSD_SortHrpdSystem(pstSysList);

    return VOS_TRUE;
}

VOS_UINT32 CNAS_HSD_MallocMemForSysRec(
    VOS_UINT16                          usSysRecNum,
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstSysList
)
{
    if (VOS_NULL_PTR != pstSysList->pstHrpdSysItem)
    {
        pstSysList->usHrpdSysNum    = 0;
        PS_MEM_FREE(UEPS_PID_HSD, pstSysList->pstHrpdSysItem);
    }

    pstSysList->pstHrpdSysItem = (CNAS_HSD_HRPD_SYS_REC_ITEM_STRU*)PS_MEM_ALLOC(
                                    UEPS_PID_HSD, usSysRecNum * (sizeof(CNAS_HSD_HRPD_SYS_REC_ITEM_STRU)));

    if (VOS_NULL_PTR == pstSysList->pstHrpdSysItem)
    {
        return VOS_FALSE;
    }

    pstSysList->usHrpdSysNum = usSysRecNum;
    PS_MEM_SET(pstSysList->pstHrpdSysItem, 0, usSysRecNum * (sizeof(CNAS_HSD_HRPD_SYS_REC_ITEM_STRU)));

    return VOS_TRUE;
}

VOS_UINT32 CNAS_HSD_CreateAllHrpdSysRecs(
    CNAS_PRL_CHECK_ASSN_INCL_ENUM_UINT16          enCheckAssnIncl,
    CNAS_PRL_SORT_ENUM_UINT16                     enIsSortNeeded,
    CNAS_HSD_HRPD_SYS_LIST_STRU                  *pstSysList
)
{
    VOS_UINT16                          usHrpdSysNum;

    /* �õ�HRPD SYS REC ������ */
    usHrpdSysNum = CNAS_HSD_GetNonHybridHrpdSysRecNum(enCheckAssnIncl);

    if (0 == usHrpdSysNum)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_CreateAllHrpdSysRecs: get hypd sys rec number fail!");
        return VOS_FALSE;
    }

    /* ΪHRPD SYS RECS ����ռ� */
    if (VOS_FALSE == CNAS_HSD_MallocMemForSysRec(usHrpdSysNum, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_CreateAllHrpdSysRecs: hypd sys rec malloc memory fail!");
        return VOS_FALSE;
    }

    if (VOS_FALSE == CNAS_HSD_GetNonHybridHrpdSysRecList(enCheckAssnIncl, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_CreateAllHrpdSysRecs: get hypd sys rec list fail!");
        return VOS_FALSE;
    }

    /* ��HRPD SYS RECS �������� */
    if (CNAS_PRL_SORT_NEEDED == enIsSortNeeded)
    {
        CNAS_HSD_SortHrpdSystem(pstSysList);
    }

    return VOS_TRUE;
}


VOS_VOID CNAS_HSD_BuildScanChanList_SystemLost(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene
)
{
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32                      enModeType;
    CNAS_HSD_1X_SYSTEM_INFO_STRU                           *pst1xSysInfo;
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstSysList;
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                      *pstScanList;

    pstScanList     = CNAS_HSD_GetScanFreqListAddr();
    pstSysList      = CNAS_HSD_GetSysRecListAddr();

    enModeType      = CNAS_HSD_GetModeType();
    pst1xSysInfo    = CNAS_HSD_Get1xSystemInfo();

    /* ����Hrpd LIST */
    if (VOS_FALSE == CNAS_HSD_CreateHrpdSysList(enModeType, pst1xSysInfo, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_SystemLost: create hrpd list fail!");
        return;
    }

    CNAS_HSD_LogSysRecList(pstSysList, enSysAcqScene);

    /* ����SWITCH ON SCAN LIST */
    if (VOS_FALSE == CNAS_HSD_CreateScanFreqList(enSysAcqScene, pstScanList, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_SystemLost: create freq list fail!");
        return;
    }

    CNAS_HSD_LogScanFreqList(pstScanList, enSysAcqScene);

    return;
}



VOS_VOID CNAS_HSD_BuildScanChanList_Redirection(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene
)
{
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32  enModeType;
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstSysList;
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstScanList;
    VOS_UINT32                          ulResult;

    pstScanList     = CNAS_HSD_GetScanFreqListAddr();
    pstSysList      = CNAS_HSD_GetSysRecListAddr();
    enModeType      = CNAS_HSD_GetModeType();

    /* ����Э�飬�ض���Ƶ����Ժ�1x������ */
    if (CNAS_HSD_SYS_MODE_HYBRID == enModeType)
    {
        ulResult = CNAS_HSD_CreateAllHrpdSysRecs(CNAS_PRL_CHECK_ASSN_INCL, CNAS_PRL_SORT_NEEDED, pstSysList);
    }
    else
    {
        ulResult = CNAS_HSD_CreateAllHrpdSysRecs(CNAS_PRL_NO_CHECK_ASSN_INCL, CNAS_PRL_SORT_NEEDED, pstSysList);
    }

    if (VOS_FALSE == ulResult)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_Redirection: create hrpd list fail!");
        return;
    }

    CNAS_HSD_LogSysRecList(pstSysList, enSysAcqScene);

    /* ����Redirction CAN LIST */
    if (VOS_FALSE == CNAS_HSD_CreateScanFreqList(enSysAcqScene, pstScanList, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_Redirection: create freq list fail!");
        return;
    }

    CNAS_HSD_LogScanFreqList(pstScanList, enSysAcqScene);

    return;
}


VOS_VOID CNAS_HSD_BuildScanChanList_DataCallQuick(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene
)
{
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32                      enModeType;
    CNAS_HSD_1X_SYSTEM_INFO_STRU                           *pst1xSysInfo;
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstSysList;
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                      *pstScanList;

    pstScanList     = CNAS_HSD_GetScanFreqListAddr();
    pstSysList      = CNAS_HSD_GetSysRecListAddr();

    enModeType      = CNAS_HSD_GetModeType();
    pst1xSysInfo    = CNAS_HSD_Get1xSystemInfo();

    /* ����Hrpd LIST */
    if (VOS_FALSE == CNAS_HSD_CreateHrpdSysList(enModeType, pst1xSysInfo, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_SystemLost: create hrpd list fail!");
        return;
    }

    CNAS_HSD_LogSysRecList(pstSysList, enSysAcqScene);

    /* ����DATA CALL SCAN LIST */
    if (VOS_FALSE == CNAS_HSD_CreateScanFreqList(enSysAcqScene, pstScanList, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_SystemLost: create freq list fail!");
        return;
    }

    CNAS_HSD_LogScanFreqList(pstScanList, enSysAcqScene);

    return;

}


void CNAS_HSD_AddMruToScanList_SwitchOn
(
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                       *pstHrpdScanFreqList
)
{
    VOS_UINT8                           i;
    VOS_UINT16                          usPos;
    VOS_UINT16                          usTotalNum;
    CNAS_HSD_HRPD_MRU_LIST_STRU        *pstMRUList;

    /* ��ȡMRU�б��ַ */
    pstMRUList = CNAS_HSD_GetMruList();

    /* ��ȡ�Ѿ���ӵ�Ƶ������Լ����� */
    usPos       = pstHrpdScanFreqList->usAvailFreqNum;
    usTotalNum  = pstHrpdScanFreqList->usTotalFreqNum;

    /* ���Ƶ�㵽Scan List */
    if (CNAS_HSD_SYS_MODE_HYBRID == CNAS_HSD_GetModeType())
    {
        if (0 == CNAS_HSD_GetMruListNum())
        {
            return;
        }

        if (VOS_FALSE == CNAS_HSD_FreqIsSupport(&pstMRUList->astHrpdSys[0].stFreq))
        {
            return;
        }

        if (VOS_FALSE == CNAS_HSD_GetMru0RelateFlg())
        {
            return;
        }

        pstHrpdScanFreqList->pstScanFreqItem[usPos].stFreq           = pstMRUList->astHrpdSys[0].stFreq;
        pstHrpdScanFreqList->pstScanFreqItem[usPos].enFreqType       = CNAS_HSD_FREQ_TYPE_MRU;
        pstHrpdScanFreqList->pstScanFreqItem[usPos++].ucPrioLevel    = CNAS_PRL_PRIO_LEVEL_1;
        pstHrpdScanFreqList->usAvailFreqNum = usPos;

    }
    else
    {
        for (i = 0; i < CNAS_HSD_GetMruListNum(); i++)
        {
            if (VOS_FALSE == CNAS_HSD_FreqIsSupport(&pstMRUList->astHrpdSys[i].stFreq))
            {
                continue;
            }

            if (VOS_TRUE == CNAS_HSD_IsRepeatFreq(pstHrpdScanFreqList, &pstMRUList->astHrpdSys[i].stFreq))
            {
                continue;
            }

            /* �ж��Ƿ�Խ�� */
            if (usPos >= usTotalNum)
            {
                CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMruToScanList_SwitchOn: scan list to small,cannot add Freq!");
                return;
            }

            pstHrpdScanFreqList->pstScanFreqItem[usPos].stFreq           = pstMRUList->astHrpdSys[i].stFreq;
            pstHrpdScanFreqList->pstScanFreqItem[usPos].enFreqType       = CNAS_HSD_FREQ_TYPE_MRU;
            pstHrpdScanFreqList->pstScanFreqItem[usPos++].ucPrioLevel    = CNAS_PRL_PRIO_LEVEL_1;
            pstHrpdScanFreqList->usAvailFreqNum = usPos;
        }
    }
    pstHrpdScanFreqList->ucLowPrioLevel = CNAS_PRL_PRIO_LEVEL_1;

    return;
}


VOS_VOID CNAS_HSD_AddMruToScanList_SystemLost
(
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                       *pstHrpdScanFreqList
)
{
    VOS_UINT16                          i;
    VOS_UINT16                          usPos;
    VOS_UINT16                          usMruNum;
    VOS_UINT16                          usTotalNum;
    CNAS_HSD_HRPD_MRU_LIST_STRU        *pstMRUList;

    /* ��ȡMRU�б��ַ */
    pstMRUList = CNAS_HSD_GetMruList();

    /* ��ȡMRU�������� */

    usMruNum    = CNAS_HSD_GetMruListNum();

    /* ��ӵ�Scan List��Ƶ������ */
    usTotalNum = pstHrpdScanFreqList->usTotalFreqNum;

    /* �Ѿ���ӵ�Ƶ����� */
    usPos      = pstHrpdScanFreqList->usAvailFreqNum;

    /* ���Ƶ�㵽Scan List */
    for (i = 0; i < usMruNum; i++)
    {
        if (VOS_FALSE == CNAS_HSD_FreqIsSupport(&pstMRUList->astHrpdSys[i].stFreq))
        {
            continue;
        }

        /* ����Ƿ������ظ�Ƶ�� */
        if (VOS_TRUE == CNAS_HSD_IsRepeatFreq(pstHrpdScanFreqList, &pstMRUList->astHrpdSys[i].stFreq))
        {
            continue;
        }

        /* �ж��Ƿ�Խ�� */
        if(usPos >= usTotalNum)
        {
            CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMruToScanList_SwitchOn: scan list to small,cannot add Freq!");
            return ;
        }

        /* ��Ƶ��ŵ�Scan List�� */
        pstHrpdScanFreqList->pstScanFreqItem[usPos].stFreq         = pstMRUList->astHrpdSys[i].stFreq;
        pstHrpdScanFreqList->pstScanFreqItem[usPos].enFreqType     = CNAS_HSD_FREQ_TYPE_MRU;
        pstHrpdScanFreqList->pstScanFreqItem[usPos++].ucPrioLevel  = CNAS_PRL_PRIO_LEVEL_1;

        pstHrpdScanFreqList->usAvailFreqNum                        = usPos;

        /* �����ĵ� CDG 130 5.2.3.3 System Lost Scan List����
           ����Figure 5-3����MRU[0] */
        /* �ж���MRU0Ӧ�ò����λ�� */
        if (VOS_TRUE == CNAS_HSD_FreqIsMru0Pos(usPos))
        {
            /* �ж��Ƿ�Խ�� */
            if (usPos >= usTotalNum)
            {
                CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMruToScanList_SwitchOn: scan list to small,cannot add Freq!");
                return ;
            }

            pstHrpdScanFreqList->pstScanFreqItem[usPos].stFreq         = pstMRUList->astHrpdSys[0].stFreq;
            pstHrpdScanFreqList->pstScanFreqItem[usPos].enFreqType     = CNAS_HSD_FREQ_TYPE_MRU;
            pstHrpdScanFreqList->pstScanFreqItem[usPos++].ucPrioLevel  = CNAS_PRL_PRIO_LEVEL_1;

            pstHrpdScanFreqList->usAvailFreqNum                        = usPos;
        }
    }

    pstHrpdScanFreqList->ucLowPrioLevel             = CNAS_PRL_PRIO_LEVEL_1;

    return;
}


VOS_UINT32 CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SwitchOn
(
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstHrpdItemInfo,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                      *pstHrpdScanFreqList
)
{
    VOS_UINT16                          usSystemIndex;
    VOS_UINT16                          usFreqIndex;
    VOS_UINT16                          usPos  =  0;
    VOS_UINT16                          usTotalFreqNum;
    CNAS_PRL_ACQ_RECORD_STRU           *pstTmpExtAcqRecord     = VOS_NULL_PTR;
    CNAS_PRL_GENERIC_ACQ_FOR_HRPD_STRU *pstHrpdSys             = VOS_NULL_PTR;

    /* ��Ҫ��Ƶ��ŵ�Scan List����ʼ��ַ */
    usPos = pstHrpdScanFreqList->usAvailFreqNum;

    /* Ԥ�ڵ�Scan List��Ƶ������ */
    usTotalFreqNum = pstHrpdScanFreqList->usTotalFreqNum;

    for (usSystemIndex = 0; usSystemIndex < pstHrpdItemInfo->usHrpdSysNum; usSystemIndex++)
    {
        /* �ų�Negativeϵͳ */
        if (CNAS_PRL_PREF_NEG_SYS_PREF != pstHrpdItemInfo->pstHrpdSysItem[usSystemIndex].enPrefNegSys)
        {
            continue;
        }

        /* ��ȡ����PRL��õ���Acq Table�ĵ�ַ */
        pstTmpExtAcqRecord  = CNAS_PRL_GetSpecifiedPrlValidAcqRecord(pstHrpdItemInfo->pstHrpdSysItem[usSystemIndex].usAcqIndex);

        if (VOS_NULL_PTR == pstTmpExtAcqRecord)
        {
            CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SwitchOn: Get AcqRec fail!");
            continue;
        }

        /* Ƶ�����ͱ��� */
        if (CNAS_PRL_ACQ_TYPE_GENERIC_ACQ_RECORD_FOR_HRPD != pstTmpExtAcqRecord->enAcqType)
        {
            continue;
        }

        /* ��ȡHRPDϵͳ���׵�ַ */
        pstHrpdSys = &(pstTmpExtAcqRecord->u.stHrpdSys);

        for (usFreqIndex = 0 ; usFreqIndex < pstHrpdSys->ucNumOfChans; usFreqIndex++)
        {
            /* �ж�Ӳ���Ƿ�֧�ָ�Ƶ�� */
            if (VOS_FALSE == CNAS_HSD_FreqIsSupport(&pstHrpdSys->astFreq[usFreqIndex]))
            {
                continue;
            }

            /* �����ڴ�ռ䣬�д� */
            if (usPos >= usTotalFreqNum)
            {
                pstHrpdScanFreqList->usAvailFreqNum  = 0;
                CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SwitchOn: scan list to small,cannot add Freq!");
                return VOS_FALSE;
            }

            /* ���ظ���Ƶ����������� */
            if (VOS_FALSE == CNAS_HSD_IsRepeatFreq(pstHrpdScanFreqList,
                                                  &pstHrpdSys->astFreq[usFreqIndex]))
            {
                pstHrpdScanFreqList->pstScanFreqItem[usPos].ucPrioLevel = pstHrpdItemInfo->pstHrpdSysItem[usSystemIndex].ucPrioLevel;
                pstHrpdScanFreqList->pstScanFreqItem[usPos].enFreqType  = CNAS_HSD_FREQ_TYPE_OTHER;
                pstHrpdScanFreqList->pstScanFreqItem[usPos++].stFreq    = pstHrpdSys->astFreq[usFreqIndex];
                pstHrpdScanFreqList->usAvailFreqNum  = usPos;
            }
        }

    }

    if (0 == usPos)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SwitchOn: no Freq added to scan list!");
        return VOS_FALSE;
    }

    /* �����ȼ����� */
    pstHrpdScanFreqList->ucLowPrioLevel  = pstHrpdScanFreqList->pstScanFreqItem[usPos - 1].ucPrioLevel;

    /* ����������� */
    pstHrpdScanFreqList->usCurrIndex     = 0;

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SystemLost
(
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstHrpdItemInfo,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                      *pstHrpdScanFreqList
)
{
    VOS_UINT16                          usSystemIndex;
    VOS_UINT16                          usFreqIndexs;
    VOS_UINT16                          usPos  =  0;
    VOS_UINT16                          usTotalFreqNum;
    CNAS_PRL_ACQ_RECORD_STRU           *pstTmpExtAcqRecord     = VOS_NULL_PTR;
    CNAS_HSD_HRPD_MRU_LIST_STRU        *pstMRUList             = VOS_NULL_PTR;
    CNAS_PRL_GENERIC_ACQ_FOR_HRPD_STRU *pstHrpdSys             = VOS_NULL_PTR;

    /*��ȡMRU�洢��ַ*/
    pstMRUList = CNAS_HSD_GetMruList();

    /* ��Ҫ��Ƶ��ŵ�Scan List����ʼ��ַ */
    usPos = pstHrpdScanFreqList->usAvailFreqNum;

    /* Ԥ�ڵ�Scan List��Ƶ������ */
    usTotalFreqNum = pstHrpdScanFreqList->usTotalFreqNum;

    for (usSystemIndex = 0; usSystemIndex < pstHrpdItemInfo->usHrpdSysNum; usSystemIndex++)
    {

        /* �ų�Negativeϵͳ */
        if (CNAS_PRL_PREF_NEG_SYS_PREF != pstHrpdItemInfo->pstHrpdSysItem[usSystemIndex].enPrefNegSys)
        {
            continue;
        }

        /* ��ȡ����PRL��õ���Acq Table�ĵ�ַ */
        pstTmpExtAcqRecord  = CNAS_PRL_GetSpecifiedPrlValidAcqRecord(pstHrpdItemInfo->pstHrpdSysItem[usSystemIndex].usAcqIndex);

        if (VOS_NULL_PTR == pstTmpExtAcqRecord)
        {
            CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SystemLost: Get AcqRec fail!");
            continue;
        }

        /* Ƶ�����ͱ��� */
        if (CNAS_PRL_ACQ_TYPE_GENERIC_ACQ_RECORD_FOR_HRPD != pstTmpExtAcqRecord->enAcqType)
        {
            continue;
        }

        /* ��ȡHRPDϵͳ���׵�ַ */
        pstHrpdSys = &(pstTmpExtAcqRecord->u.stHrpdSys);

        for (usFreqIndexs = 0 ;usFreqIndexs <pstHrpdSys->ucNumOfChans;usFreqIndexs++)
        {
            /* �ж�Ӳ���Ƿ�֧�ָ�Ƶ�� */
            if (VOS_FALSE == CNAS_HSD_FreqIsSupport(&(pstHrpdSys->astFreq[usFreqIndexs])))
            {
                continue;
            }

            /* ͬһ���ȼ����ظ���Ƶ����������� */
            if (VOS_TRUE == CNAS_HSD_IsRepeatFreq(pstHrpdScanFreqList,
                                                  &pstHrpdSys->astFreq[usFreqIndexs]))
            {
                continue;
            }

            /* �����ڴ�ռ䣬�д� */
            if (usPos >= usTotalFreqNum)
            {
                pstHrpdScanFreqList->usAvailFreqNum  = 0;
                CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SystemLost: scan list to small,cannot add Freq!");
                return VOS_FALSE;
            }

            /* ��ӷ�MRUƵ�㵽Scan List�� */
            pstHrpdScanFreqList->pstScanFreqItem[usPos].ucPrioLevel = pstHrpdItemInfo->pstHrpdSysItem[usSystemIndex].ucPrioLevel;
            pstHrpdScanFreqList->pstScanFreqItem[usPos].enFreqType  = CNAS_HSD_FREQ_TYPE_OTHER;
            pstHrpdScanFreqList->pstScanFreqItem[usPos++].stFreq    = pstTmpExtAcqRecord->u.stHrpdSys.astFreq[usFreqIndexs];
            pstHrpdScanFreqList->usAvailFreqNum                     = usPos;

            /* �����ĵ� CDG 130 5.2.3.3 System Lost Scan List����
               ����Figure 5-3����MRU[0] */
            if (VOS_TRUE == CNAS_HSD_FreqIsMru0Pos(usPos))
            {
                /* �����ڴ�ռ䣬�д� */
                if(usPos >= usTotalFreqNum)
                {
                    pstHrpdScanFreqList->usAvailFreqNum  = 0;
                    CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SystemLost: scan list to small,cannot add Freq!");
                    return VOS_FALSE;
                }

                pstHrpdScanFreqList->pstScanFreqItem[usPos].stFreq        = pstMRUList->astHrpdSys[0].stFreq;
                pstHrpdScanFreqList->pstScanFreqItem[usPos].enFreqType    = CNAS_HSD_FREQ_TYPE_MRU;
                pstHrpdScanFreqList->pstScanFreqItem[usPos].ucPrioLevel = pstHrpdScanFreqList->pstScanFreqItem[usPos-1].ucPrioLevel;
                usPos++;
                pstHrpdScanFreqList->usAvailFreqNum                       = usPos;
            }
        }

    }

    if (0 == usPos)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SystemLost: no Freq added to scan list");
        return VOS_FALSE;
    }

    /* �����ȼ����� */
    pstHrpdScanFreqList->ucLowPrioLevel  = pstHrpdScanFreqList->pstScanFreqItem[usPos - 1].ucPrioLevel;

    /* ����������� */
    pstHrpdScanFreqList->usCurrIndex     = 0;

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_AddMatchedGeoFreqToScanFreqList_Redirection
(
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstHrpdItemInfo,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                      *pstHrpdScanFreqList
)
{
    CNAS_HSD_REDIRECTION_STRU          *pstRedirection;
    CNAS_PRL_FREQENCY_CHANNEL_STRU      stFreq;
    VOS_UINT16                          usAvailFreqNum;
    VOS_UINT16                          i;

    usAvailFreqNum  =  0;
    pstRedirection  = CNAS_HSD_GetRedirectionFreqAddr();

    /* ��ȡ�տ��ϱ�Ƶ��,��ӵ�Scan list�� */
    for (i=0; i< CNAS_HSD_GetRedirectionFreqNum(); i++)
    {
        stFreq.enBandClass = pstRedirection->aulChannel[i].ucBandClass;
        stFreq.usChannel   = pstRedirection->aulChannel[i].usChannel;

        /* Ӳ���Ƿ�֧�� */
        if (VOS_FALSE == CNAS_HSD_FreqIsSupport(&stFreq))
        {
            continue;
        }

        /* Ƶ���ظ���� */
        if (VOS_TRUE == CNAS_HSD_IsRepeatFreq(pstHrpdScanFreqList, &stFreq))
        {
            continue;
        }

        pstHrpdScanFreqList->pstScanFreqItem[usAvailFreqNum].ucPrioLevel = CNAS_PRL_PRIO_LEVEL_1; /* Ĭ����һ��level */
        pstHrpdScanFreqList->pstScanFreqItem[usAvailFreqNum].enFreqType  = CNAS_HSD_FREQ_TYPE_REDIRECTION;
        pstHrpdScanFreqList->pstScanFreqItem[usAvailFreqNum].stFreq      = stFreq;

        usAvailFreqNum++;
        pstHrpdScanFreqList->usAvailFreqNum  = usAvailFreqNum;

        /* ��ֹԽ�� */
        if (usAvailFreqNum >= pstHrpdScanFreqList->usTotalFreqNum)
        {
            break;
        }
    }

    /* �����ȼ����� */
    pstHrpdScanFreqList->ucLowPrioLevel  = CNAS_PRL_PRIO_LEVEL_1;

    /* ����������� */
    pstHrpdScanFreqList->usCurrIndex     = 0;

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_GetScanChanListFromHrpdList_SwitchOn(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysRecList,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstHrpdScanFreqList
)
{
    /* ���MRU��Scan List�� */
    CNAS_HSD_AddMruToScanList_SwitchOn(pstHrpdScanFreqList);

    /* ���Ҫ������Ƶ�㵽Scan List�� */
    if (VOS_FALSE == CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SwitchOn(pstHrpdSysRecList,
                                                                       pstHrpdScanFreqList))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_GetScanChanListFromHrpdList_SystemLost
(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysRecList,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstHrpdScanFreqList
)
{
    if ((VOS_NULL_PTR   == pstHrpdSysRecList)
       || (VOS_NULL_PTR == pstHrpdScanFreqList))
    {
        return VOS_FALSE;
    }

    /* ���MRU��Scan List�� */
    CNAS_HSD_AddMruToScanList_SystemLost(pstHrpdScanFreqList);

    /* ���Ҫ������Ƶ�㵽Scan List�� */
    if (VOS_FALSE == CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SystemLost(pstHrpdSysRecList,
                                                                         pstHrpdScanFreqList))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_GetScanChanListFromHrpdList_Redirection
(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysRecList,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstHrpdScanFreqList
)
{
    if ((VOS_NULL_PTR   == pstHrpdSysRecList)
       || (VOS_NULL_PTR == pstHrpdScanFreqList))
    {
        return VOS_FALSE;
    }

    /* ���Ҫ������Ƶ�㵽Scan List�� */
    if (VOS_FALSE == CNAS_HSD_AddMatchedGeoFreqToScanFreqList_Redirection(pstHrpdSysRecList,
                                                                          pstHrpdScanFreqList))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_GetScanChanListFromHrpdList_DataCallQuick
(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysRecList,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstHrpdScanFreqList
)
{
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  stTempScanList;
    CNAS_HSD_HRPD_MRU_LIST_STRU       *pstMRUList;
    VOS_UINT16                         usFreqNum;
    VOS_UINT16                         usScanFreqIndex;
    VOS_UINT16                         i;
    VOS_UINT16                         j;
    VOS_UINT8                          ucMruNum = 0;

    if ((VOS_NULL_PTR   == pstHrpdSysRecList)
       || (VOS_NULL_PTR == pstHrpdScanFreqList))
    {
        return VOS_FALSE;
    }

    pstMRUList  = CNAS_HSD_GetMruList();

    /* ��ȡMRU�������� */
    ucMruNum    = CNAS_HSD_GetMruListNum();

    usFreqNum   = pstHrpdScanFreqList->usTotalFreqNum - ucMruNum;

    PS_MEM_SET(&stTempScanList, 0, sizeof(CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU));

    /* ������ʱ�洢�ռ� */
    stTempScanList.pstScanFreqItem = (CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU*)PS_MEM_ALLOC(UEPS_PID_HSD,
                                      usFreqNum * sizeof(CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU));

    if (VOS_NULL_PTR == stTempScanList.pstScanFreqItem)
    {
        return VOS_FALSE;
    }

    stTempScanList.usTotalFreqNum  = usFreqNum;

    /* ���� */
    PS_MEM_SET(stTempScanList.pstScanFreqItem, 0, usFreqNum * sizeof(CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU));

    /* ���Ҫ������Ƶ�㵽��Mru�������ʱ�洢�ռ��� */
    if (VOS_FALSE == CNAS_HSD_AddMatchedGeoFreqToScanFreqList_SwitchOn(pstHrpdSysRecList,
                                                                       &stTempScanList))
    {
        PS_MEM_FREE(UEPS_PID_HSD, stTempScanList.pstScanFreqItem);

        return VOS_FALSE;
    }

    /* ��ȡavail freq�·���CAS����ʱ�����avoid list�е��ڵ�Ƶ�㣬���ڵ���Ҫɾ�� */
    CNAS_HSD_CheckExpiredFreqFromAvoidList();

    usScanFreqIndex = 0;

    /* ���MruƵ����Scan list�У����� */
    for (i=0; i< ucMruNum; i++)
    {
        for (j=0; j<stTempScanList.usAvailFreqNum; j++)
        {
            if (VOS_TRUE != CNAS_PRL_CmpFreqValue(&(pstMRUList->astHrpdSys[i].stFreq),
                                                  &(stTempScanList.pstScanFreqItem[j].stFreq)))
            {
                continue;
            }

            /* ��鵱ǰƵ���Ƿ񱻽��� */
            if (VOS_TRUE == CNAS_HSD_IsCurFreqAvoided(&(pstMRUList->astHrpdSys[i].stFreq)))
            {
                continue;
            }

            /* �����������뵽��ͷ */
            pstHrpdScanFreqList->pstScanFreqItem[usScanFreqIndex].ucPrioLevel = stTempScanList.ucLowPrioLevel; /* ���õ�ǰ�ĵȼ�Ϊ��� */
            pstHrpdScanFreqList->pstScanFreqItem[usScanFreqIndex].enFreqType  = CNAS_HSD_FREQ_TYPE_MRU;
            pstHrpdScanFreqList->pstScanFreqItem[usScanFreqIndex++].stFreq    = pstMRUList->astHrpdSys[i].stFreq;
            pstHrpdScanFreqList->usAvailFreqNum  = usScanFreqIndex;

            pstHrpdScanFreqList->usCurrIndex     = 0;

            PS_MEM_FREE(UEPS_PID_HSD, stTempScanList.pstScanFreqItem);

            /* �ҵ�1�����˳���ǰѭ�� */
            return VOS_TRUE;
        }
    }

    CNAS_NORMAL_LOG(UEPS_PID_HSD, "CNAS_HSD_GetScanChanListFromHrpdList_DataCallQuick: MRU not existed!");

    /* ���Mru�������㣬�ٲ��ҵ�һ����AvoidƵ�����Scan list�� */
    for (i=0; i<stTempScanList.usAvailFreqNum; i++)
    {
        /* ��鵱ǰƵ���Ƿ񱻽��� */
        if (VOS_TRUE == CNAS_HSD_IsCurFreqAvoided(&(stTempScanList.pstScanFreqItem[i].stFreq)))
        {
            continue;
        }

        pstHrpdScanFreqList->pstScanFreqItem[usScanFreqIndex].ucPrioLevel = stTempScanList.ucLowPrioLevel; /* ���õ�ǰ�ĵȼ�Ϊ��� */
        pstHrpdScanFreqList->pstScanFreqItem[usScanFreqIndex].enFreqType  = CNAS_HSD_FREQ_TYPE_MRU;        /* Ϊפ������ */
        pstHrpdScanFreqList->pstScanFreqItem[usScanFreqIndex++].stFreq    = stTempScanList.pstScanFreqItem[i].stFreq;
        pstHrpdScanFreqList->usAvailFreqNum  = usScanFreqIndex;
        pstHrpdScanFreqList->usCurrIndex     = 0;

        /* �ҵ�1�����˳� */
        break;
    }

    PS_MEM_FREE(UEPS_PID_HSD, stTempScanList.pstScanFreqItem);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_HSD_FreqIsMru0Pos
(
    VOS_UINT16                          usPos
)
{
    VOS_UINT16                          i;
    CNAS_HSD_HRPD_MRU_LIST_STRU        *pstMRUList;

    /* ��ȡMRU�б��ַ */
    pstMRUList = CNAS_HSD_GetMruList();

    if (0 == pstMRUList->ucCurrMruNum)
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == CNAS_HSD_FreqIsSupport(&pstMRUList->astHrpdSys[0].stFreq))
    {
        return VOS_FALSE;
    }

    /* ��鵱ǰλ���Ƿ���MRU0�����λ�� */
    for (i = 0;i < CNAS_HSD_MRU0_NUM_4_IN_SCAN_LIST; i++)
    {
        if (usPos == g_aucCnasHsdSystemLostMru0Pos[i])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 CNAS_HSD_FreqIsSupport
(
    CNAS_PRL_FREQENCY_CHANNEL_STRU     *pstFreq
)
{
#ifdef DMT
    /* Ŀǰֻ֧��BC0��BC10������ͨ��NV������ */
    if ((CNAS_PRL_BAND_CLASS_TYPE_0 == pstFreq->enBandClass)
     || (CNAS_PRL_BAND_CLASS_TYPE_10 == pstFreq->enBandClass))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
#else
    VOS_UINT32                          ulSupportedBand;

    ulSupportedBand = CNAS_HSD_GetUeSupportedBandClass();

    if (0 == (ulSupportedBand & ((VOS_UINT32)1 << pstFreq->enBandClass)))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
#endif
}


VOS_VOID CNAS_HSD_SortHrpdSystem_NoHybrid
(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdList
)
{
    CNAS_HSD_HRPD_SYS_REC_ITEM_STRU    *pstHrpdItemInfo;
    CNAS_HSD_HRPD_SYS_REC_ITEM_STRU     stTmpItem;
    VOS_UINT16                          usNumOfSystem;
    VOS_UINT32                          ulCmpVal;
    VOS_UINT32                          ulCmpTmp;
    VOS_UINT16                          i;
    VOS_UINT16                          j;

    pstHrpdItemInfo = pstHrpdList->pstHrpdSysItem;
    usNumOfSystem   = pstHrpdList->usHrpdSysNum;

    /* ���������ȼ�ֵԽС���ȼ�Խ��, */
    /* �������Ϊ�ȶ�HRPDϵͳ���ȼ��ߵķ�ǰ�棬��HRPDϵͳ��ϵͳ��ͬʱ������AcqTbl�е�index��С�������� */
    /* e.g. hrpd_1���ȼ�Ϊ1��indexΪ2��hrpd_2���ȼ�Ϊ3��indexΪ1��
       ��Ϻ�����ȼ�Ϊ
            hrpd_1:0x00010002
            hrpd_2:0x00030001
       ���չ���hrpd_1��hrpd_2���ȼ���
    */
    for (i = 0; i < usNumOfSystem - 1 ; i++)
    {
        for (j = i + 1 ;j < usNumOfSystem ; j++)
        {
            ulCmpVal = (VOS_UINT32) (pstHrpdItemInfo[i].ucPrioLevel << 16) | (pstHrpdItemInfo[i].usAcqIndex);
            ulCmpTmp = (VOS_UINT32) (pstHrpdItemInfo[j].ucPrioLevel << 16) | (pstHrpdItemInfo[j].usAcqIndex);

            if (ulCmpVal > ulCmpTmp)
            {
                stTmpItem           = pstHrpdItemInfo[j];
                pstHrpdItemInfo[j]  = pstHrpdItemInfo[i];
                pstHrpdItemInfo[i]  = stTmpItem;
            }
        }
    }

    return;
}


VOS_UINT16 CNAS_HSD_CalcMatchHrpdSystemFreqNum(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdItemInfo
)
{
    VOS_UINT16                          usTotalMatchedGeoChanNum    = 0;
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstPrlSysRecList            = VOS_NULL_PTR;
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstPrlSysRec                = VOS_NULL_PTR;
    CNAS_PRL_ACQ_RECORD_STRU           *pstPrlAcqRec                = VOS_NULL_PTR;
    CNAS_PRL_ACQ_REC_FREQ_INFO_STRU     stFreqInfo;
    VOS_UINT16                          i;

    pstPrlSysRecList  = CNAS_PRL_GetPrlInfoAddr()->stPrlSysInfo.pstSysRecord;

    CNAS_HSD_SetMru0RelateFlg(VOS_FALSE);

    /* ��ȡƵ����� */
    for (i = 0; i < pstHrpdItemInfo->usHrpdSysNum; i++)
    {
        /* �ų�negativeϵͳ */
        if (CNAS_PRL_PREF_NEG_SYS_PREF == pstHrpdItemInfo->pstHrpdSysItem[i].enPrefNegSys)
        {
            pstPrlSysRec = &pstPrlSysRecList[pstHrpdItemInfo->pstHrpdSysItem[i].usSysIndex];

            if (VOS_FALSE == pstPrlSysRec->ucSysRecValid)
            {
                continue;
            }

            pstPrlAcqRec = CNAS_PRL_GetSpecifiedPrlAcqRecord(pstPrlSysRec->usAcqIndex);

            /* ��ָ����б��� */
            if (VOS_NULL_PTR == pstPrlAcqRec)
            {
                continue;
            }

            /* Ƶ�����ͱ��� */
            if (CNAS_PRL_ACQ_TYPE_GENERIC_ACQ_RECORD_FOR_HRPD != pstPrlAcqRec->enAcqType)
            {
                continue;
            }

            CNAS_PRL_GetPrlAcqRecFreqListInfo(pstPrlAcqRec, &stFreqInfo);

            usTotalMatchedGeoChanNum += (VOS_UINT16)stFreqInfo.ulNum;

            CNAS_HSD_ProcMru0Relate(&stFreqInfo);
        }
    }

    return usTotalMatchedGeoChanNum;
}


VOS_UINT32 CNAS_HSD_IsRepeatFreqInSpecPrio
(
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstHrpdScanFreqList,
    CNAS_PRL_FREQENCY_CHANNEL_STRU     *pstFreq,
    VOS_UINT8                           usPrio
)
{
    VOS_UINT16                           i;

    /* Ҫ��Ƚϵĵȼ�ΪMRU��Ƶ�㣬��ֱ����Ϊ���ظ� */
    if (0 == usPrio)
    {
        return VOS_FALSE;
    }

    /* �������ȼ�ֵ�ǴӴ�С����ģ�������ʱһ���ҵ����ȼ���ָ�����ȼ�С�����ȼ���
       ��ʾָ�����ȼ��Ѿ����������Խ���ѭ���Ƚ��� */
    for (i = pstHrpdScanFreqList->usAvailFreqNum; i != 0; i--)
    {
        /* ���ȼ�Ϊ��ͬ */
        if ((pstHrpdScanFreqList->pstScanFreqItem[i - 1].ucPrioLevel == usPrio)
            ||(CNAS_HSD_FREQ_TYPE_MRU != pstHrpdScanFreqList->pstScanFreqItem[i - 1].enFreqType))
        {
            /* Ƶ����ͬ */
            if (VOS_TRUE == CNAS_PRL_CmpFreqValue(pstFreq, &(pstHrpdScanFreqList->pstScanFreqItem[i-1].stFreq)))
            {
                return VOS_TRUE;
            }
        }

        if (usPrio > pstHrpdScanFreqList->pstScanFreqItem[i - 1].ucPrioLevel)
        {
            break;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 CNAS_HSD_IsRepeatFreq
(
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstHrpdScanFreqList,
    CNAS_PRL_FREQENCY_CHANNEL_STRU     *pstFreq
)
{
    VOS_UINT16                           i;

    /* ѭ���Ƚ� */
    for (i = 0; i < pstHrpdScanFreqList->usAvailFreqNum; i++)
    {
        /* Ƶ����ͬ */
        if (VOS_TRUE == CNAS_PRL_CmpFreqValue(pstFreq, &(pstHrpdScanFreqList->pstScanFreqItem[i].stFreq)))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 CNAS_HSD_GetScanListSameLevelFreq(
    VOS_UINT32                          ulFreqNum,
    CNAS_PRL_FREQENCY_CHANNEL_STRU     *pstFreq
)
{
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstCnasHsdScanList;
    VOS_UINT32                          ulHrpdFreqNum  = 0;
    VOS_UINT32                          usAvailFreqNum = 0;
    VOS_UINT16                          usCurrIndex    = 0;
    VOS_UINT32                          i;

    if ((VOS_NULL_PTR == pstFreq) || (0 == ulFreqNum))
    {
        CNAS_ERROR_LOG1(UEPS_PID_HSD, "CNAS_HSD_GetScanListSameLevelFreq: Para is Err. FreqNum is", ulFreqNum);
        return 0;
    }

    /* ��ȡavail freq�·���CAS����ʱ�����avoid list�е��ڵ�Ƶ�㣬���ڵ���Ҫɾ�� */
    CNAS_HSD_CheckExpiredFreqFromAvoidList();

    pstCnasHsdScanList = CNAS_HSD_GetScanFreqListAddr();

    /* ��ǰ���� */
    usCurrIndex    = pstCnasHsdScanList->usCurrIndex;

    /* �ܵĿ���Ƶ����� */
    usAvailFreqNum = pstCnasHsdScanList->usAvailFreqNum;

    /* ����ĳɲ��ж�level����ͬ��level����һ����ȥ */
    /* �Ӻ�һ��Ƶ�㿪ʼ���ж��Ƿ�����ӵ�SCAN LIST�� */
    for (i = usCurrIndex; i < usAvailFreqNum; i++)
    {
        /* ��鵱ǰƵ���Ƿ񱻽��� */
        if (VOS_TRUE == CNAS_HSD_IsCurFreqAvoided(&(pstCnasHsdScanList->pstScanFreqItem[i].stFreq)))
        {
            if (0 == ulHrpdFreqNum)
            {
                pstCnasHsdScanList->usCurrIndex++;

                continue;
            }
            else
            {
                break;
            }
        }

        /* �����������б��� */
        PS_MEM_CPY(&pstFreq[ulHrpdFreqNum],
                   &(pstCnasHsdScanList->pstScanFreqItem[i].stFreq),
                   sizeof(CNAS_PRL_FREQENCY_CHANNEL_STRU));

        ulHrpdFreqNum++;

        if (ulFreqNum == ulHrpdFreqNum)
        {
            break;
        }
    }

    return ulHrpdFreqNum;
}


VOS_UINT16 CNAS_HSD_GetNonHybridHrpdSysRecNum(
    CNAS_PRL_CHECK_ASSN_INCL_ENUM_UINT16    enCheckAssnIncl
)
{
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstSysRecord             = VOS_NULL_PTR;
    VOS_UINT16                          usSysRecordNum           = 0;
    VOS_UINT16                          usNonHybridHrpdSysRecNum = 0;
    VOS_UINT16                          i;

    /* ��ȡSYS RECORD����*/
    usSysRecordNum = CNAS_PRL_GetPrlSysRecordNum();

    for (i = 0; i < usSysRecordNum; i++)
    {
         /* ��ȡSYS RECORD */
        pstSysRecord = CNAS_PRL_GetSpecifiedPrlValidSysRecord(i);

        if (VOS_NULL_PTR == pstSysRecord)
        {
            CNAS_WARNING_LOG1(UEPS_PID_HSD, "CNAS_HSD_GetNonHybridHrpdSysRecNum: Get SysRecord is Null,i is", i);
            continue;
        }

        if (VOS_TRUE == CNAS_PRL_IsHrpdSysRec(pstSysRecord))
        {
            /* �����ȼ�����ʱ�����жϵ�ǰHRPD�Ƿ���Ҫ��ӵ�EVDO�б��� */
            if (VOS_FALSE == CNAS_HSD_IsCurHrpdSysNeedInHighPriScene(i, pstSysRecord))
            {
                continue;
            }

            if (CNAS_PRL_NO_CHECK_ASSN_INCL == enCheckAssnIncl)
            {
                /* �����עAssn Incl״ֱ̬��ͳ�� */
                usNonHybridHrpdSysRecNum++;
            }
            else
            {
                /* ���עAssn Incl״̬:ֻ��Assn InclΪYESʱ��ͳ�� */
                if (VOS_YES == pstSysRecord->ucAssociationIncl)
                {
                    usNonHybridHrpdSysRecNum++;
                }
            }
        }
    }

    return usNonHybridHrpdSysRecNum;
}


VOS_UINT16 CNAS_HSD_GetNonHybridHrpdSysRecList(
    CNAS_PRL_CHECK_ASSN_INCL_ENUM_UINT16    enCheckAssnIncl,
    CNAS_HSD_HRPD_SYS_LIST_STRU            *pstSysRecList
)
{
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstSysRecord             = VOS_NULL_PTR;
    VOS_UINT16                          usSysRecordNum           = 0;
    VOS_UINT16                          usNonHybridHrpdSysRecNum = 0;
    VOS_UINT8                           ucPrioLevel              = CNAS_PRL_PRIO_LEVEL_1;
    VOS_UINT16                          i;
    VOS_UINT16                          ucPriorityMore           = VOS_FALSE;

    /* ��ȡSYS RECORD����*/
    usSysRecordNum = CNAS_PRL_GetPrlSysRecordNum();

    for (i = 0; i < usSysRecordNum; i++)
    {
        /* ��ȡSYS RECORD */
        pstSysRecord = CNAS_PRL_GetSpecifiedPrlValidSysRecord(i);

        if (VOS_NULL_PTR == pstSysRecord)
        {
            CNAS_WARNING_LOG1(UEPS_PID_HSD, "CNAS_HSD_GetNonHybridHrpdSysRecList: Get SysRecord is Null,i is", i);
            continue;
        }

        if (CNAS_PRL_GEO_REGION_IND_NEW == pstSysRecord->enGeoInd)
        {
            /* ��GEO�����¼���PrioLevel */
            ucPrioLevel     = CNAS_PRL_PRIO_LEVEL_1;
            ucPriorityMore  = VOS_FALSE;
        }

        /* ǰһ��GEO��PriInd��more */
        if (VOS_TRUE == ucPriorityMore)
        {
            ucPrioLevel++;
            ucPriorityMore = VOS_FALSE;
        }

        if (CNAS_PRL_RELATIVE_PRI_MORE == pstSysRecord->enPriInd)
        {
            /* PrioLevelΪmore */
            ucPriorityMore = VOS_TRUE;
        }

        /* �����ȼ�����ʱ�����жϵ�ǰHRPD�Ƿ���Ҫ��ӵ�EVDO�б��� */
        if (VOS_FALSE == CNAS_HSD_IsCurHrpdSysNeedInHighPriScene(i, pstSysRecord))
        {
            continue;
        }

        if (VOS_TRUE == CNAS_PRL_IsHrpdSysRec(pstSysRecord))
        {
            if (CNAS_PRL_NO_CHECK_ASSN_INCL == enCheckAssnIncl)
            {
                /* �����עAssn Incl״̬ */
                pstSysRecList->pstHrpdSysItem[usNonHybridHrpdSysRecNum].ucPrioLevel  = ucPrioLevel;
                pstSysRecList->pstHrpdSysItem[usNonHybridHrpdSysRecNum].usAcqIndex   = pstSysRecord->usAcqIndex;
                pstSysRecList->pstHrpdSysItem[usNonHybridHrpdSysRecNum].usSysIndex   = i;
                pstSysRecList->pstHrpdSysItem[usNonHybridHrpdSysRecNum].enPrefNegSys = pstSysRecord->enPrefNegSys;

                usNonHybridHrpdSysRecNum++;

                /* �����ڴ�ռ���� */
                if (usNonHybridHrpdSysRecNum >= pstSysRecList->usHrpdSysNum)
                {
                    CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_GetNonHybridHrpdSysRecList:Assn Incl NO No Space left");
                    break;
                }
            }
            else
            {
                /* ���עAssn InclΪYES״̬ */
                if (VOS_YES == pstSysRecord->ucAssociationIncl)
                {
                    pstSysRecList->pstHrpdSysItem[usNonHybridHrpdSysRecNum].ucPrioLevel  = ucPrioLevel;
                    pstSysRecList->pstHrpdSysItem[usNonHybridHrpdSysRecNum].usAcqIndex   = pstSysRecord->usAcqIndex;
                    pstSysRecList->pstHrpdSysItem[usNonHybridHrpdSysRecNum].usSysIndex   = i;
                    pstSysRecList->pstHrpdSysItem[usNonHybridHrpdSysRecNum].enPrefNegSys = pstSysRecord->enPrefNegSys;

                    usNonHybridHrpdSysRecNum++;

                    /* �����ڴ�ռ���� */
                    if (usNonHybridHrpdSysRecNum >= pstSysRecList->usHrpdSysNum)
                    {
                        CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_GetNonHybridHrpdSysRecList:Assn Incl YES No Space left");
                        break;
                    }
                }
            }

        }
    }

    return VOS_TRUE;
}

VOS_UINT16 CNAS_HSD_GetHybridHrpdSysRecParaCheck(
    CNAS_PRL_MULTI_MATCHED_GEO_INFO_STRU    *pstGeoInfoList,
    VOS_UINT16                               usSysRecordNum,
    VOS_UINT16                               usGeoEndIndex
)
{
    VOS_UINT8                                ucIndex = 0;
    CNAS_PRL_CUSTOM_MATCH_LVL_ENUM_UINT16    enGeoMostMatchLvl;
    if (usGeoEndIndex > usSysRecordNum)
    {
        /* ����SYS RECORD��¼ֱ�ӷ��� */
        CNAS_ERROR_LOG1(UEPS_PID_HSD, "CNAS_HSD_GetHybridHrpdSysRecList: ulGeoEndIndex out of range", usGeoEndIndex);
        return VOS_FALSE;
    }

    enGeoMostMatchLvl = pstGeoInfoList->enGeoMostMatchLvl;
    /* �ж�1X index�Ƿ���������Χ�� */
    for (ucIndex = 0; ucIndex < pstGeoInfoList->astIndexList[enGeoMostMatchLvl].usMatchedIndexNum; ucIndex++)
    {
        if ((pstGeoInfoList->astIndexList[enGeoMostMatchLvl].ausMostMatchedIndex[ucIndex] < pstGeoInfoList->usGeoFirstSysRecIndex)
         || (pstGeoInfoList->astIndexList[enGeoMostMatchLvl].ausMostMatchedIndex[ucIndex] >= usGeoEndIndex))
        {
            CNAS_ERROR_LOG1(UEPS_PID_HSD, "CNAS_HSD_GetHybridHrpdSysRecList: 1xSysIndex out of range", usGeoEndIndex);
            return VOS_FALSE;
        }
    }
    return VOS_TRUE;
}


VOS_UINT16 CNAS_HSD_GetHybridHrpdSysRecNum(
    CNAS_PRL_MULTI_MATCHED_GEO_INFO_STRU     *pstGeoInfoList
)
{
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstSysRecord          = VOS_NULL_PTR;
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pst1xSysRecord[CNAS_PRL_MAX_MATCHED_SYSTEM_NUM]      = {0};
    VOS_UINT16                          usGeoEndIndex         = 0;
    VOS_UINT16                          usSysRecordNum        = 0;
    VOS_UINT16                          usHybridHrpdSysRecNum = 0;
    VOS_UINT16                          i;
    CNAS_PRL_CUSTOM_MATCH_LVL_ENUM_UINT16                   enGeoMostMatchLvl;
    VOS_UINT8                           ucMatched1xSysNum     = 0;

    /* ��ȡSYS RECORD����*/
    usSysRecordNum = CNAS_PRL_GetPrlSysRecordNum();

    usGeoEndIndex  = pstGeoInfoList->usGeoFirstSysRecIndex + pstGeoInfoList->usGeoSysRecNum;

    /* �����Ϸ��Լ�� */
    if (VOS_TRUE != CNAS_HSD_GetHybridHrpdSysRecParaCheck(pstGeoInfoList, usSysRecordNum, usGeoEndIndex))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_GetHybridHrpdSysRecList: para error");
        return VOS_NULL;
    }

    enGeoMostMatchLvl   = pstGeoInfoList->enGeoMostMatchLvl;
    ucMatched1xSysNum   = (VOS_UINT8)(pstGeoInfoList->astIndexList[enGeoMostMatchLvl].usMatchedIndexNum);

    /* �õ���ƥ���1Xϵͳ �����ܻ��ж��*/
    for (i = 0; i < ucMatched1xSysNum; i++)
    {
        pst1xSysRecord[i] = CNAS_PRL_GetSpecifiedPrlValidSysRecord(pstGeoInfoList->astIndexList[enGeoMostMatchLvl].ausMostMatchedIndex[i]);

        if (VOS_NULL_PTR == pst1xSysRecord[i])
        {
            CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_GetHybridHrpdSysRecNum: Get SysRecord null");
            return VOS_NULL;
        }
    }

    for (i = pstGeoInfoList->usGeoFirstSysRecIndex; i < usGeoEndIndex; i++)
    {
        /* ��ȡSYS RECORD */
        pstSysRecord = CNAS_PRL_GetSpecifiedPrlValidSysRecord(i);

        if (VOS_NULL_PTR == pstSysRecord)
        {
            CNAS_WARNING_LOG1(UEPS_PID_HSD, "CNAS_HSD_GetHybridHrpdSysRecNum: Get SysRecord null, i", i);
            continue;
        }

        if (VOS_FALSE == CNAS_PRL_IsHrpdSysRec(pstSysRecord))
        {
            continue;
        }

        /* �����ȼ�����ʱ�����жϵ�ǰHRPD�Ƿ���Ҫ��ӵ�EVDO�б��� */
        if (VOS_FALSE == CNAS_HSD_IsCurHrpdSysNeedInHighPriScene(i, pstSysRecord))
        {
            continue;
        }

        /* Assn InclΪYESʱ��ͳ�� */
        if (VOS_YES != pstSysRecord->ucAssociationIncl)
        {
            continue;
        }

        /* ����Ͷ��1X�����һ����ͬ����Ϊ��ͬ */
        if (VOS_TRUE == CNAS_HSD_IsTheHrpdSysAsso1xSys(pstSysRecord,pst1xSysRecord,ucMatched1xSysNum))
        {
            usHybridHrpdSysRecNum++;
        }
    }

    return usHybridHrpdSysRecNum;
}


VOS_UINT16 CNAS_HSD_GetHybridHrpdSysRecList(
    CNAS_PRL_MULTI_MATCHED_GEO_INFO_STRU    *pstGeoList,
    CNAS_HSD_HRPD_SYS_REC_ITEM_STRU         *pstHrpdSysItem,
    VOS_UINT16                               usAviHrpdSysNum,
    VOS_UINT16                               usPivotDeltLevel
)
{
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstSysRecord          = VOS_NULL_PTR;
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pst1xSysRecord[CNAS_PRL_MAX_MATCHED_SYSTEM_NUM]        = {VOS_NULL_PTR};
    VOS_UINT16                          usGeoEndIndex         = 0;
    VOS_UINT16                          usSysRecordNum        = 0;
    VOS_UINT16                          usHybridHrpdSysRecNum = 0;
    VOS_UINT8                           ucPrioLevel           = (VOS_UINT8)(CNAS_PRL_PRIO_LEVEL_1 + usPivotDeltLevel);
    VOS_UINT16                          i;
    VOS_UINT16                          ucPriorityMore        = VOS_FALSE;
    VOS_UINT8                           ucMatched1xSysNum     = 0;
    VOS_UINT8                           ucIsAll1xSysNoAss     = VOS_TRUE;
    CNAS_PRL_CUSTOM_MATCH_LVL_ENUM_UINT16                   enGeoMostMatchLvl;

    /* ��ȡSYS RECORD����*/
    usSysRecordNum = CNAS_PRL_GetPrlSysRecordNum();

    usGeoEndIndex = pstGeoList->usGeoFirstSysRecIndex + pstGeoList->usGeoSysRecNum;

    if (VOS_TRUE != CNAS_HSD_GetHybridHrpdSysRecParaCheck(pstGeoList, usSysRecordNum, usGeoEndIndex))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_GetHybridHrpdSysRecList: para error");
        return VOS_NULL;
    }

    enGeoMostMatchLvl   = pstGeoList->enGeoMostMatchLvl;
    ucMatched1xSysNum   = (VOS_UINT8)pstGeoList->astIndexList[enGeoMostMatchLvl].usMatchedIndexNum;

    /* �õ���ƥ���1Xϵͳ �����ܻ��ж��*/
    for (i = 0; i < ucMatched1xSysNum; i++)
    {
        pst1xSysRecord[i] = CNAS_PRL_GetSpecifiedPrlValidSysRecord(pstGeoList->astIndexList[enGeoMostMatchLvl].ausMostMatchedIndex[i]);

        if (VOS_NULL_PTR == pst1xSysRecord[i])
        {
            CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_GetHybridHrpdSysRecNum: Get SysRecord null");
            return VOS_NULL;
        }

        /* �ж�1X Assn Inc�Ƿ�ΪYES */
        if (VOS_NO != pst1xSysRecord[i]->ucAssociationIncl)
        {
            ucIsAll1xSysNoAss = VOS_FALSE;
        }
    }

    if (VOS_TRUE == ucIsAll1xSysNoAss)
    {
        return VOS_NULL;
    }

    for (i = pstGeoList->usGeoFirstSysRecIndex; i < usGeoEndIndex; i++)
    {
        /* ��ȡSYS RECORD */
        pstSysRecord = CNAS_PRL_GetSpecifiedPrlValidSysRecord(i);

        if (VOS_NULL_PTR == pstSysRecord)
        {
            CNAS_WARNING_LOG1(UEPS_PID_HSD, "CNAS_HSD_GetHybridHrpdSysRecList: Get SysRecord null, i", i);
            continue;
        }

        if (CNAS_PRL_GEO_REGION_IND_NEW == pstSysRecord->enGeoInd)
        {
            /* ��GEO�����¼���PrioLevel */
            ucPrioLevel     = (VOS_UINT8)(CNAS_PRL_PRIO_LEVEL_1 + usPivotDeltLevel);
            ucPriorityMore  = VOS_FALSE;
        }

        /* ǰһ��GEO��PriInd��more */
        if (VOS_TRUE == ucPriorityMore)
        {
            ucPrioLevel++;
            ucPriorityMore = VOS_FALSE;
        }

        if (CNAS_PRL_RELATIVE_PRI_MORE == pstSysRecord->enPriInd)
        {
            /* PrioLevelΪmore */
            ucPriorityMore = VOS_TRUE;
        }

        if (VOS_FALSE == CNAS_PRL_IsHrpdSysRec(pstSysRecord))
        {
            continue;
        }

        /* �����ȼ�����ʱ�����жϵ�ǰHRPD�Ƿ���Ҫ��ӵ�EVDO�б��� */
        if (VOS_FALSE == CNAS_HSD_IsCurHrpdSysNeedInHighPriScene(i, pstSysRecord))
        {
            continue;
        }

        if (VOS_TRUE == CNAS_HSD_IsTheHrpdSysAsso1xSys(pstSysRecord, pst1xSysRecord, ucMatched1xSysNum))
        {
            pstHrpdSysItem[usHybridHrpdSysRecNum].ucPrioLevel  = ucPrioLevel;
            pstHrpdSysItem[usHybridHrpdSysRecNum].usAcqIndex   = pstSysRecord->usAcqIndex;
            pstHrpdSysItem[usHybridHrpdSysRecNum].usSysIndex   = i;
            pstHrpdSysItem[usHybridHrpdSysRecNum].enPrefNegSys = pstSysRecord->enPrefNegSys;
            usHybridHrpdSysRecNum++;

            /* �����ڴ�ռ���� */
            if (usHybridHrpdSysRecNum >= usAviHrpdSysNum)
            {
                CNAS_WARNING_LOG1(UEPS_PID_HSD, "CNAS_HSD_GetHybridHrpdSysRecList: usHybridHrpdSysRecNum No Space left",
                                  usHybridHrpdSysRecNum);
                break;
            }
        }
    }

    return usHybridHrpdSysRecNum;
}


VOS_VOID CNAS_HSD_BuildScanChanList_OocMru0(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene
)
{
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32  enModeType;
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstScanList;
    CNAS_HSD_1X_SYSTEM_INFO_STRU       *pst1xSysInfo;
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstSysList;

    pstScanList     = CNAS_HSD_GetScanFreqListAddr();
    pstSysList      = CNAS_HSD_GetSysRecListAddr();
    pst1xSysInfo    = CNAS_HSD_Get1xSystemInfo();
    enModeType      = CNAS_HSD_GetModeType();

    /* ����EVDO LIST */
    if (VOS_FALSE == CNAS_HSD_CreateHrpdSysList(enModeType, pst1xSysInfo, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_OocMru0: CNAS_HSD_CreateHrpdSysList fail!");

        return;
    }

    CNAS_HSD_LogSysRecList(pstSysList, enSysAcqScene);

    /* ����SWITCH ON SCAN LIST */
    if (VOS_FALSE == CNAS_HSD_CreateScanFreqList(enSysAcqScene, pstScanList, pstSysList))
    {
        CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_OocMru0: CNAS_HSD_GetMru0SysId fail!");

        return;
    }

    CNAS_HSD_LogScanFreqList(pstScanList, enSysAcqScene);

    return ;

}



CNAS_HSD_ASSN_RLST_ENUM_UINT32 CNAS_HSD_CheckHrpdFreqAssnWith1X(
    CNAS_PRL_HRPD_SYSTEM_STRU          *pstHrpdSys
)
{
    VOS_UINT8                           ucPrio;
    VOS_UINT16                          usMatchIndex;
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysList;

    /* ���տ�����������Ƶ���б� */
    CNAS_HSD_BuildHrpdScanList(CNAS_HSD_SYS_ACQ_SCENE_SWITCH_ON);

    /* �ж��Ƿ��й�����HRPDƵ�� */
    if (0 == CNAS_HSD_GetScanFreqNum())
    {
        return CNAS_HSD_NO_ASSN_FREQ;
    }

    /* ��ǰϵͳ������� */
    if (VOS_FALSE == CNAS_HSD_IsAcqedHrpdSysMatchTheSysRecs(pstHrpdSys,
                                                           &ucPrio,
                                                           &usMatchIndex))
    {
        return CNAS_HSD_NO_ASSN_WITH_HRPD;
    }

    pstHrpdSysList = CNAS_HSD_GetSysRecListAddr();

    /* ���統ǰHRPDϵͳΪnegative�򵱲��������� */
    if (CNAS_PRL_PREF_NEG_SYS_NEG == pstHrpdSysList->pstHrpdSysItem[usMatchIndex].enPrefNegSys)
    {
        return CNAS_HSD_NO_ASSN_WITH_HRPD;
    }

    return CNAS_HSD_ASSN_SUCC;
}


VOS_UINT32 CNAS_HSD_GetScanChanListFromMru0_Ooc(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysRecList,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstHrpdScanFreqList
)
{
    VOS_UINT16                          i;
    VOS_UINT16                          usMruNum;
    CNAS_HSD_HRPD_MRU_LIST_STRU        *pstMRUList;
    /* make pclint happy */
    /* pstHrpdSysRecList = pstHrpdSysRecList; */

    /* ��ȡMRU�б��ַ */
    pstMRUList = CNAS_HSD_GetMruList();

    /* ��ȡMRU�������� */
    usMruNum    = CNAS_HSD_GetMruListNum();

    /* ��ӵ�һ��֧�ֵ�Ƶ�㵽Scan List */
    for (i = 0; i < usMruNum; i++)
    {
        /* �ж�Ӳ���Ƿ�֧�ָ�Ƶ�� */
        if (VOS_TRUE == CNAS_HSD_FreqIsSupport(&pstMRUList->astHrpdSys[i].stFreq))
        {
            /* ��ͷλ�ò���MRU0 */
            pstHrpdScanFreqList->pstScanFreqItem[0].stFreq            = pstMRUList->astHrpdSys[i].stFreq;
            pstHrpdScanFreqList->pstScanFreqItem[0].enFreqType        = CNAS_HSD_FREQ_TYPE_MRU;
            pstHrpdScanFreqList->pstScanFreqItem[0].ucPrioLevel       = CNAS_PRL_PRIO_LEVEL_1;
            pstHrpdScanFreqList->usAvailFreqNum                       = 1;
            pstHrpdScanFreqList->ucLowPrioLevel                       = CNAS_PRL_PRIO_LEVEL_1;

            /* ��ʼ����ǰ��Ҫ������Ƶ������Ϊ0 */
            pstHrpdScanFreqList->usCurrIndex                          = 0;

            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}



VOS_VOID CNAS_HSD_BuildScanChanList_HighPriority(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene
)
{
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32                      enModeType;
    CNAS_HSD_1X_SYSTEM_INFO_STRU                           *pst1xSysInfo;
    CNAS_HSD_HRPD_SYS_LIST_STRU                            *pstSysList;
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                      *pstScanList;

    pstScanList     = CNAS_HSD_GetScanFreqListAddr();
    pstSysList      = CNAS_HSD_GetSysRecListAddr();
    enModeType      = CNAS_HSD_GetModeType();
    pst1xSysInfo    = CNAS_HSD_Get1xSystemInfo();

    /* ����EVDO LIST */
    if (VOS_FALSE == CNAS_HSD_CreateHrpdSysList(enModeType, pst1xSysInfo, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_HighPriority: create hrpd list fail!");
        return;
    }

    CNAS_HSD_LogSysRecList(pstSysList, enSysAcqScene);

    /* ����SWITCH ON SCAN LIST */
    if (VOS_FALSE == CNAS_HSD_CreateScanFreqList(enSysAcqScene, pstScanList, pstSysList))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_HighPriority: create freq list fail!");
        return;
    }

    CNAS_HSD_LogScanFreqList(pstScanList, enSysAcqScene);

    return;
}


VOS_UINT32 CNAS_HSD_GetScanChanListFromHrpdList_HighPriority(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysRecList,
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU  *pstHrpdScanFreqList
)
{
    VOS_UINT16                          usSystemIndex;
    CNAS_PRL_FREQENCY_CHANNEL_STRU      stFreq;
    VOS_UINT16                          usAvailFreqNum;
    VOS_UINT8                           ucPriFreqNum;
    VOS_UINT16                          i;
    CNAS_HSD_HIGHPRIORITY_STRU         *pstHighPri          = VOS_NULL_PTR;

    usAvailFreqNum  = 0;
    pstHighPri      = CNAS_HSD_GetHighPriorityFreqAddr();
    ucPriFreqNum    = CNAS_HSD_GetHighPriorityFreqNum();

    for (usSystemIndex = 0; usSystemIndex < pstHrpdSysRecList->usHrpdSysNum; usSystemIndex++)
    {
        /* �ų�Negativeϵͳ */
        if (CNAS_PRL_PREF_NEG_SYS_PREF != pstHrpdSysRecList->pstHrpdSysItem[usSystemIndex].enPrefNegSys)
        {
            continue;
        }

        /* ÿ��HRPD�������ƥ������ȼ��ж��Ƶ�� */
        for (i = 0; i < ucPriFreqNum; i++)
        {
            /* �������ȼ�˳�����Ƶ�� */
            if (pstHrpdSysRecList->pstHrpdSysItem[usSystemIndex].usSysIndex != pstHighPri->astFreqInfo[i].stHrpdSysItem.usSysIndex)
            {
                continue;
            }

            stFreq.enBandClass = pstHighPri->astFreqInfo[i].astFreq.enBandClass;
            stFreq.usChannel   = pstHighPri->astFreqInfo[i].astFreq.usChannel;

            /* Ӳ���Ƿ�֧�� */
            if (VOS_FALSE == CNAS_HSD_FreqIsSupport(&stFreq))
            {
                continue;
            }

            /* Ƶ���ظ���� */
            if (VOS_TRUE == CNAS_HSD_IsRepeatFreq(pstHrpdScanFreqList, &stFreq))
            {
                continue;
            }

            pstHrpdScanFreqList->pstScanFreqItem[usAvailFreqNum].ucPrioLevel = pstHighPri->astFreqInfo[i].stHrpdSysItem.ucPrioLevel;
            pstHrpdScanFreqList->pstScanFreqItem[usAvailFreqNum].enFreqType  = CNAS_HSD_FREQ_TYPE_OTHER;
            pstHrpdScanFreqList->pstScanFreqItem[usAvailFreqNum].stFreq      = stFreq;

            usAvailFreqNum++;
            pstHrpdScanFreqList->usAvailFreqNum  = usAvailFreqNum;

            /* ��ֹԽ�� */
            if (usAvailFreqNum >= pstHrpdScanFreqList->usTotalFreqNum)
            {
                break;
            }
        }
    }

    /* �����ȼ�����,��ȷ���Ƿ���� */
    pstHrpdScanFreqList->ucLowPrioLevel  = pstHrpdSysRecList->pstHrpdSysItem[usAvailFreqNum-1].ucPrioLevel;

    /* ����������� */
    pstHrpdScanFreqList->usCurrIndex     = 0;

    return VOS_TRUE;
}


VOS_UINT8 CNAS_HSD_IsCurHrpdSysNeedInHighPriScene(
    VOS_UINT16                          usSysIndex,
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstSysRecord
)
{
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32  enSysAcqScene;
    VOS_UINT8                           ucExist;

    enSysAcqScene = CNAS_HSD_GetCurSysAcqScene_SysAcq();

    if (CNAS_HSD_SYS_ACQ_SCENE_HIGH_PRIORITY == enSysAcqScene)
    {
        /* �����ڸ����ȼ������У��򷵻�TURE */
        ucExist     = CNAS_HSD_IsCurHrpdSysInHighPriList(usSysIndex);
    }
    else
    {
        ucExist = VOS_TRUE;
    }

    return ucExist;
}



VOS_UINT8 CNAS_HSD_IsCurHrpdSysInHighPriList(
    VOS_UINT16                          usSysIndex
)
{
    VOS_UINT16                          i;
    VOS_UINT8                           ucExist;
    VOS_UINT8                           ucPriFreqNum;
    CNAS_HSD_HIGHPRIORITY_STRU         *pstHighPri;

    ucExist         = VOS_FALSE;
    ucPriFreqNum    = CNAS_HSD_GetHighPriorityFreqNum();
    pstHighPri      = CNAS_HSD_GetHighPriorityFreqAddr();

    for (i = 0; i < ucPriFreqNum; i++)
    {
        if (usSysIndex == pstHighPri->astFreqInfo[i].stHrpdSysItem.usSysIndex)
        {
            ucExist = VOS_TRUE;
            break;
        }
    }

    return ucExist;
}


VOS_UINT8 CNAS_HSD_IsCurSysSubnetWildCard(
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstSysRecord
)
{
    VOS_UINT8                           i;
    VOS_UINT8                           ucRslt;

    ucRslt = VOS_TRUE;

    /* ���Ȳ�Ϊ0���Ҳ�ȫΪ0ʱ��Ϊ��ͨ�� */
    for (i = 0; i < pstSysRecord->u.stIs856Id.ucSubnetBitLen; i++)
    {
        if (0 != pstSysRecord->u.stIs856Id.aucSubnet[i])
        {
            ucRslt = VOS_FALSE;
            break;
        }
    }


    return ucRslt;
}


CNAS_HSD_ASSN_RLST_ENUM_UINT32 CNAS_HSD_CheckHrpdSysAssnWith1X(
    CNAS_PRL_HRPD_SYSTEM_STRU          *pstHrpdSys
)
{
    VOS_UINT8                           ucPrio;
    VOS_UINT16                          usMatchIndex;
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdSysList;

    /* ���տ�����������Ƶ���б� */
    CNAS_HSD_BuildHrpdScanList(CNAS_HSD_SYS_ACQ_SCENE_SWITCH_ON);

    /* �ж��Ƿ��й�����HRPDϵͳ */
    if (0 == CNAS_HSD_GetSysRecItemNum())
    {
        return CNAS_HSD_NO_ASSN_SYS;
    }

    /* ��ǰϵͳ������� */
    if (VOS_FALSE == CNAS_HSD_IsAcqedHrpdSysMatchTheSysRecs(pstHrpdSys,
                                                           &ucPrio,
                                                           &usMatchIndex))
    {
        return CNAS_HSD_NO_ASSN_WITH_HRPD;
    }

    pstHrpdSysList = CNAS_HSD_GetSysRecListAddr();

    /* ���統ǰHRPDϵͳΪnegative�򵱲��������� */
    if (CNAS_PRL_PREF_NEG_SYS_NEG == pstHrpdSysList->pstHrpdSysItem[usMatchIndex].enPrefNegSys)
    {
        return CNAS_HSD_NO_ASSN_WITH_HRPD;
    }

    return CNAS_HSD_ASSN_SUCC;
}



VOS_VOID CNAS_HSD_GetMostMatched1xGeoList(
    CNAS_PRL_1X_SYSTEM_STRU                                *pstCurSysInfo,
    CNAS_PRL_MATCHED_GEO_LIST_INFO_STRU                    *pstMostMatchGeoListInfo
)
{
    CNAS_PRL_MATCHED_GEO_LIST_INFO_STRU                     stGeoListInfo;

    PS_MEM_SET(&stGeoListInfo, 0, sizeof(CNAS_PRL_MATCHED_GEO_LIST_INFO_STRU));

    /* ��ȡ����ƥ���GEO list */
    CNAS_PRL_Get1xSysGeoList(pstCurSysInfo, &stGeoListInfo);

    /* ��GEO list�л�ȡ��match��GEO list */
    CNAS_PRL_GetAllMostMatched1xGeoFrom1xGeoList(pstCurSysInfo, &stGeoListInfo, pstMostMatchGeoListInfo);

    pstMostMatchGeoListInfo->usSid = pstCurSysInfo->usSid;
    pstMostMatchGeoListInfo->usNid = pstCurSysInfo->usNid;

    return;
}


VOS_VOID CNAS_HSD_BuildScanChanList_CFreqLock(
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene
)
{
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU                      *pstScanList;
    CNAS_HSD_CFREQ_LOCK_SET_PARA_STRU                      *pstCnasHsdCFreqLockValue;

    pstScanList                 = CNAS_HSD_GetScanFreqListAddr();
    pstCnasHsdCFreqLockValue    = CNAS_HSD_GetFreqLockAddr();

    if (VOS_FALSE == CNAS_HSD_MallocMemForScanList(pstScanList, CNAS_HSD_CFREQLOCK_FREQ_NUM))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_BuildScanChanList_CFreqLock: malloc memory fail!");
        return ;
    }

    pstScanList->usCurrIndex                             = 0;
    pstScanList->usTotalFreqNum                          = 1;
    pstScanList->usAvailFreqNum                          = 1;
    pstScanList->ucLowPrioLevel                          = 0;

    pstScanList->pstScanFreqItem[0].stFreq.enBandClass   = pstCnasHsdCFreqLockValue->usHrpdBandClass;
    pstScanList->pstScanFreqItem[0].stFreq.usChannel     = pstCnasHsdCFreqLockValue->usHrpdFreq;
    pstScanList->pstScanFreqItem[0].ucPrioLevel          = 0;
    pstScanList->pstScanFreqItem[0].enFreqType           = CNAS_HSD_FREQ_TYPE_OTHER;

    CNAS_HSD_LogScanFreqList(pstScanList, enSysAcqScene);

    return;
}


VOS_VOID CNAS_HSD_ProcMru0Relate(
    CNAS_PRL_ACQ_REC_FREQ_INFO_STRU    *pstFreqInfo
)
{
    CNAS_PRL_FREQENCY_CHANNEL_STRU      stMru0Freq;
    UINT16                              i;

    if (VOS_FALSE == CNAS_HSD_GetMru0RelateFlg())
    {
        stMru0Freq = CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stMruList.astHrpdSys[0].stFreq;

        for (i = 0; i < (VOS_UINT16)pstFreqInfo->ulNum; i++)
        {
            if (VOS_TRUE == CNAS_PRL_CmpFreqValue(&pstFreqInfo->astFreqInfo[i], &stMru0Freq))
            {
                CNAS_HSD_SetMru0RelateFlg(VOS_TRUE);
                break;
            }
        }
    }

    return;
}



VOS_VOID CNAS_PRL_SortHrpdSystem_Hybrid
(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdList
)
{
    CNAS_HSD_HRPD_SYS_REC_ITEM_STRU    *pstHrpdItemInfo;
    CNAS_HSD_HRPD_SYS_REC_ITEM_STRU     stTmpItem;
    VOS_UINT16                          usNumOfSystem;
    VOS_UINT32                          ulCmpVal;
    VOS_UINT32                          ulCmpTmp;
    VOS_UINT16                          i;
    VOS_UINT16                          j;
    CNAS_HSD_1X_SYSTEM_INFO_STRU        *pst1xSys;

    pst1xSys =  CNAS_HSD_Get1xSystemInfo();

    pstHrpdItemInfo = pstHrpdList->pstHrpdSysItem;
    usNumOfSystem   = pstHrpdList->usHrpdSysNum;

    /* ���������ȼ�ֵԽС���ȼ�Խ��, */
    /* �������: ��ʹƥ�䵽��ͬ��GEO Ҳ�ǰ���SYSTEM ���ϵ��������� */
    /* ���������ȼ�ֵԽС���ȼ�Խ��, */
    /* ���1x�����������Ϊ�ȶ�HRPDϵͳ���ȼ��ߵķ�ǰ�棬��HRPDϵͳ��ϵͳ��ͬʱ������sustbl�е�index��С�������� */
    /* e.g. hrpd_1���ȼ�Ϊ1��indexΪ2��hrpd_2���ȼ�Ϊ3��indexΪ1��
       ��Ϻ�����ȼ�Ϊ
            hrpd_1:0x00010002
            hrpd_2:0x00030001
       ���չ���hrpd_1��hrpd_2���ȼ���
    */
    /* ���1X�����ڣ�����sys ˳������ */

    if (CNAS_HSD_1X_SYSTEM_AVAILABLE == pst1xSys->enStatus)
    {
        for (i = 0; i < usNumOfSystem - 1 ; i++)
        {
            for (j = i + 1 ;j < usNumOfSystem ; j++)
            {
                ulCmpVal = (VOS_UINT32) (pstHrpdItemInfo[i].ucPrioLevel << 16) | (pstHrpdItemInfo[i].usSysIndex);
                ulCmpTmp = (VOS_UINT32) (pstHrpdItemInfo[j].ucPrioLevel << 16) | (pstHrpdItemInfo[j].usSysIndex);

                if (ulCmpVal > ulCmpTmp)
                {
                    stTmpItem           = pstHrpdItemInfo[j];
                    pstHrpdItemInfo[j]  = pstHrpdItemInfo[i];
                    pstHrpdItemInfo[i]  = stTmpItem;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < usNumOfSystem - 1 ; i++)
        {
            for (j = i + 1 ;j < usNumOfSystem ; j++)
            {
                ulCmpVal = (VOS_UINT32)(pstHrpdItemInfo[i].usSysIndex);
                ulCmpTmp = (VOS_UINT32)(pstHrpdItemInfo[j].usSysIndex);

                if (ulCmpVal > ulCmpTmp)
                {
                    stTmpItem           = pstHrpdItemInfo[j];
                    pstHrpdItemInfo[j]  = pstHrpdItemInfo[i];
                    pstHrpdItemInfo[i]  = stTmpItem;
                }
            }
        }
    }


    return;
}


VOS_VOID CNAS_HSD_SortHrpdSystem
(
    CNAS_HSD_HRPD_SYS_LIST_STRU        *pstHrpdList
)
{
    CNAS_HSD_SYS_MODE_TYPD_ENUM_UINT32  enModeType;

    enModeType      = CNAS_HSD_GetModeType();

    if (CNAS_HSD_SYS_MODE_HYBRID == enModeType)
    {
        CNAS_PRL_SortHrpdSystem_Hybrid(pstHrpdList);
    }
    else
    {
        CNAS_HSD_SortHrpdSystem_NoHybrid(pstHrpdList);
    }
    return;
}


VOS_UINT32 CNAS_HSD_GetPivot1xSysPriosInGeos
(
    CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU              *pstGeoList,
    VOS_UINT8                                              *pucPivotDeltPrio
)
{
    VOS_UINT32                          ulIndex             = 0;
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstPrlSysRec        = VOS_NULL_PTR;
    CNAS_PRL_EXT_SYS_RECORD_STRU       *pstPrlGeoFirstSysRec= VOS_NULL_PTR;
    VOS_UINT16                          usLowestPrio        = 0;
    VOS_UINT8                           aucPivotPrio[CNAS_PRL_MAX_MATCHING_GEO_NUM]   = {0};
    CNAS_PRL_CUSTOM_MATCH_LVL_ENUM_UINT16                   enGeoMostMatchLvl;

    for (ulIndex = 0; ulIndex < pstGeoList->ucGeoNum; ulIndex++)
    {
        /* �õ�ƥ��ȼ��͵�ǰGEO��ȵ�1X��prio */
        /* Ϊ�˵õ���ƥ���1Xϵͳ�ڵ�ǰGEO�����ȼ�(CNAS�����ȼ�GEO�л�û����)��
            �õ��õ�ǰ1X�����ȼ� ��ȥGEO�����һ��ϵͳ�����ȼ� */

        enGeoMostMatchLvl       = pstGeoList->astGeoInfoList[ulIndex].enGeoMostMatchLvl;

        pstPrlSysRec            = CNAS_PRL_GetSpecifiedPrlSysRecord(pstGeoList->astGeoInfoList[ulIndex].astIndexList[enGeoMostMatchLvl].ausMostMatchedIndex[0]);

        pstPrlGeoFirstSysRec    = CNAS_PRL_GetSpecifiedPrlSysRecord(pstGeoList->astGeoInfoList[ulIndex].usGeoFirstSysRecIndex);

        if ((VOS_NULL_PTR == pstPrlSysRec)
         || (VOS_NULL_PTR == pstPrlGeoFirstSysRec))
        {

            CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_GetPivot1xSysPriosInGeos: pstPrlSysRec NULL_PTR");

            return VOS_ERR;
        }

        aucPivotPrio[ulIndex] = (VOS_UINT8)(pstPrlSysRec->usPriLvl - pstPrlGeoFirstSysRec->usPriLvl);

        if (usLowestPrio < aucPivotPrio[ulIndex])
        {
            usLowestPrio = aucPivotPrio[ulIndex];
        }
    }

    for (ulIndex = 0; ulIndex < pstGeoList->ucGeoNum; ulIndex++)
    {
        /* GEO ƴ����Ҫ���ӵ� PRIO */
        pucPivotDeltPrio[ulIndex] = (VOS_UINT8)(usLowestPrio - aucPivotPrio[ulIndex]);
    }

    return VOS_OK;
}



VOS_UINT32 CNAS_HSD_Is1xSysStillMatchHrpdSearchList
(
    CNAS_PRL_1X_SYSTEM_STRU            *pstHrpdSrcSys, /*  ����HRPD�б��1xϵͳ */
    CNAS_PRL_1X_SYSTEM_STRU            *pstDstSys
)
{
    /* ��������1X�õ���ƥ���GEO Ȼ��Ƚϵõ���GEO����ƥ���1xλ���Ƿ�һ�� */
    CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU                    *pstSrcMostMatchGeoListInfo     = VOS_NULL_PTR;
    CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU                    *pstDestMostMatchGeoListInfo    = VOS_NULL_PTR;

    /* ���ǰ��1x��֮ǰ��1X��ȫһ��������Ҫ���½��� */
    if ((pstHrpdSrcSys->usSid == pstDstSys->usSid)
     && (pstHrpdSrcSys->usNid== pstDstSys->usNid)
     && (pstHrpdSrcSys->stFreq.enBandClass == pstDstSys->stFreq.enBandClass)
     && (pstHrpdSrcSys->stFreq.usChannel == pstDstSys->stFreq.usChannel))
    {
        return VOS_TRUE;
    }

    pstSrcMostMatchGeoListInfo      = (CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU*)VOS_MemAlloc(UEPS_PID_HSD, DYNAMIC_MEM_PT, sizeof(CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU));
    pstDestMostMatchGeoListInfo     = (CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU*)VOS_MemAlloc(UEPS_PID_HSD, DYNAMIC_MEM_PT, sizeof(CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU));

    if ((VOS_NULL_PTR == pstSrcMostMatchGeoListInfo)
     || (VOS_NULL_PTR == pstDestMostMatchGeoListInfo))
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Is1xSysStillMatchHrpdSearchList : alloc err");
        return VOS_FALSE;
    }
    PS_MEM_SET(pstSrcMostMatchGeoListInfo, 0, sizeof(CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU));
    PS_MEM_SET(pstSrcMostMatchGeoListInfo, 0, sizeof(CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU));

    CNAS_HSD_Get1XMostMatchedGeoList(pstHrpdSrcSys, pstSrcMostMatchGeoListInfo);

    CNAS_HSD_Get1XMostMatchedGeoList(pstDstSys, pstDestMostMatchGeoListInfo);

    if (0 == PS_MEM_CMP(pstSrcMostMatchGeoListInfo, pstDestMostMatchGeoListInfo, sizeof(CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU)))
    {
        VOS_MemFree(UEPS_PID_HSD, pstSrcMostMatchGeoListInfo);
        VOS_MemFree(UEPS_PID_HSD, pstDestMostMatchGeoListInfo);
        return VOS_TRUE;
    }

    VOS_MemFree(UEPS_PID_HSD, pstSrcMostMatchGeoListInfo);
    VOS_MemFree(UEPS_PID_HSD, pstDestMostMatchGeoListInfo);

    return VOS_FALSE;
}



VOS_UINT32 CNAS_HSD_Get1XMostMatchedGeoList
(
    CNAS_PRL_1X_SYSTEM_STRU                                *pst1xSrcSys,
    CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU              *pstMostMatchedGeoList
)
{
    /* Step1 :ͨ��1X�õ�����matche�� GEO,����õ���GEO NUMΪ0 ����VOS_ERR
       Step2 : ��������ƥ��ĵõ���ƥ��ļ���
       step3 : ��������
    */

    CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU              *pstMatchedGeoList = VOS_NULL_PTR;
    VOS_UINT8                                               ucIndex             = 0;
    VOS_UINT8                                               ucMostMatchGeoNum   = 0;
    CNAS_PRL_CUSTOM_MATCH_LVL_ENUM_UINT16                   enGeoMostMatchLvl   = CNAS_PRL_CUSTOM_MATCH_LVL_WILDCARD_SID_EXCEPT_CHANNEL;
    VOS_UINT8                                               aucGeoIndex[CNAS_PRL_MAX_MATCHING_GEO_NUM]       = {0};
    CNAS_PRL_MULTI_MATCHED_GEO_INFO_STRU                   *pstMulTIMatchedGeo  = VOS_NULL_PTR;


    pstMatchedGeoList = (CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU*)PS_MEM_ALLOC(UEPS_PID_HSD, sizeof(CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU));

    if (VOS_NULL_PTR == pstMatchedGeoList)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Get1XMostMatchedGeoList: pstMatchedGeoList NULL_PTR");
        return VOS_ERR;
    }

    PS_MEM_SET(pstMatchedGeoList, 0, sizeof(CNAS_PRL_MULTI_MATCHED_GEO_LIST_INFO_STRU));

    CNAS_PRL_GetMostMultiMatched1xGeoList(pst1xSrcSys, pstMatchedGeoList);

    /* Step1 :ͨ��1X�õ�����matched�� GEO */
    if (0 == pstMatchedGeoList->ucGeoNum)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Get1XMostMatchedGeoList: no GEO matched");
        PS_MEM_FREE(UEPS_PID_HSD, pstMatchedGeoList);
        return VOS_ERR;
    }

    /* Step2 : ��������ƥ��ĵõ���ƥ��ļ��� */
    for (ucIndex = 0; ucIndex < pstMatchedGeoList->ucGeoNum; ucIndex++)
    {
        pstMulTIMatchedGeo = &(pstMatchedGeoList->astGeoInfoList[ucIndex]);

        if (CNAS_PRL_CUSTOM_MATCH_LVL_BUTT <= pstMulTIMatchedGeo->enGeoMostMatchLvl)
        {
            CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_Get1XMostMatchedGeoList: MATCH LVL >= CNAS_PRL_CUSTOM_MATCH_LVL_BUTT");
            PS_MEM_FREE(UEPS_PID_HSD, pstMatchedGeoList);
            return VOS_ERR;
        }

        /* ���Match lvl������ʱ���Match lvl;��Ҫ����ƥ����������㣬�������� */
        if (enGeoMostMatchLvl < pstMulTIMatchedGeo->enGeoMostMatchLvl)
        {
            enGeoMostMatchLvl = pstMulTIMatchedGeo->enGeoMostMatchLvl;

            ucMostMatchGeoNum = 0;
            PS_MEM_SET(aucGeoIndex, 0, CNAS_PRL_MAX_MATCHING_GEO_NUM);

            aucGeoIndex[0] = ucIndex;
            ucMostMatchGeoNum++;

        }
        else if  (enGeoMostMatchLvl == pstMulTIMatchedGeo->enGeoMostMatchLvl)
        {
            /* ���Match lvl������ʱ���Match lvl�����Ҳ��ǲ�ƥ��;��Ҫ��index������ƥ������飬����++ */
            aucGeoIndex[ucMostMatchGeoNum] = ucIndex;
            ucMostMatchGeoNum++;
        }
        else
        {
            ;
        }
    }

    /* �����ǰ��MOST MATCHED GEO LIST*/
    pstMostMatchedGeoList->ucGeoNum = ucMostMatchGeoNum;
    pstMostMatchedGeoList->usSid    = pst1xSrcSys->usSid;
    pstMostMatchedGeoList->usNid    = pst1xSrcSys->usNid;

    for (ucIndex = 0; ucIndex < ucMostMatchGeoNum; ucIndex++)
    {
        pstMostMatchedGeoList->astGeoInfoList[ucIndex] = pstMatchedGeoList->astGeoInfoList[(aucGeoIndex[ucIndex])];
    }

    PS_MEM_FREE(UEPS_PID_HSD, pstMatchedGeoList);
    return VOS_OK;

}


VOS_UINT32 CNAS_HSD_IsTheHrpdSysAsso1xSys
(
    CNAS_PRL_EXT_SYS_RECORD_STRU                           *pstSrcHrpdSys,
    CNAS_PRL_EXT_SYS_RECORD_STRU                          **ppst1xSys,
    VOS_UINT8                                               uc1xSysNum
)
{
    VOS_UINT8                           ucIndex = 0;

    for (ucIndex = 0; ucIndex < uc1xSysNum; ucIndex++)
    {
        if ((VOS_YES == ppst1xSys[ucIndex]->ucAssociationIncl)
         && (pstSrcHrpdSys->ucAssociationTag == ppst1xSys[ucIndex]->ucAssociationTag))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}




VOS_VOID CNAS_HSD_GetAcqedHrpdSysMatchIndexList(
    CNAS_PRL_HRPD_SYSTEM_STRU                              *pstHrpdSys
)
{
    VOS_UINT16                                              i;
    VOS_UINT16                                              usSysRecordNum;
    VOS_UINT16                                              usSysIndexNum;
    CNAS_PRL_SUBNET_MATCH_LVL_ENUM_UINT16                   enMatchLvl;
    CNAS_PRL_SUBNET_MATCH_LVL_ENUM_UINT16                   enMostMatchLvl;
    CNAS_PRL_EXT_SYS_RECORD_STRU                           *pstSysRecord;
    CNAS_HSD_MATCHED_HRPD_SYS_INDEX_LIST_STRU              *pstMatchedSysIndex;

    usSysIndexNum       = 0;
    enMostMatchLvl      = CNAS_PRL_SUBNET_MATCH_LVL_NOT_MATCH;
    usSysRecordNum      = CNAS_PRL_GetPrlSysRecordNum();
    pstMatchedSysIndex  = CNAS_HSD_GetMatchedHrpdSysIndexListAddr();

    /* ��ʼ��ƥ���ϵͳ�����б� */
    CNAS_HSD_InitMatchedHrpdSysIndexList();

    for (i = 0; i < usSysRecordNum; i++)
    {
         /* ��ȡSYS RECORD */
        pstSysRecord = CNAS_PRL_GetSpecifiedPrlValidSysRecord(i);

        if (VOS_NULL_PTR == pstSysRecord)
        {
            CNAS_WARNING_LOG1(UEPS_PID_HSD, "CNAS_HSD_GetAcqedHrpdSysMatchIndexList: Get SysRecord is Null,i is", i);
            continue;
        }

        /* Э��CDG 143 CHAPTER 2.10.2 */
        enMatchLvl = CNAS_PRL_GetHrpdSysRecMatchLvl(pstHrpdSys, pstSysRecord);

        if (CNAS_PRL_SUBNET_MATCH_LVL_NOT_MATCH == enMatchLvl)
        {
            continue;
        }

        /* ƥ��ȼ������ */
        if (enMatchLvl > enMostMatchLvl)
        {
            enMostMatchLvl  = enMatchLvl;
            usSysIndexNum   = 0;
            pstMatchedSysIndex->ausMatchedHrpdSysIndexList[usSysIndexNum] = i;
            usSysIndexNum++;
        }
        else if (enMatchLvl == enMostMatchLvl)
        {
            pstMatchedSysIndex->ausMatchedHrpdSysIndexList[usSysIndexNum] = i;
            usSysIndexNum++;
        }
        else
        {}

        pstMatchedSysIndex->usIndexNum = usSysIndexNum;

        if (usSysIndexNum >= CNAS_PRL_MAX_MATCHED_HRPD_SYSTEM_NUM)
        {
            CNAS_WARNING_LOG(UEPS_PID_HSD, "CNAS_HSD_GetAcqedHrpdSysMatchIndexList: Mactched HRPD sys index list is full");
            return;
        }

    }

    return;

}


VOS_UINT32 CNAS_HSD_IsAcqedHrpdSysInMatchIndexList(
    VOS_UINT16                                              usSysIndex
)
{
    VOS_UINT16                                              usIndexNum;
    CNAS_HSD_MATCHED_HRPD_SYS_INDEX_LIST_STRU              *pstMatchedSysIndex;

    pstMatchedSysIndex  = CNAS_HSD_GetMatchedHrpdSysIndexListAddr();

    for (usIndexNum = 0; usIndexNum < pstMatchedSysIndex->usIndexNum; usIndexNum++)
    {
        if ((VOS_UINT32)usSysIndex == pstMatchedSysIndex->ausMatchedHrpdSysIndexList[usIndexNum])
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;

}


#endif

/*lint -restore*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



