# MD5: 7acf35ebc39b7cf1bf532bde0a576ad1
set(CFG_BALONG_OBUILD_VERSION 10) 

set(CFG_BUILD_PTABLE YES) 

set(CFG_ARCH_TYPE	 3339) 

set(CFG_PRODUCT_NAME "hi6950_udp") 

set(CFG_PRODUCT_CFG_CHIP_SOLUTION_NAME "Balong") 

set(CFG_BUILD_TYPE RELEASE) 

set(CFG_PRODUCT_VERSION V700R500C31B090) 

set(CFG_PRODUCT_VERSION_STR "V700R500C31B090") 

set(CFG_PRODUCT_FULL_VERSION_STR "Balong V700R500C31B090") 

set(CFG_PRODUCT_FILE_VERSION_STR "2.1.6.8") 

set(CFG_PRODUCT_DLOAD_SOFTWARE_VER		 "21.120.00.00.090") 

set(CFG_FEATURE_OBJ_CMP 				 NO) 

set(CFG_FEATURE_OBJ_DUMP				 NO) 

set(CFG_USE_USBLOADER_MERGE 			 YES) 

set(CFG_BSP_ENBALE_PACK_IMAGE			 YES) 

set(CFG_BSP_USB_BURN					 YES) 

set(CFG_PLATFORM hi6950) 

set(CFG_CONFIG_DYNAMIC_LOAD YES) 

set(CFG_OS_PATH modem/config/product/hi6950_udp/os) 

set(CFG_OS_IMG_FILE VXWORKS.6.8.3.IMG.wpj) 

set(CFG_OS_LIB VXWORKS.6.8.3.LIB.RELEASE) 

set(CFG_FTP_REPLY_TIME_OUT 0) 

set(CFG_OS_LINUX_PRODUCT_NAME hi6950_udp_defconfig) 

set(CFG_TARGET_BALONG_PRODUCT hi6950_udp) 

set(CFG_PLATFORM_CHIP_TYPE hi6950) 

set(CFG_CHIP_MODEM_ARM_ARCH CORTEX_A9) 

set(CFG_PRODUCT_CONTROL_VERSION "121") 

set(CFG_PRODUCT_HISILICON_VERSION hi6950) 

set(CFG_PRODUCT_NV_SPLITFILE binfilesplit "0") 

set(CFG_PRODUCT_NV_FILE_MAX_SIZE binfileLimitSize "100") 

set(CFG_PRODUCT_NV_BIN_MODEM_NUNBER bin_modem_number "1") 

set(CFG_PRODUCT_NV_XNV_MODEM_NUNBER xnv_modem_number "1") 

set(CFG_PRODUCT_NV_XNV_PDT_VERSION hi6950_udp) 

set(CFG_XTENSA_CORE				 v7r5_bbe16_cache) 

set(CFG_XTENSA_SYSTEM			 $(ROOT_XTENSA_PATH_W)/XtDevTools/install/builds/RE-2014.5-linux/$(CFG_XTENSA_CORE)/config) 

set(CFG_TENSILICA_BUILDS	 $(ROOT_XTENSA_PATH_W)/XtDevTools/install/builds/RE-2014.5-linux) 

set(CFG_TENSILICA_TOOLS	 $(ROOT_XTENSA_PATH_W)/XtDevTools/install/tools/RE-2014.5-linux) 

set(CFG_TENSILICA_BUILDS_HIFI $(ROOT_XTENSA_PATH_W)/XtDevTools/install/builds/RE-2014.5-linux) 

set(CFG_TENSILICA_TOOLS_HIFI $(ROOT_XTENSA_PATH_W)/XtDevTools/install/tools/RE-2014.5-linux) 

set(LPHY_LD_MAP_PUB_PATH modem/phy/lt/Tensilica_LSP/BBE16_V7R5_LSPA/mainlsp) 

set(LPHY_LD_MAP_LTE_PATH modem/phy/lt/Tensilica_LSP/BBE16_V7R5_LSPA/lte_lsp) 

set(LPHY_LD_MAP_TDS_PATH modem/phy/lt/Tensilica_LSP/BBE16_V7R5_LSPA/tds_lsp) 

set(LPHY_LD_SDR_LIB_FILE	 BBE16_V7R5_O2) 

set(LPHY_XTENSA_RULES cc_tensilica5.0.5_rules.mk) 

set(CFG_LPHY_PUB_DTCM_BASE	 0x72700000) 

set(CFG_LPHY_PUB_ITCM_BASE	 0x72800000) 

set(CFG_LPHY_PRV_DTCM_BASE	 0x72758000) 

set(CFG_LPHY_PRV_ITCM_BASE	 0x72858000) 

set(CFG_LPHY_PUB_DTCM_SIZE		 0x58000) 

set(CFG_LPHY_PUB_ITCM_SIZE		 0x58000) 

set(CFG_LPHY_PRV_DTCM_SIZE 0x78000) 

set(CFG_LPHY_PRV_ITCM_SIZE 0x78000) 

set(CFG_LPHY_LTE_DTCM_SIZE		 0x78000) 

set(CFG_LPHY_LTE_ITCM_SIZE		 0x78000) 

set(CFG_LPHY_TDS_DTCM_SIZE		 0x30000) 

set(CFG_LPHY_TDS_ITCM_SIZE		 0x38000) 

set(CFG_LPHY_TOTAL_IMG_SIZE ((LPHY_PUB_DTCM_SIZE)+(LPHY_PUB_ITCM_SIZE)+(LPHY_LTE_DTCM_SIZE)*2+(LPHY_LTE_ITCM_SIZE)*2+(LPHY_TDS_DTCM_SIZE)+(LPHY_TDS_ITCM_SIZE))) 

