#ifndef __CUST_SKYCAM_H__
#define __CUST_SKYCAM_H__

/*
Modified files which have "SKYCAM" tag:
	android/build/core/Makefile
	android/kernel/arch/arm/configs/qsd8650_defconfig
	android/kernel/drivers/media/video/msm/Kconfig
	android/kernel/drivers/media/video/msm/Makefile
	android/vendor/qcom/android-open/libcamera2/Android.mk
	android/vendor/qcom-proprietary/mm-camera/Android.mk
	android/vendor/qcom-proprietary/mm-camera/camera.mk
	android/vendor/qcom-proprietary/mm-camera/targets/Android.mk

Modified files which don't have "SKYCAM" tag:
	android/kernel/drivers/media/video/msm/mt9p111.h
	android/kernel/drivers/media/video/msm/mt9p111_reg.c
	android/vendor/qcom-proprietary/mm-camera/targets/tgtcommon/sensor/mt9p111/mt9p111.h
	android/kernel/drivers/media/video/msm/yacbac1sddas.h
	android/kernel/drivers/media/video/msm/yacbac1sddas_reg.c
	android/vendor/qcom-proprietary/mm-camera/targets/tgtcommon/sensor/yacbac1sddas/yacbac1sddas.h

Local features:
	CONFIG_MSM_CAMERA_DEBUG (MSM_CAMERA_DEBUG)
	CONFIG_SKYCAM_MT9P111 (SKYCAM_MT9P111)
	F_SKYCAM_SENSOR_MT9P111
	CONFIG_SKYCAM_YACBAC1SDDAS (SKYCAM_YACBAC1SDDAS)
	F_SKYCAM_SENSOR_YACBAC1SDDAS
	F_SKYCAM_LOG_VERBOSE (enable LOGV/LOGD/LOGI in userspace)
	F_SKYCAM_LOG_CDBG (enable CDBG in userspace)
	F_SKYCAM_LOG_OEM (enable SKYCDBG/SKYCERR in userspace)

How to turn off all camera logs (kernel/userspace):
	android/kernel/arch/arm/config/qsd8650-defconfig
		- CONFIG_MSM_CAMERA_DEBUG -> no (default)
	Disable F_SKYCAM_LOG_PRINTK (default enabled)
	Find all F_SKYCAM_LOG_OEM,
		- comment out (default)
	Find all F_SKYCAM_LOG_CDBG
		- comment out (default)
	Find all F_SKYCAM_LOG_VERBOSE
		- comment out (default)

How to exclude module "MT9P111":
	android/kernel/arch/arm/config/msm7630_defconfig
		- disable CONFIG_MT9P111
	android/vendor/qcom-proprietary/mm-camera/Android.mk
	android/vendor/qcom-proprietary/mm-camera/camera.mk
		- CONFIG_MT9P111=no (default yes)

How to exclude module "OLAWORKS":



(2)  카메라 관련 모든 kernel/userspace/android 로그를 제거

kernel/arch/arm/config/qsd8650-perf_defconfig 를 수정한다.

	# CONFIG_MSM_CAMERA_DEBUG is not set (default)

CUST_SKYCAM.h 에서 F_SKYCAM_LOG_PRINTK 을 #undef 한다.

	#define F_SKYCAM_LOG_PRINTK (default)

모든 소스 파일에서 F_SKYCAM_LOG_OEM 을 찾아 주석 처리한다.
	선언 된 경우, 해당 파일에 사용된 SKYCDBG/SKYCERR 매크로를 이용한
	메시지들을 활성화 시킨다. (user-space only)

모든 소스 파일에서 F_SKYCAM_LOG_CDBG 를 찾아 주석 처리한다.
	선언 된 경우, 해당 파일에 사용된 CDBG 매크로를 이용한 메시지들을
	활성화 시킨다. (user-space only)

모든 소스 파일에서 F_SKYCAM_LOG_VERBOSE 를 찾아 주석 처리한다.
	선언 된 경우, 해당 파일에 사용된 LOGV/LOGD/LOGI 매크로를 이용한
	메시지들을 활성화 시킨다. (user-space only)


(4)  안면인식 관련 기능 빌드 환경

vendor/qcom/android-open/libcamera2/Android.mk 를 수정한다.
	3rd PARTY 솔루션 사용 여부를 결정한다.

	SKYCAM_FD_ENGINE := 0		미포함
	SKYCAM_FD_ENGINE := 1		올라웍스 솔루션 사용
	SKYCAM_FD_ENGINE := 2		기타 솔루션 사용

CUST_SKYCAM.h 에서 F_SKYCAM_ADD_CFG_FACE_FILTER 를 #undef 한다.
	안면인식 기능 관련 인터페이스 포함 여부를 결정한다.

libOlaEngine.so 를 기존 libcamera.so 에 링크하므로 기존 대비 libcamera.so 의
크기가 증가하여 링크 오류가 발생 가능하며, 이 경우 아래 파일들에서
liboemcamera.so 의 영역을 줄여 libcamera.so 의 영역을 확보할 수 있다.

build/core/prelink-linux-arm-2G.map (for 2G/2G)
build/core/prelink-linux-arm.map (for 1G/3G)

2010-10-04  올라웍스 라이브러리 및 소스코드 업데이트

*/


/* 내수 CS 부서에서는 소비자 시료 분석을 위해 별도 PC 프로그램을 사용하여
 * 카메라 구동 시간 정보를 PC 로 추출한다.
 *
 * 구현 방법은 공정 커맨드 사양서에 명시되어 있으므로 관련 코드들은 공정 커맨드
 * 관련 모듈에 포함되어 있으나, 공정 커맨드 용 PC 프로그램을 사용하지 않고 별도
 * 프로그램을 사용하여, 시료의 DIAG 포트로부터 구동 시간 정보를 확인할 수 있다.
 *
 * 공정 커맨드 사양서 v10.35 기반 구현
 * PhoneInfoDisplay v4.0 프로그램으로 확인
 * 사양서와 프로그램은 DS2팀 박경호 선임에게 문의 */
#define F_SKYCAM_FACTORY_PROC_CMD

/*========================================================
 * SKYCAM Lee Jitae 2010-11-09
 * 카메라 ANR발생이나 Device이상으로 open fail이 일어 날 경우 정상적인 재진입을 위한
 * cameraservice connect처리 및 vfe reset코드 작업
 *F_SKYCAM_INVALIDATE_CAMERA_CLIENT feature는 이미 들어가 있었음
======================================================== */
/* 카메라 장치 파일 OPEN 에 실패한 경우 (단순 I2C 버스 R/W 오류, 카메라 미장착)
 * 오류 처리를 위해 수정한다.
 *
 * 장치 파일을 OPEN 하는 과정에서 VFE 초기화 이후 카메라 HW 초기화가 이루어
 * 지는데, HW 초기화에 실패할 경우 VFE 는 초기화 된 상태로 남게되고 다음
 * OPEN 시도 시 HW 초기화에 성공한다 하더라도 이미 VFE 가 초기화된 상태이므로
 * VFE 초기화 시 오류가 발생한다.
 *
 * 호출순서 : vfefn.vfe_init() -> sctrl.s_init()
 *
 * HW 초기화에 실패할 경우, 이미 초기화된 VFE 를 RELEASE (vfe_release) 시켜
 * 다음 열기 시도 시 정상 동작하도록 수정한다.
 *
 * ECLAIR 버전에서는 위와 같은 에러 처리에도 불구하고 센서가 연결되어 있지
 * 않거나 센서 하드웨어에 이상이 발생한 경우 카메라 응용이 ANR 오류로 인해
 * 비정상 종료되고 이후 재부팅 전까지는 지속하여 재진입이 불가능하다.
 *
 * 센서가 비 정상인 경우, ISP 초기화 시 ISP 와 센서를 프리뷰 모드로 설정하는
 * 과정에서 3초 간 POLLING 수행하며, 이로 인해 타임아웃 발생하고 ANR 오류로
 * 이어진다. 비 정상 종료 이후 카메라 재진입 시 센서가 정상이라 하더라도 ANR
 * 오류 이후 응용이 비 정상적으로 종료되었으므로 FRAMEWORK 내부는 비 정상인
 * 상태로 유지되고, 이로 인해 재부팅 전까지는 카메라 응용 진입 시 "Unable to
 * connect camera device" 팝업과 함께 무조건 진입에 실패한다.
 *
 * ISP 초기화 시 프리뷰 모드 설정 이전에, ISP 를 통해 센서의 특정 레지스터를
 * 1회 READ 하고 비 정상일 경우, 이를 FRAMWORK 을 통해 응용으로 전달하여
 * 정상적으로 종료되도록 수정한다.
 *
 * 또한 ISP 자체에 이상이 발생한 경우에도, PROBE 시에 오류 발생하여 해당
 * 디바이스 파일들을 생성할 수 없으므로 FRAMWORK 내부에서 함께 처리 가능하다. */
#define F_SKYCAM_INVALIDATE_CAMERA_CLIENT


/* SKYCAM Lee Jitae 2010-11-12
 *  Antina sensor Mt9p111에서 열화 현상 발생
 * OTP렌즈쉐이딩 값을 1이라고 인식하는 레퍼런스 볼티지를 조절하는 레지스터
 * 정확한 값은 Antina에서 가르쳐 주지 않음
 * 적용 되지 않으면 열화 현상이 발생하는 모듈이 나온다 (EF13S에서 많이 나왔음) EF18K에서는 한개 발견해서 먼저 적용을 함
 */
#define F_SKYCAM_SENSOR_BUG

/* SKYCAM Lee Jitae 2010-11-12
 * MSM7X30, MSM8x55에서 2030패치 이후 갤러리의 아이콘 밑에 검은 선이 나타난다.
 * 이것을 패치하는 피쳐. 하지만, Assemble파일이기 때문에 여기서 정의만 해놓고
 * 실질적으로는 주석으로 막혀있다. 검색으로 찾을 것, 위치는
 * LINUX\android\external\libpng\contrib\pngneon\png_read_filter_row_neon.s  이다.
 */
#define F_SKYCAM_LIBPNG_ERROR

#define F_SKYCAM_LOG_DEBUG

#define F_SKYCAM_EF18K

/* 카메라 관련 GPIO 들을 설정한다. 
 *
 * 총 12 라인의 데이터 버스에서 상위 8비트만 사용한다. 
 *
 * 카메라 HW 종료 시점과 SLEEP 진입 시점 충돌 시 HW 이상 동작을 방지하기 위해
 * SLEEP_CONFIGS[] 에서 카메라 관련 GPIO 들은 모두 제외시킨다.
 *
 * VCM/FLUID 관련 GPIO 들은 사용하지 않으므로 설정 부분을 삭제한다.
 *
 * 관련 파일 참고
 * AMSS/products/7x30/core/systemdrivers/tlmm/src/bsp/7x30/TLMMBsp.c
 * AMSS/products/7x30/core/systemdrivers/tlmm/src/bsp/7x30/TLMMBspMsm.h */
#define F_SKYCAM_HW_GPIO

/* HW I2C 버스의 클럭을 수정한다. 
 *
 * MT9P111 의 경우, 초기화 및 각종 설정 변경 시 WRITE 해야 할 레지스터의 수가 
 * 많으므로 성능 향상을 위해 최대 클럭을 사용한다. */
#define F_SKYCAM_CUST_I2C_CLK_FREQ

/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC                                                            */
/*  EF18K 에만 적용되는 또는 EF18K 에서만 검증된 FEATURE 목록                 */
/*----------------------------------------------------------------------------*/
 
#if defined(T_EF18K)	
#define F_SKYCAM_TARGET_EF18K
#endif
#if defined(T_EF30S)
#define F_SKYCAM_TARGET_EF30S
#endif

/*
 * 현재 VREG 헤더파일 포함과 i2c에 관련되어 있음
 *  F_SKYCAM_VREG와 F_SKYCAM_I2C_INIT로 바꿀 예정
 */

#define F_SKYCAM

/* SKYCAM Lee Jitae 2010-10-06
 * 듀얼카메라를 구현한 부분에서 VT 관련된 부분의 작업을 위해 추가했다
 * 듀얼 카메라 구현에서 QualcommCameraHardware에서 CamID로 구분을 하는데 이 부분에서
 * 3,4번을 VT카메라로 이용했다. DUAL CAM에서 VT구현 부분을 분리
 */
#define F_SKYCAM_ADD_VT_CAM

/* "ro.product.device" 설정 값 변경으로 인한 코드 오동작을 막기 위해 카메라
 * 관련 코드들에서는 이 값을 읽지 않거나 'TARGET_QSD8250' 으로 고정한다.
 * 시스템 설정 "ro.product.device" 는 릴리즈 버전의 경우 "qsd8250_surf" 이고,
 * EF18K 의 경우 "ef18k" 으로 설정되어 있다. */
#define F_SKYCAM_CUST_TARGET_TYPE_7X30

/* SKYCAM Lee Jitae 2010-09-25
 * VPE의사용으로 인해서 비디오 프레임이 올라오지 않는 문제점이 발생
 * mDisEnabled의 미사용으로 문제 해결
 * mDisEnabled의 경우에는 BayerSensor의 경우에만 Enable한다.
 * mDisEnabled 은 DIS(Digital Image Stabilization) 기능을 ON- OFF하는 것이다.
 */
 #define F_SKYCAM_VIDEO_FRAME_VPE

/* SKYCAM Lee Jitae 2010-12-11
 * 유저 영역의 LOGV/LOGD/LOGI 메세지 on off
*/
#define F_SKYCAM_LOG_VERBOSE


#define F_SKYCAM_LOG_PRINTK	//커널 영역의 SKYCDBG/SKYCERR 메세지 on off
#define F_SKYCAM_LOG_OEM		//유저 영역의 SKYCDBG/SKYCERR 메세지 on off


/* 녹화 시작/종료 시, 효과음이 해당 동영상 파일에 녹화되는 문제를 임시 수정한다.
 *
 * 녹화 시작 시에는 효과음 재생 완료 시까지 POLLING 후 녹화를 시작하고,
 * 완료 시에는 녹화 완료 이후 800ms 고정 DELAY 적용 후 효과음을 재생한다.
 *
 * R4070 까지는 녹화 시작 시 오디오 클럭이 초기화되는 시간이 오래 걸려
 * 다수의 프레임이 DROP 되면서 녹화 시작음이 녹음 될 가능성이 희박했으나,
 * R4075 에서 이 이슈가 수정되면서 녹화 시작 시 효과음이 100% 확률로 녹음되고,
 * 종료 시 80% 이상 확률로 녹음된다.
 *
 * F_SKYCAM_TODO, QUALCOMM 수정 확인 후 삭제해야 한다. (SR#307114) */
#define F_SKYCAM_QBUG_REC_BEEP_IS_RECORDED


/* 다운 사이즈를 하기 위한 함수를 사용하기 위해서,  캡쳐 화면을 보여주는 곳과 캡쳐 화면이  틀린 것을 수정하기 위해서
 * 썸네일을 가지고 스냅샷프리뷰를 만든다.
 * IPL 라이브러리에 있는 downsize를 이용해서 줄인다.
 * 현재 SR진행 중 -> 퀄컴 패치에 적용 될 수 있도록 수정 받기 위해서 진행 중
 */

/* #define F_SKYCAM_SNAPSHOT_PREVIEW  */


/* SKYCAM_PSJ_100513
 * 전면 카메라의 프리뷰를 90도 회전시키기 위한 작업
 * Surface만 돌려서 구현
 * cameraservice.cpp의 handlesutter함수에 스냅샷 후 post view rotation도 해줘야 함
 * MDP 3.1 이하에서만 사용된다.
 * MSM7230이후부터는 MDP4.0이 사용되기 때문에 의미는 두지 않는다.
 */
/* #define F_SKYCAM_PREVIEW_ROTATION_FOR_SUBCAM */

/*
 * 전면 카메라의 JPEG 인코딩시 Rotation적용
 */
#define F_SKYCAM_JPEG_ENCODE_ROTATION_FOR_SUBCAM

/*
 * 일반 카메라 진입, 종료 후 VT카메라 실행시 start preview fail나는 상황 수정
 * picture size설정이 안되면 mDimension의 값들이 모두 0으로 변해 set dimension fail나는 현상
 */
/* SKYCAM Lee Jitae 2010-09-01
 *  VT카메라를 시작할 때 디폴트 preview사이즈가 800*480이 된다.
 *  이것을 176*144로 세팅하기 위한 작업
 */
#define F_SKYCAM_VT_FIX_CAMERA_START_ERROR

/*
 * VT 메인카메라 실행시 preview size를 176, 176으로 설정하므로 인해
 * 내 화면 176, 144로 보여지는 것이 깨짐
 * preview size를 176, 176으로 하나 receivePreviewFrame함수에서 176, 144로 crop하여 90 rotate하게 수정 되었음
 ******
 *  - SKYCAM Lee Jitae 2010-08-30 추가사항MSM7x30,8x55 계열 문제-
 * MDP4.0 을 사용하면서 문제가 발생하기 시작한다.
 * EF13과는 많은 차이를 보여서 수정을 가함
 * 아직 테스트는 하지 못함(2010-09-01) 테스트 필요
 */
 #define F_SKYCAM_VT_FIX_MAIN_CAM_PREVIEW_SIZE

 /*
 * pantech VT는 호가 연결되면 전송 버퍼를 video 버퍼로부터 얻기 위해 start recording
 * 을 시작하며 따라서 connect/disconnect 시에 촬영음이 발생한다.
 * pantech VT에서 촬영음이 발생하는 것을 막기 위해 CameraService에 
 * CAMERA_CMD_SET_SHUTTER_DISABLE commad를 추가 하였다.
*/
 #define F_SKYCAM_VT_SHUTTER_DISABLE

/*
 * VT카메라 On/Off 설정
*/
 #define F_SKYCAM_CFG_VT

 /* 
 * MEDIA_RECORDER_INFO_FILESIZE_PROGRESS CallBack, Only Use KT Phone 
 * KT폰에서는 비디오 파일 사이즈를 기록하는데 파일 사이즈를 APP에 알려주기 위해서
 * 추가 
 */
#define F_SKYCAM_ADD_EVT_RECSIZE

 /* SKYCAM Lee Jitae 2010-08-30
 *  Main 카메라 VT 프리뷰는 176 *176로 설정되어 있다.
 *  VT Overlay는 176*144로 내보내어 진다. 이러한 설정을 IPL 라이브러리를
 *  이용해서 사이즈와 90도 로테이션을 하고 VT쪽으로 올려 보낸다.
 *  receivePreview에서 EF18K VT화면을 출력한다. overlay의 setCrop를 이용해서
 *  화면을 176*144화면을 출력해준다. 144*176인 이유는 90도 회전하기 때문이다.
 *  runVideoThread에서는 IPL을 이용해서 90도 회전 후 memcpy를 이용해서 Y값 추출후 CbCr값을 이후에 추출한다.
 *  vframe를 쓰면 남은 이미지 때문에 이미지깨짐,  new를 이용해서 메모리 할당 후 90도
 *  돌아간 이미지를 넣어준다.
 *  2010-09-30 Froyo Update
 *  record-size 가 추가 되었다. initPreview에서 Record size를 세팅하는 부분이  있다.
 *  VT에서 필요한 사이즈는 176*144이고 실제 센서에서 나와야하는 사이즈는 176*176인데
 *  initPreview에 추가 전의 코드는 record size를 176*144로 세팅해서 화면이 제대로 나오지 않는다.
 *  이 부분을 다신 176*176이 될 수 있게 해주는 코드 추가
 */
#define F_SKYCAM_MAINCAM_VT_PREVIEW

/* SKYCAM Lee Jitae 2010-10-05
 * VGA카메라에서 지원하지 않는 Paramter이다.
 * 실제 Paramter세팅에 함수에서NO_ERROR를 리턴함으로서 파라메터 세팅을 막음.
 */
#define F_SKYCAM_VGA_NOT_SUPPORT_PARAMTER

/* SKYCAM Lee Jitae 2010-10-05
 * VT의 경우에는 YCrCb Inpu 인코더를 사용하고 캠코더의 경우는 YCbCr Input 인코더를 사용한다.
 * MSM7227과 동일하게 VT응용을 하기 위해서 encoder input format을 바꾸어 준다.
 */
#define F_SKYCAM_VT_ENCOER_FORMAT

/* EF18K 에서 사용되는 카메라 정보 테이블 (최대 출력 가능 해상도, 최대 스냅샷
 * 해상도, AF 지원 여부) 을 수정한다. */
#define F_SKYCAM_CUST_SENSOR_TYPE

/* Service단에서 이전 connect가 남아있을  경우 종료 해주는 부분 추가 */
#define F_SKYCAM_DEVICE_CONNECT_FAIL_FIXED

/* MSM단에서 이전 control, config가 남아있을  경우 종료 해주는 부분 추가 */
#define F_SKYCAM_FIX_MSM_OPEN_FAIL

/*
*camera id별로 검색하여 각각 app에서 후면 카메라, 전면 카메라 따로 동작시 진입 가능하게 되어
*진입시 open이 비슷한 시기에 되거나(홈키 long 키, 전환), setparameter가 셋팅되는 현상등이 발생하여,
*전혀 의도하지 않은 값이 써져 오동작 하는 문제로
*froyo와 마찬가지로 전 후면 모든 카메라가 이전 카메라 release 이전에는 진입 불가하도록 수정
*
*HW, QCH 모두 개별의 카메라 동작을 지원한다면 아래를 제거한 후 테스트 할 것.
*/
#define F_SKYCAM_GB_ORIGIN_CONNECT

/* 카메라 IOCTL 커맨드 MSM_CAM_IOCTL_SENSOR_IO_CFG 를 확장한다.
 *
 * EF10S 에서 추가한 기능 및 SOC 카메라를 감안하지 않고 미구현된 부분들을
 * 수정 및 추가 구현한다. */
#define F_SKYCAM_CUST_MSM_CAMERA_CFG

/* SKY캠코더 녹화파일이 Qparser로 확인시 에러발생.(deocde thumnail할수없음)
 * 캠코더 레코딩시 구글캠코더와 SKY캠코더의 차이중 하나가 
 * app에서 내려오는 stagefrightrecorder의 mMaxFileDurationUs 값이다.
 * (SKY캠코더: 3600000000(us)=1시간 / 구글캠코더: 600000000(us)=10분.)
 * mMaxFileDurationUs의 차이로인해 Mpeg4write에서 
 * SKY캠코더는 64bitfileoffset / 구글캠코더는 32bitfileoffset를 사용하게 된다.
 * 이를 32bitfileoffset으로 동일하게 설정하기 위해서 해당부분을 수정한다.
 * 임시로 수정되는 부분이므로 추가 검토 및 지속적인 모니터링이 필요함.
*/
#define F_SKYCAM_VIDEO_REC_FILEOFFSET

/*
 * 카메라 드라이버가 어플이 종료되지 않았을 때, suspend 되는 것을 막는다.
 * power control 에 의해 커널 드라이버가 suspend 되는 것을 막는다.
 * 일반적인 경우 카메라 어플이 카메라 드라이버를 종료 시키며, 이 때 커널 드라이버도 내려간다.
 * HD 영상통화의 경우 조도 센서의 control이 불가능해 LCD가 OFF 되는 상황에서 suspend가 발생한다.
 * 이 때 커널 드라이버가 suspend 되지 않도록 한다.
*/
#define F_SKYCAM_FIX_SUSPENDLOCK_ADD

/* ZOOM LEVEL 이 51 이상인경우 wide size 에서 preview frame 의 width/height ratio가 달라진다.
 * zoom level 0~50 : 1.6666
 * zoom level 51 이상 : width 만 확대 됨.
 * zoom level 60 : 1.3333
 * R5040에서 Qualcomm SBA 받아서 patch 적용. 이후 release에 적용 여부 확인 후 삭제 한다.
 */
#define F_SKYCAM_QBUG_ZOOM_MORE51

/* SKYCAM Lee Jitae 2010-10-05
 * QualcommCameraHardware.cpp에서 setParameters는 VT와 카메라 관련된 세팅이 된다. VT에서 필요없는 세팅을 피하기 위해서
 * 카메라일때만 파라메터 세팅을 하기 위해서 구현한 부분
 * EF18K의경우 필요하지 않은 세팅은 ifndef로 막아 두었다.
 */
#define F_SKYCAM_CAMERA_SETTING_ONLY

/* SKYCAM_PSJ_0100331 : 카메라 튜닝 파일 로드를 위해... */
/* #define F_SKYCAM_SENSOR_TUNEUP */
#ifdef F_SKYCAM_SENSOR_TUNEUP
#define F_SKYCAM_TUP_LOAD_FILE
#define MT9P111_TUP_FNAME       "/sdcard/DCIM/Camera/camsensor_mt9p111_tuneup_ef18k.txt"
#define YACBAC1SDDAS_TUP_FNAME "/sdcard/DCIM/Camera/camsensor_yacbac1sddas_tuneup_ef18k.txt"
#endif

/* SKYCAM 2010-10-21
 * 2010-10-26 Qualcomm에서 수정 코드를 받아서 수정
 * GPS, JPEG THUMBNAIL, JPEG_EXIF 에서 fail이 발생한다.
 * 현재 MT9P111센서에서 조도가 낮을 경우에는 checkpreview가 failed가 발생.
 * Timeout Error가 발생한다. 이러한 이유는WAIT_FOR_COMMAND_TO_COMPLETE가 1.5초로 설정되어서이다.
 * WAIT_FOR_COMMAND_TO_COMPLETE의 경우 EF13S, EF13L의 경우는 5초로 설정되어  있다.
 * 현재까지 원인으로는 저조도에서는 센서 안정화 까지의 시간이 많이 걸리기 때문이라고 파악하고 있다.
 */
#define F_SKYCAM_QBUG_CTS

/* SKYCAM Lee Jitae 2010-11-27
 * 촬영이 종료되지 않은 상황에서 Stop preview를 하여 메모리가 해제되는 상황이 발생, NULL POINTER REFERENCE오류로 인해서 각종 서버들이
 * 죽고 이후에는 이상 화면 표시, 사운드 안남 등의 옲들이 발생합니다.
 */
#define F_SKYCAM_QBUG_SNAPSHOT_RELEASE_INTERRUPT

#ifdef F_SKYCAM_TARGET_EF18K
/* SKYCAM_PSJ_110531
 *  stagefright로 인코딩 된 동영상이 KT에서 전송이 안되는 현상
 *  동영상 트랙 헤더의 "pasp" 부분을 KT서버에서 파싱을 못하는 것으로 보여지며 SKT나 LG향에서는 정상적으로 동작이 되어짐
 *  KT향의 경우 서버 수정이 불가한 상황을 대비하여 헤더의 해당 부분을 넣지 않도록 함. 향 후 KT에서 수정 될 수 있는 사항
 */
#define F_SKYCAM_FIX_MMS_CAMCORDER_FOR_KT
#endif

 /* SKYCAM_PSJ_110607
 *  VT카메라 시작시 setPreviewDisplay이후에 preview size의 파라미터 설정을 하여 
 *  setPreviewDisplay에서 만들어졌던 overlay0가 free되고 다시 만들어지는 현상이 있음
 *  다시 정상적으로 만들어져야 하나 destroy(free)와 create사이에 적절한 delay가 없으면 overlay1이 생기고 overlay0이 free되는 경우가 있음
 *  이런 상황을 방지하기 위하여 setPreviewDisplay를 파라미터 설정 이후로 수정
 */
#define F_SKYCAM_FIX_VT_OVERLAY_FAIL

/* SKYCAM_PSJ_110802
 * GB 업그레이드 작업 이후 음악 재생중 연속 촬영시 Halt나는 현상이 있음
 * sound파트에서 적절한 delay가 필요하다고 하여 stop시가 아니더라도 체크하여
 * delay가 적용될 수 있도록 수정 */
#define F_SKYCAM_FIX_CONTINUOUS_CAMERA_SOUND_ERROR

/* SKYCAM_PSJ_110812
 * takepicture가 되는 도중 종료 버튼을 눌藥뗌?경우 snapshot thread가 mLock의 deadlock에 걸려
 종료하지 못하는 case 발생. mLock의 영향이 미치지 않는 곳으로 release시에 snapshot thread의 종료를
 기다려 주는 부분 위치 수정
 기존 preview frame sync feature도 여기로 옮김
 */
#define F_SKYCAM_SYNC_SNAPSHOT_THREAD_N_CONTROL_THREAD
#define F_SKYCAM_SYNC_FRAME_THREAD_N_CONTROL_THREAD

 /* SKYCAM_PSJ_110818
 * 세로 방향기준으로 실장된 전면카메라를 가로 방향기준으로 있는것 처럼 카메라의 동작을 수정함
 * Preview, Capture, Postview, Recording 이 LCD의 화면 전체에 꽉 차도록 crop, scale, orientation등의 작업이 되어있음
 * 8255에서는 Overlay가 사용됨에 따라 EF31S와 같이 7227에서 작업된 내용하고 조금씩 차이가 있음
 * Capture의 경우 image crop, scale시에 우리가 필요한 사이즈 변환시 bottom, right의 3줄 정도가 짤리는 현상이 있음(IPL 버그)
 * 실제로는 336*448에서 328*440으로 crop하여 사용함
 */
#define F_SKYCAM_FAKE_FRONT_CAMERA

 /* SKYCAM_PSJ_110818
 * IPL을 이용하여 Image crop, scale시에 3줄 정도 짤리는 현상의 피하고자 MDP의 C2D composition으로 
 * Image Processing을 하기위해 테스트 한 코드
 * 실제 동작이 되지 않으며 퀄컴이 작업을 더 해줘야 하는 부분임
 */
/*#define F_SKYCAM_TEST_C2D_COMPOSITION */

/*----------------------------------------------------------------------------*/
/*  SENSOR CONFIGURATION                                                      */
/*  모델 별 사용 센서(ISP)를 위해 수정/추가된 FEATURE 목록                    */
/*----------------------------------------------------------------------------*/
/* If you use YUV sensor only, define this feature. If you use two cameras,
 * and one is YUV and another is BAYER, don't define this. If you define this,
 * some BAYER-codes will not be processed.
 */
#define F_SKYCAM_YUV_SENSOR_ONLY

#if defined(F_SKYCAM_TARGET_EF18K) || defined(F_SKYCAM_TARGET_EF30S)
/* EF18K, EF30S에서사용되는 센서*/
#define F_SKYCAM_MT9P111
#define F_SKYCAM_YACBAC1SDDAS
#endif


#ifdef F_SKYCAM_YUV_SENSOR_ONLY
#define F_SKYCAM_ADD_CFG_ANTISHAKE
#define F_SKYCAM_FIX_CFG_EXPOSURE
#define F_SKYCAM_FIX_CFG_SCENE_MODE
#define F_SKYCAM_FIX_CFG_FOCUS_RECT
#define F_SKYCAM_FIX_CFG_REFLECT
#define F_SKYCAM_FIX_CFG_FOCUS_STEP
#define F_SKYCAM_FIX_CFG_ROTATION
#define F_SKYCAM_ADD_CFG_MULTISHOT
#define F_SKYCAM_CUST_PICTURE_SIZES
#define F_SKYCAM_FIX_CFG_BRIGHTNESS
#define F_SKYCAM_FIX_CFG_EFFECT
#define F_SKYCAM_FIX_CFG_PREVIEW_FPS
#define F_SKYCAM_FIX_CFG_ANTIBANDING
#define F_SKYCAM_FIX_CFG_AF
#define F_SKYCAM_CUST_PREVIEW_SIZES
#define F_SKYCAM_FIX_CFG_WB
/* F_SKYCAM Lee Jitae 2010-11-15
 *  Camcoder일 경우 CAF를 ON, OFF를 할 수 있다
 * EF18K의 경우 해상력 문제가 발생해서 제외
 * ==> GB업그레이드에서 테스트 후 문제 없으면 18K에도 적용 함
 */
#define F_SKYCAM_FIX_CFG_CAF
#endif


#ifndef F_SKYCAM_YUV_SENSOR_ONLY
/*
 *  DIS(Digital Image Stabilization)기능 ON
 */
#define F_SKYCAM_VIDEO_FRAME_VPE
#endif

/*
 * 올라웍스 얼굴 인식 솔루션 사용하기 위한 FEATURE
 * 2010-10-04  올라웍스 라이브러리 및 소스코드 업데이트
 */
#define F_SKYCAM_ADD_CFG_FACE_FILTER

#ifdef F_SKYCAM_ADD_CFG_FACE_FILTER
#define F_SKYCAM_ADD_CFG_FACE_FILTER_RECT

/* SKYCAM Lee Jitae 2010-11-23
 *[문제점] 사진촬영 특수촬영/얼굴꾸미기/셀카촬영일때 영상전화수신시 지정된mp3벨 출력되는중에 기본벨로 변경되서 출력함
 * Call event 수신시, stop-preveiw 중(or 후)  olaworks process 동작으로 인해 media-serve kill
 * mutex 처리로 stop-preview 중(or 후) olaworks process 동작 되지 않도록 추가.
*/
#define F_SKYCAM_OLAWORKS_LOCK
#endif


/* 플래쉬 LED 설정을 위한 인터페이스를 수정한다.
 *
 * QUALCOMM 에서는 별도의 IOCTL (MSM_CAM_IOCTL_FLASH_LED_CFG) 커맨드를
 * 사용하여 구현되어 있으며, PMIC 전원을 사용하는 LED 드라이버를 제공한다.
 * EF10S 에서는 이를 사용하지 않으며, MSM_CAM_IOCTL_SENSOR_IO_CFG 을 확장하여
 * 구현한다.
 *
 * AUTO 모드로 설정할 경우, 저조도 일 경우에만 AF 수행 중 AF/AE 를 위해
 * 잠시 ON 되고, 실제 스냅샷 시점에서 한 번 더 ON 된다. */
#define F_SKYCAM_FIX_CFG_LED_MODE


/* SKYCAM Lee Jitae 2010-09-25
 * OTP모듈의 불량률을 줄이기 위해 메모리 zone의 2개를 detecting하여
 * 현재 사용해야 될 zone을 찾아낸 후 해당 zone의 메모리를 사용 할 수 있도록 추가
 * WS02차에서 적용 예정
*/
#define F_SKYCAM_ADD_CFG_OTP_MODULE



/* 2010-10-06
 *Continous AF 를 테스트 하기 위해 작업 할 내용
 */
/* #define F_SKYCAM_CAF
*/

/* oen exif tag 수정 */
#define F_SKYCAM_OEM_EXIF_TAG

/*
 * 800*480, 640*480 을 제외한 다른 이미지 사이즈에서 세로 모드(90도)로 해서 줌해서 사진 촬영을 한 경우 이미지가 깨짐
 * 이미지 왼쪽 끝쪽이 깨어진다. 구글 기본 카메라에서는 3M 1.4배에서 깨지기 시작하는 것을 확인함
 * 이러한 이미지가 깨지는 경우를 작업한 것
 */
#define F_SKYCAM_QBUG_ROTATION_ZOOM

/* ES01 차용 바이너리 제어 Feature  Lee Jitae 2010-10-13 */
/* EF18K의WS02차 시료 H/W 문제및 power sequence가 맞지  않았음
 *  그러한 Power sequence문제 해결을 위한 Feature
 * EF18K WS02차 시료는 피쳐를 막고 ES1차 이후는 열어준다.
 */
#define F_SKYCAM_EF18K_ES01

  /* SKYCAM_PSJ_100622
 * BGM을 틀어놓은 상태에서 카메라 연속촬영시 촬영음 깨지는 현상이 있으며
 * get_duration함수를 이용하여 음원의 길이를 얻은 후 그만큼 usleep을 주면
 * 깨지는 현상이 없어지나 촬영음이 다 끝난 후에 찍은 사진이 보여짐
 * 음원이 깨지는 현상은 성능상의 문제이므로 일단 우리가 수정하지 않는 걸로 한다
*/

/* SKYCAM Lee Jitae 2010-10-22
 * VGA로 폴라로이드를 찍을 때 사운드가 뭉개지는 현상이 일어난다. EF13에서 연속 촬영에서 문제가 생기나
 * EF18K에서는 문제 발생이 보이지 않았다. 약간 느려진 듯한 감은 있음.
 * 일단 EF13S에 나온 것은 상기에 표기한다.
 */
#define F_SKYCAM_FIX_SNAPSHOT_BEEP_SPLIT


/* SKYCAM Lee Jita3 2010-11-23
 * 720P에서 프리뷰가 1280*720이 되지 않는다.
 * 표현 될 수 있는 프리뷰 사이즈로 변경해준다.
 */
#define F_SKYCAM_720P_PREVIEW

/* SKYCAM Lee Jitae 2010-12-10
 * MARKET CAMERA APP나 I'M IN APP의 POSTVIEW가 깨진다.
 * 원인은 Set_demension을 하면 POSTVIEW사이즈가 변하는 어플들이 있다.
 * 이런  어플들은 CAMERA_SET_PARM_DIMENSION 이후 값이 변경된다.
 * SKYCAM에서는 1.3333으로 비율에 변화가 없이 나오기 때문에 문제없다.
 */
#define F_SKYCAM_QBUG_MARKET_APP


 /* SKYCAM Lee JItae 2010-11-23
  * WVGA일 경우 줌을 하면 POSTVIEW가 커진다.
  * Qualcomm으로 부터 패치 받은 코드
  */
#define F_SKYCAM_QBUG_WVGA_ZOOM

/* CAMERA전원의 경우 Sensor Driver에 정의되어져 있다.
 * Surf Board의 경우는 msm_camera.c의 __msm_open(), msm_camio_sensor_clk_on을 호출할 경우
 * Power를 On시키고  있다. 이미 Sensor Driver에서 전원을 정의 해 주었기 때문에 여기서의 Power On
 * 은 하지 않는다. 불필요한 전원을 On 시키고  있다.
*/
#define F_SKYCAM_POWER


