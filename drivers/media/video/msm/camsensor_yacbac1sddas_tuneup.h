/*
================================================================================
TUNEUP Interface for MICRON YACBAC1SDDAS CMOS 3.1MP sensor

DESCRIPTION
    This file contains the definitions needed for the camera tuneup interface.

Copyright (c) 2007 by PANTECH, Incorporated. All Rights Reserved.
================================================================================
*/

#if !defined(__CAMSENSOR_YACBAC1SDDAS_TUNEUP_H__)
#define __CAMSENSOR_YACBAC1SDDAS_TUNEUP_H__
#include <linux/types.h>
#include <media/msm_camera.h>

/*
================================================================================
INCLUDE FILES
================================================================================
*/
//#include "camsensor_yacbac1sddas_config_ef13s.h"

/*
================================================================================
GLOBAL FEATURES
================================================================================
*/

/*
================================================================================
GLOBAL CONSTANTS
================================================================================
*/

//#define YACBAC1SDDAS_TUP_FNAME       "camsensor_yacbac1sddas_tuneup_ef13s.txt"

#define YACBAC1SDDAS_TUP_INIT_MARK            "YACBAC1SDDAS_INIT"

#define YACBAC1SDDAS_TUP_WB_AUTO_MARK         "YACBAC1SDDAS_WB_AUTO"
#define YACBAC1SDDAS_TUP_WB_CLOUDY_MARK       "YACBAC1SDDAS_WB_CLOUDY"
#define YACBAC1SDDAS_TUP_WB_DAYLIGHT_MARK     "YACBAC1SDDAS_WB_DAYLIGHT"
#define YACBAC1SDDAS_TUP_WB_FLUORESCENT_MARK  "YACBAC1SDDAS_WB_FLUORESCENT"
#define YACBAC1SDDAS_TUP_WB_INCANDESCENT_MARK "YACBAC1SDDAS_WB_INCANDESCENT"

#define YACBAC1SDDAS_TUP_BRIGHT_M5_MARK       "YACBAC1SDDAS_BRIGHT_M5"
#define YACBAC1SDDAS_TUP_BRIGHT_M4_MARK       "YACBAC1SDDAS_BRIGHT_M4"
#define YACBAC1SDDAS_TUP_BRIGHT_M3_MARK       "YACBAC1SDDAS_BRIGHT_M3"
#define YACBAC1SDDAS_TUP_BRIGHT_M2_MARK       "YACBAC1SDDAS_BRIGHT_M2"
#define YACBAC1SDDAS_TUP_BRIGHT_M1_MARK       "YACBAC1SDDAS_BRIGHT_M1"
#define YACBAC1SDDAS_TUP_BRIGHT_0_MARK        "YACBAC1SDDAS_BRIGHT_0"
#define YACBAC1SDDAS_TUP_BRIGHT_P1_MARK       "YACBAC1SDDAS_BRIGHT_P1"
#define YACBAC1SDDAS_TUP_BRIGHT_P2_MARK       "YACBAC1SDDAS_BRIGHT_P2"
#define YACBAC1SDDAS_TUP_BRIGHT_P3_MARK       "YACBAC1SDDAS_BRIGHT_P3"
#define YACBAC1SDDAS_TUP_BRIGHT_P4_MARK       "YACBAC1SDDAS_BRIGHT_P4"
#define YACBAC1SDDAS_TUP_BRIGHT_P5_MARK       "YACBAC1SDDAS_BRIGHT_P5"

#define YACBAC1SDDAS_TUP_EXPOSURE_NORMAL      "YACBAC1SDDAS_EXPOSURE_NORMAL"
#define YACBAC1SDDAS_TUP_EXPOSURE_SPOT        "YACBAC1SDDAS_EXPOSURE_SPOT"
#define YACBAC1SDDAS_TUP_EXPOSURE_AVERAGE     "YACBAC1SDDAS_EXPOSURE_AVERAGE"