set(CFG_TL_PHY_LALG_PATH    	 modem/include/phy) 

set(CFG_TL_PHY_ASIC_HI6950    	 YES) 

set(CFG_TL_PHY_BBE16_CACHE NO) 

set(CFG_TL_PHY_HI6950         	 YES) 

set(CFG_TL_PHY_FEATURE_LTE_LCS  	 YES) 

set(CFG_TL_PHY_RTT_COMM_DEBUG YES) 

set(CFG_FEATURE_TLPHY_SINGLE_XO		 YES) 

set(CFG_GU_INC_PATH COMM_CODE_GU/Balong_GU_Inc/GUL_CS) 

set(CFG_GU_PRODUCT_VERSION VERSION_V7R1) 

set(CFG_RAT_GU 0) 

set(CFG_RAT_GUL 1) 

set(CFG_RAT_MODE RAT_GUL) 

set(CFG_GU_RAT_MODE RAT_GUL) 

set(CFG_PLATFORM_HISI_BALONG hi6950) 

set(CFG_TTF_SKB_EXP NO) 

set(CFG_HIFI_INCLUDE YES) 

set(CFG_HIFI_LINK_DIR_NAME hifi_ld_V7R2_SFT) 

set(CFG_XTENSA_CORE_HIFI v7r2_hifi) 

set(CFG_CONFIG_SMART_SYSTEM_MODEM NO) 

set(CFG_CONFIG_OF YES) 

set(CFG_BSP_CONFIG_SFT NO) 

set(CFG_BSP_CONFIG_EDA NO) 

set(CFG_BSP_CONFIG_EMU NO) 

set(CFG_BSP_CONFIG_EMU_PALADIN NO) 

set(CFG_BSP_CONFIG_EMU_ZEBU NO) 

set(CFG_BSP_CONFIG_NOT_DDR_BYPASSPLL NO) 

set(CFG_BSP_CONFIG_HI6950 YES) 

set(CFG_BSP_HAS_SEC_FEATURE NO) 

set(CFG_BSP_CONFIG_LPDDR2 YES) 

set(CFG_BSP_CONFIG_LPDDR3 NO) 

set(CFG_BSP_CONFIG_HIMDDRCV510_533 NO) 

set(CFG_BSP_CONFIG_HIMDDRCV510_333 NO) 

set(CFG_BSP_CONFIG_HIMDDRCV510_133 NO) 

set(CFG_BSP_CONFIG_HIMDDRCV510_ALL YES) 

set(CFG_CONFIG_FASTBOOT_DEBUG NO) 

set(CFG_CONFIG_FASTBOOT_UART_NUM 1) 

set(CFG_CONFIG_GPIO_PL061 YES) 

set(CFG_CONFIG_GPIO_SYNOPSYS NO) 

set(CFG_CONFIG_PINCTRL_BALONG YES) 

set(CFG_CONFIG_COMPRESS_CCORE_IMAGE YES) 

set(CFG_CONFIG_MODULE_VIC NO) 

set(CFG_CONFIG_PCIE_CFG YES) 

set(CFG_CONFIG_CIPHER YES) 

set(CFG_CONFIG_AT_UART YES) 

set(CFG_CONFIG_CSHELL YES) 

set(CFG_CONFIG_IPF YES) 

set(CFG_CONFIG_PSAM YES) 

set(CFG_CONFIG_MODULE_BUSSTRESS NO) 

set(CFG_CONFIG_ICC YES) 

set(CFG_CONFIG_PM_OM YES) 

set(CFG_CONFIG_RTC_BUILD_CTRL YES) 

set(CFG_CONFIG_RTC_ON_PMU YES) 

set(CFG_CONFIG_MEM YES) 

set(CFG_CONFIG_CPUFREQ YES) 

set(CFG_CONFIG_CCORE_REGULATOR YES) 

set(CFG_CONFIG_CCORE_WDT YES) 

set(CFG_CONFIG_ACORE_WDT YES) 

set(CFG_CONFIG_AVOID_RFIC6362_LDO27_LEAK NO) 

set(CFG_CONFIG_PMU_NEW YES) 

set(CFG_CONFIG_COUL NO) 

set(CFG_CONFIG_USE_TIMER_STAMP NO) 

set(CFG_CONFIG_SFLASH NO) 

set(CFG_CONFIG_ABB YES) 

set(CFG_CONFIG_DSP YES) 

set(CFG_CONFIG_HIFI YES) 

set(CFG_CONFIG_HKADC YES) 

set(CFG_CONFIG_ONOFF YES) 

set(CFG_CONFIG_TSENSOR YES) 

set(CFG_CONFIG_CROSS_MIPI NO) 

set(CFG_CONFIG_BBP YES) 

set(CFG_CONFIG_NVIM YES) 

set(CFG_CONFIG_SC NO) 

set(CFG_FEATURE_NV_FLASH_ON YES) 

set(CFG_FEATURE_NV_EMMC_ON NO) 

set(CFG_FEATURE_NV_LFILE_ON NO) 

set(CFG_FEATURE_NV_RFILE_ON NO) 

set(CFG_FEATURE_UPGRADE_TL YES) 

set(CFG_FEATURE_TLPHY_MAILBOX YES) 

set(CFG_CONFIG_HIFI_MAILBOX NO) 

set(CFG_CONFIG_MAILBOX_TYPE YES) 

set(CFG_CONFIG_RFFE_MIPI YES) 

set(CFG_CONFIG_RFFE_ANTEN YES) 

set(CFG_CONFIG_RFFE_POWER YES) 

