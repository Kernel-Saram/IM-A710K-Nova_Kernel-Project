#ifndef CUST_PANTECH_H
#define CUST_PANTECH_H

#include "BOARD_REV.h"

#define FIRM_VER "S0000000"
#define PANTECH_MODEL_NAME "IM-A710K"
#define T_EF18K
#define SYS_MODEL_NAME "EF18K"

#ifdef __MODEMBUILD__
/*
    ONLY MODEM BUILD FEATURE
*/

/*******************************************************************************
  Pantech MODEM System
*******************************************************************************/

/*
      !!!!!!!!!!!!!!!!!! MUST BE DEFINED AS FOLLOWS (ONLY MODEM)!!!!!!!!!!!!!!!!!!
      FEATURE_{COMPANY_NAME}_{FUNCTION_NAME}_{ACTION_NAME}
      ex) PMIC function.

      #define FEATURE_PANTECH_PMIC
*/


// TO DO ..
/*******************************************************************************
  PMIC
*******************************************************************************/
#if defined(T_EF18K) || defined(T_EF30S)
#define FEATURE_PANTECH_PMIC

#if defined(FEATURE_PANTECH_PMIC)
#define FEATURE_PANTECH_PMIC_DEBUG
//#define FEATURE_PANTECH_PMIC_OUTPUT_REGULATOR //p13156@lks - GB Patch(I dont know why need this)
#define FEATURE_PANTECH_PMIC_INPUT_POWER
#define FEATURE_PANTECH_PMIC_GPIO_KEYPAD
#define FEATURE_PANTECH_PMIC_GENERAL_HOUSEKEEPING
#define FEATURE_PANTECH_PMIC_RTC
#define FEATURE_PANTECH_PMIC_AUTO_PUSH_POWER_ON_OFF
#define FEATURE_PANTECH_PMIC_FUEL_GAUGE
#define FEATURE_PANTECH_PMIC_HW_RESET_DETECT
#endif
#endif // #if defined(T_EF18K) || defined(T_EF30S)

#ifdef FEATURE_PANTECH_ERR_CRASH_LOGGING
#define FEATURE_SKY_ASSERT_LOG
#define FEATURE_SKY_ERROR_LOGGING_EXTENDED
#endif /* FEATURE_PANTECH_ERR_CRASH_LOGGING */

/* usb driver for emergency mode */
#if defined(T_EF18K) || defined(T_EF30S)
#define FEATURE_HS_USB_SKY
#endif

#endif /* __MODEMBUILD__ */


#if !defined(__KERNELBUILD__) && !defined(__MODEMBUILD__)
/*
      !!!!!!!!!!!!!!!!!! MUST BE DEFINED AS FOLLOWS (ANDROID)!!!!!!!!!!!!!!!!!!
      CONFIG_{COMPANY_NAME}_{FUNCTION_NAME}_{ACTION_NAME}
      ex) PMIC function.
      #define CONFIG_PANTECH_PMIC

      for using BOTH (android & kernel) definition, please read engineer note about chapter 5 Arm Linux Kernel.

      IF YOU ADD FEATURE IN KERNEL , YOU CHECK THE RELEASE ENGINNER NOTE

      __KERNELBUILD__ :  for avoid redefined , this is predefined name in kernel makefile.

*/

/* TO DO define */
#if defined(T_EF18K) || defined(T_EF30S)/*ps1 team shs : add here*/
#define FEATURE_SKY_MMC_COMMON
#endif

#define FEATURE_SKY_FLASH_ACCESS /*yjw 20110329*/
#define FEATURE_SKY_BT_FC
#define FEATURE_SKY_BT_DEVICE_ADDR_BACKUP
#define CONFIG_PANTECH_BT /* 20110414 KSJ_device BT init on BlueZ ++++*/
#define CONFIG_SKY_BT /* 20110622 KSJ_device BT init on BlueZ ++++*/
/*******************************************************************************
**  RAWDATA PARTITION ACCESS, FOR BACKUP
*******************************************************************************/
/* #define FEATURE_SKY_RAWDATA_ACCESS */ /*yjw 20110329*/

/* ef30s from cust_sky_surfaceflinger.h */
#ifndef FEATURE_AARM_RELEASE_MODE
#define	SKY_FEATURE_ALLOW_FASTBOOT_MODE
#endif

#if 1//defined(FEATURE_AARM_RELEASE_MODE)   //GPL_KERNEL SYLEE
#define FEATURE_SKY_DM_MSG_VIEW_DISABLE
#define FEATURE_SW_RESET_RELEASE_MODE // use in release mode
#endif

#if defined(T_EF36S) || defined(T_EF18K)
#define CONFIG_PANTECH_EF36S_3POLE_HEADSET  //20110415 jhsong
#define CONFIG_PANTECH_EF36S_1MIC  //20110415 jhsong
#define CONFIG_PANTECH_EF36S_DEVIDE_ADD  //20110419 jhsong
#define CONFIG_PANTECH_EF36S_ASR_PATH //20110420 jhsong
#endif
/*******************************************************************************
  Pantech LEDS      PS2 P13106 KANG YOONKOO
*******************************************************************************/
#define FEATURE_SKY_LEDS

#if defined(T_EF36S) || defined(T_EF38L)
#define CONFIG_PANTECH_ERR_CRASH_LOGGING
#endif

