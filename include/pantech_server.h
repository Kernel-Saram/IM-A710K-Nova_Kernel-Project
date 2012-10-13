#ifndef __PANTECH_SERVER_H__
#define __PANTECH_SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "oem_rapi.h"

#define SKYTEST_R_PIPE    "/dev/pantech/skytestwpipe"
#define SKYTEST_W_PIPE    "/dev/pantech/skytestrpipe"

#define SKYTEST_PWRON_CNT "/data/skytestpwroncnt"

#ifdef FEATURE_SKY_WLAN_FC
#include "wlm.h"
/*WLAN*/
typedef struct WLAN_DATARATE {
	int    RateNum;
	int    rate;       /* in Mbps */
} WLAN_DATARATE;

extern const WLAN_DATARATE wlan_datarate[];
#endif

typedef enum {
	/* request commands */
	REQ_CMD_AUDIOTEST_LB_EN = 0x10001001,
	REQ_CMD_AUDIOTEST_LB_SETDEVICE,
	REQ_CMD_AUDIOTEST_SET_AMPVOL,
	REQ_CMD_AUDIOTEST_SET_DEVICE,

	REQ_CMD_BT_ENABLE = 0x10002001,
	REQ_CMD_BT_DISABLE,
	REQ_CMD_BT_COEX,
	REQ_CMD_BT_IS_ENABLED,
	REQ_CMD_BT_TEST_MODE,
	REQ_CMD_BT_SCAN,
	REQ_CMD_BT_POWER_ON,
	REQ_CMD_BT_POWER_OFF,

	REQ_CMD_RFTEST_FTM_SET_MODE = 0x10003001,
	REQ_CMD_RFTEST_FTM_GET_MODE,
	REQ_CMD_RFTEST_FTM_GET_RF_DB_STATE,
	REQ_CMD_RFTEST_FTM_CMD,
	REQ_CMD_RFTEST_FTM_GET_PADAC,
	REQ_CMD_RFTEST_FTM_GET_PDM,

	REQ_CMD_RFTEST_CHECK_CAL_DONE,

	REQ_CMD_UIMTEST_GET_STATUS,

	REQ_CMD_UIMTEST_DETECT_CNT,
	REQ_CMD_UIMTEST_POWERON_CNT,

	REQ_CMD_BATT_DISCHARGING_TEST,

/// [ ds2_min.woojung_2010.01.17 ]
	REQ_CMD_CHECKSUM_READ = 0x10004001,

/// [ ds2_min.woojung_2010.01.17 ]
//#ifdef FEATURE_SKY_PWR_ONOFF_REASON_CNT
	REQ_CMD_SYS_RESET_GET_HW_RESET_COUNT = 0x10005001,
	REQ_CMD_SYS_RESET_GET_SW_RESET_COUNT,
	REQ_CMD_SYS_RESET_GET_ALL_RESET_COUNT,
	REQ_CMD_SYS_RESET_INIT_RESET_COUNT,
	REQ_CMD_SYS_RESET_REQ_SW_RESET,
#ifdef FEATURE_SKY_SAVE_LOG_SDCARD
	REQ_CMD_SAVE_LOG_TO_SDCARD,
#endif
  
//#endif
#ifdef FEATURE_SKY_PDIP_MODIFY_FOR_CTS
	REQ_CMD_PDIP_REQ_CMD = 0x10005a01,
#endif
	REQ_CMD_WLAN_ENABLE = 0x10006001,
	REQ_CMD_WLAN_DISABLE,
	REQ_CMD_WLAN_TX_ON,
	REQ_CMD_WLAN_TX_OFF,

	REQ_CMD_FUNCTEST_SET_RESULT = 0x10007001,

	REQ_CMD_PDL_IDLE_DOWNLOAD = 0x10008001,
//#ifdef FEATURE_SKY_SDCARD_UPGRADE
	REQ_CMD_SD_IMAGE_UPDATE = 0x10009001,
//#endif
	REQ_CMD_LED_COLOR_TEST = 0x1000a001,

	REQ_CMD_SDCARD_SET_DETECTION_COUNT = 0x1000b001,
	REQ_CMD_SDCARD_GET_DETECTION_COUNT = 0x1000b002,

// FEATURE_SKY_PATTERN_UNLOCK
// park.byungchul 20101005
	REQ_CMD_PATTERN_UNLOCK = 0x1000b003,

	/*response commands */
	RES_CMD_OK = 0x20000001,
	RES_CMD_UNKNOWN,
	CMD_MAX = 0xffffffff
} pkt_cmd_t;

typedef struct {
	int cmd;
	int param;
	int param2;
	int param3; /* only for response packet. do not use */
	char param4[16];
}pkt_type_t;

typedef struct {
	pkt_type_t hdr;
	byte data[OEM_RAPI_MAX_CLIENT_INPUT_BUFFER_SIZE]; //512
}extend_pkt_type_t;

#endif