set(CFG_CONFIG_PASTAR NO) 

set(CFG_CONFIG_MIPI YES) 

set(CFG_CONFIG_SEC_BOOT_BALONG YES) 

set(CFG_CONFIG_RSA YES) 

set(CFG_ENABLE_SHELL_SYM YES) 

set(CFG_ENABLE_DEBUG YES) 

set(CFG_BUILD_DRV_ONLY NO) 

set(CFG_ENABLE_TEST_CODE NO) 

set(CFG_ENABLE_BUILD_VARS YES) 

set(CFG_ENABLE_BUILD_OM YES) 

set(CFG_FEATURE_OM_PHONE NO) 

set(CFG_ENABLE_BUILD_SYSVIEW YES) 

set(CFG_ENABLE_BUILD_CPUVIEW YES) 

set(CFG_ENABLE_BUILD_MEMVIEW YES) 

set(CFG_ENABLE_BUILD_AMON YES) 

set(CFG_ENABLE_BUILD_UTRACE NO) 

set(CFG_CONFIG_CORESIGHT YES) 

set(CFG_ENABLE_BUILD_SOCP YES) 

set(CFG_CONFIG_WATCHPOINT YES) 

set(CFG_CONFIG_PDLOCK YES) 

set(CFG_CONFIG_DSPDVS YES) 

set(CFG_CONFIG_PASwitch_V7R5 YES) 

set(CFG_CONFIG_CCORE_CPU_IDLE YES) 

set(CFG_CONFIG_BSP_TEST_CCORE NO) 

set(CFG_CONFIG_BALONG_CCLK YES) 

set(CFG_FEATURE_E5_ONOFF NO) 

set(CFG_CONFIG_CCORE_PM YES) 

set(CFG_CONFIG_MODULE_IPC YES) 

set(CFG_CONFIG_MODULE_TIMER YES) 

set(CFG_CONFIG_MODULE_SYNC YES) 

set(CFG_CONFIG_LEDS_CCORE YES) 

set(CFG_CONFIG_HAS_CCORE_WAKELOCK YES) 

set(CFG_CONFIG_CCORE_BALONG_PM YES) 

set(CFG_CONFIG_M3PM YES) 

set(CFG_CONFIG_M3PM_TEST NO) 

set(CFG_CONFIG_BALONG_EDMA YES) 

set(CFG_CONFIG_BALONG_EDMA_TEST YES) 

set(CFG_CONFIG_BALONG_L2CACHE YES) 

set(CFG_CONFIG_AMON_FOR_BUS_LOCK YES) 

set(CFG_CONFIG_IPF_VESION 2) 

set(CFG_CONFIG_IPF_ADQ_LEN 3) 

set(CFG_CONFIG_IPF_PROPERTY_MBB YES) 

set(CFG_CONFIG_FB_SPI_BALONG NO) 

set(CFG_CONFIG_FB_EMI_BALONG NO) 

set(CFG_CONFIG_FB_1_4_5_INCH_BALONG NO) 

set(CFG_CONFIG_FB_2_4_INCH_BALONG NO) 

set(CFG_CONFIG_SLIC m) 

set(CFG_CONFIG_AUDIO n) 

set(CFG_CONFIG_TEMPERATURE_PROTECT YES) 

set(CFG_CONFIG_ANTEN NO) 

set(CFG_CONFIG_EFUSE YES) 

set(CFG_CONFIG_BURN_EFUSE_NANDC NO) 

set(CFG_CONFIG_DDM YES) 

set(CFG_CONFIG_DESIGNWARE_I2C NO) 

set(CFG_CONFIG_USB_DWC3_VBUS_DISCONNECT YES) 

set(CFG_USB3_SYNOPSYS_PHY YES) 

set(CFG_CONFIG_USB_FORCE_HIGHSPEED NO) 

set(CFG_CONFIG_TCXO_BALONG NO) 

set(CFG_FEATURE_SIM_NOTIFY NO) 

set(CFG_CONFIG_VFP YES) 

set(CFG_CONFIG_BALONG_DPM NO) 

set(CFG_CONFIG_BALONG_HPM_TEMP NO) 

set(CFG_CONFIG_HWADP YES) 

set(CFG_PRODUCT_CFG_PTABLE_VER_NAME "V7R500_UDP") 

set(CFG_PRODUCT_CFG_PTABLE_VER_ID			 "ptable 1.00") 

set(CFG_PRODUCT_CFG_FLASH_M3BOOT_LEN     	 0x40000) 

set(CFG_PRODUCT_CFG_FLASH_FASTBOOT_LEN 0xC0000) 

set(CFG_PRODUCT_CFG_FLASH_NV_LTE_LEN 0x400000) 

set(CFG_PRODUCT_CFG_FLASH_NV_IMG_LEN 0x400000) 

set(CFG_PRODUCT_CFG_FLASH_NV_DLD_LEN      	 0x300000) 

set(CFG_PRODUCT_CFG_FLASH_SEC_STORAGE_LEN 0) 

set(CFG_PRODUCT_CFG_FLASH_NV_FACTORY_LEN   	 0x300000) 

set(CFG_PRODUCT_CFG_FLASH_OEMINFO_LEN 0x200000) 

set(CFG_PRODUCT_CFG_FLASH_YAFFS_USERDATA_LEN 0x600000) 

set(CFG_PRODUCT_CFG_FLASH_YAFFS_ONLINE_LEN 0) 

set(CFG_PRODUCT_CFG_FLASH_BOOTIMG_LEN	 0x600000) 

set(CFG_PRODUCT_CFG_FLASH_BOOTIMGBAK_LEN	 0) 

