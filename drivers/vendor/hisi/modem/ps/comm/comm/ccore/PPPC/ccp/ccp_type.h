

#ifdef		__cplusplus
extern		"C"{
#endif

#ifndef _CCP_TYPE_H_
#define _CCP_TYPE_H__
/********************************************************************/

/********************************************************************/
typedef struct tagPppCcpInfo
{
    PPPINFO_S *pstPppInfo ;            /* PPP���ƿ�ָ�� */
    PPP_CCP_options_S stGotOptions;    /* �Ѿ�Э�̵����ҵ�ѡ�� */
    PPP_CCP_options_S stHisOptions;    /* �Ѿ�Э�̵��ĶԷ���ѡ�� */
    PPP_CCP_options_S stWantOptions;   /* ��ϣ����Է�Э�̵��ҵ�ѡ�� */
    PPP_CCP_options_S stAllowOptions;  /* ������Է�Э�̵��ҵ�ѡ�� */
    PPPFSM_S  stFsm;                   /* CCPЭ��״̬�� */
    HTIMER     ulCcpTimeoutID;          /* CCPЭ�̳�ʱ��ʱ�� */

    VOS_UINT32     ulTimeOutTime;
    UCHAR  ucPppCompType;           /* 1��MPPC��2��LZS0��3��LZS1*/
    UCHAR  ucUsed;               /*ccp���ƿ�ռ�ñ��*/
    UCHAR  ucPppCcpInfoRev[2];     /*�����ֶ�*/


} PPP_CCP_INFO_S ;
/********************************************************************/

#endif /*_CCP_TYPE_H_*/

#ifdef		__cplusplus
}
#endif
