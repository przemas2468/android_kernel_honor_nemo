
#ifndef _ptp_var_h_
#define _ptp_var_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "ptp_timer.h"

/*ȫ�ֱ��Ĳ�����ָ��*/
extern PTP_CAPTURE_PACKET_FUNC g_pfPtpCapturePacketFunc;

extern PTP_GRANT_ACQUIRED_FUNC g_pfPtpGrantAcquired;
extern PTP_GRANT_ACQUIRED_ID_FUNC g_pfPtpGrantAcquiredById;

/* ά��ȫ��ָ�� */
extern PTP_DBG_OUTPUT_HOOK_FUNC     g_pfDbgOutput;
extern PTP_WARN_OUTPUT_HOOK_FUNC    g_pfWarnOutput;

/*����OC/BC PTPʵ��ȫ�ֱ���*/
extern PTP_CLOCK_S* g_pstPtpClock;

/*����TC PTPʵ��ȫ�ֱ���*/
extern PTP_TCLOCK_S* g_pstTcPtpClock;

/*����ʱ���豸���豸����ȫ�ֱ���*/
extern PTP_DEV_TYPE_E g_ePtpDevType;

/*����ʱ���豸�����ģʽ*/
extern PTP_SOFT_MODE_E g_ePtpSoftMode;

extern PTP_LOGIC_TIMER_H_S g_stLogicTimerHead ;

extern ULONG g_ulPtpTimerQId;

/*PTP���Ķ��е�ID*/
extern ULONG g_ulPtpPktQId;

/*PTP�ڲ����ƺ�������Ϣ���е�ID*/
extern ULONG g_ulPtpCtrolQId;

/*PTP����ID*/
extern ULONG g_ulPtpTaskId ;

/*PTP����ʱ��ID��VOS��ʱ��,100ms*/
extern ULONG g_ulPtpBaseTimerId ;

/*PTP���Ľ��յ��¼�ID����ΧShell�ɸ�������������������*/
extern ULONG g_ulPtpPktRcvEventId;

/* ά�⿪�� */
extern ULONG g_ulPtpGlobalDbgSwitch;

extern ULONG g_ulPtpTestSyncTimerFlag;


/*����ֻ����һ�γ�ʼ��*/
extern ULONG g_ulPtpInitFlag;


/*PTP�ź���*/
extern ULONG g_ulSemForPTP;

/* keep alive����ʱ���� */
extern CHAR g_cLogLiveInterval;

/* OVER physical��ַ All except peer delay mechanism messages */
extern UCHAR gucPTPMultiMac[MACADDRLEN];

extern UCHAR gucPTPDelayMultiMac[MACADDRLEN];

/*Added by utc ���� */
extern SHORT g_sCrtUtcOffsetValid;
extern SHORT g_sCrtUtcOffset;


/*qinyun&luogaowei 0��ַ���� 2011-12-12 start*/
/*qinyun&luogaowei 0��ַ���� 2011-12-12 end*/

extern DLL_S *g_stPtpvlanList;

/*extern ULONG g_ull3ifindex;*//*qinyun&luogaowei 0��ַ���� 2011-12-12 */

extern UCHAR g_ucPtpDomainSwitch;

extern UCHAR g_ucPtpConformitySwitch;

extern UCHAR g_ucPtpAlternateMasterFlag;

extern ULONG g_ulPtpMultiSelectMode;  /*5·ѡԴ����,ֻ��Slave��,Ŀǰֻ��Ե���,Ĭ��ΪFALSE,����·ѡԴ*/

extern ULONG g_ulPtpMasterDurationSwitch;

extern USHORT g_usPtpMaxAddrNum;

extern ULONG g_ulDelSlavePeriod;

/*PPI ȫ��PTP_PPI_HOOK_FUNC ���͵ĺ���ָ�붨��*/
extern PTP_PPI_HOOK_FUNC g_pfPtpPpiHookFunc;

/*���PTP PPI�·��ɲ�������ر������궨�� 2008-12-15.*/
typedef struct tagPTP_CPUTICK
{
    ULONG  ulLowTick;   
    ULONG  ulHighTick; 
}PTP_CPUTICK_S;

#define PTP_PPI_COUNT 4  /*�·�����*/
typedef struct tagPTP_PPI_TIME
{
    PTP_CPUTICK_S stStartTime[PTP_PPI_COUNT];/*�·���¼�Ŀ�ʼʱ��*/
    PTP_CPUTICK_S stEndTime[PTP_PPI_COUNT];  /*�·���¼�Ľ���ʱ��*/
    ULONG         ulOprType[PTP_PPI_COUNT];  /*�·��Ĳ�������*/
    ULONG         ulPTPPpiCount;             /*�·�����*/
}PTP_PPI_TIME_S;

extern ULONG g_ulPTPPpiTimeSwitch;   /*PTP�·���ʱ���㿪��*/
extern PTP_PPI_TIME_S g_ulPTPPpiTime;/*PTP�·���ʱͳ�ƴ������*/