set(CFG_PRODUCT_CFG_FLASH_LOGO_LEN 0x100000) 

set(CFG_PRODUCT_CFG_FLASH_HIFI_LEN  	 0x300000) 

set(CFG_PRODUCT_CFG_FLASH_MISC_LEN 0x100000) 

set(CFG_PRODUCT_CFG_FLASH_M3IMAGE_LEN 	 0x100000) 

set(CFG_PRODUCT_CFG_FLASH_RESERVE1_LEN 0) 

set(CFG_PRODUCT_CFG_FLASH_RESERVE2_LEN 0) 

set(CFG_PRODUCT_CFG_FLASH_RESERVE3_LEN 0) 

set(CFG_PRODUCT_CFG_FLASH_OM_LEN 0xC00000) 

set(CFG_PRODUCT_CFG_FLASH_YAFFS_APP_LEN 0x0) 

set(CFG_PRODUCT_CFG_FLASH_YAFFS_WEBUI_LEN 	 0x0) 

set(CFG_PRODUCT_CFG_FLASH_SYSIMG_LEN 0x2800000) 

set(CFG_PRODUCT_CFG_FLASH_DTIMG_LEN 0x00800000) 

set(CFG_PRODUCT_CFG_FLASH_ISO_LEN 0x1000000) 

set(CFG_PRODUCT_CFG_FLASH_CACHE_LEN   	 0x400000) 

set(CFG_PRODUCT_CFG_FLASH_RECOVERY_LEN 0x400000) 

set(CFG_PRODUCT_CFG_FLASH_MULTI_CARRIER_LEN 0x00000) 

set(CFG_FLASH_PTABLE_OFFSET						 0x1F800) 

set(CFG_PRODUCT_CFG_FLASH_MODEM_FW_LEN 0x01400000) 

set(CFG_MCORE_TEXT_START_ADDR 0x40400000) 

set(CFG_FASTBOOT_DDR_ENTRY 0x4FD00000) 

set(CFG_PRODUCT_CFG_KERNEL_ENTRY 		 0x46E08000) 

set(CFG_PRODUCT_KERNEL_PARAMS_PHYS		 0x46E00100) 

set(CFG_ONCHIP_FASTBOOT_ADDR 0x4F900000) 

set(CFG_RTX_KERNEL_ENTRY 0x10000000) 

set(CFG_DFS_DDR_TRANING_DATA_OFFSET 0x17000) 

set(CFG_HI_SRAM_MEM_ADDR 0x80800000) 

set(CFG_HI_SRAM_SIZE                		 0x10000) 

set(CFG_DRV_SRAM_ADDR               		 (HI_SRAM_MEM_ADDR)) 

set(CFG_DRV_SRAM_SIZE						 0x10000) 

set(CFG_DDR_MEM_ADDR       		 0x40000000) 

set(CFG_DDR_MEM_SIZE       		 0x10000000) 

set(CFG_DDR_APP_ACP_ADDR (DDR_MEM_ADDR)) 

set(CFG_DDR_APP_ACP_SIZE 0x000000) 

set(CFG_DDR_GU_ADDR ((DDR_APP_ACP_ADDR) + (DDR_APP_ACP_SIZE))) 

set(CFG_DDR_GU_SIZE 0x00C0000) 

set(CFG_DDR_UPA_ADDR DDR_GU_ADDR) 

set(CFG_DDR_UPA_SIZE 0x00024000) 

set(CFG_DDR_CQI_ADDR ((DDR_UPA_ADDR) + (DDR_UPA_SIZE))) 

set(CFG_DDR_CQI_SIZE 0x00003400) 

set(CFG_DDR_APT_ADDR ((DDR_CQI_ADDR) + (DDR_CQI_SIZE))) 

set(CFG_DDR_APT_SIZE 0x00008400) 

set(CFG_DDR_ET_ADDR ((DDR_APT_ADDR) + (DDR_APT_SIZE))) 

set(CFG_DDR_ET_SIZE 0x00004800) 

set(CFG_DDR_NV_ADDR ((DDR_ET_ADDR) + (DDR_ET_SIZE))) 

set(CFG_DDR_NV_SIZE 0x00000000) 

set(CFG_DDR_ZSP_UP_ADDR ((DDR_NV_ADDR) + (DDR_NV_SIZE))) 

set(CFG_DDR_ZSP_UP_SIZE 0x00008000) 

set(CFG_DDR_ECS_TEE_ADDR ((DDR_ZSP_UP_ADDR) + (DDR_ZSP_UP_SIZE))) 

set(CFG_DDR_ECS_TEE_SIZE 0x00001800) 

set(CFG_DDR_RESERVE_ADDR ((DDR_ECS_TEE_ADDR) + (DDR_ECS_TEE_SIZE))) 

set(CFG_DDR_RESERVE_SIZE 0x00082800) 

set(CFG_DDR_TLPHY_IMAGE_ADDR     	 ((DDR_RESERVE_ADDR) + (DDR_RESERVE_SIZE))) 

set(CFG_DDR_TLPHY_IMAGE_SIZE     	 0x340000) 

set(CFG_DDR_MCORE_ADDR (( DDR_TLPHY_IMAGE_ADDR ) + (DDR_TLPHY_IMAGE_SIZE))) 

set(CFG_DDR_MCORE_SIZE 0x5D00000) 

set(CFG_DDR_MCORE_DTS_ADDR ((DDR_MCORE_ADDR) + (DDR_MCORE_SIZE))) 

set(CFG_DDR_MCORE_DTS_SIZE 0x0100000) 

