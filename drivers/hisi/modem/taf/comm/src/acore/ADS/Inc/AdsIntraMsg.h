

#ifndef __ADS_INTRA_MSG_H__
#define __ADS_INTRA_MSG_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include "vos.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* ADS普通道消息起始值 */
#define ID_ADS_COMM_MSG_BASE_ID     (0x1000)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 枚举名称  : AT_INTER_MSG_ID_ENUM
 枚举说明  : 定义AT到AT内部消息ID
*****************************************************************************/
enum ADS_INTRA_MSG_ID_ENUM
{
    /* 保留消息     [0x0000, 0x0FFF] */

    /* 普通消息     [0x1000, 0x10FF] */
    ID_ADS_CCPU_RESET_START_IND         = ID_ADS_COMM_MSG_BASE_ID + 0x01,       /* _H2ASN_MsgChoice ADS_CCPU_RESET_IND_STRU */
    ID_ADS_CCPU_RESET_END_IND           = ID_ADS_COMM_MSG_BASE_ID + 0x02,       /* _H2ASN_MsgChoice ADS_CCPU_RESET_IND_STRU */


};
typedef VOS_UINT32 ADS_INTRA_MSG_ID_ENUM_UINT32;


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


typedef struct
{
    VOS_MSG_HEADER                                                              /* 消息头 */    /* _H2ASN_Skip */
    ADS_INTRA_MSG_ID_ENUM_UINT32        enMsgId;                                /* 消息ID */    /* _H2ASN_Skip */
} ADS_CCPU_RESET_IND_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/






#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of AdsIntraMsg.h */
