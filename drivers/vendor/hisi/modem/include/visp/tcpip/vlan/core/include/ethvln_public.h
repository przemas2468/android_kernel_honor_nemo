
#ifndef _ETHVLN_PUBLIC_H_
#define _ETHVLN_PUBLIC_H_
#ifdef  __cplusplus
extern "C"{
#endif

#include"tcpip/vlan/include/ethvln_api.h"

enum 
{
    ETHVLAN_DEBUG_RECEIVE,
    ETHVLAN_DEBUG_SEND
};

/* VLANһ���Ը澯����ʱ��ƥ�䷽ʽ,��ȫƥ�� */
#define ETHVLAN_WARN_MATCH_EXACT        0
/* VLANһ���Ը澯����ʱ��ƥ�䷽ʽ,ֻƥ�䱾��IP */
#define ETHVLAN_WARN_MATCH_LOCALIP      1
/* VLANһ���Ը澯����ʱ��ƥ�䷽ʽ,ֻƥ��VLANID */
#define ETHVLAN_WARN_MATCH_VLANID       2
/* VLANһ���Ը澯����ʱ��ƥ�䷽ʽ,ͨ�� */
#define ETHVLAN_WARN_MATCH_WILD         3


typedef  struct tagETHVLAN_VID_NODE
{
 
    ULONG        ulMainIfIndex;                  /*��VID �����õ����ӿ�����*/
    ULONG        ulSubIfIndex;                   /*VID���ڵ��ӽӿ�����*/
    struct  tagETHVLAN_VID_NODE * pstNextSubInf; /*��һ������ͬVID����̫�ӽӿ�*/
    struct  tagETHVLAN_VID_NODE * pstPrevSubInf; /*ǰһ������ͬVID����̫�ӽӿ�*/   
    struct  tagETHVLAN_VID_NODE * pstNextVID;    /*�ӽӿ������õ���һ��VID�Ľڵ�*/

    struct  tagETHVLAN_VID_NODE * pstPrevVID;    /*�ӽӿ������õ�ǰһ��VID�Ľڵ�*/

    /*��VID��ͳ����Ϣ*/
    ULONG      ulVIDReceivePacketCount;
    ULONG      ulVIDTransmitPacketCount;
    ULONG      ulVIDReceiveByteCount;
    ULONG      ulVIDTransmitByteCount;

    USHORT      usVID ;                         /*���ڵ��VLANID*/
    USHORT      encapType ;
    UCHAR        ucDebugVlanPacket;
    UCHAR        ucRsv[3];
}ETHVLAN_VID_NODE_S;

/* VLANһ���Ը澯�ڵ� */
typedef struct tagVLAN_CONSISTENCY_WARN_NODE
{
    struct tagVLAN_CONSISTENCY_WARN_NODE *pstPrevNode;
    struct tagVLAN_CONSISTENCY_WARN_NODE *pstNextNode;
    ULONG  ulSubIfIndex;        /* �ӽӿ����� */
    ULONG  ulLocalIP;           /* ����IP,������ */
    ULONG  ulNeighbourIP;       /* �ھ�IP,������ */
    USHORT usNeighbourVlanID;   /* �ھ�IP����VLAN */
    USHORT usReserve;           /* �����ֶ� */
}VLAN_CONSISTENCY_WARN_NODE_S;
/* VLANһ���Ը澯���� */
typedef struct tagVLAN_CONSISTENCY_WARN_LIST
{
    VLAN_CONSISTENCY_WARN_NODE_S    *pstHeadList;
    VLAN_CONSISTENCY_WARN_NODE_S    *pstTailList;
    ULONG  ulCurWarningNum; /* �����еĵ�ǰ�澯��Ŀ */
    ULONG  ulMaxWarningNum; /* ���õ����澯��Ŀ */
}VLAN_CONSISTENCY_WARN_LIST_S;

typedef  struct tagETHVLAN_PORTINFO
{
    ULONG   ulEncapType;                        /*��װ��VLAN �ĸ�ʽ 0-ETHVLAN_NONE 1-DOT1Q 2-ISL */
    ULONG   ulRecvMbufErr;                      /*���ӿ�ͳ����Ϣ*/
    ULONG   ulRecvInvalidVid;                   /*���ӿ�ͳ����Ϣ*/
    ULONG   ulRecvInvalidSubIfStat;             /*�ӽӿ�ͳ����Ϣ*/
    ULONG   ulRecvInvalidSubEncapType;          /*�ӽӿ�ͳ����Ϣ*/
    ULONG   ulSendMbufErr;                      /*�ӽӿ�ͳ����Ϣ*/
    ULONG   ulSendInvalidVid;                   /*�ӽӿ�ͳ����Ϣ*/
    ULONG   ulSendPhyErr;                       /*�ӽӿ�ͳ����Ϣ*/

    ULONG   ulVlanNum;                          /*�ӿ��·�װ��VLAN������*/ 
    USHORT  usMinVID ;                          /*�ӽӿ��Ϸ�װ����С��VLANID*/
    USHORT  usMaxVID ;                          /*�ӽӿ��Ϸ�װ������VLANID*/

    ETHVLAN_VID_NODE_S * pstHeadNode;           /*ָ��VLAN���ӽ����õ�VLAN��ͷ���*/
    ETHVLAN_VID_NODE_S * pstTailNode;           /*ָ��VLAN���ӽ����õ�VLAN��β���*/
    
    VLAN_CONSISTENCY_WARN_LIST_S stVlanWarningList; /* VLAN���ò�һ�¸澯���� */
 }ETHVLAN_PORTINFO_S;

#define  ETHVLAN_GETPORTINFOBYSUBIF(pstSubIf) \
     ((ETHVLAN_PORTINFO_S *)((IF_GETMTSOFT(pstSubIf))?(((ETHARP_MTSOFTCSUB_S*)IF_GETMTSOFT(pstSubIf))->ulVLANPortInfo):NULL))

#define ETHVLAN_GETPORTINFOBYIF(pstIf) \
     ((ETHVLAN_PORTINFO_S *)((IF_GETMTSOFT(pstIf))?(((ETHARP_MTSOFTC_S*)IF_GETMTSOFT(pstIf))->ulVLANPortInfo):NULL))
#define  ETHVLAN_GETVLANNODE( usVID, ifIndex,ulEncapType,ppVidNode)\
{\
    (*(ppVidNode)) = g_pstVIDNode[(usVID)];\
    while (*(ppVidNode) != NULL)\
    {\
        if ( (((USHORT)(ulEncapType) == ((*(ppVidNode))->encapType)) )  \
            && ((*(ppVidNode))->ulMainIfIndex == (ifIndex)\
            || (*(ppVidNode))->ulSubIfIndex == (ifIndex)))\
        {\
            break;\
        }\
        (*(ppVidNode)) = (*(ppVidNode))->pstNextSubInf;/*ȡ����һ��ͬһVID */\
    }\
}

/* ��VLAN�澯������ȡ����ǰ��ĸ澯�ڵ� */
#define ETHVLN_WARNINGLIST_REMOVE_FIRSTNODE(pstWarningList, pstWarningNode) \
{\
    (pstWarningNode) = (pstWarningList)->pstHeadList;\
    if ((pstWarningNode) != NULL)\
    {\
        (pstWarningList)->pstHeadList =  (pstWarningNode)->pstNextNode;\
        if ((pstWarningNode)->pstNextNode)\
        {\
            (pstWarningNode)->pstNextNode->pstPrevNode = NULL;\
        }\
        (pstWarningList)->ulCurWarningNum--;\
        if ((pstWarningList)->pstTailList == (pstWarningNode) )\
        {\
            (pstWarningList)->pstTailList = (pstWarningNode)->pstPrevNode;\
        }\
    }\
}

/* ���澯�ڵ����澯����β�� */
#define ETHVLN_WARNINGLIST_INSERT_TO_TAIL(pstWarningList, pstWarningNode) \
{\
    /* ������ */\
    if (NULL == (pstWarningList)->pstTailList)\
    {\
        (pstWarningList)->pstTailList = (pstWarningList)->pstHeadList = (pstWarningNode);\
    }\
    else\
    {\
        (pstWarningList)->pstTailList->pstNextNode = (pstWarningNode);\
        (pstWarningNode)->pstPrevNode = (pstWarningList)->pstTailList;\
        (pstWarningList)->pstTailList = (pstWarningNode);\
    }\
    (pstWarningList)->ulCurWarningNum++;\
}


#ifdef __cplusplus
}
#endif  /* end of __cplusplus */

#endif /* end of  _ETHVLN_PUBLIC_H_*/


