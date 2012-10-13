/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __ASM__ARCH_OEM_RAPI_CLIENT_H
#define __ASM__ARCH_OEM_RAPI_CLIENT_H

/*
 * OEM RAPI CLIENT Driver header file
 */

#include <linux/types.h>
#include <mach/msm_rpcrouter.h>

enum {
	OEM_RAPI_CLIENT_EVENT_NONE = 0,

	/*
	 * list of oem rapi client events
	 */
  OEM_RAPI_CLIENT_EVENT_SKY_RAPI = 1,
  /* send event only */
  OEM_RAPI_CLIENT_EVENT_SKY_ERASE_STORED_CELL_LIST = 2,

  //#ifdef FEATURE_SKY_CP_OTA_KT_SUBSCRIPTION
  OEM_RAPI_CLIENT_EVENT_SKY_KT_OTA_SEND_SMS_SUBMIT_MSG = 3,

  //#ifdef FEATURE_SKY_SNDTEST
  OEM_RAPI_CLIENT_EVENT_SKY_AUDIO_LOOPBACK_ENABLE = 4,
  OEM_RAPI_CLIENT_EVENT_SKY_AUDIO_LOOPBACK_SETDEVICE = 5,

  //#ifdef FEATURE_SKY_ADAPTIVE_VOLUME_CONTROL
  OEM_RAPI_CLIENT_EVENT_SKY_AUDIO_SETDEVICE = 6,
  OEM_RAPI_CLIENT_EVENT_SKY_AUDIO_SETVOLUME = 7,

  //#ifdef FEATURE_SKY_RF_TESTMENU, 20090821 choiseulkee add for rf testmenu
  OEM_RAPI_CLIENT_EVENT_SKY_RF_FTM_GET_MODE = 8,
  OEM_RAPI_CLIENT_EVENT_SKY_RF_FTM_GET_RF_DB_STATE = 9,
  OEM_RAPI_CLIENT_EVENT_SKY_RF_FTM_CMD = 10,
  OEM_RAPI_CLIENT_EVENT_SKY_RF_FTM_GET_PADAC = 11,
  OEM_RAPI_CLIENT_EVENT_SKY_RF_FTM_GET_PDM = 12,
  OEM_RAPI_CLIENT_EVENT_SKY_RF_CHECK_CAL_DONE = 13,

  //#ifdef FEATURE_SKY_CHARGER_DETECT, 20091215 choiseulkee add, not used now
  OEM_RAPI_CLIENT_EVENT_SKY_CHARGER_GET_DETECT = 14,
  //#ifdef FEATURE_SKY_BATT_DISCHARGING_TESTMENU, 20091215 choiseulkee add for battery testmen:u
  OEM_RAPI_CLIENT_EVENT_SKY_BATT_DISCHARGING_TEST = 15,

  // shpark 091229 add : ambient light sensor
  OEM_RAPI_CLIENT_EVENT_SKY_LIGHT_GET_ADC = 16,

  //#ifdef FEATURE_SKY_RPC_MSECTOR
  OEM_RAPI_CLIENT_EVENT_SKY_MSECTOR_GET = 20,
  OEM_RAPI_CLIENT_EVENT_SKY_MSECTOR_SET = 21,

  //#ifdef FEATURE_SKY_RPC_CHECKSUM
  OEM_RAPI_CLIENT_EVENT_SKY_CHECKSUM_GET = 22,

  //#ifdef FEATURE_SKY_PWR_ONOFF_REASON_CNT
  OEM_RAPI_CLIENT_EVENT_SKY_PWR_ONOFF_REASON_CNT_GET = 23,
  OEM_RAPI_CLIENT_EVENT_SKY_PWR_ONOFF_REASON_CNT_SET = 24,
  OEM_RAPI_CLIENT_EVENT_SKY_PWR_ONOFF_REASON_CNT_INIT= 25,

  //#ifdef FEATURE_EFS
  OEM_RAPI_CLIENT_EVENT_SKY_ERASE_EFS = 26,

  OEM_RAPI_CLIENT_EVENT_SKY_SET_PROCESSCMD_FUNCRESULT = 27,
  OEM_RAPI_CLIENT_EVENT_SKY_TOUCH_RECOVERY = 28,
  OEM_RAPI_CLIENT_EVENT_SKY_TOUCH_READ_CAL = 29,
  OEM_RAPI_CLIENT_EVENT_SKY_TOUCH_WRITE_CAL = 30,

  // #ifdef FEATURE_SKY_BT_DEVICE_ADDR_BACKUP
  OEM_RAPI_CLIENT_EVENT_SKY_BT_ADDRESS_SET = 31,


//#ifdef FEATURE_SKY_PDL_IDL_DLOAD  
  OEM_RAPI_CLIENT_EVENT_SKY_PDL_IDLE_DOWNLOAD = 32,
//#endif

//#ifdef FEATURE_SKY_SDCARD_UPGRADE  
  OEM_RAPI_CLIENT_EVENT_SKY_SD_IMAGE_UPDATE = 33,
//#endif

//#ifdef FEATURE_SKY_WIFI_ADDR_BACKUP  
  OEM_RAPI_CLIENT_EVENT_SKY_WIFI_ADDRESS_SET = 35,
//#endif

//#ifdef FEATURE_SKY_UIM_TESTMENU
  OEM_RAPI_CLIENT_EVENT_SKY_UIM_DETECT_CNT_TEST = 36,
//#endif

//#ifdef FEATURE_SKY_GPS_FTM_MODE_SET
  OEM_RAPI_CLIENT_EVENT_SKY_FACTORY_GPS_GET_CNo = 37,
  OEM_RAPI_CLIENT_EVENT_SKY_SET_MANUAL_TEST = 38,
//#endif

#ifdef FEATURE_SKY_SAVE_LOG_SDCARD
  OEM_RAPI_CLIENT_EVENT_SKY_GET_SKYERRORLOG = 39,
  OEM_RAPI_CLIENT_EVENT_SKY_GET_SKYERRORLOG_SIZE = 40,
#endif

	OEM_RAPI_CLIENT_EVENT_MAX

};

struct oem_rapi_client_streaming_func_cb_arg {
	uint32_t  event;
	void      *handle;
	uint32_t  in_len;
	char      *input;
	uint32_t out_len_valid;
	uint32_t output_valid;
	uint32_t output_size;
};

struct oem_rapi_client_streaming_func_cb_ret {
	uint32_t *out_len;
	char *output;
};

struct oem_rapi_client_streaming_func_arg {
	uint32_t event;
	int (*cb_func)(struct oem_rapi_client_streaming_func_cb_arg *,
		       struct oem_rapi_client_streaming_func_cb_ret *);
	void *handle;
	uint32_t in_len;
	char *input;
	uint32_t out_len_valid;
	uint32_t output_valid;
	uint32_t output_size;
};

struct oem_rapi_client_streaming_func_ret {
	uint32_t *out_len;
	char *output;
};

int oem_rapi_client_streaming_function(
	struct msm_rpc_client *client,
	struct oem_rapi_client_streaming_func_arg *arg,
	struct oem_rapi_client_streaming_func_ret *ret);

int oem_rapi_client_close(void);

struct msm_rpc_client *oem_rapi_client_init(void);

#endif