set(CFG_DDR_SHARED_MEM_ADDR ((DDR_MCORE_DTS_ADDR) + (DDR_MCORE_DTS_SIZE))) 

set(CFG_DDR_SHARED_MEM_SIZE 0x400000) 

set(CFG_DDR_MNTN_ADDR ((DDR_SHARED_MEM_ADDR) + (DDR_SHARED_MEM_SIZE))) 

set(CFG_DDR_MNTN_SIZE 0x100000) 

set(CFG_DDR_LPHY_SDR_ADDR ((DDR_MNTN_ADDR) + (DDR_MNTN_SIZE))) 

set(CFG_DDR_LPHY_SDR_SIZE 0x280000) 

set(CFG_DDR_LCS_ADDR ((DDR_LPHY_SDR_ADDR) + (DDR_LPHY_SDR_SIZE))) 

set(CFG_DDR_LCS_SIZE 0x280000) 

set(CFG_DDR_HIFI_ADDR ((DDR_LCS_ADDR) + (DDR_LCS_SIZE))) 

set(CFG_DDR_HIFI_SIZE 0x200000) 

set(CFG_DDR_ACORE_ADDR ((DDR_HIFI_ADDR) + (DDR_HIFI_SIZE))) 

set(CFG_DDR_ACORE_SIZE 0x2E00000) 

set(CFG_DDR_ACORE_DTS_ADDR ((DDR_ACORE_ADDR) + (DDR_ACORE_SIZE))) 

set(CFG_DDR_ACORE_DTS_SIZE 0x0100000) 

set(CFG_DDR_SOCP_ADDR ((DDR_ACORE_DTS_ADDR) + (DDR_ACORE_DTS_SIZE))) 

set(CFG_DDR_SOCP_SIZE 0x4000000) 

set(CFG_DDR_MDM_ACP_ADDR ((DDR_SOCP_ADDR) + (DDR_SOCP_SIZE))) 

set(CFG_DDR_MDM_ACP_SIZE 0x0) 

set(CFG_DDR_HIFI_MBX_ADDR (DDR_SHARED_MEM_ADDR)) 

set(CFG_DDR_HIFI_MBX_SIZE (0X9800)) 

set(CFG_NV_DDR_SIZE 0x200000) 

set(CFG_SHM_SIZE_HIFI_MBX (DDR_HIFI_MBX_SIZE)) 

set(CFG_SHM_SIZE_HIFI (10*1024)) 

set(CFG_SHM_SIZE_TLPHY (12*1024)) 

set(CFG_SHM_SIZE_TEMPERATURE (3*1024)) 

set(CFG_SHM_SIZE_DDM_LOAD (1*1024)) 

set(CFG_SHM_SIZE_MEM_APPA9_PM_BOOT (0x10000)) 

set(CFG_SHM_SIZE_MEM_MDMA9_PM_BOOT (0x2000)) 

set(CFG_SHM_SIZE_TENCILICA_MULT_BAND (0x8000)) 

set(CFG_SHM_SIZE_ICC (0x61800)) 

set(CFG_SHM_SIZE_IPF (0x10000)) 

set(CFG_SHM_SIZE_PSAM (0x1000)) 

set(CFG_SHM_SIZE_WAN (0x8000)) 

set(CFG_SHM_SIZE_NV (NV_DDR_SIZE)) 

set(CFG_SHM_SIZE_M3_MNTN (0x20000)) 

set(CFG_SHM_SIZE_TIMESTAMP (1*1024)) 

set(CFG_SHM_SIZE_IOS (6*1024)) 

set(CFG_SHM_SIZE_RESTORE_AXI (96*1024)) 

set(CFG_SHM_SIZE_PMU (3*1024)) 

set(CFG_SHM_SIZE_PTABLE (2*1024)) 

set(CFG_SHM_SIZE_CCORE_RESET (0x400)) 

set(CFG_SHM_SIZE_PM_OM (256*1024)) 

set(CFG_SHM_SIZE_M3PM (0x1000)) 

set(CFG_SHM_SIZE_SLICE_MEM (0x1000)) 

set(CFG_SHM_SIZE_OSA_LOG (1024)) 

set(CFG_SHM_SIZE_WAS_LOG (1024)) 

set(CFG_SHM_SIZE_SRAM_BAK (HI_SRAM_SIZE)) 

set(CFG_SHM_SIZE_SRAM_TO_DDR (0)) 

set(CFG_SHM_SIZE_M3RSRACC_BD (0)) 

set(CFG_FEATURE_DELAY_MODEM_INIT FEATURE_OFF) 

set(CFG_FEATURE_ON 1) 

set(CFG_FEATURE_OFF 0) 

set(CFG_MEMORY_SIZE_32M 1) 

set(CFG_MEMORY_SIZE_64M 2) 

set(CFG_MEMORY_SIZE_128M 3) 

set(CFG_MEMORY_SIZE_256M 4) 

set(CFG_MEMORY_SIZE_512M 5) 

set(CFG_PS_PTL_VER_DANAMIC 20) 

set(CFG_PS_PTL_VER_PRE_R99 (-2)) 

set(CFG_PS_PTL_VER_R99 (-1)) 

set(CFG_PS_PTL_VER_R3 0) 

set(CFG_PS_PTL_VER_R4 1) 

set(CFG_PS_PTL_VER_R5 2) 

set(CFG_PS_PTL_VER_R6 3) 

set(CFG_PS_PTL_VER_R7 4) 

set(CFG_PS_PTL_VER_R8 5) 

set(CFG_PS_PTL_VER_R9 6) 

set(CFG_PS_UE_REL_VER PS_PTL_VER_R9) 

