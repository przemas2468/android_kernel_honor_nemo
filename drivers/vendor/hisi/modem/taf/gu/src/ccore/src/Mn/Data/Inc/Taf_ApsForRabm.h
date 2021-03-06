

#ifndef _TAF_APSFORRABM_H_
#define _TAF_APSFORRABM_H_


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  #pragma pack(*)    设置字节对齐方式
*****************************************************************************/
#pragma pack(4)

/*****************************************************************************
  2 宏定义
  #define
*****************************************************************************/

/*---------------------------------------------------------------------------
        RABM发给APS的消息类型
---------------------------------------------------------------------------*/
#define ID_RABM_APS_SYS_CHG_TO_GPRS_IND     0x0130

#define RABM_APS_LOCAL_PDP_DEACT_REQ_MSG    0x0160


/*******************************************************************************
  3 枚举定义
  typedef enum
{
    模块名_意义描述_ENUM
    MAPS_TEMPLATE_ENUM1 = 0,
    MAPS_TEMPLATE_ENUM2


    MAPS_TEMPLATE_BUTT,    用这个统一的BUTT，作为最后一个枚举值。

}MAPS_TEMPLATE_ENUM;   模块名_枚举类型描述_ENUM
*******************************************************************************/



/*****************************************************************************
  4 全局变量声明
extern VOS_UINT8* pgucMapsTemplateBuffer;   指针＋全局标识＋类型＋模块名＋意义描述
*****************************************************************************/




/*****************************************************************************
  6 消息定义
  typedef struct
{
    MAPS_TEMPLATE_HEADER
    VOS_UINT8    aucData[2];     消息体

} MAPS_TEMPLATE_MSG;
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
  typedef struct
{
    VOS_UINT8    ucTemplate;
    VOS_UINT8    aucData[2];

} MAPS_TEMPLATE_ST;
*****************************************************************************/



/*****************************************************************************
  8 UNION定义
  typedef union
{
    VOS_UINT8    ucTemplate;
    VOS_UINT8    aucData[2];

} MAPS_TEMPLATE_UNION;
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
  模块名＋意义＋HEADER
#define MAPS_TEMPLATE_HEADER            VOS_UINT16    usTotalsMsgLen;\
                                        VOS_UINT32    ulSendModuleId;\
                                        VOS_UINT32    ulRecvModuleId;\
*****************************************************************************/
/*---------------------------------------------------------
            MSG used between APS and RABM.
---------------------------------------------------------*/
typedef struct{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgType;          /*消息类型*/
}RABM_APS_SYS_CHANGE_MSG;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgType;          /*消息类型*/
    VOS_UINT8                           ucNsapi;            /*NSAPI*/
    VOS_UINT8                           ucRsv;              /*字节对齐*/
    VOS_UINT8                           ucReserved[2];
}RABM_APS_LOCAL_PDP_DEACT_REQ;    /*PDP本地去激活请求*/


/*****************************************************************************
  8 FUNCTIONS定义
*****************************************************************************/



/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of MapsTemplate.h*/