#define YACBAC1SDDAS_TUP_FFPS_5_MARK   "YACBAC1SDDAS_FFPS_5"
#define YACBAC1SDDAS_TUP_FFPS_6_MARK   "YACBAC1SDDAS_FFPS_6"
#define YACBAC1SDDAS_TUP_FFPS_7_MARK   "YACBAC1SDDAS_FFPS_7"
#define YACBAC1SDDAS_TUP_FFPS_8_MARK   "YACBAC1SDDAS_FFPS_8"
#define YACBAC1SDDAS_TUP_FFPS_9_MARK   "YACBAC1SDDAS_FFPS_9"
#define YACBAC1SDDAS_TUP_FFPS_10_MARK  "YACBAC1SDDAS_FFPS_10"
#define YACBAC1SDDAS_TUP_FFPS_11_MARK  "YACBAC1SDDAS_FFPS_11"
#define YACBAC1SDDAS_TUP_FFPS_12_MARK  "YACBAC1SDDAS_FFPS_12"
#define YACBAC1SDDAS_TUP_FFPS_13_MARK  "YACBAC1SDDAS_FFPS_13"
#define YACBAC1SDDAS_TUP_FFPS_14_MARK  "YACBAC1SDDAS_FFPS_14"
#define YACBAC1SDDAS_TUP_FFPS_15_MARK  "YACBAC1SDDAS_FFPS_15"

#define YACBAC1SDDAS_TUP_REFLECT_NONE_MARK   "YACBAC1SDDAS_REFLECT_NONE"
#define YACBAC1SDDAS_TUP_REFLECT_MIRROR_MARK "YACBAC1SDDAS_REFLECT_MIRROR"
#define YACBAC1SDDAS_TUP_REFLECT_WATER_MARK  "YACBAC1SDDAS_REFLECT_WATER"
#define YACBAC1SDDAS_TUP_REFLECT_MIRROR_WATER_MARK "YACBAC1SDDAS_REFLECT_MIRROR_WATER"

#define YACBAC1SDDAS_TUP_EFFECT_NONE_MARK     "YACBAC1SDDAS_EFFECT_NONE"
#define YACBAC1SDDAS_TUP_EFFECT_GRAY_MARK     "YACBAC1SDDAS_EFFECT_GRAY"
#define YACBAC1SDDAS_TUP_EFFECT_GREEN_MARK    "YACBAC1SDDAS_EFFECT_GREEN"
#define YACBAC1SDDAS_TUP_EFFECT_COOL_MARK     "YACBAC1SDDAS_EFFECT_COOL"
#define YACBAC1SDDAS_TUP_EFFECT_YELLOW_MARK   "YACBAC1SDDAS_EFFECT_YELLOW"
#define YACBAC1SDDAS_TUP_EFFECT_SEPIA_MARK    "YACBAC1SDDAS_EFFECT_SEPIA"
#define YACBAC1SDDAS_TUP_EFFECT_PURPLE_MARK   "YACBAC1SDDAS_EFFECT_PURPLE"
#define YACBAC1SDDAS_TUP_EFFECT_RED_MARK      "YACBAC1SDDAS_EFFECT_RED"
#define YACBAC1SDDAS_TUP_EFFECT_PINK_MARK     "YACBAC1SDDAS_EFFECT_PINK"
#define YACBAC1SDDAS_TUP_EFFECT_AQUA_MARK     "YACBAC1SDDAS_EFFECT_AQUA"
#define YACBAC1SDDAS_TUP_EFFECT_NEGATIVE_MARK "YACBAC1SDDAS_EFFECT_NEGATIVE"
#define YACBAC1SDDAS_TUP_EFFECT_SOLARIZE_1_MARK "YACBAC1SDDAS_EFFECT_SOLARIZE_1"
#define YACBAC1SDDAS_TUP_EFFECT_SOLARIZE_2_MARK "YACBAC1SDDAS_EFFECT_SOLARIZE_2"