set(CFG_FEATURE_UE_MODE_G FEATURE_ON) 

set(CFG_FEATURE_UE_MODE_W FEATURE_ON) 

set(CFG_FEATURE_CS FEATURE_ON) 

set(CFG_FEATURE_GFAX FEATURE_OFF) 

set(CFG_FEATURE_WFAX FEATURE_OFF) 

set(CFG_FEATURE_GCBS FEATURE_ON) 

set(CFG_FEATURE_WCBS FEATURE_ON) 

set(CFG_FEATURE_AP FEATURE_OFF) 

set(CFG_BALONG_UDP_V1R1 FEATURE_OFF) 

set(CFG_BALONG_UDP_V1R2 FEATURE_OFF) 

set(CFG_FEATURE_PTABLE_UDP FEATURE_ON) 

set(CFG_FEATURE_STICK				 FEATURE_OFF) 

set(CFG_FEATURE_E5 FEATURE_OFF) 

set(CFG_FEATURE_E5_UDP FEATURE_OFF) 

set(CFG_FEATURE_PC_VOICE FEATURE_OFF) 

set(CFG_FEATURE_MBB_MODULE_AUDIO FEATURE_OFF) 

set(CFG_FEATURE_HILINK FEATURE_OFF) 

set(CFG_FEATURE_SEC_BOOT FEATURE_OFF) 

set(CFG_FEATURE_LTE FEATURE_ON) 

set(CFG_FEATURE_CSD FEATURE_OFF) 

set(CFG_FEATURE_IPV6 FEATURE_ON) 

set(CFG_FEATURE_PPP FEATURE_ON) 

set(CFG_FEATURE_HARDWARE_HDLC_FUNC FEATURE_ON) 

set(CFG_FEATURE_HARDWARE_HDLC_ON_CCPU FEATURE_OFF) 

set(CFG_FEATURE_PPPOE FEATURE_OFF) 

set(CFG_FEATURE_VCOM_EXT FEATURE_OFF) 

set(CFG_FEATURE_MERGE_OM_CHAN						 FEATURE_ON) 

set(CFG_FEATURE_UPDATEONLINE FEATURE_OFF) 

set(CFG_FEATURE_WIFI FEATURE_OFF) 

set(CFG_FEATURE_HIFI FEATURE_OFF) 

set(CFG_FEATURE_HIFI_USE_ICC FEATURE_ON) 

set(CFG_FEATURE_SDIO FEATURE_OFF) 

set(CFG_FEATURE_KEYBOARD FEATURE_OFF) 

set(CFG_FEATURE_CHARGE FEATURE_OFF) 

set(CFG_FEATURE_ICC_DEBUG FEATURE_ON) 

set(CFG_FEATURE_POWER_ON_OFF FEATURE_OFF) 

set(CFG_FEATURE_OLED FEATURE_OFF) 

set(CFG_FEATURE_TFT FEATURE_OFF) 

set(CFG_FEATURE_MMI_TEST FEATURE_OFF) 

set(CFG_FEATURE_DL_E_CELL_FACH FEATURE_ON) 

set(CFG_FEATURE_INTERPEAK FEATURE_OFF) 

set(CFG_FEATURE_INTERPEAK_MINI FEATURE_OFF) 

set(CFG_IPWEBS_FEATURE_E5 FEATURE_OFF) 

set(CFG_IPWEBS_FEATURE_WIRELESS_DONGLE FEATURE_OFF) 

set(CFG_IPWEBS_FEATURE_LIGHTNING_CARD FEATURE_OFF) 

set(CFG_FEATURE_WEBNAS FEATURE_OFF) 

set(CFG_FEATURE_WIRELESS_DONGLE FEATURE_OFF) 

set(CFG_FEATURE_PMU_PROTECT FEATURE_OFF) 

set(CFG_FEATURE_VXWORKS_TCPIP FEATURE_OFF) 

set(CFG_FEATURE_M2 FEATURE_OFF) 

set(CFG_FEATURE_MEMORY_SIZE MEMORY_SIZE_64M) 

set(CFG_FEATURE_ECM_RNDIS FEATURE_OFF) 

set(CFG_FEATURE_RNIC FEATURE_OFF) 

set(CFG_FEATURE_NFEXT FEATURE_OFF) 

set(CFG_FEATURE_UL_E_CELL_FACH FEATURE_ON) 

set(CFG_FEATURE_BREATH_LIGHT FEATURE_OFF) 

set(CFG_FEATURE_LEDSTATUS FEATURE_OFF) 

set(CFG_FEATURE_E5_LED FEATURE_OFF) 

set(CFG_FEATURE_RTC FEATURE_OFF) 

set(CFG_FEATURE_SMALL_MAILBOX FEATURE_OFF) 

set(CFG_FEATURE_RECONFIG FEATURE_ON) 

set(CFG_FEATURE_HWENCRY_REWORK FEATURE_OFF) 

set(CFG_FEATURE_MMU_BIG FEATURE_OFF) 

set(CFG_FEATURE_MMU_MEDIUM FEATURE_ON) 

set(CFG_FEATURE_MMU_SMALL FEATURE_OFF) 

set(CFG_FEATURE_TTFMEM_CACHE FEATURE_OFF) 

set(CFG_FEATURE_SDUPDATE FEATURE_OFF) 

set(CFG_FEATURE_SECURITY_SHELL          			 FEATURE_ON) 

set(CFG_FEATURE_EPAD FEATURE_OFF) 

set(CFG_FEATURE_HSIC_SLAVE FEATURE_OFF) 

set(CFG_FEATURE_MANUFACTURE_LOG FEATURE_OFF) 

