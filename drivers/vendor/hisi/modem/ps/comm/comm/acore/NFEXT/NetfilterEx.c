

/******************************************************************************
   1 ͷ�ļ�����
******************************************************************************/
#include "product_config.h"

#if (FEATURE_ON == FEATURE_NFEXT)

#include "NetfilterEx.h"

#if (VOS_OS_VER == VOS_WIN32)
#include <linuxstub.h>
#else
#include "linux/inet.h"
#endif

/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/

#define THIS_FILE_ID PS_FILE_ID_ACPU_NFEX_C


/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define             PACKAGE_HEAD_LEN        (80)        /*�趨�Ľ�ȡ��ͷ�ĳ���*/
#define             OM_SOCK_PORT_NUM        (3000)      /*��OM�ĺ�SOCK_PORT_NUM����һ��*/
/*****************************************************************************
  3 �ⲿ������������
*****************************************************************************/

extern NF_EXT_ENTITY_STRU           g_stExEntity;
extern NF_EXT_FLOW_CTRL_ENTITY      g_stExFlowCtrlEntity;

/******************************************************************************
  4 ����ʵ��
******************************************************************************/

NF_EXT_FLAG_OM_DATA_ENUM_U32 NFExt_IsOmData(struct sk_buff *skb)
{
    struct iphdr        *ipHeader;
    __be32               srcIp;
    __be32               destIp;
    struct tcphdr       *tcpHeader;
    __be16               srcPort;
    __be16               destPort;
    __be32               ulOmSocketIp;

    ipHeader        = (struct iphdr *)(skb_network_header(skb));

    /*�������TCP������ֱ�ӷ���*/
    if ( NF_EXT_RPO_TCP != ipHeader->protocol )
    {
        return NF_EXT_FLAG_NOT_OM_DATA;
    }

    /* ������������ip��֮�� */
    tcpHeader       = (struct tcphdr *)(skb_network_header(skb) + sizeof(struct iphdr));

    srcIp           = ipHeader->saddr;
    destIp          = ipHeader->daddr;
    srcPort         = ntohs(tcpHeader->source);
    destPort        = ntohs(tcpHeader->dest);
    ulOmSocketIp    = g_stExEntity.ulOmIp;

    if ( ((ulOmSocketIp == srcIp) && (OM_SOCK_PORT_NUM == srcPort))
      || ((ulOmSocketIp == destIp) && (OM_SOCK_PORT_NUM == destPort)) )
    {
        return NF_EXT_FLAG_OM_DATA;
    }

    return NF_EXT_FLAG_NOT_OM_DATA;
}


VOS_VOID NFExt_BrDataExport( struct sk_buff *skb,
                                const struct net_device *device_in,
                                const struct net_device *device_out,
                                TTF_MNTN_MSG_TYPE_ENUM_UINT16 enType)
{
    VOS_UINT8                       *pucData;
    VOS_UINT32                       ulHookDataLen;

    /* skb->dataָ�����ݰ���IPͷ��������14���ֽ��� pucDataָ�����ݰ���macͷ�� */
    pucData             = skb->data - MAC_HEADER_LENGTH;
    ulHookDataLen       = ((skb->len > NF_EXT_MAX_IP_SIZE) ? NF_EXT_MAX_IP_SIZE : skb->len) + MAC_HEADER_LENGTH;

    IPS_MNTN_BridgePktInfoCB((const VOS_UINT8 *)device_in->name, (const VOS_UINT8 *)device_out->name, pucData, (VOS_UINT16)ulHookDataLen, enType);
}


VOS_VOID NFExt_ArpDataExport( struct sk_buff *skb,
                                    const struct net_device *device,
                                    TTF_MNTN_MSG_TYPE_ENUM_UINT16 enType)
{
    VOS_UINT8                       *pucData;
    VOS_UINT32                       ulHookDataLen;

    pucData             = skb->data;
    ulHookDataLen       = skb->len;

    IPS_MNTN_CtrlPktInfoCB((const VOS_UINT8 *)device->name, (const VOS_UINT8 *)pucData, (VOS_UINT16)ulHookDataLen, enType);
}


VOS_VOID NFExt_IpDataExport( struct sk_buff *skb,
                         const struct net_device *device,
                         TTF_MNTN_MSG_TYPE_ENUM_UINT16 enType)
{
    VOS_UINT8                       *pucData;
    VOS_UINT32                       ulHookDataLen;

    if ( NF_EXT_FLAG_OM_DATA == NFExt_IsOmData(skb) )
    {
        return;
    }

    /* skb->dataָ�����ݰ���IPͷ��������14���ֽ��� pucDataָ�����ݰ���macͷ�� */
    pucData           = skb->data - MAC_HEADER_LENGTH;
    ulHookDataLen     = ((skb->len > NF_EXT_MAX_IP_SIZE) ? NF_EXT_MAX_IP_SIZE : skb->len) + MAC_HEADER_LENGTH;

    IPS_MNTN_PktInfoCB((const VOS_UINT8 *)device->name, (const VOS_UINT8 *)pucData, (VOS_UINT16)ulHookDataLen, enType);
}



