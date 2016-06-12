

#ifndef __TLPHY_ERRNO_H__
#define __TLPHY_ERRNO_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(push)
#pragma pack(4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/


enum TLPHY_REBOOT_MOD_ID_ENUM
{
    TLPHY_REBOOT_MOD_ID_START        = 0XC0000000,
	TLPHY_REBOOT_MOD_ID_DL_MBX_FULL  = 0XC0001050,
    TLPHY_REBOOT_MOD_ID_BUTT         = 0XCFFFFFFF
};
typedef unsigned int TLPHY_REBOOT_MOD_ID_ENUM_UINT32;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of tlphy_errno.h */