set(CFG_FEATURE_MEM_MONITOR FEATURE_ON) 

set(CFG_FEATURE_DRV_REPLAY_DUMP						 FEATURE_OFF) 

set(CFG_FEATURE_SKB_EXP FEATURE_OFF) 

set(CFG_FEATURE_MULTI_FS_PARTITION FEATURE_OFF) 

set(CFG_FEATURE_MULTI_MODEM FEATURE_OFF) 

set(CFG_FEATURE_SOCP_CHANNEL_REDUCE FEATURE_OFF) 

set(CFG_FEATURE_SOCP_DECODE_INT_TIMEOUT FEATURE_ON) 

set(CFG_FEATURE_RTOSCK FEATURE_ON) 

set(CFG_FEATURE_VISP_VPP FEATURE_ON) 

set(CFG_FEATURE_UE_MODE_TDS FEATURE_ON) 

set(CFG_FEATURE_ETWS FEATURE_ON) 

set(CFG_FEATURE_AGPS FEATURE_OFF) 

set(CFG_FEATRUE_XML_PARSER FEATURE_ON) 

set(CFG_FEATURE_PTM FEATURE_OFF) 

set(CFG_FEATURE_LPP FEATURE_OFF) 

set(CFG_FEATURE_UE_UICC_MULTI_APP_SUPPORT FEATURE_OFF) 

set(CFG_CONFIG_HISI_PTM FEATURE_OFF) 

set(CFG_FEATURE_COMPRESS_WRITE_LOG_FILE FEATURE_OFF) 

set(CFG_FEATURE_CBT_LOG FEATURE_OFF) 

set(CFG_FEATURE_IMS FEATURE_ON) 

set(CFG_FEATURE_SAMPLE_LTE_CHAN FEATURE_OFF) 

set(CFG_FEATURE_AT_HSUART FEATURE_OFF) 

set(CFG_NAS_FEATURE_SMS_NVIM_SMSEXIST FEATURE_ON) 

set(CFG_NAS_FEATURE_SMS_FLASH_SMSEXIST FEATURE_ON) 

set(CFG_FEATURE_AT_HSIC FEATURE_OFF) 

set(CFG_FEATURE_HUAWEI_VP FEATURE_OFF) 

set(CFG_FEATURE_RMNET_CUSTOM FEATURE_ON) 

set(CFG_FEATURE_ECALL FEATURE_OFF) 

set(CFG_GAS_PTL_VER_PRE_R99 (PS_PTL_VER_PRE_R99)) 

set(CFG_GAS_PTL_VER_R99 (PS_PTL_VER_R99)) 

set(CFG_GAS_PTL_VER_R3 (PS_PTL_VER_R3)) 

set(CFG_GAS_PTL_VER_R4 (PS_PTL_VER_R4)) 

set(CFG_GAS_PTL_VER_R5 (PS_PTL_VER_R5)) 

set(CFG_GAS_PTL_VER_R6 (PS_PTL_VER_R6)) 

set(CFG_GAS_PTL_VER_R7 (PS_PTL_VER_R7)) 

set(CFG_GAS_PTL_VER_R8 (PS_PTL_VER_R8)) 

set(CFG_GAS_PTL_VER_R9 (PS_PTL_VER_R9)) 

set(CFG_GAS_UE_REL_VER (GAS_PTL_VER_R9)) 

set(CFG_FEATURE_BBPMST_FREQUENCY_REDUCE FEATURE_OFF) 

set(CFG_TTF_CF_FEATURE FEATURE_OFF) 

set(CFG_FEATURE_TTF_MEM_DEBUG FEATURE_ON) 

set(CFG_FEATURE_TTF_RLC_PIGGY_BACKED FEATURE_OFF) 

set(CFG_FEATURE_W_R99_DL_DUAL_MAILBOX FEATURE_ON) 

set(CFG_FEATURE_ASSEMBLY_MEM_CPY FEATURE_ON) 

set(CFG_FEATURE_HSPA_PERF_IMP FEATURE_ON) 

set(CFG_FEATURE_RACH_NO_ACK_DEBUG FEATURE_OFF) 

set(CFG_FEATURE_RLC_REASSEMBLE FEATURE_ON) 

set(CFG_FEATURE_TCP_ACK_IN_FRONT FEATURE_ON) 

set(CFG_FEATURE_CST_ASYN_OR_NOT_TRANS FEATURE_OFF) 

set(CFG_FEATURE_CIPHER_MASTER FEATURE_ON) 

set(CFG_FEATURE_HDLC_ENHANCE FEATURE_ON) 

set(CFG_FEATURE_ACPU_STAT FEATURE_OFF) 

set(CFG_FEATURE_ACPU_FC_POINT_REG FEATURE_OFF) 

set(CFG_SC_CTRL_MOD_6620_FPGA 1) 

set(CFG_SC_CTRL_MOD_6620_SFT 2) 

set(CFG_SC_CTRL_MOD_6758_FPGA 3) 

set(CFG_SC_CTRL_MOD_6758_SFT 4) 

set(CFG_SC_CTRL_MOD_6930_FPGA 5) 

set(CFG_SC_CTRL_MOD_6930_SFT 6) 

set(CFG_SC_CTRL_MOD_3630_UDP 7) 

set(CFG_SC_CTRL_MOD_6210_FPGA 8) 

set(CFG_SC_CTRL_MOD_6210_SFT 9) 

set(CFG_SC_CTRL_MOD_P532 10) 

set(CFG_SC_CTRL_MOD_6950_SFT 11) 

set(CFG_SC_CTRL_MOD_3650_SFT 12) 