/*PTP�·���ǰ��ʱ���*/
#define MAKE_PTPPPI_STARTDOT(OprType)\
{\
    if (g_ulPTPPpiTimeSwitch)\
    {\
        ULONG ii = g_ulPTPPpiTime.ulPTPPpiCount;\
        ii %= PTP_PPI_COUNT;\
        VOS_GetCpuTick(&(g_ulPTPPpiTime.stStartTime[ii].ulLowTick),  \
                        &(g_ulPTPPpiTime.stStartTime[ii].ulHighTick));\
        g_ulPTPPpiTime.ulOprType[ii] = OprType;\
    }\
}
/*PTP�·��ķ��غ�ʱ����*/
#define MAKE_PTPPPI_ENDDOT()\
{\
    if (g_ulPTPPpiTimeSwitch)\
    {\
        ULONG ii = g_ulPTPPpiTime.ulPTPPpiCount;\
        ii %= PTP_PPI_COUNT;\
        VOS_GetCpuTick(&(g_ulPTPPpiTime.stEndTime[ii].ulLowTick),  \
                        &(g_ulPTPPpiTime.stEndTime[ii].ulHighTick));\
        g_ulPTPPpiTime.ulPTPPpiCount++;\
    }\
}

/*��·SyncЭ��ѡԴ�·�PPI��ʹ�øú�*/
#define PTP_PPI_NOTIFY(usPtpPortNumber, ulPtpPpiCmd, ulVrfIndex, pstPtpPpiInfo)\
{                                    \
    if(PTP_NO == g_ulPtpMultiSelectMode) \
    {                                \
        PTP_PPI_NOTIFY_COMMON(usPtpPortNumber, ulPtpPpiCmd, ulVrfIndex, pstPtpPpiInfo);\
    }                                \
}

/*5·SyncЭ��ѡԴ�·�PPI��ʹ�øú�*/
#define PTP_PPI_NOTIFY5(usPtpPortNumber, ulPtpPpiCmd, ulVrfIndex, pstPtpPpiInfo)\
{                                     \
    if(PTP_YES == g_ulPtpMultiSelectMode) \
    {                                 \
        PTP_PPI_NOTIFY_COMMON(usPtpPortNumber, ulPtpPpiCmd, ulVrfIndex, pstPtpPpiInfo);\
    }                                 \
}

/*�ú겻��������ֱ�ӵ��ã���ʹ������������*/
#define PTP_PPI_NOTIFY_COMMON(usPtpPortNumber, ulPtpPpiCmd, ulVrfIndex, pstPtpPpiInfo)\
{                                                                                \
    PTP_ALL_PORT_ADDR_VRF_NODE_S stPtpPpiAddrVrfNode = {0};                    \
    ULONG ulPpiRet = PTP_OK;                                                   \
    ULONG ulDefVrfIndex = 0;                                                   \
    if (NULL != g_pfPtpPpiHookFunc)                                          \
    {                                                                          \
        if (ulDefVrfIndex != (ulVrfIndex))                                     \
        {                                                                      \
            (VOID)PTP_GetPortAddrVrfByVrf(usPtpPortNumber, ulVrfIndex, &stPtpPpiAddrVrfNode);\
            if(0 != stPtpPpiAddrVrfNode.ulNum)                                 \
            {                                                                  \
                (pstPtpPpiInfo)->ulIfIndex = stPtpPpiAddrVrfNode.szPortAddrVrf[0].ulIfIndex;\
            }                                                                  \
            else                                                               \
            {                                                                  \
                VOS_DBGASSERT(BOOL_FALSE);                                     \
            }                                                                  \
        }                                                                      \
        MAKE_PTPPPI_STARTDOT(ulPtpPpiCmd);                                     \
        ulPpiRet = g_pfPtpPpiHookFunc(usPtpPortNumber, ulPtpPpiCmd, pstPtpPpiInfo);\
        MAKE_PTPPPI_ENDDOT();                                                  \
        PTP_PPI_LogMsg(ulPpiRet, usPtpPortNumber, ulPtpPpiCmd, pstPtpPpiInfo); \
    }                                                                          \
}

extern PTP_ADDRINFO_S g_stPtpPacketFilterAddr;

extern ULONG g_ulPtpSeqIdCompatType;

extern ULONG g_ulPtpClassCheckSwitch; /* PTP_NO:У��(Ĭ��)  PTP_YES:��У��(PT5000�Խ���) */

extern UCHAR g_ucBmcSourceMode;
extern ULONG g_ulNegoAndBmcSplitMode;
extern UCHAR g_ucPtpMutexCfgFlag;       /*����ģʽ��keeplive �ӿڻ����ǣ���ʼ��Ϊ0, �������ڻ������*/

#ifdef __cplusplus
}
#endif

#endif