#define YACBAC1SDDAS_TUP_FLICKER_NONE_MARK   "YACBAC1SDDAS_FLICKER_NONE"
#define YACBAC1SDDAS_TUP_FLICKER_60HZ_MARK "YACBAC1SDDAS_FLICKER_60HZ"
#define YACBAC1SDDAS_TUP_FLICKER_50HZ_MARK  "YACBAC1SDDAS_FLICKER_50HZ"
#define YACBAC1SDDAS_TUP_FLICKER_AUTO_MARK "YACBAC1SDDAS_FLICKER_AUTO"

#define YACBAC1SDDAS_TUP_BEGIN_MARK   "@BEGIN"
#define YACBAC1SDDAS_TUP_END_MARK     "@END"
#define YACBAC1SDDAS_TUP_NOP_MARK     "NOP="
#define YACBAC1SDDAS_TUP_REG_MARK     "REG="
#define YACBAC1SDDAS_TUP_REG_DATA_1BYTE_MARK     "REG_D1="
#define YACBAC1SDDAS_TUP_DELAY_MARK   "DELAY="
#define YACBAC1SDDAS_TUP_POLL_REG_MARK       "POLL_REG="
#define YACBAC1SDDAS_TUP_POLL_MCU_VAR_MARK   "POLL_MCU_VAR="


#define YACBAC1SDDAS_CFG_FFPS_MIN   5
#define YACBAC1SDDAS_CFG_FFPS_MAX   15

#define YACBAC1SDDAS_TUP_INIT_MAX_PARAMS      4096
#define YACBAC1SDDAS_TUP_WB_MAX_PARAMS        128
#define YACBAC1SDDAS_TUP_BRIGHT_MAX_PARAMS    128
#define YACBAC1SDDAS_TUP_EXPOSURE_MAX_PARAMS  128
#define YACBAC1SDDAS_TUP_FFPS_MAX_PARAMS      32
#define YACBAC1SDDAS_TUP_REFLECT_MAX_PARAMS   32
#define YACBAC1SDDAS_TUP_EFFECT_MAX_PARAMS    32
#define YACBAC1SDDAS_TUP_FLICKER_MAX_PARAMS  32

#define TRUE  1
#define FALSE 0

 typedef enum {
 #ifndef BRIGHTNESS_STEP_8
    YACBAC1SDDAS_CFG_BRIGHT_M5,
#endif    
    YACBAC1SDDAS_CFG_BRIGHT_M4,
    YACBAC1SDDAS_CFG_BRIGHT_M3,
    YACBAC1SDDAS_CFG_BRIGHT_M2,
    YACBAC1SDDAS_CFG_BRIGHT_M1,
    YACBAC1SDDAS_CFG_BRIGHT_0,
    YACBAC1SDDAS_CFG_BRIGHT_P1,
    YACBAC1SDDAS_CFG_BRIGHT_P2,
    YACBAC1SDDAS_CFG_BRIGHT_P3,
    YACBAC1SDDAS_CFG_BRIGHT_P4,
#ifndef BRIGHTNESS_STEP_8    
    YACBAC1SDDAS_CFG_BRIGHT_P5,
#endif    
    YACBAC1SDDAS_CFG_BRIGHT_MAX
} yacbac1sddas_cfg_bright_e;

 typedef enum {
    YACBAC1SDDAS_CFG_WB_AUTO,
    YACBAC1SDDAS_CFG_WB_CUSTOM,
    YACBAC1SDDAS_CFG_WB_CLOUDY,
    YACBAC1SDDAS_CFG_WB_DAYLIGHT,
    YACBAC1SDDAS_CFG_WB_FLUORESCENT,
    YACBAC1SDDAS_CFG_WB_INCANDESCENT,
    YACBAC1SDDAS_CFG_WB_MAX
} yacbac1sddas_cfg_wb_e;

