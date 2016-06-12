


#if( VRP_MODULE_LINK_PPP == VRP_YES )

#if( VRP_MODULE_LINK_PPP_PAP == VRP_YES )

#ifndef      _PAP_DEF_H_
#define      _PAP_DEF_H_

#ifdef      __cplusplus
extern "C" {
#endif		/* end of __cplusplus */

#define PAP_HEADERLEN	(sizeof (UCHAR) + sizeof (UCHAR) + sizeof (VOS_UINT16))


/* PAP ����֡���� codes. */
#define PAP_AUTHREQ	1	/* Authenticate-Request */
#define PAP_AUTHACK	2	/* Authenticate-Ack */
#define PAP_AUTHNAK	3	/* Authenticate-Nak */

/*PAP ״̬����״ֵ̬ */
#define PAP_STATE_CLIENT_INITIAL	0
#define PAP_STATE_SERVER_INITIAL	0
#define PAP_STATE_SEND_AUTHREQ		1
#define PAP_STATE_SERVER_LISTEN	    1
#define PAP_STATE_SERVER_SUCCESS	2
#define PAP_STATE_CLIENT_SUCCESS	2
#define PAP_STATE_SERVER_FAILED	    3
#define PAP_STATE_CLIENT_FAILED	    3
#define PAP_STATE_WAIT_AAA			4

/*PAP ����CORE���¼����� */
#define PAP_EVENT_CLIENTLOWERUP	0
#define PAP_EVENT_SERVERLOWERUP 1
#define	PAP_EVENT_LOWERDOWN		2

/*PAP �ȴ��Զ���֤�ʱ�� */
#define PAP_WAITREQUESTTIME	    60000   
#define PAP_WAITLNSAUTHTIME             10000
#define PAP_WAITAAARSPTIME      20000
#define PAP_MAXAUTHTRANSMITS	1	/* max # times to send AUTH msg */

#define PPP_PAP_AAAAuthFailed(pstPapInfo) \
{   \
    AAA_AUTHREQ_S *pstMsgTemp = pstPapInfo->pstAAAAuthReqMsg;  \
    PPPINFO_S *pstPppInfoTemp = pstPapInfo->pstPppInfo;     \
    if(pstMsgTemp)                                          \
    {                                                   \
        PPP_Free (pstMsgTemp);                              \
        pstPapInfo->pstAAAAuthReqMsg = NULL;            \
    }                                                   \
    pstPapInfo->ucAuthTransmits = 0 ;                   \
    PAP_Debug_StateChange(pstPppInfoTemp, PAP_STATE_SERVER_FAILED, SERVER_STATE) ;  \
    pstPapInfo->ucServerState = PAP_STATE_SERVER_FAILED ;   \
    PPP_Core_ReceiveEventFromProtocol (pstPppInfoTemp,PPP_EVENT_PAPFAILED,NULL) ;    \
}

#ifdef      __cplusplus
}
#endif      /* end of __cplusplus */

#endif      /* end of _PAP_DEF_H_ */

#endif  /* #if( VRP_MODULE_LINK_PPP_PAP == VRP_YES ) */

#endif	/* #if( VRP_MODULE_LINK_PPP == VRP_YES ) */