/* iPhone에서 MMS가 재생되지 않는 문제가 발생
   SID패킷 문제라는 Qualcomm에서의 SR의 답변으로
   패치함
 */
#define F_SKYCAM_IPHONE_MMS

/* 
 * Gingerbread의 CameraService에서 lockIfMessageWanted(int32_t msgType) 함수가 추가 되었다.
 * CameraService의 callback 처리 함수에서 mLock을 check하여 LOCK 상태이면, UNLOCK까지 waiting 한다.
 * capture 수행 도중 UI 로부터 command가 내려오면 callback 함수에서 이로 인해 지연이 발생한다.
 * capture 수행 중 카메라 종료시 이로 인해 CameraHAL보다 UI가 먼저 종료 되는 경우가 발생한다.
 * UI가 먼저 종료되고 CameraHAL 종료전에 다시 Camera가 실행되면 정상적으로 Open 하지 못한다.
 * lockIfMessageWanted 함수를 사용 하지 않도록 수정하였다.
*/
#define F_SKYCAM_FIX_CS_TRYLOCK

/* 동영상 녹화 시작/종료를 빠르게 반복하거나, 이어잭을 장착한 상태에서 연속촬영
 * 모드로 촬영할 경우, MediaPlayer 가 오동작하면서 HALT 발생한다.
 *
 * MediaPlayer 의 경우, 동일한 음원을 재생 중에 또 다시 재생 시도할 경우 100%
 * 오동작하므로 동일 음원을 연속하여 재생해야 할 경우, 반드시 이전 재생이 완료
 * 되었는지 여부를 확인 후 재생해야 한다. */
#define F_SKYCAM_QBUG_SKIP_CAMERA_SOUND

/*
 * 비디오 레코딩 시작과 종료 음이 끊기는 현상에 대한 디버깅
 * 효과음 재생 중 시스템 부하로 인해 소리가 끊기는 경우 발생
 * 레코딩 시작음 start 이 후 시작음 종료 될 때까지 wating
 * EF31S/EF32K 에서는 Sound쪽 kernel message가 나오는 경우 소리가 끊기며,
 * kernel message를 막거나 release build에서는 현상 발생 안함.
*/
#define F_SKYCAM_QBUG_REC_BEEP_SOUND

/* 촬영음/녹화음 재생 중에 응용이 종료될 경우, CLIENT 소멸 시에 해당 촬영음/
 * 녹화음 오브젝트가 강제로 disconnect/clear 되면서 MEDIA SERVER 가 죽는 것을
 * 방지한다. */
#define F_SKYCAM_QBUG_STOP_CAMERA_SOUND
/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC CONSTANTS                                                  */
/*  모델 관련 상수 선언                                       */
/*----------------------------------------------------------------------------*/

#ifdef F_SKYCAM_FACTORY_PROC_CMD
#define C_SKYCAM_FNAME_FACTORY_PROC_CMD	"/data/.factorycamera.dat"
#endif


/* 설정 가능한 최소/최대 포커스 레벨이다. */
#ifdef F_SKYCAM_FIX_CFG_FOCUS_STEP
#define C_SKYCAM_MIN_FOCUS_STEP 0	/* 무한대 (default) */
#define C_SKYCAM_MAX_FOCUS_STEP 9	/* 매크로 */
#endif

/*  FPS 세팅*/
#ifdef F_SKYCAM_FIX_CFG_PREVIEW_FPS
#define C_SKYCAM_MIN_PREVIEW_FPS	5
#define C_SKYCAM_MAX_PREVIEW_FPS	31
#define C_SKYCAM_VT_PREVIEW_FPS		7
#endif

/* Brightness 세팅 */
#ifdef F_SKYCAM_FIX_CFG_BRIGHTNESS
#define C_SKYCAM_MIN_BRIGHTNESS 0
#define C_SKYCAM_MAX_BRIGHTNESS 8
#endif

#define C_SKYCAM_SELECT_MAIN_CAM 		0
#define C_SKYCAM_SELECT_SUB_CAM 		1
#ifdef F_SKYCAM_ADD_VT_CAM
#define C_SKYCAM_SELECT_VT_SUB_CAM 		2
#define C_SKYCAM_SELECT_VT_MAIN_CAM		3
/* VT camera preview size */
#define VT_CAMERA_PREVIEW_WIDTH		176
#define VT_CAMERA_PREVIEW_HEIGHT 	144
#endif
#ifdef F_SKYCAM_FAKE_FRONT_CAMERA
#define FAKE_CAMERA_PREVIEW_WIDTH			336
#define FAKE_CAMERA_PREVIEW_HEIGHT 			448
#define FAKE_CAMERA_CAPTURE_CROP_WIDTH		328
#define FAKE_CAMERA_CAPTURE_CROP_HEIGHT 	440
#define FAKE_CAMERA_OUT_IMAGE_WIDTH			640
#define FAKE_CAMERA_OUT_IMAGE_HEIGHT 		480
#endif

#ifdef F_SKYCAM_OEM_EXIF_TAG
#define C_SKYCAM_EXIF_MAKE		"PANTECH"
#define C_SKYCAM_EXIF_MAKE_LEN		8		/* including NULL */
#define C_SKYCAM_EXIF_MODEL		"IM-A710K"
#define C_SKYCAM_EXIF_MODEL_LEN		9		/* including NULL */
#endif

#endif /* CUST_SKYCAM.h */