set(CFG_SC_CTRL_MOD_6250_SFT 13) 

set(CFG_SC_CTRL_MOD_6932_SFT 14) 

set(CFG_SC_CTRL_MOD (SC_CTRL_MOD_6950_SFT)) 

set(CFG_FEATURE_EDMAC FEATURE_OFF) 

set(CFG_ZSP_DSP_CHIP_BB_TYPE		 9) 

set(CFG_ZSP_DSP_PRODUCT_FORM		 4) 

set(CFG_BOARD SFT) 

set(CFG_FEATURE_DC_DPA			 FEATURE_ON) 

set(CFG_FEATURE_DC_UPA FEATURE_OFF) 

set(CFG_FEATURE_DC_MIMO FEATURE_OFF) 

set(CFG_FEATURE_HARQ_OUT FEATURE_OFF) 

set(CFG_FEATURE_UPHY_SIO_WPLL_ELUSION_ON		 FEATURE_OFF) 

set(CFG_FEATURE_RFIC_RESET_GPIO_ON			 FEATURE_OFF) 

set(CFG_FEATURE_EXTERNAL_32K_CLK			 FEATURE_ON) 

set(CFG_FEATURE_MULTI_RFIC_MEAS			 FEATURE_ON) 

set(CFG_FEATURE_SRAM_400K FEATURE_OFF) 

set(CFG_FEATURE_TEMP_DSP_CORE_POWER_DOWN FEATURE_OFF) 

set(CFG_FEATURE_TUNER				 FEATURE_ON) 

set(CFG_FEATURE_TEMP_MULTI_MODE_LP FEATURE_ON) 

set(CFG_FEATURE_ANT_SHARE				 FEATURE_OFF) 

set(CFG_FEATURE_VIRTUAL_BAND			 FEATURE_OFF) 

set(CFG_FEATURE_MODE_FEM_CHAN_EXT FEATURE_ON) 

set(CFG_FEATURE_POWER_DRV FEATURE_OFF) 

set(CFG_FEATURE_SIM_NOTIFY FEATURE_OFF) 

set(CFG_FEATURE_HW_CODEC FEATURE_OFF) 

set(CFG_FEATURE_VSIM FEATURE_ON) 

set(CFG_FEATURE_VOS_REDUCE_MEM_CFG FEATURE_OFF) 

set(CFG_FEATURE_VOICE_UP FEATURE_OFF) 

set(CFG_FEATURE_ANT_SHARE				 FEATURE_OFF) 

set(CFG_FEATURE_DSDS FEATURE_OFF) 

set(CFG_FEATURE_SOCP_ON_DEMAND			 FEATURE_OFF) 

set(CFG_FEATURE_POWER_TIMER FEATURE_OFF) 

set(CFG_FEATURE_CL_INTERWORK FEATURE_OFF) 

set(CFG_FEATURE_BSP_LCH_OM FEATURE_ON) 

set(CFG_FEATURE_HISOCKET				 FEATURE_OFF) 

set(CFG_FEATURE_GU_DSP_SPLIT FEATURE_ON) 

set(CFG_FEATURE_UE_MODE_CDMA			 FEATURE_OFF) 

set(CFG_FEATURE_CHINA_TELECOM_VOICE_ENCRYPT FEATURE_OFF) 

set(CFG_FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE FEATURE_OFF) 

set(CFG_FEATURE_BASTET FEATURE_OFF) 

set(CFG_FEATURE_NV_PARTRION_MULTIPLEX FEATURE_ON) 

set(CFG_FEATURE_BBP_MASTER FEATURE_ON) 

set(CFG_BBP_MASTER_INT_MODE FEATURE_ON) 

set(CFG_FEATURE_BBP_MASTER_CHAN_EXTEND		 FEATURE_OFF) 

set(CFG_FEATURE_BBP_MASTER_CHAN_U2              	 FEATURE_OFF) 

set(CFG_FEATURE_GUBBP_HANDSHAKE FEATURE_ON) 

set(CFG_FEATURE_GUC_BBP_TRIG FEATURE_OFF) 

set(CFG_FEATURE_GUDRX_NEWVERSION FEATURE_OFF) 

set(CFG_FEATURE_OM_BBPLOG FEATURE_OFF) 

set(CFG_FEATURE_RTC_TIMER_DBG FEATURE_ON) 

set(CFG_FEATURE_LTE_R11 FEATURE_ON) 

set(CFG_FEATURE_LTE_MBMS FEATURE_OFF) 

set(CFG_FEATURE_LTE_NAS_R11 FEATURE_OFF) 

set(CFG_FEATURE_LTE_CDMA_R11 FEATURE_OFF) 

set(CFG_FEATURE_LPHY_RFIC_HI6362 FEATURE_ON) 

set(CFG_FEATURE_LTE_4RX FEATURE_ON) 

set(CFG_FEATURE_BALONG_CL FEATURE_UE_MODE_CDMA) 

set(CFG_FEATURE_DCX0_TLHPA_WRITE_PHY_NV FEATURE_ON) 

set(CFG_FEATURE_VERSION_V8 FEATURE_OFF) 

set(CFG_FEATURE_DFS_SYNC FEATURE_OFF) 

set(CFG_FEATURE_MULTI_CHANNEL			 FEATURE_OFF) 

set(CFG_FEATURE_LTE_CAT_PART_R11 YES) 

set(CFG_FEATURE_TDS_WCDMA_DYNAMIC_LOAD FEATURE_OFF) 

set(CFG_FEATURE_CSG FEATURE_OFF) 

set(CFG_FEATURE_CSS_CLOUD_MEMORY_IMPROVE FEATURE_OFF) 