typedef enum {
    YACBAC1SDDAS_CFG_EFFECT_NONE,
    YACBAC1SDDAS_CFG_EFFECT_GRAY,
    YACBAC1SDDAS_CFG_EFFECT_GREEN,
    YACBAC1SDDAS_CFG_EFFECT_COOL,
    YACBAC1SDDAS_CFG_EFFECT_YELLOW,
    YACBAC1SDDAS_CFG_EFFECT_SEPIA,
    YACBAC1SDDAS_CFG_EFFECT_PURPLE,
    YACBAC1SDDAS_CFG_EFFECT_RED,
    YACBAC1SDDAS_CFG_EFFECT_PINK,
    YACBAC1SDDAS_CFG_EFFECT_AQUA,
    YACBAC1SDDAS_CFG_EFFECT_NEGATIVE,
    YACBAC1SDDAS_CFG_EFFECT_SOLARIZE_1,
    YACBAC1SDDAS_CFG_EFFECT_SOLARIZE_2,
    YACBAC1SDDAS_CFG_EFFECT_MAX
} yacbac1sddas_cfg_effect_e;

 typedef enum {
    YACBAC1SDDAS_CFG_EXPOSURE_NORMAL,
    YACBAC1SDDAS_CFG_EXPOSURE_CENTER,
    YACBAC1SDDAS_CFG_EXPOSURE_AVERAGE,
    YACBAC1SDDAS_CFG_EXPOSURE_SPOT,
    YACBAC1SDDAS_CFG_EXPOSURE_MAX
} yacbac1sddas_cfg_exposure_e;

typedef enum {
    YACBAC1SDDAS_CFG_FLICKER_OFF,
    YACBAC1SDDAS_CFG_FLICKER_60HZ,
    YACBAC1SDDAS_CFG_FLICKER_50HZ,
    YACBAC1SDDAS_CFG_FLICKER_AUTO,
    YACBAC1SDDAS_CFG_FLICKER_MAX
} yacbac1sddas_cfg_flicker_e;

typedef enum {
    YACBAC1SDDAS_CFG_REFLECT_NONE,
    YACBAC1SDDAS_CFG_REFLECT_MIRROR,
    YACBAC1SDDAS_CFG_REFLECT_WATER,
    YACBAC1SDDAS_CFG_REFLECT_MIRROR_WATER,
    YACBAC1SDDAS_CFG_REFLECT_MAX
} yacbac1sddas_cfg_reflect_e;

#ifdef CONFIG_PANTECH_CAMERA_TUNER
typedef enum
{
   YACBAC1SDDAS_TUNE_STATE_NONE,
   YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE,
   YACBAC1SDDAS_TUNE_STATE_SET_VALUE
} yacbac1sddas_tune_state_type;

typedef enum
{
   YACBAC1SDDAS_TUNE_STATE_TUNNING_MODE_OFF,
   YACBAC1SDDAS_TUNE_STATE_TUNNING_MODE_ON
 } yacbac1sddas_tune_mode_type;

 // PSJ_0100402 : 커널로 넘겨주기 위한 튜닝 파라미터 구조체 생성
 typedef enum {          // <addr>           <data>
    CAMOP_NOP,          // don't care       don't care
    CAMOP_DELAY,        // don't care       delay
    CAMOP_WRITE,        // address          data
    CAMOP_WRITE_DATA1,        // address 2byte          data 1byte일 경우 사용
    CAMOP_WRITE_BIT,    // address          position[15:8], value[7:0] (0 or non-zero)
    CAMOP_POLL_REG,     // 1st, address     mask
                        // 2nd, value       delay[15:8], retry[7:0] 
                        // 'address' 의 값이 'value' 가 아닌 경우, 'delay' 를 'retry' 번 하면서 polling
    CAMOP_POLL_MCU_VAR, // 1st, mcu var     mask
                        // 2nd, value       delay[15:8], retry[7:0] 
                        // 'mcu var' 의 값이 'value' 가 아닌 경우, 'delay' 를 'retry' 번 하면서 polling
    CAMOP_MAX
} yacbac1sddas_cfg_op_e;

 typedef struct {
    yacbac1sddas_cfg_op_e op;
    u_int16_t addr;
    u_int16_t data;
} yacbac1sddas_cfg_param_t;