unsigned int NFExt_BrPreRoutingHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    /* �ж��Ƿ�OM������ */
    if ( NF_EXT_FLAG_OM_DATA == NFExt_IsOmData(skb) )
    {
        return NF_ACCEPT;
    }

    NFExt_BrDataExport(skb, in, out, ID_IPS_TRACE_BRIDGE_PRE_ROUTING_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_BrPostRoutingHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    if ( NF_EXT_FLAG_OM_DATA == NFExt_IsOmData(skb) )
    {
        return NF_ACCEPT;
    }

    NFExt_BrDataExport(skb, in, out, ID_IPS_TRACE_BRIDGE_POST_ROUTING_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_BrLocalInHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    if ( NF_EXT_FLAG_OM_DATA == NFExt_IsOmData(skb) )
    {
        return NF_ACCEPT;
    }

    NFExt_BrDataExport(skb, in, out, ID_IPS_TRACE_BRIDGE_LOCAL_IN_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_BrLocalOutHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    if ( NF_EXT_FLAG_OM_DATA == NFExt_IsOmData(skb) )
    {
        return NF_ACCEPT;
    }

    NFExt_BrDataExport(skb, in, out, ID_IPS_TRACE_BRIDGE_LOCAL_OUT_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_BrForwardHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_BrDataExport(skb, in, out, ID_IPS_TRACE_BRIDGE_DATA_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_ArpInHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_ArpDataExport(skb, in, ID_IPS_TRACE_RECV_ARP_PKT);

    return NF_ACCEPT;
}



unsigned int NFExt_ArpOutHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{

    NFExt_ArpDataExport(skb, out, ID_IPS_TRACE_SEND_ARP_PKT);

    return NF_ACCEPT;
}


unsigned int NFExt_Ip4PreRoutingHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_IpDataExport(skb, in, ID_IPS_TRACE_INPUT_DATA_INFO);

    return NF_ACCEPT;
}



unsigned int NFExt_Ip4PostRoutingHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_IpDataExport(skb, out, ID_IPS_TRACE_OUTPUT_DATA_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_Ip4LocalInHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{

    NFExt_IpDataExport(skb, in, ID_IPS_TRACE_IP4_LOCAL_IN_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_Ip4LocalOutHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_IpDataExport(skb, out, ID_IPS_TRACE_IP4_LOCAL_OUT_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_Ip4ForwardHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_IpDataExport(skb, in, ID_IPS_TRACE_IP4_FORWARD_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_Ip6PreRoutingHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_IpDataExport(skb, in, ID_IPS_TRACE_INPUT_DATA_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_Ip6PostRoutingHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_IpDataExport(skb, out, ID_IPS_TRACE_OUTPUT_DATA_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_Ip6LocalInHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_IpDataExport(skb, in, ID_IPS_TRACE_IP6_LOCAL_IN_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_Ip6LocalOutHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_IpDataExport(skb, out, ID_IPS_TRACE_IP6_LOCAL_OUT_INFO);

    return NF_ACCEPT;
}


unsigned int NFExt_Ip6ForwardHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NFExt_IpDataExport(skb, in, ID_IPS_TRACE_IP6_FORWARD_INFO);

    return NF_ACCEPT;
}

/*****************************************************************************
                        ���ع���
*****************************************************************************/



unsigned int NFExt_BrForwardFlowCtrlHook(unsigned int hooknum,
                            struct sk_buff *skb,
                            const struct net_device *in,
                            const struct net_device *out,
                            int (*okfn)(struct sk_buff *))
{
    NF_EXT_STATS_INC(1, NF_EXT_STATS_BR_FC_ENTER);

    /* ����ת��ͳ�� */
    NF_EXT_TX_BYTES_INC(skb->len, NF_EXT_TX_BYTES_CNT_BR);


    /* ��ǰ������forward����״̬��ֱ�Ӷ��� */
    if (NF_EXT_BR_FORWARD_FLOW_CTRL_MASK == (g_stExFlowCtrlEntity.ulFlowCtrlMsk & NF_EXT_BR_FORWARD_FLOW_CTRL_MASK))
    {
        NF_EXT_STATS_INC(1, NF_EXT_STATS_BR_FC_DROP);
        return NF_DROP;
    }

    return NF_ACCEPT;
}

#endif      /* #if (FEATURE_ON == FEATURE_NFEXT) */
