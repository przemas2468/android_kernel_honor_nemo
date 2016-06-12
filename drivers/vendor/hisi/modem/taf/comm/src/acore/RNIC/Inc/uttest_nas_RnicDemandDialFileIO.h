
#ifndef _UTTEST_NAS_RNIC_DEMAND_DIAL_FILE_IO_H_
#define _UTTEST_NAS_RNIC_DEMAND_DIAL_FILE_IO_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "RnicDemandDialFileIO.h"
#include "RnicCtx.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define    THIS_FILE_ID                 PS_FILE_ID_RNIC_DEMAND_DIAL_FILE_IO_C

/*****************************************************************************
3 枚举定义
*****************************************************************************/



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


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern void UT_STUB_INIT(void);

extern VOS_UINT32 RNIC_TransferStringToInt(VOS_CHAR *pcString);

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


#endif /* end of uttest_nas_RnicDemandDialFileIO.h */