typedef struct {
    yacbac1sddas_cfg_op_e op;
    unsigned short addr;
    unsigned short mask;
    unsigned short data;
} yacbac1sddas_cfg_param_ext_t;

typedef struct{
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_init_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_wb_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_bright_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_exposure_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_ffps_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_reflect_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_effect_params;
}yacbac1sddas_tup_params;



/*
================================================================================
GLOBAL MACROS
================================================================================
*/

/*
================================================================================
GLOBAL DATA TYPES
================================================================================
*/
#if 0
 // PSJ_0100402 : 커널로 넘겨주기 위한 튜닝 파라미터 구조체 생성
typedef struct{
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_init_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_wb_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_bright_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_exposure_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_ffps_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_reflect_params;
	yacbac1sddas_cfg_param_t* yacbac1sddas_tup_effect_params;
}yacbac1sddas_tup_params;
#endif

typedef struct {
    u_int32_t num_params;
    yacbac1sddas_cfg_param_t *params;
} yacbac1sddas_params_t;

typedef struct {
    yacbac1sddas_params_t init;
    yacbac1sddas_params_t wb[YACBAC1SDDAS_CFG_WB_MAX];
    yacbac1sddas_params_t bright[YACBAC1SDDAS_CFG_BRIGHT_MAX];
    yacbac1sddas_params_t exposure[YACBAC1SDDAS_CFG_EXPOSURE_MAX];
    yacbac1sddas_params_t ffps[YACBAC1SDDAS_CFG_FFPS_MAX - YACBAC1SDDAS_CFG_FFPS_MIN + 1];
    yacbac1sddas_params_t reflect[YACBAC1SDDAS_CFG_REFLECT_MAX];
    yacbac1sddas_params_t effect[YACBAC1SDDAS_CFG_EFFECT_MAX];
    yacbac1sddas_params_t flicker[YACBAC1SDDAS_CFG_FLICKER_MAX];
} yacbac1sddas_params_tbl_t;
/*
================================================================================
GLOBAL VARIABLES
================================================================================
*/

/*
================================================================================
GLOBAL API DECLARATIONS
================================================================================
*/

extern void camsensor_yacbac1sddas_tup_init (int8_t *stream);
extern u_int32_t camsensor_yacbac1sddas_tup_get_init_params (yacbac1sddas_cfg_param_t **init_params);
extern u_int32_t camsensor_yacbac1sddas_tup_get_wb_params (yacbac1sddas_cfg_wb_e wb, yacbac1sddas_cfg_param_t **wb_params);
extern u_int32_t camsensor_yacbac1sddas_tup_get_bright_params (yacbac1sddas_cfg_bright_e bright, yacbac1sddas_cfg_param_t **bright_params);
extern u_int32_t camsensor_yacbac1sddas_tup_get_exposure_params (yacbac1sddas_cfg_exposure_e exposure, yacbac1sddas_cfg_param_t **exposure_params);
extern u_int32_t camsensor_yacbac1sddas_tup_get_ffps_params (u_int32_t ffps, yacbac1sddas_cfg_param_t **ffps_params);
extern u_int32_t camsensor_yacbac1sddas_tup_get_reflect_params (yacbac1sddas_cfg_reflect_e reflect, yacbac1sddas_cfg_param_t **reflect_params);
extern u_int32_t camsensor_yacbac1sddas_tup_get_effect_params (yacbac1sddas_cfg_effect_e effect, yacbac1sddas_cfg_param_t **effect_params);
extern u_int32_t camsensor_yacbac1sddas_tup_get_flicker_params (yacbac1sddas_cfg_flicker_e flicker, yacbac1sddas_cfg_param_t **flicker_params);
#endif
#endif /* __CAMSENSOR_YACBAC1SDDAS_TUNEUP_H__ */
