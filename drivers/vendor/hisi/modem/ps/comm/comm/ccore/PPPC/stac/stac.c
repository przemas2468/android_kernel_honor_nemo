



/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "ppp_inc.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
#define         THIS_FILE_ID            PS_FILE_ID_PPPC_VSNCP_STAC_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

#if( VRP_MODULE_STAC == VRP_YES )
/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
/*lint -save -e958 */


LONG STAC_InitStacCompress( STAC_COMPRESSINFO_S *pstInfo )
{

    pstInfo->pucInputStream = VOS_NULL_PTR;
    pstInfo->usInlen = 0;
    pstInfo->pucOutputStream = VOS_NULL_PTR;
    pstInfo->usOutlen = 0;

    pstInfo->usBufferTail = 0;
    pstInfo->pucHoldBuf = VOS_NULL_PTR;
    pstInfo->usHoldLen = 0;

    /*�����λ���*/
    pstInfo->ucBitMask = 0x80;

    pstInfo->ucSequenceNum = 1;
    pstInfo->pucOutput = VOS_NULL_PTR;

    return STAC_SUCCESS;
}



VOID  STAC_ExitStacCompress( STAC_COMPRESSINFO_S * pstInfo )
{
 #if 0
    if (pstInfo->pstHashTable != VOS_NULL_PTR)
    {
        STAC_HASH_FreeHashTable( pstInfo->pstHashTable );
        pstInfo->pstHashTable = VOS_NULL_PTR;
    }

    if (pstInfo->pucBuffer != VOS_NULL_PTR)
    {
        (VOID)PPP_Free( pstInfo->pucBuffer );
        pstInfo->pucBuffer = VOS_NULL_PTR;
    }
 #endif


    /*�ص���ʼ״̬*/
    pstInfo->pucInputStream = VOS_NULL_PTR;
    pstInfo->usInlen = 0;
    pstInfo->pucOutputStream = VOS_NULL_PTR;
    pstInfo->usOutlen = 0;
    pstInfo->usBufferTail = 0;
    pstInfo->pucHoldBuf = VOS_NULL_PTR;
    pstInfo->usHoldLen = 0;
    pstInfo->ucBitMask = 0x80;
    pstInfo->ucSequenceNum = 0;
    pstInfo->pucOutput = VOS_NULL_PTR;

    return;
}



LONG     STAC_InitStacDecompress( STAC_DECOMPRESSINFO_S *pstInfo )
{

    if(VOS_NULL_PTR == pstInfo)
    {
        VOS_DBGASSERT(pstInfo);
        return STAC_FAIL;
    }
    pstInfo->pucInputStream = VOS_NULL_PTR;
    pstInfo->pucInput = VOS_NULL_PTR;
    pstInfo->usInlen = 0;
    pstInfo->pucOutputStream = VOS_NULL_PTR;
    pstInfo->usOutlen   = 0;
    pstInfo->pucHoldBuf = VOS_NULL_PTR;
    pstInfo->usHoldLen  = 0;

    /*�����λ����*/
    pstInfo->ucBitMask = 0x80;

    pstInfo->usBufferTail  = 0;
    pstInfo->ucSequenceNum = 1;

    pstInfo->ucResetReqSend = 0;

    pstInfo->ulTimeoutID   = 0;
    pstInfo->ulTimeOutTime = STAC_SENDRSTREQTIMEOUT;
    return STAC_SUCCESS;
}


VOID     STAC_ExitStacDecompress( STAC_DECOMPRESSINFO_S *pstInfo )
{

    if(VOS_NULL_PTR == pstInfo)
    {
        VOS_DBGASSERT(pstInfo);
        return;
    }
    pstInfo->pucBuffer = VOS_NULL_PTR;

    /*�ص���ʼ״̬*/
    pstInfo->pucInputStream = VOS_NULL_PTR;
    pstInfo->pucInput = VOS_NULL_PTR;
    pstInfo->usInlen = 0;
    pstInfo->pucOutputStream = VOS_NULL_PTR;
    pstInfo->usOutlen   = 0;
    pstInfo->pucHoldBuf = VOS_NULL_PTR;
    pstInfo->usHoldLen  = 0;
    pstInfo->ucBitMask  = 0x80;

    pstInfo->usBufferTail  = 0;
    pstInfo->ucSequenceNum = 1;

    pstInfo->ucResetReqSend = 0;

    return;
}




VOID STAC_ResetHistorybuffer(STAC_COMPRESSINFO_S *pstInfo)
{

    if(VOS_NULL_PTR ==  pstInfo)
    {
        VOS_DBGASSERT(pstInfo);
        return;
    }

    /*��βָ��*/
    pstInfo->usBufferTail = 0;

    /*����ر�־λ*/
    pstInfo->pstHashTable->ulHashTable_NextNode = 0L;
    pstInfo->pstHashTable->ucHashTable_Recycled = 0L;
    pstInfo->ucBitMask = 0x80;

}



#endif

/*lint -restore */


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif