/*
 lived 2009.11.05
 FEATURE define
*/

#ifndef SKY_FRAMEWORK_FEATURE
#define SKY_FRAMEWORK_FEATURE

/* Debug Msg */
#if 1//defined(FEATURE_AARM_RELEASE_MODE) //GPL_KERNEL SYLEE
#define TEST_MSG(...)
#else
#define TEST_MSG(...)   ((void)LOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define SKY_LOG_DEBUG
#endif

#define SKY_LCD_LOADBAR_ANDROID  // 20100422, kkcho, aArm에서 부팅 로그바 출력 
#define SKY_EF18K_LCD

#define CONFIG_SKY_FB_24BPP_RGB888 // 20100609, kkcho, for boot_image via MSMFB_FRAMEBUF_32 

#define SKY_OVERLAY_SURFACEVIEW_ROTATION // 20101029, kkcho, for only TDMB_180_Rotation!!!

/*#define DIM_WITH_TEXTURE*/
#define SKY_ADD_SURFACE_BUFFER_HEAP_270

/* 20100902, kkcho, 영상통화시 overlay를 2개 생성하면, 화면 play가 안되는 문제로 인해 RGB_PIPE를 사용하도록 수정.*/
#define SKY_MDP4_VT_CALL_SCREEN

/* 20100905, kkcho, 영상통화시 내화면 좌우 반전 */  
#ifdef SKY_ADD_SURFACE_BUFFER_HEAP_270
#define SKY_VT_CALL_VGA_LR_CHANGE
#endif

/* 2011.11.12, lived, Surface View size 1x1로 잘못 setting되는 경우 방지 */
#define F_SKYDISP_GBUG_INVALID_SURFACE_SIZE

/* 2011.12.06, lived, Surface View의 (x,y) 좌표가 잘못되는 경우 방지
 * LCD WxH와 SurfaceView의 W 혹은 H 가 같은 경우에는 x 또는 y를 0으로 치환한다.
 */
#define F_SKYDISP_GBUG_INVALID_SURFACE_POSITION

/* 20100916, kkcho, MDP초기화시 noise화면제거를 위해 lcd를 off한다. */ 
#define SKY_REMOVE_NOISE_MDP_INIT

/* 20101112, kkcho, LCD_OFF->LCD_ON 시 stanby_mode를 사용 하여 on/off수행 : Not yet availble*/
/*#define SKY_FAST_LCD_ON */

/* 20101112, kkcho, Overlay-view play시 hole-key에 의한 Hold화면 ON/OFF시, video화면 잠시 나타나는 현상. : Not yet availble */
/*#define SKY_MDP_VG_CLEAR_HOLD_CHANGE*/
/* 2011.01.14 code by EffectiveSKY
 * Description : SKY_FEATURE_ALLOW_FASTBOOT_MODE allow access to fastboot-mode.
 *               If SKY_FEATURE_ALLOW_FASTBOOT_MODE is not defined, fastboot is not allowed access from user. 
 */
/* #define	SKY_FEATURE_ALLOW_FASTBOOT_MODE*/
#endif  /* SKY_FRAMEWORK_FEATURE */