/* msy_sky_factory_command */
#define CONFIG_PANTECH_SKY_FACTORY_COMMAND

/*****************************************************************
 * **************** Qualcomm USB Driver fix bug ******************
 * ***************************************************************/
#define CONFIG_USB_ANDROID_PANTECH_QC_BUG_FIX

/* Global features for PANTECH camera framework. */
/* You can find any modification for camera framework by searching "F_SKYCAM" */
/* #include "CUST_PANTECH_CAMERA.h" */

/*******************************************************************************
  Pantech Factory Command     PS2 P11764 Kim Changhun
*******************************************************************************/
#if defined(T_EF18K) || defined(T_EF30S)
#define CONFIG_PANTECH_SKY
#endif



#define FEATURE_PANTECH_MDS_MTC  /* MTC */

#if defined(FEATURE_PANTECH_MDS_MTC)
#define FEATURE_PANTECH_MAT      /* MAT */
#endif

#if defined(FEATURE_PANTECH_MDS_MTC)
#define FEATURE_DIAG_LARGE_PACKET
#endif

/*******************************************************************************
  SENSOR     PS2.P12095
*******************************************************************************/
#if defined(T_EF18K) || defined(T_EF30S)
#define FEATURE_PANTECH_SENSOR_UTIL
#endif

#define FEATURE_PANTECH_VOLUME_CTL

/* drop event at booting time */
#define FEATURE_PANTECH_BOOTING_EVENT_DROP
/*******************************************************************************
  WIFI     PS2 P13783 HS Kim
*******************************************************************************/
#if defined(T_EF18K) || defined(T_EF30S)
#define CONFIG_PANTECH_WIFI_BROADCOM_BCM4329	//20110609 hskim_wifi for wifi_init
#define FEATURE_SKY_WLAN 	//20110620 hskim_wifi for wifi testmode
#define FEATURE_SKY_WLAN_FC 	//20110620 hskim_wifi for wifi factory command
#define FEATURE_SKY_WIFI_ADDR_BACKUP 	//20110620 hskim_wifi for wifi mac address backup 
#endif

#endif /* __KERNELBUILD__ */

/* DEV/SKT USB TEST #define CONFIG_HSUSB_PANTECH_USB_TEST */

#if !defined(__KERNELBUILD__)
/*******************************************************************************
**  RF
*******************************************************************************/
#if defined(T_EF36S)
#define FEATURE_SKY_RF_TESTMENU   // 20110425 thkim add for rf testmode
#endif

#if defined(T_EF36S) || defined(T_EF30S)  || defined(T_EF18K) || defined(T_EF38L)
#define FEATURE_PANTECH_UIM_TESTMENU
#endif

#endif/* __KERNELBUILD__ */

#if defined(T_EF36S)
#define FEATURE_PANTECH_NFC_PN544
#endif

/* PS10 add cust_sky_cp_linux.h */
/*****************************************
**  define FEATURE by PS10
*****************************************/
#ifdef T_SKY_MODEL_TARGET_COMMON
#include "cust_sky_cp_linux.h"
#include "cust_sky_ds_linux.h"

#endif
/* */

/*******************************************************************************
**  Save Log to SDCARD.
*******************************************************************************/
#define FEATURE_SKY_SAVE_LOG_SDCARD		

/*******************************************************************************
**  SW reset changed to HW reset.
*******************************************************************************/		
#define FEATURE_SW_RESET_CONVERT_HW_RESET

/*******************************************************************************
  Pantech Surfaceinger     PS1 P13156 Lee KyungSoo
*******************************************************************************/
#include "CUST_PANTECH_SURFACEFLINGER.h"

/* Global features for PANTECH camera framework.
 * You can find any modification for camera framework by searching "F_PTCAM" */
#include "CUST_PANTECH_CAMERA.h"
/*
  2011/03/02 권오윤
  PANTECH multimedia engine/codec 개발 관련 feature 정의파일 (최상위 FEATURE_PANTECH_MMP 등)
*/
#include "cust_pantech_mmp.h"

/****************************************************
** SOUND
****************************************************/
#define FEATURE_SKYSND

/* coded by PZ1643@PS2(revival) / 2011/07/15 / FOTA */
#if defined(T_EF30S) && 1
#ifndef FEATURE_SKY_FOTA
#define FEATURE_SKY_FOTA 	//20110715
#endif
#endif
/* end of PZ1643@PS2(revival)   / 2011/07/15 / FOTA */
/*******************************************************************************
**  CTS TEST
*******************************************************************************/
#define FEATURE_SKY_PDIP_MODIFY_FOR_CTS	// PZ1569_PDIP_MODIFY_FOR_CTS - 20110413 - BSH

/*******************************************************************************
**  PATTERN RESET
*******************************************************************************/
#define FEATURE_SKY_PATTERN_RESET

/*******************************************************************************
**  SELF UPGRADE
*******************************************************************************/
#define FEATURE_SKY_SDCARD_UPGRADE
#define FEATURE_SKY_SELF_UPGRADE_SDCARD	// BSH

//#define FIRM_VER "S0000000"
//#define PANTECH_MODEL_NAME "IM-AA710K "

#endif/* CUST_PANTECH_H */
